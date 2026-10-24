#!/usr/bin/env node
// SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception
import {readFile, writeFile} from 'node:fs/promises';
import {basename} from 'node:path';
import {importMidiProject} from './midi-project.mjs';

function usage() {
  console.error('Usage: node tools/import-midi.mjs melody.mid template.fm1project output.fm1project [--track 1-8] [--midi-track 1-N] [--grid 1|2|4|8]');
  process.exit(2);
}

const args = process.argv.slice(2), positional = [];
const options = {};
for (let i = 0; i < args.length; ++i) {
  if (!args[i].startsWith('--')) { positional.push(args[i]); continue; }
  const value = args[++i];
  if (value == null) usage();
  if (args[i - 1] === '--track') options.deviceTrack = Number(value) - 1;
  else if (args[i - 1] === '--midi-track') options.midiTrack = Number(value) - 1;
  else if (args[i - 1] === '--grid') options.stepsPerQuarter = Number(value);
  else usage();
}
if (positional.length !== 3) usage();
options.title = basename(positional[0]).replace(/\.[^.]+$/, '');
try {
  const [midi, template] = await Promise.all([readFile(positional[0]), readFile(positional[1])]);
  const result = importMidiProject(template, midi, options);
  await writeFile(positional[2], result.project);
  console.log(JSON.stringify(result.summary, null, 2));
} catch (error) {
  console.error(`Import failed: ${error.message}`);
  process.exit(1);
}
