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
  VFile f;
  TrackState tracks[MAX_TRACKS];
  u16 ppqn;
  u16 res1;
} PlayerState;

void PlayerInit(PlayerState* state, SoundArea* snd, SoundBank* bnk, u8** data);
void PlayerPlay(PlayerState* state);
void PlayerMain(PlayerState* state);

#endif