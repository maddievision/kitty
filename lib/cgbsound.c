#include "cgbsound.h"
#include "sound.h"

const s16 cgbfreqbase[12] = {
  -2004, -1891, -1785, -1685,
  -1591, -1501, -1417, -1337,
  -1262, -1192, -1125, -1062
};

const u8 noisefreq[68] = {
  0xF7, 0xF6, 0xF5, 0xF4, 0xE7, 0xF3, 0xE5, 0xF2, 0xD7, 0xD6, 0xD5, 0xF1,
  0xC7, 0xD3, 0xC5, 0xF0, 0xB7, 0xC3, 0xB5, 0xE0, 0xA7, 0xB3, 0xA5, 0xD0,
  0x97, 0xA3, 0x95, 0xC0, 0x87, 0x93, 0x85, 0xB0, 0x77, 0x83, 0x75, 0xA0,
  0x67, 0x73, 0x65, 0x90, 0x57, 0x63, 0x55, 0x80, 0x47, 0x53, 0x45, 0x70,
  0x37, 0x43, 0x35, 0x60, 0x27, 0x33, 0x25, 0x50, 0x17, 0x23, 0x15, 0x40,
  0x07, 0x13, 0x05, 0x30, 0x03, 0x20, 0x10, 0x00  
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

u8 MidiKey2FreqCGBNoise(u8 mk) {
  return noisefreq[mk <= 67 ? mk : 67];
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
  // REG_WAVE_RAM0 = 0xBD896776;
  // REG_WAVE_RAM1 = 0x4285B8DC;
  // REG_WAVE_RAM2 = 0x24225222;
  // REG_WAVE_RAM3 = 0x88886756;

  REG_WAVE_RAM0 = 0x67452301;
  REG_WAVE_RAM1 = 0xEFCDAB89;
  REG_WAVE_RAM2 = 0x98BADCFE;
  REG_WAVE_RAM3 = 0x10325476;


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
        case CGB_CHANNEL_PULSE_A: {        
          u8 duty = chn->duty >> 5;
          REG_SND1CNT = (chn->voll << 12) | (chn->cgbenv << 8) | (duty << 6);
          REG_SND1FREQ = SFREQ_RESET | chn->freq;
          chn->status = 1;
        }
        break;
        case CGB_CHANNEL_PULSE_B: {
          u8 duty = chn->duty >> 5;
          REG_SND2CNT = (chn->voll << 12) | (chn->cgbenv << 8) | (duty << 6);
          REG_SND2FREQ = SFREQ_RESET | chn->freq;
          chn->status = 1;
        }
        break;
        case CGB_CHANNEL_WAVE:
        // todo vol
          REG_SND3CNT = (1 << 0xD);
          REG_SND3FREQ = SFREQ_RESET | chn->freq;
          chn->status = 1;
        break;
        case CGB_CHANNEL_NOISE: {
          u8 duty = chn->duty >> 7;
          REG_SND4CNT = (chn->voll << 12) | (chn->cgbenv << 8);
          REG_SND4FREQ = SFREQ_RESET | chn->freq | (duty << 3);
          chn->status = 1;
        }
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
        case CGB_CHANNEL_NOISE:
          REG_SND4CNT = 0;
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
        case CGB_CHANNEL_PULSE_A: {
          if (chn->lfreq != chn->freq) {
            chn->lfreq = chn->freq;
            REG_SND1FREQ = chn->freq;
          }
          u8 duty = chn->duty >> 5;
          REG_SND1CNT = (REG_SND1CNT & ~(u16)(0xC0)) | (duty << 6);
        }
        break;
        case CGB_CHANNEL_PULSE_B: {
          if (chn->lfreq != chn->freq) {
            chn->lfreq = chn->freq;
            REG_SND2FREQ = chn->freq;
          }
          u8 duty = chn->duty >> 5;
          REG_SND2CNT = (REG_SND2CNT & ~(u16)(0xC0)) | (duty << 6);
        }
        break;
        case CGB_CHANNEL_WAVE:
          if (chn->lfreq != chn->freq) {
            chn->lfreq = chn->freq;
            REG_SND3FREQ = chn->freq;
          }
        break;
        case CGB_CHANNEL_NOISE:
          if (chn->lfreq != chn->freq) {
            u8 duty = chn->duty >> 7;
            chn->lfreq = chn->freq;
            REG_SND4FREQ = chn->freq | (duty << 3);
          }
        break;
      }
      chn->status = 1;
    }
  }
}