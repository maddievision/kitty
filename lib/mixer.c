#include "mixer.h"
#include <tonc.h>

// "Rose"
#define MixerID 0x65736F52
#define MixerID1 0x65736F53 
// "MOff"
#define MixerIDOff 0x66664F4D

#define SoundMainRAMDest 0x3005200

extern void* SoundMainRAM;	
extern void* SoundMainRAM_end;	

const u32 freqs[12] = {
  5734,  7884,  10512,  13379,  15768,  18157,  21024,  26758,  31536,  36314,  40137,  42048
};

const u32 frameSizes[12] = {
  96, 132, 176, 224, 264, 304, 352, 448, 528, 608, 672, 704
};

const u32 divs[12] = {
  1463, 1064, 798, 627, 532, 462, 399, 313, 266, 231, 209, 200
};

const u32 timerVals[12] = {
  0xF492, 0xF7B0, 0xF9C4, 0xFB1A, 0xFBD8, 0xFC64, 0xFCE2, 0xFD8D, 0xFDEC, 0xFE32, 0xFE5E, 0xFE71
};

void SetupSoundMainRAM() {
  memcpy32((void*) SoundMainRAMDest, (void*) &SoundMainRAM, (u32*)&SoundMainRAM_end - (u32*)&SoundMainRAM);
}

void MixerInit(SoundArea *snd, u8 voices, u8 mvol, u8 freqMode, u8 reverb) {
  toncset(snd, 0, sizeof(snd));
  SetupSoundMainRAM();

  if (REG_DMA1CNT & DMA_REPEAT) {
      REG_DMA1CNT = DMA_ENABLE | DMA_32 | DMA_SRC_INC | DMA_DST_FIXED | DMA_AT_NOW;
  }
  if (REG_DMA2CNT & DMA_REPEAT) {
      REG_DMA2CNT = DMA_ENABLE | DMA_32 | DMA_SRC_INC | DMA_DST_FIXED | DMA_AT_NOW;
  }

  REG_DMA1CNT = DMA_32;
  REG_DMA2CNT = DMA_32;
  REG_SOUNDCNT_X = SSTAT_ENABLE | SSTAT_SQR1 | SSTAT_SQR2 | SSTAT_WAVE | SSTAT_NOISE;
  REG_SOUNDCNT_H = SDS_ARESET | SDS_BRESET | SDS_ATMR0 | SDS_BTMR0 | SDS_BL | SDS_AR | SDS_A100 | SDS_B100 | SDS_DMG100;
  REG_SOUNDBIAS = (REG_SOUNDBIAS & 0x3FFF) | 0x8000;
  REG_DMA1SAD = (u32)snd->pcmbuf;
  REG_DMA1DAD = (u32)&REG_FIFO_A;
  REG_DMA2SAD = (u32)snd->pcmbuf + PCM_BUFFER_SIZE;
  REG_DMA2DAD = (u32)&REG_FIFO_B;
  
  snd->maxVoice = voices;
  snd->reverb = reverb;
  snd->mvol = mvol;
  snd->freqMode = freqMode;

  u8 idx = freqMode - 1;
  snd->samplesPerFrame = frameSizes[idx];
  snd->DmaPeriod = PCM_BUFFER_SIZE / snd->samplesPerFrame;
  snd->freq = freqs[idx];
  snd->div = divs[idx];

  REG_TM0CNT = 0;
  REG_TM0D = timerVals[idx];
  REG_TM0CNT = TM_ENABLE;

  snd->ident = MixerIDOff;
  snd->DmaCount = 1;
  // Starts with VSync disabled.
}

void MixerVSyncOn(SoundArea *snd) {
  if (snd->ident != MixerIDOff) {
    return;
  }
  
  REG_DMA1CNT = DMA_ENABLE | DMA_32 | DMA_AT_SPECIAL | DMA_REPEAT;
  REG_DMA2CNT = DMA_ENABLE | DMA_32 | DMA_AT_SPECIAL | DMA_REPEAT;
  snd->DmaCount = 1;
  snd->ident = MixerID;
}

void MixerVSyncOff(SoundArea *snd) {
  if (snd->ident == MixerID || snd->ident == MixerID1) {
    snd->ident = MixerIDOff;

    if (REG_DMA1CNT & DMA_REPEAT) {
        REG_DMA1CNT = DMA_ENABLE | DMA_32 | DMA_SRC_INC | DMA_DST_FIXED | DMA_AT_NOW;
    }
    
    if (REG_DMA2CNT & DMA_REPEAT) {
        REG_DMA2CNT = DMA_ENABLE | DMA_32 | DMA_SRC_INC | DMA_DST_FIXED | DMA_AT_NOW;
    }
    
    REG_DMA1CNT = DMA_32;
    REG_DMA2CNT = DMA_32;
    
    // Clear PCM buffer
    toncset(snd->pcmbuf, 0, sizeof(snd->pcmbuf));
  }
}

void MixerVSync(SoundArea *snd) {
  if (snd->ident != MixerID && snd->ident != MixerID1) {
    return;
  }
  
  snd->DmaCount--;
  if (snd->DmaCount > 0) {
    return;
  }

  snd->DmaCount = snd->DmaPeriod;
  
  if (REG_DMA1CNT & DMA_REPEAT) {
    REG_DMA1CNT = DMA_ENABLE | DMA_32 | DMA_SRC_INC | DMA_DST_FIXED | DMA_AT_NOW;
  }
  if (REG_DMA2CNT & DMA_REPEAT) {
    REG_DMA2CNT = DMA_ENABLE | DMA_32 | DMA_SRC_INC | DMA_DST_FIXED | DMA_AT_NOW;
  }
  
  REG_DMA1CNT = DMA_32;
  REG_DMA2CNT = DMA_32;
  REG_DMA1CNT = DMA_ENABLE | DMA_32 | DMA_AT_SPECIAL | DMA_REPEAT;
  REG_DMA2CNT = DMA_ENABLE | DMA_32 | DMA_AT_SPECIAL | DMA_REPEAT;
}
