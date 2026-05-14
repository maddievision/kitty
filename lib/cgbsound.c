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
  
  REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 3) | SDMG_BUILD_LR(SDMG_SQR2, 3) | SDMG_BUILD_LR(SDMG_WAVE, 3) | SDMG_BUILD_LR(SDMG_NOISE, 3);
  REG_SND1SWEEP = SSW_OFF;
  
  // 766789BD 
  // DCB88542
  // 22522224
  // 56678888
  REG_SND3SEL = (1 << 7) | (1 << 6);
  REG_WAVE_RAM0 = 0xBD896776;
  REG_WAVE_RAM1 = 0x4285B8DC;
  REG_WAVE_RAM2 = 0x24225222;
  REG_WAVE_RAM3 = 0x88886756;
  REG_SND3SEL = (1 << 7);
//   REG_SND3CNT = (7 << 0xD);
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
        case CGB_CHANNEL_WAVE:
        // todo vol
          REG_SND3CNT = (1 << 0xD);
          REG_SND3FREQ = SFREQ_RESET | chn->freq;
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
          break;
        case CGB_CHANNEL_WAVE:
          REG_SND3CNT = 0;
          chn->status = VOICE_STATUS_OFF;
          break;        
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
        break;
        case CGB_CHANNEL_WAVE:
            REG_SND3FREQ = chn->freq;
            chn->status = 1;
      }
    }
  }
}