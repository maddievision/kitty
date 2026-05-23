## Kitty Advance

MIDI-based GBA sound driver supporting samples + PSG channels

Demos:
- Demo song: ([Video](https://streamable.com/yfdqu0)) ([GSF](https://files.maddie.rocks/fonsett/s4a_demo.gsf)) ([GBA](https://files.maddie.rocks/fonsett/s4a_demo.gba))
- Hare Hare Yukai: ([Video](https://streamable.com/e16lkr)) ([GSF](https://files.maddie.rocks/fonsett/s4a_harehareyukai.gsf)) ([GBA](https://files.maddie.rocks/fonsett/s4a_harehareyukai.gba))

## Features

- Reads directly from MIDI file
- Uses ipatix's HQ-Mixer https://github.com/ipatix/gba-hq-mixer to mix up to 32* sample voices
- Soundbank format similar to the one used in MP2k/Sappy. Currently supported:
  - Single sample instruments
  - Multi sample instruments with keymap
  - Drum kit
- Samples can be converted using [wav2agb](https://github.com/ipatix/wav2agb).
- Loop points using "[" and "]" markers
- Vibrato/LFO via CC 1 (Mod wheel). Uses triangle wave. Speed/depth not yet configurable. For DirectSound and GBC
- Velocity, volume (CC 7), and expression (CC 11) for DirectSound and GBC(except mid note)
  - Volume levels are based off of sqrt, so are more aligned with standard MIDI files and DAWs.
- Pan (CC 10) for DirectSound and GBC
- Sustain (CC 64) for DirectSound and GBC
- Set voice stealing priority (CC 33). Higher number means higher priority. If not set, or tracks are equal priority, then favour is ordered by descending track number (e.g, Track 1 has first priority).
- Pitch bend and pitch bend range via CC 20 or the standard RPN 0,0
- GBC channel support
  - Supports the 2 GBC Pulse wave channels, only by switching channel output via CC 4 (1 = Channel 1, 2 = Channel 2)
  - GBC Duty cycle change via CC 2 (0 = 12.5%, 32 = 25%, 64 = 50%, 96 = 75%)
  - GBC hardware envelope set via CC 3 (64 = center, lowest speed (0). 0 = downwards at fastest speed, 127 = upwards at fastest speed)
- Live MIDI input support by reading a buffer of MIDI commands in memory

(* practical maximum depends on mixing sample rate. e.g., this can go up to 48 in frequency mode 3, but only up to 16 in frequency mode 8, so TBD/needs more testing)

## TODO

- TBD
- GBC Wave setting
- GBC Noise channel support
- Complete soundbank implementation:
  - GBC with ADSR envelopes
  - HQ Mixer synth instruments
- GBC software envelopes for duty and pitch
- Tooling soundbank creation
- Additional mixer features:
  - Lowpass filters per voice
