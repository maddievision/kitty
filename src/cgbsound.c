#include "cgbsound.h"

const s16 cgbfreqbase[12] = {
  -2004, -1891, -1785, -1685,
  -1591, -1501, -1417, -1337,
  -1262, -1192, -1125, -1062
};

u32 MidiKey2FreqCGB(u8 mk, u8 fp) {
  u8 base = mk % 12;
  u8 oct = mk / 12;
  oct = (oct > 3) ? (oct - 3) : 0;
  s32 freq = cgbfreqbase[base] >> (s32)oct;

  mk++;
  base = mk % 12;
  oct = mk / 12;
  oct = (oct > 3) ? (oct - 3) : 0;
  s32 next = cgbfreqbase[base] >> (s32)oct;
  
  u32 frac = (fp * (next - freq)) >> 8;
  return 2048 + freq + frac;
}

void SoundInitCGB(SoundArea *snd) {
  for (int i = 0; i < 4; i++) {
    snd->cgb[i].status = VOICE_STATUS_OFF;
  }
  
  // // turn sound on
  // REG_SNDSTAT = SSTAT_ENABLE;
  // snd1 on left/right ; both full volume
  REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 3) | SDMG_BUILD_LR(SDMG_SQR2, 3) | SDMG_BUILD_LR(SDMG_WAVE, 3) | SDMG_BUILD_LR(SDMG_NOISE, 3);
  // DMG ratio to 50%
  REG_SNDDSCNT &= ~SDS_DMG100; 
  REG_SNDDSCNT |= SDS_DMG50 | SDS_A100 | SDS_B100;
  // no sweep
  REG_SND1SWEEP = SSW_OFF;
  // envelope: vol=12, decay, max step time (7) ; 50% duty
  REG_SND1FREQ = 0;
}

void SoundMainCGB(SoundArea *snd) {
  for (int i = 0; i < 4; i++) {
    SoundChannel* chn = &snd->cgb[i];
    if (chn->status == VOICE_STATUS_START) {
      if (chn->volr & 1) {
        REG_SNDDMGCNT |= (0x100 << i);
      } else {
        REG_SNDDMGCNT &= ~(0x100 << i);
      }
      
      if (chn->volr & 2) {
        REG_SNDDMGCNT |= (0x1000 << i);
      } else {
        REG_SNDDMGCNT &= ~(0x1000 << i);
      }
      switch (i) {
        case CGB_CHANNEL_PULSE_A:
          REG_SND1CNT = (chn->voll << 12) | (chn->cgbenv << 8) | (chn->duty << 6);
          REG_SND1FREQ = SFREQ_RESET | chn->freq;
          chn->status = 1;
        break;
        case CGB_CHANNEL_PULSE_B:
          REG_SND2CNT = (chn->voll << 12) | (chn->cgbenv << 8) | (chn->duty << 6);
          REG_SND2FREQ = SFREQ_RESET | chn->freq;
          chn->status = 1;
        break;
        default:
          chn->status = VOICE_STATUS_OFF;
      }
    } else if (chn->status == VOICE_STATUS_RELEASE) {
      switch (i) {
        case CGB_CHANNEL_PULSE_A:
          REG_SND1CNT = 0;
          chn->status = VOICE_STATUS_OFF;
        break;
        case CGB_CHANNEL_PULSE_B:
          REG_SND2CNT = 0;
          chn->status = VOICE_STATUS_OFF;
      }
    } else if (chn->status == 0xC0) {
      // update vol/pan;
      
      // need to consolidate into one write
      if (chn->volr & 1) {
        REG_SNDDMGCNT |= (0x100 << i);
      } else {
        REG_SNDDMGCNT &= ~(0x100 << i);
      }

      if (chn->volr & 2) {
        REG_SNDDMGCNT |= (0x1000 << i);
      } else {
        REG_SNDDMGCNT &= ~(0x1000 << i);
      }
      
      chn->status = 1;      
    } else if (chn->status & 1) {
      switch (i) {
        case CGB_CHANNEL_PULSE_A:
          REG_SND1FREQ = chn->freq;
          REG_SND1CNT = (REG_SND1CNT & ~(u16)(0xC0)) | (chn->duty << 6);
          chn->status = 1;
        break;
        case CGB_CHANNEL_PULSE_B:
          REG_SND2FREQ = chn->freq;
          REG_SND2CNT = (REG_SND2CNT & ~(u16)(0xC0)) | (chn->duty << 6);
          chn->status = 1;
      }
    }
  }
}