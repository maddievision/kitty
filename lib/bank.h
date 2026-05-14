#ifndef _BANK_H
#define _BANK_H

#include "sound.h"

#define SOUND_ENTRY_TYPE_SINGLE 0x00
#define SOUND_ENTRY_TYPE_CGB_PULSE_A 0x01
#define SOUND_ENTRY_TYPE_CGB_PULSE_B 0x02
#define SOUND_ENTRY_TYPE_CGB_PULSE_WAVE 0x03
#define SOUND_ENTRY_TYPE_CGB_PULSE_NOISE 0x04
#define SOUND_ENTRY_TYPE_FIXED_FLAG 0x08
#define SOUND_ENTRY_TYPE_MULTI 0x40
#define SOUND_ENTRY_TYPE_KIT 0x80
#define SOUND_ENTRY_TYPE_DISABLED 0xFF

typedef struct {
  u8 type;
  u8 rootnote;
  u8 amp;
  u8 cgbps;  
  WaveData *sample;
  u8 attack;
  u8 decay;
  u8 sustain;
  u8 release;
} SoundEntry;

typedef struct {
  SoundEntry entries[128];
} SoundBank;

typedef struct {
  u8 entries[128];
} SoundMap;

typedef struct {
  u8 type;
  u8 basenote;
  u8 res[2];
  SoundBank *group;
  SoundMap *map;
} SoundEntryMulti;

typedef struct {
  u8 type;
  u8 rootnote;
  u8 cgblen;
  u8 cgbps;  
  u8 duty;
  u8 res[3];
  u8 attack;
  u8 decay;
  u8 sustain;
  u8 release;  
} SoundEntryCGB;

typedef struct {
  u8 data[16];
} SoundCGBWave;

typedef struct {
  u8 type;
  u8 rootnote;
  u8 cgblen;
  u8 cgbps;  
  SoundCGBWave *sample;
  u8 attack;
  u8 decay;
  u8 sustain;
  u8 release;  
} SoundEntryCGBWave;

#endif