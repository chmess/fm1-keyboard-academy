# FM1 Keyboard Academy v0.1.2 — preview

LED synchronization fix for fast demonstration passages. This release also
contains the first/last-note timing corrections from v0.1.1.

## Fixed

- demonstration LEDs no longer depend on the slower LCD paint schedule;
- each observed sequencer step updates the physical key LEDs immediately;
- fast adjacent notes retain their correct light sequence;
- demonstrations explicitly rewind after a short lead-in;
- the first learner key is scored and the final note can ring until release.

## Verification

- host learning, curriculum and MIDI-import tests pass;
- strict host compilation of the changed screen controller passes;
- full PI32v2 target compile/link passes;
- original flash layout and 536,576-byte user region retained;
- verified stock V15 startup region preserved byte-for-byte;
- package identity: `FM-1_8241004`.

Firmware SHA-256:
`c65d04439f57011d43483d336c501472a3e38ac51ebed327093225b5b30bff7c`

This remains a preview. Back up the device before installing and keep USB and
power connected during the update.
