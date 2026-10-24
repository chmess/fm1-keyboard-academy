// SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception
// Convert a standard MIDI melody into an FM1 Open project while preserving
// every timbre byte, including the selected DX7 preset and algorithm.

export const PROJECT_BYTES = 4064;
const TRACKS = 8;
const STEPS = 64;
const STEP_BYTES = 4;
const PRESET_OFFSET = TRACKS * STEPS * STEP_BYTES;
const PRESET_BYTES = 223;
const TEMPO_OFFSET = PRESET_OFFSET + TRACKS * PRESET_BYTES;
const NAME_OFFSET = TEMPO_OFFSET + 2 + 2;

function u16(data, at) { return (data[at] << 8) | data[at + 1]; }
function u32(data, at) { return ((data[at] * 0x1000000) + (data[at + 1] << 16) + (data[at + 2] << 8) + data[at + 3]) >>> 0; }
function variable(data, state) {
  let value = 0;
  for (let count = 0; count < 4; ++count) {
    if (state.at >= state.end) throw new Error('truncated variable-length value');
    const byte = data[state.at++];
    value = (value << 7) | (byte & 0x7f);
    if (!(byte & 0x80)) return value;
  }
  throw new Error('invalid variable-length value');
}

export function parseMidi(input) {
  const data = input instanceof Uint8Array ? input : new Uint8Array(input);
  if (data.length < 14 || String.fromCharCode(...data.slice(0, 4)) !== 'MThd') throw new Error('not a standard MIDI file');
  const header = u32(data, 4);
  if (header < 6 || 8 + header > data.length) throw new Error('invalid MIDI header');
  const format = u16(data, 8), trackCount = u16(data, 10), division = u16(data, 12);
  if (format > 1 || !trackCount || division & 0x8000) throw new Error('only PPQN MIDI format 0/1 is supported');
  const tracks = [];
  let at = 8 + header, tempo = 120;
  for (let index = 0; index < trackCount; ++index) {
    if (at + 8 > data.length || String.fromCharCode(...data.slice(at, at + 4)) !== 'MTrk') throw new Error('missing MIDI track');
    const bytes = u32(data, at + 4); at += 8;
    if (at + bytes > data.length) throw new Error('truncated MIDI track');
    const state = {at, end: at + bytes};
    let tick = 0, running = 0, name = '', order = 0;
    const open = new Map(), notes = [];
    while (state.at < state.end) {
      tick += variable(data, state);
      let status = data[state.at++];
      if (status < 0x80) {
        if (!running) throw new Error('running status without status byte');
        --state.at; status = running;
      } else if (status < 0xf0) running = status;
      if (status === 0xff) {
        if (state.at >= state.end) throw new Error('truncated meta event');
        const type = data[state.at++], length = variable(data, state);
        if (state.at + length > state.end) throw new Error('truncated meta payload');
        if (type === 0x03) name = new TextDecoder().decode(data.slice(state.at, state.at + length));
        if (type === 0x51 && length === 3) {
          const micros = (data[state.at] << 16) | (data[state.at + 1] << 8) | data[state.at + 2];
          if (micros) tempo = Math.max(30, Math.min(300, Math.round(60000000 / micros)));
        }
        state.at += length;
        continue;
      }
      if (status === 0xf0 || status === 0xf7) {
        const length = variable(data, state); state.at += length;
        if (state.at > state.end) throw new Error('truncated SysEx event');
        running = 0; continue;
      }
      const kind = status & 0xf0, channel = status & 15;
      const one = kind === 0xc0 || kind === 0xd0;
      if (state.at + (one ? 1 : 2) > state.end) throw new Error('truncated channel event');
      const a = data[state.at++], b = one ? 0 : data[state.at++];
      if ((kind === 0x90 && b) || kind === 0x80 || (kind === 0x90 && !b)) {
        const key = `${channel}:${a}`;
        if (kind === 0x90 && b) open.set(key, {tick, note: a, velocity: b, order: order++});
        else if (open.has(key)) {
          const value = open.get(key); open.delete(key);
          notes.push({...value, duration: Math.max(1, tick - value.tick), channel});
        }
      }
    }
    for (const value of open.values()) notes.push({...value, duration: division, channel: 0});
    notes.sort((a, b) => a.tick - b.tick || a.order - b.order);
    tracks.push({index, name, notes});
    at = state.end;
  }
  return {format, division, tempo, tracks};
}

export function importMidiProject(template, midi, options = {}) {
  const source = template instanceof Uint8Array ? template : new Uint8Array(template);
  if (source.length !== PROJECT_BYTES) throw new Error(`template must be exactly ${PROJECT_BYTES} bytes`);
  const parsed = parseMidi(midi);
  const deviceTrack = options.deviceTrack ?? 0;
  if (deviceTrack < 0 || deviceTrack >= TRACKS) throw new Error('FM1 track must be 0..7');
  let midiTrack = options.midiTrack == null
    ? parsed.tracks.reduce((best, item) => item.notes.length > best.notes.length ? item : best, parsed.tracks[0])
    : parsed.tracks[options.midiTrack];
  if (!midiTrack || !midiTrack.notes.length) throw new Error('selected MIDI track has no completed notes');
  const stepsPerQuarter = options.stepsPerQuarter ?? 4;
  if (![1, 2, 4, 8].includes(stepsPerQuarter)) throw new Error('stepsPerQuarter must be 1, 2, 4, or 8');
  const ticksPerStep = parsed.division / stepsPerQuarter;
  const firstTick = midiTrack.notes[0].tick;
  const converted = midiTrack.notes.map(note => ({...note, step: Math.round((note.tick - firstTick) / ticksPerStep)}));
  if (converted.some(note => note.step >= STEPS)) throw new Error('melody exceeds the FM1 64-step limit; shorten it or use a coarser grid');
  const occupied = new Set();
  for (const note of converted) {
    if (occupied.has(note.step)) throw new Error(`melody is polyphonic after quantization at step ${note.step + 1}`);
    occupied.add(note.step);
  }
  const project = new Uint8Array(source);
  const start = deviceTrack * STEPS * STEP_BYTES;
  project.fill(0, start, start + STEPS * STEP_BYTES);
  for (const note of converted) {
    const offset = start + note.step * STEP_BYTES;
    const fraction = note.duration / ticksPerStep;
    const gate = Math.max(1, Math.min(15, Math.round(fraction * 15)));
    project[offset] = note.note;
    project[offset + 1] = note.velocity;
    project[offset + 2] = (gate << 5) & 0xff;
    project[offset + 3] = (gate << 5) >> 8;
  }
  const used = converted[converted.length - 1].step + 1;
  const length = Math.min(64, Math.max(16, Math.ceil(used / 16) * 16));
  project[PRESET_OFFSET + deviceTrack * PRESET_BYTES + 221] = length === 64 ? 0 : length / 16;
  project[TEMPO_OFFSET] = parsed.tempo & 0xff;
  project[TEMPO_OFFSET + 1] = parsed.tempo >> 8;
  const title = (options.title || midiTrack.name || 'IMPORTED').toUpperCase().replace(/[^ A-Z0-9-]/g, ' ').slice(0, 12).padEnd(12, ' ');
  for (let i = 0; i < 12; ++i) project[NAME_OFFSET + i] = title.charCodeAt(i);
  return {project, summary: {midiTrack: midiTrack.index, deviceTrack, notes: converted.length, length, tempo: parsed.tempo, title: title.trim()}};
}
