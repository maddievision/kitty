#ifndef _MIDI_H
#define _MIDI_H
#include <stdio.h>
#include <tonc.h>
#include "debug.h"
#include "sound.h"
#include "bank.h"

#define TRACK_STATUS_INACTIVE 0
#define TRACK_STATUS_ACTIVE 1
#define PLAYER_STATUS_INACTIVE 0
#define PLAYER_STATUS_READY 1
#define PLAYER_STATUS_ACTIVE 2
#define NOTE_ACTIVE_BIT 0x80

#define MAX_TRACKS 32
#define MAX_INST 128

#define FRAME_MS 16

#define MVOL_DEFAULT 0xFF
#define VOL_BITS 10

typedef struct {
  void *ptr;
} VFile;

typedef struct {
  u8 status;
  u8 note;
  u8 run;
  u8 priority;

  SoundEntry *inst;
  u32 wait;
  VFile f;
  u8* loopptr;
  u32 loopwait;
  u32 res1;
  u32 res2;  

  u8 pan;
  u8 exp;
  u8 vol;
  u8 pbr;
  
  u8 datalo;
  u8 datahi;
  u8 rpnlo;
  u8 rpnhi;
  
  u8 sus;
  u8 pb;
  u8 voll;
  u8 volr;
  
  u16 wheel;
  s8 pbsemi;
  u8 pbfp;
  
  u8 id;
  u8 chan;
  u8 output;
  u8 duty;
  
  u8 linvol;
  u8 cgbenv;
  u8 res3[2];
  
  void* startptr;

} TrackState;

typedef struct {
  u8 status;
  u8 priority;
  u16 trackcount;

  u32 t;
  u32 ms;
  u32 nextMs;
  u32 loopstart;
  u32 loopstartms;
  u32 loopend;
  u32 mspt;
  u32 tempo;
  SoundArea* snd;
  SoundBank* bnk;
  SoundBank* dbnk;
  VFile f;

  TrackState tracks[MAX_TRACKS];

  u16 ppqn;
  u8 mvol;
  u8 res;
} PlayerState;

void PlayerInit(PlayerState* state, SoundArea* snd, SoundBank* bnk, SoundBank* dbnk);
void PlayerPlay(PlayerState* state);
void PlayerStop(PlayerState* state);
void PlayerMain(PlayerState* state);
void PlayerOpen(PlayerState* state, u8** data);

#endif