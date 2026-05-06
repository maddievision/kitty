#ifndef _SOUND_H
#define _SOUND_H
#include <stdio.h>
#include <tonc.h>

#define MAX_VCE 8
#define PCM_BF 3168

#define VOICE_STATUS_OFF 0
#define VOICE_STATUS_START 0x80
#define VOICE_STATUS_RELEASE 0x40

typedef struct {
  u16 type;
  u16 status;
  u32 freq;
  u32 loopStart;
  u32 size;
  s8 data[1];
} WaveData;

typedef struct {
  u8 status; //0
  u8 type; //1
  u8 volr; //2
  u8 voll; //3
  u8 attack; //4
  u8 decay; //5
  u8 sustain; //6
  u8 release; //7
  u8 note; //8
  u8 env; //9
  u8 volf1; //A
  u8 volf2; //B
  u8 volecho; //C
  u8 echorem; //D
  u16 res; //E-F
  u32 res2[2];  //10
  u32 smptimer; //18
  u32 finepos; //1C
  u32 freq; //20
  WaveData* wave; //24
  u8* waveptr; //28
  void* userptr; //2C
  u8 duty; //30
  u8 vel; //31
  u8 susoff; // 32
  u8 actnote; //33
  u32 priority; //34 [song] [control] [track]
  u8 cgbenv; //38
  u8 res4[3]; //39-3B
  u8 res5[3]; //3C
  u8 samp; //3F
} SoundChannel;

typedef struct {
  u32 ident;
  vu8 DmaCount;
  u8 reverb;
  u8 maxVoice;
  u8 freqMode;
  u8 res[6];
  u32 res2[16];
  SoundChannel vchn[MAX_VCE];
  s8 pcmbuf[PCM_BF*2];
  SoundChannel cgb[4];
} SoundArea;

#endif