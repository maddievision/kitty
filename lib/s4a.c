#include "s4a.h"
#include "cgbsound.h"
#include "mixer.h"

void s4aInit(SappyState* sappy, SoundBank* bank0, SoundBank* bank127, u8 voices, u8 mvol, u8 freqMode, u8 reverb, u8 livemidi) {
  MixerInit(&sappy->snd, voices, mvol, freqMode, reverb);
  SoundInitCGB(&sappy->snd);
  PlayerInit(&sappy->player, &sappy->snd, bank0, bank127, livemidi);
}

void s4aSetVSync(SappyState* sappy, u8 enabled) {
  if (enabled == 1) {
    MixerVSyncOn(&sappy->snd);
  } else {
    MixerVSyncOff(&sappy->snd);    
  }
}

void s4aVSync(SappyState* sappy) {
  MixerVSync(&sappy->snd);
}

void s4aMain(SappyState* sappy) {
  SoundMainCGB(&sappy->snd);
  PlayerMain(&sappy->player);
  MixerMain(&sappy->snd);
}


void s4aLoadSong(SappyState* sappy, u8** data) {
  PlayerOpen(&sappy->player, data);
}

void s4aPlaySong(SappyState* sappy) {
  PlayerPlay(&sappy->player);
}

void s4aStopSong(SappyState* sappy) {
  PlayerStop(&sappy->player);
}
