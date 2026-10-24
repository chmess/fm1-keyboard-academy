# FM1 Keyboard Academy v0.1.1 — preview

Timing fix for the first and last notes of every learning round.

## Fixed

- demonstrations now wait for a short lead-in and explicitly rewind before playing;
- the first key pressed on the get-ready screen starts the learner's turn and is scored;
- the final correct note is no longer cut off when the result screen appears.

## Verification

- host learning, curriculum and MIDI-import tests pass;
- strict host compilation of the changed screen controller passes;
- full PI32v2 target compile/link passes;
- original flash layout and 536,576-byte user region retained;
- verified stock V15 startup region preserved byte-for-byte;
- package identity: `FM-1_8241003`.

Firmware SHA-256:
`5f9f71425095c601f98d7fd2b8b40f1417162d695a935531a4f086509a2ccce2`

This remains a preview. Back up the device before installing and keep USB and
power connected during the update.
