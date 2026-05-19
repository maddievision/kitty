#ifndef _SOUND_H
#define _SOUND_H
#include <stdio.h>
#include <tonc.h>

#define MAX_VCE 32

// recommended absolute max voices per freq mode
// 1 (5734hz)
// 2 (7884hz)
// 3 (10512hz)
// 4 (13379hz) - 40
// 5 (15768hz)
// 6 (18157hz)
// 7 (21024hz)
// 8 (26758hz) - 18
// 9 (31536hz) - 16
// 10 (36314hz)
// 11 (40137hz)
// 12 (42048hz)

#define PCM_BUFFER_SIZE 0x630

#define VOICE_STATUS_OFF 0
#define VOICE_STATUS_START 0x80
#define VOICE_STATUS_RELEASE 0x40

// wav2agb compatible
typedef struct {
  u16 type;
  u16 status;
  u32 freq;
  u32 loopStart;
  u32 size;
  s8 data[1];
} WaveData;

// Maintains compatibility with gba-hq-mixer, but anything beyond that is
// completely custom state for our player implementation.
typedef struct {
  u8 status;            // 0
  u8 type;              // 1
  u8 volr;              // 2
  u8 voll;              // 3
  u8 attack;            // 4
  u8 decay;             // 5
  u8 sustain;           // 6
  u8 release;           // 7
  u8 note;              // 8
  u8 env;               // 9
  u8 volf1;             // A
  u8 volf2;             // B
  u8 volecho;           // C
  u8 echorem;           // D
  u16 res;              // E-F
  u32 res2[2];          // 10
  u32 smptimer;         // 18
  u32 finepos;          // 1C
  u32 freq;             // 20
  WaveData* wave;       // 24
  u8* waveptr;          // 28
  void* userptr;        // 2C
  u8 duty;              // 30
  u8 vel;               // 31
  u8 susoff;            // 32
  u8 actnote;           // 33
  u32 priority;         // 34 [song] [control] [track]
  u8 cgbenv;            // 38
  u8 amp;               // 39
  u8 res4[2];           // 3A-3B
  u8 res5[3];           // 3C
  u8 samp;              // 3F
} SoundChannel;

// Maintains compatibility with gba-hq-mixer, besides our CGB implementation.
typedef struct {
  u32 ident;            // 0
  vu8 DmaCount;         // 4
  u8 reverb;            // 5
  u8 maxVoice;          // 6
  u8 mvol;              // 7
  u8 freqMode;          // 8
  u8 res[2];            // 9-A
  u8 DmaPeriod;         // B
  u8 res2[4];           // C-F
  u32 samplesPerFrame;  // 10
  u32 freq;             // 14
  u32 div;              // 18
  u32 res3[13];         // 1C-4F
  SoundChannel vchn[MAX_VCE];  // 50
  s8 pcmbuf[PCM_BUFFER_SIZE * 2];  // 350 // 450
  SoundChannel cgb[4];             // FB0 // 10B0
} SoundArea;

#endif