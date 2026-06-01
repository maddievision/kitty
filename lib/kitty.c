#include "kitty.h"
#include "cgbsound.h"
#include "mixer.h"
#include "player.h"

void KittyInit(KittyState* kitty, KittyConfig config) {
  MixerInit(&kitty->snd, config.voices, config.masterVolume, config.freqMode, config.reverb);
  SoundInitCGB(&kitty->snd);
  PlayerInit(&kitty->player, &kitty->snd, config.bank0, config.bank127);
}

void KittyLiveMidiInit(KittyState *kitty, KittyLiveMidiMode mode, u8* buffer, vu16* bufferSizeInfo) {
  PlayerLiveMidiInit(&kitty->player, mode, buffer, bufferSizeInfo);
}

void KittySetVSync(KittyState* kitty, u8 enabled) {
  if (enabled == 1) {
    MixerVSyncOn(&kitty->snd);
  } else {
    MixerVSyncOff(&kitty->snd);    
  }
}

void KittyVSync(KittyState* kitty) {
  MixerVSync(&kitty->snd);
}

void KittyMain(KittyState* kitty) {
  SoundMainCGB(&kitty->snd);
  PlayerMain(&kitty->player);
  MixerMain(&kitty->snd);
}

void KittyLoadSong(KittyState* kitty, u8** data, char* error) {
  PlayerOpen(&kitty->player, data, error);
}

void KittyPlaySong(KittyState* kitty) {
  PlayerPlay(&kitty->player);
}

void KittyStopSong(KittyState* kitty) {
  PlayerStop(&kitty->player);
  SoundMainCGB(&kitty->snd); // force CGB update ahead of next frame
}

void KittyAllNotesOff(KittyState* kitty) {
  PlayerAllNotesOff(&kitty->player);
}

void KittyReset(KittyState* kitty) {
  PlayerResetParams(&kitty->player);
}
