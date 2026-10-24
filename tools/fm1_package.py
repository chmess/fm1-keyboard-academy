#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Python host port of sdk/Package.cs for Linux/QEMU builds.
import argparse
import hashlib
import json
import pathlib
import re
import struct

AREA = 0x4000
ENTRY = 0x02000120
APP_DATA = 0x120
FLASH_LIMIT = 0xFF000
USER_SIZE = 536576
BT_SIZE = 4096
MINIMUM_VM = 32768
PUBLIC_CODE = 0x980F
STOCK_V15_SHA256 = "db1642b2b6fa5c2cccb11ffd13878068bb28601678d3644049f99dc40e7edb8a"
WRAPPED_FLASH_OFFSET = 0x414


def crc16(data):
    value = 0
    for byte in data:
        value ^= byte << 8
        for _ in range(8):
            value = ((value << 1) ^ (0x1021 if value & 0x8000 else 0)) & 0xFFFF
    return value


def cipher(data, begin, length, state):
    for at in range(begin, begin + length):
        data[at] ^= state & 255
        state = ((state << 1) ^ (0x1021 if state & 0x8000 else 0)) & 0xFFFF


def round_up(value, alignment):
    return (value + alignment - 1) & -alignment


def number(data, offset, value, width):
    data[offset:offset + width] = int(value).to_bytes(width, "little")


def record(data, offset, name, checksum, address, size, attributes, last, extra=None):
    data[offset:offset + 32] = bytes(32)
    number(data, offset + 2, checksum, 2)
    number(data, offset + 4, address, 4)
    number(data, offset + 8, size, 4)
    number(data, offset + 12, attributes, 2)
    number(data, offset + 14, last, 2)
    data[offset + 16:offset + 32] = bytes([255]) * 16
    encoded = name.encode("ascii")
    data[offset + 16:offset + 16 + len(encoded)] = encoded
    data[offset + 16 + len(encoded)] = 0
    if extra is not None:
        number(data, offset + 28, extra, 4)
    number(data, offset, crc16(data[offset + 2:offset + 32]), 2)


def encode_boot(plain):
    result = bytearray(plain)
    banks = int.from_bytes(plain[0:2], "little")
    if banks < 1 or banks > len(plain) // 16:
        raise ValueError("invalid boot directory")
    previous_end = banks * 16
    for index in range(banks):
        header = index * 16
        length = int.from_bytes(plain[header + 2:header + 4], "little")
        start = int.from_bytes(plain[header + 8:header + 12], "little")
        if (not length or start < previous_end or start > len(plain) - length or
                crc16(plain[header:header + 14]) != int.from_bytes(plain[header + 14:header + 16], "little") or
                crc16(plain[start:start + length]) != int.from_bytes(plain[header + 12:header + 14], "little")):
            raise ValueError("invalid boot bank")
        cipher(result, header, 16, 65535)
        cipher(result, start, length, 65535)
        previous_end = start + length
    return result


def checked_resource(directory, name, size):
    data = (directory / name).read_bytes()
    if len(data) != size:
        raise ValueError(f"unexpected FM1 resource: {name}")
    return data


def build_flash(application, resources, end, vm_end):
    boot = checked_resource(resources, "uboot.boot", 14488)
    config = checked_resource(resources, "boot-config.bin", 721)
    tool = checked_resource(resources, "cfg_tool.bin", 383)
    image = bytearray([255]) * end
    settings = bytearray(34 + len(config))
    for bit in range(16):
        settings[bit] = 16
        settings[16 + bit] = 16 ^ (255 if not ((PUBLIC_CODE >> bit) & 1) else 0)
    number(settings, 32, crc16(settings[:32]), 2)
    settings[34:] = config
    image[:32] = bytes((0, 0, 0x20, 2, 0x2F, 0x10, 0x4C, 0xC9, 0, 0xF0, 0x0F, 0, 0x10, 0x10, 0xA4, 0xFF,
                        0x4E, 0x5D, 0x0B, 0x41, 0xE0, 0xCD, 0x78, 0x1D, 0xC8, 0x56, 0x41, 0x7F, 0xB3, 0x67, 0xCF, 0x9F))
    number(image, 0, crc16(image[2:32]), 2)
    config_at = 160 + len(boot)
    record(image, 32, "uboot.boot", crc16(boot), 160, len(boot), 0, 0)
    record(image, 64, "isd_config.ini", crc16(settings), config_at, len(settings), 0x8002, 0)
    record(image, 96, "app_dir_head", 65535, AREA, 0xFFFFFFFF, 0xFF81, 0)
    record(image, 128, "key_mac", 65535, FLASH_LIMIT, 4096, 0x0112, 65535)
    for at in range(0, 160, 32):
        cipher(image, at, 32, 65535)
    image[160:160 + len(boot)] = encode_boot(boot)
    image[config_at:config_at + len(settings)] = settings
    payload_size = APP_DATA + len(application) + len(tool)
    app_at = AREA + APP_DATA
    image[app_at:app_at + len(application)] = application
    image[app_at + len(application):app_at + len(application) + len(tool)] = tool
    record(image, AREA + 32, "app.bin", crc16(application), APP_DATA, len(application), 0xFF82, 0)
    record(image, AREA + 64, "cfg_tool.bin", crc16(tool), APP_DATA + len(application), len(tool), 0xFF82, 0)
    record(image, AREA + 96, "VM", 65535, end, vm_end - end, 0x8112, 0, MINIMUM_VM)
    record(image, AREA + 128, "PRCT", 65535, 0, end, 0x8292, 0, 0x45444F43)
    record(image, AREA + 160, "BTIF", 65535, vm_end, BT_SIZE, 0x8192, 1, 0x4F545541)
    record(image, AREA, "app_area_head", crc16(image[AREA + 32:AREA + payload_size]), ENTRY, payload_size, 0xFF83, 0)
    reserved = AREA + payload_size
    record(image, reserved + 32, "USER", 65535, FLASH_LIMIT - USER_SIZE, USER_SIZE, 0x8192, 1, 0x4F545541)
    record(image, reserved, "EXT_RESERVED", crc16(image[reserved + 32:reserved + 64]), 32, 64, 0xFF93, 1)
    for at in range(0, payload_size + 64, 32):
        cipher(image, AREA + at, min(32, payload_size + 64 - at), PUBLIC_CODE ^ (at >> 2))
    return image


def wrap(flash, loader, product):
    if not loader or len(loader) >= 32768:
        raise ValueError("invalid FM1 update loader")
    loader_at = 1024 + round_up(len(flash), 32)
    total = loader_at + round_up(len(loader), 32)
    ufw = bytearray([255]) * total
    ufw[:224] = bytes(224)
    for index, (payload, name) in enumerate(((flash, "flash.bin"), (loader, "ota.bin"))):
        at = 64 + index * 80
        number(ufw, at, 0 if index == 0 else 100, 2)
        number(ufw, at + 2, index, 2)
        number(ufw, at + 4, crc16(payload), 2)
        number(ufw, at + 8, 1024 if index == 0 else loader_at, 4)
        number(ufw, at + 12, len(payload), 4)
        number(ufw, at + 16, round_up(len(payload), 32), 4)
        ufw[at + 64:at + 64 + len(name)] = name.encode("ascii")
        cipher(ufw, at, 80, 65535)
    number(ufw, 4, total, 4)
    number(ufw, 8, 2, 2)
    number(ufw, 10, 4, 2)
    number(ufw, 12, 512, 4)
    ufw[16:22] = b"AC791N"
    number(ufw, 2, crc16(ufw[64:224]), 2)
    number(ufw, 0, crc16(ufw[2:64]), 2)
    cipher(ufw, 0, 64, 65535)
    ufw[1024:1024 + len(flash)] = flash
    ufw[loader_at:loader_at + len(loader)] = loader
    output = bytearray()
    for source, byte in enumerate(ufw):
        output.append(byte)
        if source < 940 and (source + 1) % 47 == 0:
            index = source // 47
            output.append(ord(product[index]) + index + 1 if index < len(product) else 125)
    return output


def create(app, resources, output, product, preserve_head_from):
    if not re.fullmatch(r"FM-1_8[0-9]{6}", product):
        raise ValueError("invalid FM1 update identity")
    maximum = FLASH_LIMIT - USER_SIZE - BT_SIZE - MINIMUM_VM - AREA - APP_DATA - 383 - 64
    if not app or len(app) > maximum:
        raise ValueError("application does not fit the FM1 layout")
    image_size = round_up(AREA + APP_DATA + len(app) + 383 + 64, 4096)
    user_at = FLASH_LIMIT - USER_SIZE
    vm_end = user_at - BT_SIZE
    if image_size > vm_end - MINIMUM_VM:
        raise ValueError("insufficient FM1 settings space")
    flash = build_flash(app, resources, image_size, vm_end)
    stock = preserve_head_from.read_bytes()
    if hashlib.sha256(stock).hexdigest() != STOCK_V15_SHA256:
        raise ValueError("boot-head source is not the verified stock FM-1 V15 package")
    head = stock[WRAPPED_FLASH_OFFSET:WRAPPED_FLASH_OFFSET + AREA]
    if len(head) != AREA:
        raise ValueError("stock FM-1 package has no complete flash head")
    flash[:AREA] = head
    package = wrap(flash, (resources / "ota.bin").read_bytes(), product)
    output.write_bytes(package)
    return {"flash_bytes": 1048576, "board_access": False, "fits": True, "packager": "source-python-port",
            "FLASH_BIN_SIZE": image_size, "FLASH_NEED_SIZE": FLASH_LIMIT, "VM_START_ADDR": image_size,
            "VM_END_ADDR": vm_end, "VM_REAL_SIZE": vm_end - image_size, "BTIF_RESERVED_START": vm_end,
            "BTIF_RESERVED_END": user_at, "USER_RESERVED_START": user_at, "USER_RESERVED_END": FLASH_LIMIT,
            "vm_minimum_bytes": MINIMUM_VM, "vm_extra_gap_bytes": vm_end - image_size - MINIMUM_VM,
            "btif_bytes": BT_SIZE, "user_bytes": USER_SIZE, "package_bytes": len(package),
            "boot_head_preserved": True, "boot_head_source_sha256": STOCK_V15_SHA256}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("app", type=pathlib.Path)
    parser.add_argument("resources", type=pathlib.Path)
    parser.add_argument("output", type=pathlib.Path)
    parser.add_argument("product")
    parser.add_argument("--preserve-head-from", required=True, type=pathlib.Path)
    parser.add_argument("--report", type=pathlib.Path)
    args = parser.parse_args()
    report = create(args.app.read_bytes(), args.resources, args.output, args.product,
                    args.preserve_head_from)
    if args.report:
        args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
