#include "kitty.h"
#include "cgbsound.h"
#include "mixer.h"

void KittyInit(KittyState* kitty, SoundBank* bank0, SoundBank* bank127, u8 voices, u8 mvol, u8 freqMode, u8 reverb, u8 livemidi) {
  MixerInit(&kitty->snd, voices, mvol, freqMode, reverb);
  SoundInitCGB(&kitty->snd);
  PlayerInit(&kitty->player, &kitty->snd, bank0, bank127, livemidi);
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

void KittyReset(KittyState* kitty) {
  PlayerResetParams(&kitty->player);
}
