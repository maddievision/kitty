#include "midi.h"


void PlayNote(SoundArea *snd, WaveData *inst, u8 channel, u8 note) {
    u32 freq = MidiKey2Freq(inst, note, 0);
    SoundChannel *chn = &snd->vchn[channel];
    chn->status = 0;
    chn->type = 0;
    chn->volr = 0x40;
    chn->voll = 0x40;
    chn->attack = 0xFF;
    chn->decay = 0xFF;
    chn->sustain = 0xFF;
    chn->release = 0x10;
    chn->volecho = 0;
    chn->echorem = 0;
    chn->freq = freq;
    chn->wave = inst;
    chn->status = 0x80;		
}

void NoteOff(SoundArea *snd, u8 channel) {
    SoundChannel *chn = &snd->vchn[channel];
    chn->status |= 0x40;

}

void printaddr(VFile *f) {
  static char str[32];
  siprintf(str, "Addr: %08X", f->ptr);
  dputs(str);  
}

u32 ReadHead(VFile *f) {
  // printaddr(f);
  // u32 v = *((u32*) f->ptr);
  // can't read 32 outside of 32 boundary
  u32 v = *((u8*) f->ptr++);
  v |= *((u8*) f->ptr++) << 8;
  v |= *((u8*) f->ptr++) << 16;
  v |= *((u8*) f->ptr++) << 24;
  return v;
}

u32 ReadBEU32(VFile *f) {
//   printaddr(f);
//   u32 v = *((u32*) f->ptr);
//   f->ptr += 4;
//   return (v >> 24) | ((v & 0xFF0000) >> 8) | ((v & 0xFF00) << 8) | ((v & 0xFF) << 24);
  // can't read 32 outside of 32 boundary

  u32 v = *((u8*) f->ptr++) << 24;
  v |= *((u8*) f->ptr++) << 16;
  v |= *((u8*) f->ptr++) << 8;
  v |= *((u8*) f->ptr++);
  
  return v;
}

u16 ReadBEU16(VFile *f) {
  // printaddr(f);
  u16 v = *((u8*) f->ptr++) << 8;
  v |= *((u8*) f->ptr++);
  return v;
//   u16 v = *((u16*) f->ptr);
//   f->ptr += 2;
//   return (v >> 8) | ((v & 0xFF) << 8);
}

u32 ReadVLQ(VFile* f) {
  // printaddr(f);
  u8 r = *((u8*) f->ptr++);
  u32 v = r & 0x7F;
  u8 c = 0;  
  while (r & 0x80) {
    if (c > 3) {
      dputs("INVALID VLQ EXCEEDS 4 BYTES");
      return 0;
    }
    v <<= 8;
    r = *((u8*) f->ptr++);
    v &= r & 0x7F;
    c++;
  }
  return v;
}

inline u8 ReadU8(VFile *f) {
//  printaddr(f);
  return *((u8*) f->ptr++);
}

#define MTHD 0x6468544D
#define MTRK 0x6B72544D

void PlayerInit(PlayerState* p, SoundArea* snd, SoundBank* bnk, u8** data) {  
  char str[32];
  p->status = PLAYER_STATUS_INACTIVE;
  p->snd = snd;
  p->bnk = bnk;
  p->f.ptr = data;
  VFile *f = &p->f;
  u32 r;
  r = ReadHead(f);
  if (r != MTHD) {
    dputs("MThd not found");
    return;
  }
  r = ReadBEU32(f);
  if (r != 6) {
    siprintf(str, "Got: %d", r);
    dputs(str);
    return;
  }
  r = ReadBEU16(f);
  siprintf(str, "Type %d", r);
  dputs(str);
  
  if (r != 1) {
    dputs("Unsupported");
    return;
  }
  
  u16 trackCount = ReadBEU16(f);
  siprintf(str, "Tracks: %d", trackCount);
  dputs(str);

  if (trackCount > MAX_TRACKS) {
    dputs("Exceeds 32 max");
  }  
  p->trackcount = trackCount;
  
  u16 ppqn = ReadBEU16(f);
  siprintf(str, "PPQN: %d", ppqn);
  dputs(str);
  
  p->ppqn = ppqn;

  // scan tracks and set starting pointers
  for (u8 i = 0; i < trackCount; i++) {
    TrackState* trk = &p->tracks[i];
    trk->wait = 0;
    trk->run = 0;
    trk->status = TRACK_STATUS_ACTIVE;
    siprintf(str, "Track %d", i + 1);
    r = ReadHead(f);
    if (r != MTRK) {
      dputs("Expected MTrk");
      siprintf(str, "Got: %04X", r);
      dputs(str);
      return;
    }  
    
    r = ReadBEU32(f);

    dputs(str);
    u8 events = 0;
    u8 run = 0;
    u8 first = 1;
    while (1) {
      events++;
      u32 dt = ReadVLQ(f);
      if (first) {
        trk->f.ptr = f->ptr;
        trk->wait = dt;
        first = 0;
      }
    //   siprintf(str, "dt: %d", dt);
    //   dputs(str);
    
      u8 status = ReadU8(f);
      if (status == 0xFF) {
        u8 meta = ReadU8(f);
        // siprintf(str, "Meta event %02X", meta);
        // dputs(str);
        u32 len = ReadVLQ(f);
        f->ptr += len;
        if (meta == 0x2F) {
          break;
        }
      } else if (status == 0xF0 || status == 0xF7) {
        dputs("Sysex unsupported");
      } else if (status >= 0xF0) {
        siprintf(str, "Unknown status byte: %02X", status);
        dputs(str);
        return;
      } else {
        u8 b1 = 0;
        u8 b2 = 0;
        u8 chan = 0;
        u8 s = 0;
        if ((status & 0x80) == 0) {
          if (run == 0) {
            dputs("Expected running status");
            return;
          }
          b1 = status;
          status = run;
          s = status >> 4;
          chan = status & 0xF;
        } else {
          s = status >> 4;
          chan = status & 0xF;
          b1 = ReadU8(f);
        }
        run = status;
    
        if (s == 0xC || s == 0xD) {
        //   siprintf(str, "%02X %02X", status, b1);
        //   dputs(str);          
        } else {
          b2 = ReadU8(f);
        //   siprintf(str, "%02X %02X %02X", status, b1, b2);
        //   dputs(str);
        }
      }
    }
    siprintf(str, "Pos: %08X", trk->f.ptr);
    dputs(str);
    siprintf(str, "Wait: %d", trk->wait);
    dputs(str);
    siprintf(str, "Events: %d", events);
    dputs(str);

  }
  
  

  p->status = PLAYER_STATUS_READY;
}

void PlayerPlay(PlayerState* p) {
  if (p->status != PLAYER_STATUS_READY) {
    return;
  }
  p->status = PLAYER_STATUS_ACTIVE;
}

void PlayerMain(PlayerState* p) {
  char str[32];
  if (p->status != PLAYER_STATUS_ACTIVE) {
    return;
  }
  u8 activeCount = 0;
  for (u8 i = 0; i < p->trackcount; i++) {
    TrackState* trk = &p->tracks[i];
    if (trk->status != TRACK_STATUS_ACTIVE) {
      continue;
    }
    activeCount++;
    if (trk->wait > 0) {
      trk->wait--;      
    }
    VFile* f = &trk->f;
    while (trk->wait == 0) {
      u8 status = ReadU8(f);
      if (status == 0xFF) {
        u8 meta = ReadU8(f);
        // siprintf(str, "Meta event %02X", meta);
        // dputs(str);
        u32 len = ReadVLQ(f);
        f->ptr += len;
        if (meta == 0x2F) {
          trk->status = TRACK_STATUS_INACTIVE;
          break;
        }
      } else if (status == 0xF0 || status == 0xF7) {
        dputs("Sysex unsupported");
      } else if (status >= 0xF0) {
        siprintf(str, "Unknown status byte: %02X", status);
        dputs(str);
        return;
      } else {
        u8 b1 = 0;
        u8 b2 = 0;
        u8 chan = 0;
        u8 s = 0;
        if ((status & 0x80) == 0) {
          if (trk->run == 0) {
            dputs("Expected running status");
            p->status = PLAYER_STATUS_INACTIVE;
            return;
          }
          b1 = status;
          status = trk->run;
          s = status >> 4;
          chan = status & 0xF;
        } else {
          s = status >> 4;
          chan = status & 0xF;
          b1 = ReadU8(f);
        }
        trk->run = status;
    
        if (s == 0xC || s == 0xD) {
          //siprintf(str, "%02X %02X", status, b1);
          //dputs(str);          
        } else {
          b2 = ReadU8(f);
          //siprintf(str, "%02X %02X %02X", status, b1, b2);
        //  dputs(str);
        }
        
        switch (s) {
          case 0xC:
            trk->inst = p->bnk->insts[b1];
            break;
          case 0x9:
            if (b2 == 0) {
              NoteOff(p->snd, i);
            } else {
              PlayNote(p->snd, trk->inst, i, b1);
            }
        }
      }
      
      u32 dt = ReadVLQ(f);
      trk->wait = dt;
    }
  }
  
  if (activeCount == 0) {
    p->status = PLAYER_STATUS_INACTIVE;
  }
}
