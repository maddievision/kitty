## sappy4all (S4A)

MIDI player using built in mixer. Very WIP

Wave files converted using https://github.com/ipatix/wav2agb

Demos:
- Included MIDI: https://streamable.com/9gmq9a
- Hare Hare Yukai: https://streamable.com/wb69tc

## Features

- Reads directly from MIDI file
- Uses ipatix's HQ-Mixer https://github.com/ipatix/gba-hq-mixer to mix up to 12 sample voices
- Soundbank format similar to the one used in Sappy. Currently supported:
  - Single sample instruments
  - Multi sample instruments with keymap
  - Drum kit
- Tempo changes. Note that currently files at 24/48 PPQN resolution are most reliable. If PPQN is too high, or too low, player might freeze up or crash for the moment.
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

## TODO

- TBD
- Better support for all PPQN resolutions.
- Add support for GBC Wave and Noise channels in general
- Additional Soundbank instrument types:
  - GBC with ADSR envelopes
  - HQ Mixer synth instruments
- Better tooling for adding soundbanks, samples, and MIDI files
- Generally clean up code
- Live MIDI In support (emulator memory patch in + link cable planned)
- Add IT-style lowpass filter to mixer
