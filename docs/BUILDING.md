# Building and verification

The target is the JieLi AC791N/PI32v2 used by the M-VAVE FM-1. This is not an
Arduino project. Do not substitute an ESP32 toolchain or infer a board profile
from the USB identifier.

## ARM64 Linux / Raspberry Pi

`tools/build-linux-qemu.py` runs JieLi's official x86-64 Linux compiler under
`qemu-x86_64`. It expects:

- `JIELI_TOOLCHAIN`: extracted `jieli-linux-toolchains-20260730.1` directory;
- `FM1_X86_ROOT`: private extracted amd64 libc/libstdc++ runtime;
- `FM1_STOCK_PACKAGE`: verified stock V15 `FM-1.fwsc`.

The stock package must have SHA-256
`db1642b2b6fa5c2cccb11ffd13878068bb28601678d3644049f99dc40e7edb8a`.
Its first 16 KiB are copied unchanged into the custom package.

~~~sh
bash tests/run.sh
JIELI_TOOLCHAIN=/path/to/jieli-linux-toolchains-20260730.1 \
FM1_X86_ROOT=/path/to/amd64-runtime \
FM1_STOCK_PACKAGE=/path/to/FM-1_V15.fwsc \
python3 tools/build-linux-qemu.py
~~~

Outputs are written below `build/linux-dx7/`. `firmware-report.json` records
section sizes, hashes, product identity and whether hardware was tested.

## Release checks

1. `bash tests/run.sh`
2. strict host syntax check for `app/screen.c`
3. complete PI32v2 compile and link
4. package size/layout check
5. independent `.fwsc` product and stock-head verification
6. test on physical hardware before promoting a preview to stable

An empty serial or MIDI response is not considered hardware validation.
