#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception
"""Build FM1 Learn on ARM64 Linux with JieLi's x86-64 Linux toolchain."""
import hashlib
import json
import os
import pathlib
import re
import shutil
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
BUILD = ROOT / "build/linux-dx7"
DEFAULT_TC = ROOT / ".toolchain/extracted/jieli-linux-toolchains-20260730.1"
DEFAULT_X86_ROOT = ROOT / ".toolchain/x86-root"


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(tool, arguments, cwd=ROOT, capture=False):
    environment = os.environ.copy()
    environment["QEMU_LD_PREFIX"] = str(X86_ROOT)
    command = ["qemu-x86_64", "-L", str(X86_ROOT), str(tool), *map(str, arguments)]
    if os.environ.get("FM1_BUILD_VERBOSE"):
        print("+", " ".join(map(str, command)))
    result = subprocess.run(command, cwd=cwd, env=environment, check=True, text=True,
                            stdout=subprocess.PIPE if capture else None)
    return result.stdout if capture else None


def verify_sdk():
    manifest = json.loads((ROOT / "sdk/manifest.json").read_text())
    for relative, expected in manifest["files"].items():
        path = ROOT / relative
        if not path.is_file() or sha(path) != expected:
            raise SystemExit(f"SDK payload differs: {relative}")


def version_values():
    text = (ROOT / "app/version.h").read_text()
    version = re.search(r'^#define FM1_RELEASE_VERSION "(V(?:0[1-9]|[1-9][0-9]{1,2}))"$', text, re.M)
    product = re.search(r'^#define FM1_UPDATE_PRODUCT "(FM-1_8[0-9]{6})"$', text, re.M)
    if not version or not product:
        raise SystemExit("Invalid app/version.h")
    return version.group(1), product.group(1)


def source_identity():
    items = ["dx7", sha(ROOT / "sdk/manifest.json")]
    for path in sorted((ROOT / "app").rglob("*")):
        if path.is_file():
            items.append(f"{path.relative_to(ROOT).as_posix()}:{sha(path)}")
    return hashlib.sha256("\n".join(items).encode()).hexdigest()


def main():
    global X86_ROOT
    X86_ROOT = pathlib.Path(os.environ.get("FM1_X86_ROOT", DEFAULT_X86_ROOT)).resolve()
    tc = pathlib.Path(os.environ.get("JIELI_TOOLCHAIN", DEFAULT_TC)).resolve()
    clang = tc / "pi32v2/bin/clang"
    ld = tc / "pi32v2/bin/ld"
    nm = tc / "pi32v2/bin/nm"
    objcopy = tc / "pi32v2/bin/objcopy"
    plugin = tc / "common/bin/LLVMgold.so"
    for required in (clang, ld, nm, objcopy, plugin, X86_ROOT / "lib64/ld-linux-x86-64.so.2"):
        if not required.exists():
            raise SystemExit(f"Missing build prerequisite: {required}")
    verify_sdk()
    version, product = version_values()
    stock_default = pathlib.Path("/home/mac/FM1-update/.cache/FM-1_V15.fwsc")
    stock_package = pathlib.Path(os.environ.get("FM1_STOCK_PACKAGE", stock_default)).resolve()
    if not stock_package.is_file():
        raise SystemExit("Set FM1_STOCK_PACKAGE to the verified stock FM-1 V15 .fwsc package")
    build_id = source_identity()
    objects = BUILD / "obj"
    generated = BUILD / "generated"
    if BUILD.exists():
        shutil.rmtree(BUILD)
    objects.mkdir(parents=True)
    generated.mkdir()
    include = ["sdk/include", "sdk/include/libc", "app/synth", "sdk/config", "app", "app/board"]
    flags = ["-target", "pi32v2", "-integrated-as", "-mcpu=r3", "-mfprev1", "-Wuninitialized",
             "-Wno-invalid-noreturn", "-fno-common", "-flto", "-fallow-pointer-null", "-fprefer-gnu-section",
             "-femulated-tls", "-Wno-shift-negative-value", "-Wframe-larger-than=2560", "-fms-extensions",
             "-fno-unwind-tables", "-ffunction-sections", "-fdata-sections", "-fmessage-length=0",
             "-cl-single-precision-constant", "-mllvm", "-pi32v2-large-program=true", "-D__GCC_PI32V2__", "-O2"]
    flags += ["-I" + str(ROOT / item) for item in include]
    sources = [p for p in sorted((ROOT / "app").rglob("*")) if p.suffix in (".c", ".S") and (ROOT / "app/synth") not in p.parents]
    object_files = []
    commands = []
    compact = {"project.c", "transfer.c", "samples.c", "screen.c", "presets.c", "midi_router.c", "fm_factory.c", "fm_library.c",
               "learn.c", "lessons.c"}
    for index, source in enumerate(sources):
        relative = source.relative_to(ROOT).as_posix()
        output = objects / f"{index}_{relative.replace('/', '_')}.o"
        options = list(flags)
        if relative in ("app/screen.c", "app/transfer.c", "app/midi_router.c"):
            options.append("-fno-lto")
        if relative == "app/board/panel.c":
            options += ["-fno-lto", "-fno-pic"]
        options += ["-fwrapv", "-fno-fast-math", "-ffp-contract=off"]
        if source.name in compact:
            options.append("-Oz")
        if relative == "app/screen.c":
            options.append("-fno-inline-functions")
        args = [*options, "-MMD", "-MF", str(output) + ".d", "-c", source, "-o", output]
        run(clang, args)
        object_files.append(output)
        commands.append({"directory": str(BUILD), "file": str(source), "output": str(output), "arguments": [str(clang), *map(str, args)]})
    identity = generated / "build_identity.c"
    id_bytes = ",".join("0x" + build_id[i:i + 2] for i in range(0, 64, 2))
    identity.write_text('#include "layout.h"\nconst struct fm1_firmware_layout fm1_layout = {536576U,7U};\n' +
                        f'const unsigned char fm1_build_id[32] = {{{id_bytes}}};\n' +
                        f'const unsigned char fm1_update_identity[27] = "{product}";\n')
    identity_object = objects / "build_identity.o"
    run(clang, [*flags, "-c", identity, "-o", identity_object])
    object_files.append(identity_object)
    linker = (ROOT / "sdk/config/linker.ld.in").read_text().replace("@SBC_DECODER@", "sbc_decoder\n").replace("@ACL_TX_EXTRA@", " + 1024").replace("@ACL_RX_EXTRA@", " + 4096")
    linker_path = generated / "linker.ld"
    linker_path.write_text(linker)
    elf = BUILD / "sdk.elf"
    link = ["--plugin", plugin, "-o", elf, *object_files, "-flto",
            "--plugin-opt=-pi32v2-always-use-itblock=false", "--plugin-opt=-enable-ipra=true",
            "--plugin-opt=-pi32v2-merge-max-offset=4096", "--plugin-opt=-pi32v2-enable-simd=true",
            "--plugin-opt=mcpu=r3", "--plugin-opt=-global-merge-on-const", "--plugin-opt=-inline-threshold=5",
            "--plugin-opt=-inline-max-allocated-size=32", "--plugin-opt=-inline-normal-into-special-section=true",
            "--plugin-opt=-dont-used-symbol-list=malloc,free,sprintf,printf,puts,putchar",
            "--plugin-opt=-pi32v2-enable-rep-memop", "--plugin-opt=-emulated-tls", "--sort-common",
            "--plugin-opt=-pi32v2-large-program=true", "--plugin-opt=-pi32v2-csync-before-rti=true", "--start-group",
            "--whole-archive", ROOT / "sdk/lib/libfm1_engine.a", "--no-whole-archive",
            ROOT / "sdk/lib/libc.a", ROOT / "sdk/lib/libcompiler_rt.a", "--end-group", "--gc-sections",
            "-T", linker_path, "-M=" + str(BUILD / "sdk.map"), "--undefined=connectivity_wl82_bluetooth_start",
            "--undefined=audio_app_bluetooth_open", "--undefined=bt_get_local_name", "--undefined=bt_get_mac_addr"]
    run(ld, link)
    data = elf.read_bytes()
    if data[:6] != b"\x7fELF\x01\x01" or struct.unpack_from("<I", data, 24)[0] != 0x02000120:
        raise SystemExit("Linked firmware has the wrong ELF format or entry point")
    symbols = run(nm, ["--print-size", elf], capture=True)
    required = ["fm1_update_init", "fm1_app_sysex", "groove_learn_begin", "groove_lesson_load", "groove_project_apply_edit"]
    missing = [name for name in required if not re.search(rf"\b{re.escape(name)}$", symbols, re.M)]
    if missing:
        raise SystemExit("Required linked symbols missing: " + ", ".join(missing))
    app = bytearray()
    section_report = {}
    for section in ("text", "data", "ram0_data", "cache_ram_data"):
        output = generated / f"{section}.bin"
        run(objcopy, ["-O", "binary", "-j", "." + section, elf, output])
        payload = output.read_bytes()
        section_report[section] = len(payload)
        app += payload
    app_file = generated / "FM1_KEYBOARD_ACADEMY.app.bin"
    app_file.write_bytes(app)
    package = generated / f"FM1_KEYBOARD_ACADEMY_{version}.fwsc"
    report_file = BUILD / "package-report.json"
    subprocess.run([sys.executable, ROOT / "tools/fm1_package.py", app_file, ROOT / "sdk/tools/pack", package,
                    product, "--preserve-head-from", stock_package, "--report", report_file], check=True)
    report = {"passed": True, "board_access": False, "hardware_validated": False, "release_version": version,
              "product": product, "build_id": build_id, "elf_sha256": sha(elf), "package_sha256": sha(package),
              "package_bytes": package.stat().st_size, "app_bytes": len(app), "sections": section_report,
              "toolchain": str(tc), "host": "linux-arm64-qemu-x86_64", "boot_head_preserved": True}
    (BUILD / "firmware-report.json").write_text(json.dumps(report, indent=2) + "\n")
    (BUILD / "compile_commands.json").write_text(json.dumps(commands, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
