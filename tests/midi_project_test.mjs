// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import {importMidiProject, parseMidi, PROJECT_BYTES} from '../tools/midi-project.mjs';

const bytes = (...items) => Uint8Array.from(items.flatMap(item =>
  typeof item === 'number' ? [item] : Array.from(item)));
const track = bytes(
  0x00, 0xff, 0x03, 0x04, ...Buffer.from('TEST'),
  0x00, 0xff, 0x51, 0x03, 0x07, 0xa1, 0x20,
  0x00, 0x90, 60, 100, 0x83, 0x60, 0x80, 60, 0,
  0x00, 0x90, 62, 90, 0x83, 0x60, 0x80, 62, 0,
  0x00, 0xff, 0x2f, 0x00
);
const midi = bytes(
  ...Buffer.from('MThd'), 0, 0, 0, 6, 0, 0, 0, 1, 0x01, 0xe0,
  ...Buffer.from('MTrk'), (track.length >>> 24) & 255, (track.length >>> 16) & 255,
  (track.length >>> 8) & 255, track.length & 255, track
);
const parsed = parseMidi(midi);
assert.equal(parsed.division, 480);
assert.equal(parsed.tempo, 120);
assert.deepEqual(parsed.tracks[0].notes.map(note => note.note), [60, 62]);

const template = new Uint8Array(PROJECT_BYTES).fill(0x55);
const beforePreset = template.slice(2048, 2048 + 223);
const {project, summary} = importMidiProject(template, midi, {deviceTrack: 0, stepsPerQuarter: 4});
assert.equal(summary.notes, 2);
assert.equal(project[0], 60);
assert.equal(project[1], 100);
assert.equal(project[4 * 4], 62);
assert.equal(project[4 * 4 + 1], 90);
assert.deepEqual(project.slice(2048, 2048 + 221), beforePreset.slice(0, 221));
assert.equal(project[2048 + 221], 1);
assert.equal(project[3832] | project[3833] << 8, 120);
assert.equal(new TextDecoder().decode(project.slice(3836, 3848)), 'TEST        ');
console.log('midi_project_test: ok');
