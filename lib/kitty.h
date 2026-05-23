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

//MAX_VCE, 11, 9, 10
void KittyInit(KittyState* kitty, SoundBank* bank0, SoundBank* bank127, u8 voices, u8 mvol, u8 freqMode, u8 reverb, u8 livemidi);
void KittySetVSync(KittyState* kitty, u8 enabled);
void KittyVSync(KittyState* kitty);
void KittyMain(KittyState* kitty);
void KittyLoadSong(KittyState* kitty, u8** data, char* error);
void KittyPlaySong(KittyState* kitty);
void KittyStopSong(KittyState* kitty);
void KittyReset(KittyState* kitty);

#endif