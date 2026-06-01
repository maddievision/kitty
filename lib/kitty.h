#ifndef _KITTY_H
#define _KITTY_H

#include <tonc.h>
#include "sound.h"
#include "bank.h"
#include "player.h"

typedef struct {
  SoundArea snd;
  PlayerState player;
} KittyState;

typedef enum {
  KT_FREQ_MODE_5734HZ = 1,
  KT_FREQ_MODE_7884HZ = 2,
  KT_FREQ_MODE_10512HZ = 3,
  KT_FREQ_MODE_13379HZ = 4,
  KT_FREQ_MODE_15768HZ = 5,
  KT_FREQ_MODE_18157HZ = 6,
  KT_FREQ_MODE_21024HZ = 7,
  KT_FREQ_MODE_26758HZ = 8,
  KT_FREQ_MODE_31536HZ = 9,
  KT_FREQ_MODE_36314HZ = 10,
  KT_FREQ_MODE_40137HZ = 11,
  KT_FREQ_MODE_42048HZ = 12
} KittyFreqMode;

typedef struct {
  u8 voices;
  u8 masterVolume;
  KittyFreqMode freqMode;
  u8 reverb;
  SoundBank* bank0;
  SoundBank* bank127;
} KittyConfig;

typedef enum {
  KT_LIVEMIDI_MODE_OFF = 0,
  KT_LIVEMIDI_MODE_FIXED_BUFFER = 1,
  KT_LIVEMIDI_MODE_RINGBUFFER = 2
} KittyLiveMidiMode;

//MAX_VCE, 11, 9, 10
void KittyInit(KittyState* kitty, KittyConfig config);
void KittyLiveMidiInit(KittyState *kitty, KittyLiveMidiMode mode, u8* buffer, vu16* bufferSizeInfo);
void KittySetVSync(KittyState* kitty, u8 enabled);
void KittyVSync(KittyState* kitty);
void KittyMain(KittyState* kitty);
void KittyLoadSong(KittyState* kitty, u8** data, char* error);
void KittyPlaySong(KittyState* kitty);
void KittyStopSong(KittyState* kitty);
void KittyAllNotesOff(KittyState* kitty);
void KittyReset(KittyState* kitty);

#endif