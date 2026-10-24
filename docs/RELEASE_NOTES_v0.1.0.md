# FM1 Keyboard Academy v0.1.0 — preview

First public preview of the beginner-first FM-1 learning firmware.

## Highlights

- boots into Keyboard Academy;
- eight chapters with twelve foundation exercises and thirty melodies;
- demonstration, guided play, red wrong-note feedback and assessment;
- automatic replay after inactivity and mastery-based continuation;
- four-knob learning home and optional assistance settings;
- standard DX7 sounds and the original 32-algorithm editor;
- MIDI-to-project importer and normal sequence editing;
- ARM64 Raspberry Pi build using the official JieLi toolchain through QEMU.

## Verification

- host learning, curriculum and MIDI-import tests pass;
- full PI32v2 target compile/link passes;
- original flash layout and 536,576-byte user region retained;
- verified stock V15 startup region preserved byte-for-byte.

Firmware SHA-256:
`2751e350b584086aa49c64fe43916c4566fb7731cd7c85a377891bd8e733fc07`

This preview has not yet completed physical learning-flow validation. Back up
the device before installing and keep USB and power connected during update.
