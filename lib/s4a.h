#ifndef _S4A_H
#define _S4A_H

#include <tonc.h>
#include "sound.h"
#include "bank.h"
#include "player.h"

typedef struct {
  SoundArea snd;
  PlayerState player;
} SappyState;

//MAX_VCE, 11, 9, 10
void s4aInit(SappyState* sappy, SoundBank* bank0, SoundBank* bank127, u8 voices, u8 mvol, u8 freqMode, u8 reverb, u8 livemidi);
void s4aSetVSync(SappyState* sappy, u8 enabled);
void s4aVSync(SappyState* sappy);
void s4aMain(SappyState* sappy);
void s4aLoadSong(SappyState* sappy, u8** data, char* error);
void s4aPlaySong(SappyState* sappy);
void s4aStopSong(SappyState* sappy);
void s4aReset(SappyState* sappy);

#endif