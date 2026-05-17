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

#define MVOL_DEFAULT 0xFF
#define VOL_BITS 10

#define COUNTER_SHIFT 3
#define FRAME_INTERVAL (16666 >> COUNTER_SHIFT)

#define MIDI_IN_BUF 0x3007000

typedef struct {
  void *ptr;
} VFile;

typedef struct {
  u8 status;                //0
  u8 note;                  //1
  u8 run;                   //2
  u8 priority;              //3

  SoundEntry *inst;         //4
  u32 wait;                 //8
  VFile f;                  //C
  u8* loopptr;              //10
  u32 loopwait;             //14

  u8 pan;                   //18
  u8 exp;                   //19
  u8 vol;                   //1A
  u8 pbr;                   //1B
  
  u8 datalo;                //1C
  u8 datahi;                //1D
  u8 rpnlo;                 //1E
  u8 rpnhi;                 //1F
  
  u8 sus;                   //20
  u8 pb;                    //21
  u8 voll;                  //22
  u8 volr;                  //23
  
  u16 wheel;                //24
  s8 pbsemi;                //26
  u8 pbfp;                  //27
  
  u8 id;                    //28
  u8 chan;                  //29
  u8 output;                //2A
  u8 duty;                  //2B
  
  u8 linvol;                //2C
  u8 cgbenv;                //2D
  u8 mod;                   //2E
  u8 lfospd;                //2F

  u8 lfophs;                //30
  u8 lfodep;                //31
  u8 lfoamt;                //32
  u8 res;                   //33

  s16 lfo;                  //34
  u8 bankmsb;               //36
  u8 banklsb;               //37
  
  u8 program;               //38
  u8 res2[3];               //39-3B
  
  void* startptr;           //3C

} TrackState;               //40

typedef struct {
  u8 status;                //0
  u8 priority;              //1
  u16 trackcount;           //2

  u32 t;                    //4
  u32 framecount;           //8
  u32 nextcount;            //C
  u32 loopstart;            //10
  u32 loopstartcount;       //14
  u32 loopend;              //18
  u32 tickinterval;         //1C
  u32 tempo;                //20
  SoundArea* snd;           //24
  SoundBank* bnk;           //28
  SoundBank* dbnk;          //2C
  VFile f;                  //30

  TrackState tracks[MAX_TRACKS];    //34 - 1033

  u16 ppqn;                 //834
  u16 trackmode;            //836

  u8 mvol;                  //838
  u8 livemidi;              //839
  u8 smftype;               //83A
  u8 res;                   //83B
  
  VFile midiinbuf;                //83C
  TrackState midiintracks[16];    //840
} PlayerState; //1054

void PlayerInit(PlayerState* state, SoundArea* snd, SoundBank* bnk, SoundBank* dbnk, u8 livemidi);
void PlayerPlay(PlayerState* state);
void PlayerStop(PlayerState* state);
void PlayerResetParams(PlayerState* state);
void PlayerMain(PlayerState* state);
void PlayerOpen(PlayerState* state, u8** data, char* error);

#endif