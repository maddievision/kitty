#include "midi.h"
#include "cgbsound.h"
#include "shapes.h"

void KillAllNotes(PlayerState *p) {
  for (int i = 0; i < 4; i++) {
    SoundChannel *chn = &p->snd->cgb[i];
    if (chn->status) {
      chn->status = VOICE_STATUS_RELEASE;
      chn->userptr = 0;
    }
  }
  for (int i = 0; i < MAX_VCE; i++) {
    SoundChannel *chn = &p->snd->vchn[i];
    if (chn->status) {
      chn->status = VOICE_STATUS_RELEASE;
      chn->userptr = 0;
    }
  }
}

void PlayNote(PlayerState *p, TrackState *trk, u8 note, u8 vel) {
    u8 actnote;
    SoundArea* snd = p->snd;
    SoundEntry* inst = trk->inst;
    
    u32 priority = (((u32)trk->priority) << 8) | (u32)(0x40 - trk->id);
    
    if (trk->output > 0) {
      int i = trk->output - 1;
      SoundChannel *chn = &snd->cgb[i];
      
      if (chn->status && priority < chn->priority) {
        return;
      }

      chn->status = VOICE_STATUS_OFF;
      chn->voll = ((u16)trk->linvol * (vel << 1)) >> 12;
      chn->volr = trk->pan < 0x20 ? 1 : trk->pan > 0x60 ? 2 : 3;
      chn->vel = vel;
      chn->attack = 0;
      chn->decay = 0;
      chn->sustain = 0;
      chn->release = 0;
      chn->note = note;
      chn->actnote = note;
      chn->volecho = 0;
      chn->echorem = 0;
      chn->duty = trk->duty;
      chn->cgbenv = trk->cgbenv;

      u8 freqnote = (s8)note + trk->pbsemi;
      switch (i) {
        case 3:
        break;
        case 2:
          freqnote += 12;
        default:
          chn->freq = MidiKey2FreqCGB(freqnote, trk->pbfp);
      }
      chn->wave = 0;
      chn->userptr = trk;
      chn->status = VOICE_STATUS_START;
      chn->priority = priority;
      chn->susoff = 0;
      return;
    }
    
    if (trk->chan == 9) { //drum channel override
      inst = &p->dbnk->entries[note];
      actnote = inst->rootnote;
    } else {        
      s8 trans = (s8)60 - (s8)inst->rootnote;
      actnote = note + trans;
    }
    
    if (inst->type == SOUND_ENTRY_TYPE_DISABLED) {
      return;
    }
    
    if (inst->type == SOUND_ENTRY_TYPE_MULTI) {
      SoundEntryMulti* m = (SoundEntryMulti*)((void*) inst);
      SoundBank *grp = m->group;
      SoundMap *map = m->map;
      u8 idx = map->entries[note];
      if (idx == 0xFF) {
        return;
      }
      
      inst = &grp->entries[idx];      
      s8 trans = (s8)60 - (s8)inst->rootnote;
      actnote = note + trans;
    }

    u8 freqnote = (s8)actnote + trk->pbsemi;
    u32 freq = MidiKey2Freq(inst->sample, freqnote, trk->pbfp);
    u8 free = 0xFF;

    // find same note
    for (int i = 0; i < snd->maxVoice; i++) {
      SoundChannel *chn = &snd->vchn[i];
      if (chn->userptr == trk && chn->note == note) {
        free = i;
        break;
      }
    }
    
    // find inactive notes
    if (free == 0xFF) {
      for (int i = 0; i < snd->maxVoice; i++) {
        SoundChannel *chn = &snd->vchn[i];
        if (chn->status == VOICE_STATUS_OFF || chn->userptr == 0) {
          free = i;
          break;
        }
      }
    }
    // 
    // // find released notes
    // if (free == 0xFF) {
    //   for (int i = 0; i < snd->maxVoice; i++) {
    //     SoundChannel *chn = &snd->vchn[i];
    //     if (chn->userptr == 0) { //|| chn->status & VOICE_STATUS_RELEASE) {
    //       free = i;
    //       break;
    //     }
    //   }      
    // }

    // otherwise steal lower priority
    if (free == 0xFF) {
      for (int i = 0; i < snd->maxVoice; i++) {
        SoundChannel *chn = &snd->vchn[i];
        if (priority >= chn->priority) {
          free = i;
          break;
        }
      }      
    }
    
    // none found :(
    if (free == 0xFF) {
      return;
    }
    
    u16 avel = sq7lut[vel];
    
    SoundChannel *chn = &snd->vchn[free];
    chn->status = VOICE_STATUS_OFF;
    chn->type = 0;
    chn->amp = inst->amp;
    u16 amp = chn->amp * 2;
    avel = (avel * amp) >> 8;
    chn->volr = ((u16)trk->volr * avel) >> VOL_BITS;
    chn->voll = ((u16)trk->voll * avel) >> VOL_BITS;
    chn->vel = vel;
    chn->attack = inst->attack;
    chn->decay = inst->decay;
    chn->sustain = inst->sustain;
    chn->release = inst->release;
    chn->note = note;
    chn->actnote = actnote;
    chn->volecho = 0;
    chn->echorem = 0;
    chn->freq = freq;
    chn->wave = inst->sample;
    chn->userptr = trk;
    chn->status = VOICE_STATUS_START;
    chn->priority = priority;
    chn->susoff = 0;
}

void NoteOff(SoundArea *snd, TrackState *trk, u8 note) {
  if (trk->output > 0) {
    int i = trk->output - 1;
    SoundChannel *chn = &snd->cgb[i];
    if (chn->userptr == trk) {
      if (trk->sus == 127) {
        chn->susoff = 1;
      } else {
        chn->status = VOICE_STATUS_RELEASE;
        chn->userptr = 0;
      }
    }
    return;
  }
  for (int i = 0; i < snd->maxVoice; i++) {
    SoundChannel *chn = &snd->vchn[i];
    if (chn->userptr == trk && chn->note == note) {
      if (trk->sus == 127) {
        chn->susoff = 1;
      } else {
        chn->status = VOICE_STATUS_RELEASE;
        chn->userptr = 0;
      }
      break;
    }
  }
}

void TrackSusOff(SoundArea *snd, TrackState *trk) {
  for (int i = 0; i < 4; i++) {
    SoundChannel *chn = &snd->cgb[i];
    if (chn->userptr == trk && chn->susoff == 1) {
      chn->status = VOICE_STATUS_RELEASE;
      chn->userptr = 0;
    }
  }

  for (int i = 0; i < snd->maxVoice; i++) {
    SoundChannel *chn = &snd->vchn[i];
    if (chn->userptr == trk && chn->susoff == 1) {
      chn->status = VOICE_STATUS_RELEASE;
      chn->userptr = 0;
      break;
    }
  }
}

void TrackUpdateVol(PlayerState *p, TrackState* trk) {
  SoundArea *snd = p->snd;
  u16 vol = ((u16)sq7lut[trk->vol] * (u16)(trk->exp << 1)) >> 8;
  vol = (vol * (u16)p->mvol) >> 8;
  trk->linvol = ((u16)(trk->vol << 1) * (u16)(trk->exp << 1)) >> 8;
  u16 panl = trk->pan <= 0x40 ? 0xFF : ((0x7F - trk->pan) << 2); //sq7lut[(0x7F - trk->pan) << 1];
  u16 panr = trk->pan >= 0x40 ? 0xFF : (trk->pan << 2);//sq7lut[trk->pan << 1];
  trk->voll = (panl * vol) >> 8;
  trk->volr = (panr * vol) >> 8;

  for (int i = 0; i < 4; i++) {
    SoundChannel *chn = &snd->cgb[i];
    if (chn->userptr == trk) {
      chn->voll = ((u16)trk->linvol * (chn->vel << 1)) >> 12;
      chn->volr = trk->pan < 0x20 ? 1 : trk->pan > 0x60 ? 2 : 3;
      chn->status = 0xC0;
    }
  }
  
  for (int i = 0; i < snd->maxVoice; i++) {
    SoundChannel *chn = &snd->vchn[i];
    if (chn->userptr == trk) {
      u16 avel = sq7lut[chn->vel];
      u16 amp = chn->amp * 2;
      avel = (avel * amp) >> 8;
      chn->voll = ((u16)trk->voll * avel) >> VOL_BITS;
      chn->volr = ((u16)trk->volr * avel) >> VOL_BITS;
    }
  }
}

void TrackUpdateDuty(SoundArea *snd, TrackState* trk) {
  for (int i = 0; i < 4; i++) {
    SoundChannel *chn = &snd->cgb[i];
    if (chn->userptr == trk) {
      chn->duty = trk->duty;
    }
  }
}

void TrackUpdatePitch(SoundArea *snd, TrackState* trk) {
  trk->pb = trk->wheel >> 6;
  u16 ext = (u16)trk->pb * ((u16)trk->pbr << 1);
  
  ext += trk->lfo;
  
  trk->pbsemi = (s8)(ext >> 8) - (s8)(trk->pbr);
  trk->pbfp = ext & 0xFF;


  for (int i = 0; i < 4; i++) {
    SoundChannel *chn = &snd->cgb[i];
    if (chn->userptr == trk) {
      switch (i) {
        case 3:
          break;
        default:
          u8 freqnote = (s8)chn->actnote + trk->pbsemi;
          chn->freq = MidiKey2FreqCGB(freqnote, trk->pbfp);
      }
    }
  }
  
  for (int i = 0; i < snd->maxVoice; i++) {
    SoundChannel *chn = &snd->vchn[i];
    if (chn->userptr == trk) {
      u8 freqnote = (s8)chn->actnote + trk->pbsemi;
      chn->freq = MidiKey2Freq(chn->wave, freqnote, trk->pbfp);
    }
  }
}

void ResetTrackParams(PlayerState *p, TrackState* trk) {
  trk->vol = 0x7F;
  trk->pan = 0x40;
  trk->exp = 0x7F;
  trk->pbr = 2;
  
  trk->datalo = 0;
  trk->datahi = 0;
  trk->rpnlo = 0;
  trk->rpnhi = 0;
  
  trk->voll = 0xFF;
  trk->volr = 0xFF;
  trk->linvol = 0xFC;
  trk->pb = 0x80;
  trk->sus = 0;
  trk->priority = 0;
  
  trk->wheel = 0x2000;
  trk->pbsemi = 0;
  trk->pbfp = 0;
  
  trk->duty = 2;
  trk->cgbenv = 0;
  trk->output = 0;
  
  trk->mod = 0;
  trk->lfospd = 0x20;
  trk->lfophs = 0;
  trk->lfodep = 0xC0;
  trk->lfo = 0;
  trk->lfoamt = 0;
    
  
  trk->inst = &p->bnk->entries[0];
}

void printaddr(VFile *f) {
  static char str[32];
  siprintf(str, "Addr: %p", f->ptr);
  dputs(str);  
}

void ReadStr(VFile *f, char *str, u32 size) {
  for (int i = 0; i < size; i++) {
    str[i] = *((u8*) f->ptr++);
  }
  str[size] = '\0';
}

u32 ReadU32(VFile *f) {
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

u32 ReadBEU24(VFile *f) {
  u32 v = *((u8*) f->ptr++) <<16;
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
    v <<= 7;
    r = *((u8*) f->ptr++);
    v |= r & 0x7F;
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

void PlayerReset(PlayerState* p) {
  KillAllNotes(p);
  p->status = PLAYER_STATUS_INACTIVE;
  p->mvol = MVOL_DEFAULT;
  for (int i = 0; i < p->trackcount; i++) {
    p->tracks[0].status = TRACK_STATUS_INACTIVE;
  }

  p->t = 0;
  p->ms = 0;
  p->nextMs = 0;
  p->mspt = 500 / p->ppqn;
}

void PlayerInit(PlayerState* p, SoundArea* snd, SoundBank* bnk, SoundBank* dbnk) {  
  p->snd = snd;
  p->bnk = bnk;
  p->dbnk = dbnk;
  p->f.ptr = 0;
  p->trackcount = 0;
  PlayerReset(p);
}

void PlayerStop(PlayerState* p) {
  if (p->status != PLAYER_STATUS_ACTIVE ){
    return;
  }
  PlayerReset(p);
  p->status = PLAYER_STATUS_READY;
  for (int i = 0; i < p->trackcount; i++) {
    TrackState* trk = &p->tracks[i];
    trk->id = i;
    trk->wait = 0;
    trk->run = 0;
    trk->loopptr = 0;
    trk->loopwait = 0;
    trk->status = TRACK_STATUS_INACTIVE;    
  }

}

void PlayerOpen(PlayerState* p, u8** data) {
  PlayerReset(p);
  char str[32];
  p->f.ptr = data;
  VFile *f = &p->f;
  u32 r;
  r = ReadU32(f);
  if (r != MTHD) {
    dputs("MThd not found");
    return;
  }
  r = ReadBEU32(f);
  if (r != 6) {
    siprintf(str, "Got: %ld", r);
    dputs(str);
    return;
  }
  r = ReadBEU16(f);
  siprintf(str, "Type %ld", r);
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
  p->loopstart = 0;
  p->loopend = 0;

  // scan tracks and set starting pointers
  for (u8 i = 0; i < trackCount; i++) {
    TrackState* trk = &p->tracks[i];
    ResetTrackParams(p, trk);
    trk->id = i;
    trk->wait = 0;
    trk->run = 0;
    trk->loopptr = 0;
    trk->loopwait = 0;
    trk->status = TRACK_STATUS_INACTIVE;
    siprintf(str, "Track %d @ %p", i + 1, f->ptr);
    r = ReadU32(f);
    if (r != MTRK) {
      dputs("Expected MTrk");
      siprintf(str, "Got: %04lX", r);
      dputs(str);
      return;
    }  
    
    r = ReadBEU32(f);

    dputs(str);
    u8 events = 0;
    u8 run = 0;
    u8 first = 1;
    u32 ct = 0;
    u32 ms = 0;
    u32 mspt = 500 / ppqn;
    while (1) {
      events++;
      u32 dt = ReadVLQ(f);
      ct += dt;
      ms += dt * mspt;
      if (first) {
        trk->f.ptr = f->ptr;
        trk->startptr = f->ptr;
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
        switch (meta) {
          case 0x06:
            if (len == 1) {
              char c = ReadU8(f);
              if (c == '[') {
                p->loopstart = ct;
                p->loopstartms = ms;
              } else if (c == ']') {
                p->loopend = ct;
                siprintf(str, "Found Loop @ %ld -> %ld", p->loopstart, p->loopend);
                dputs(str);
              }
            // } else if (len < 31) {
            //   char temp[32];
            //   ReadStr(f, temp, len);
            //   siprintf(str, "Marker \"%s\" @ %d", temp, ct);
            //   dputs(str);
            } else {
              f->ptr += len;
            }
            break;
          case 0x51:
            if (len != 3) {
              dputs("Tempo event must be 3 bytes long!");
              return;
            }
            u32 uspt = ReadBEU24(f);
            mspt = uspt / (ppqn * 1000);
            break;
          default:
            f->ptr += len;
        }
        
        if (meta == 0x2F) {
          break;
        }
      } else if (status == 0xF0 || status == 0xF7) {
        // dputs("Sysex unsupported");
        u32 len = ReadVLQ(f);
        f->ptr += len;
      } else if (status >= 0xF0) {
        siprintf(str, "Unknown status byte: %02X", status);
        dputs(str);
        return;
      } else {
        if ((status & 0x80) == 0) {
          if (run == 0) {
            dputs("Expected running status");
            return;
          }
          status = run;
        } else {
          ReadU8(f);
        }
        u8 s = status >> 4;
        run = status;
    
        if (s != 0xC && s != 0xD) {
          ReadU8(f);
        }
      }
    }
//     siprintf(str, "Wait: %d", trk->wait);
//     dputs(str);
//     siprintf(str, "Events: %d", events);
//     dputs(str);
  }
  
  

  p->status = PLAYER_STATUS_READY;  
}

void PlayerPlay(PlayerState* p) {
  if (p->status == PLAYER_STATUS_ACTIVE) {
    PlayerStop(p);
  } else if (p->status != PLAYER_STATUS_READY) {
    return;
  }

  p->status = PLAYER_STATUS_ACTIVE;
  p->t = 0;
  p->ms = 0;
  p->nextMs = 0;
  p->mspt = 500 / p->ppqn;
  for (int i = 0; i < p->trackcount; i++) {
    TrackState *trk = &p->tracks[i];
    trk->f.ptr = trk->startptr;
    trk->status = TRACK_STATUS_ACTIVE;    
  }
}

void PlayerMain(PlayerState* p) {
  char str[32];
  if (p->status != PLAYER_STATUS_ACTIVE) {
    return;
  }
  u32 activeCount = 0;
  
  // lfo
  for (u8 i = 0; i < p->trackcount; i++) {
    TrackState* trk = &p->tracks[i];
    if (trk->lfoamt) {
      trk->lfophs += trk->lfospd;
      trk->lfo = (s16)(((u16)(tri8lut[trk->lfophs]) * (u16)trk->lfoamt) >> 8) - (trk->lfoamt >> 1);
      TrackUpdatePitch(p->snd, trk);
    }
  }
  
  
  while (p->ms >= p->nextMs) {
    p->t++;
    activeCount = 0;
    for (u8 i = 0; i < p->trackcount; i++) {
      TrackState* trk = &p->tracks[i];
      if (trk->status == TRACK_STATUS_ACTIVE) {
        activeCount++;
      }
      if (trk->wait > 0) {
        trk->wait--;
      }
      VFile* f = &trk->f;
      while (trk->status == TRACK_STATUS_ACTIVE && trk->wait == 0) {
        if (p->loopend > p->loopstart && p->t >= p->loopstart && trk->loopptr == 0) {
          trk->loopptr = f->ptr;
          trk->loopwait = p->t - p->loopstart;
        }
        
        u8 status = ReadU8(f);
        if (status == 0xFF) {
          u8 meta = ReadU8(f);
          // siprintf(str, "Meta event %02X", meta);
          // dputs(str);
          u32 len = ReadVLQ(f);
          if (meta == 0x51) {
            u32 uspt = ReadBEU24(f);
            p->mspt = uspt / (p->ppqn * 1000);
            p->tempo = 60000000 / uspt;
          //   siprintf(str, "MSPT: %d", p->mspt);
          //   dputs(str);
          } else if (meta == 0x2F) {
            trk->status = TRACK_STATUS_INACTIVE;
            break;
          } else {
            f->ptr += len;            
          }
        } else if (status == 0xF0 || status == 0xF7) {
          u32 len = ReadVLQ(f);
          f->ptr += len;
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
          trk->chan = chan;
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
              trk->inst = &p->bnk->entries[b1];
              break;
            case 0xE:
              u16 wheel = b1 | (b2 << 7);
              if (trk->wheel != wheel) {
                trk->wheel = wheel;
                TrackUpdatePitch(p->snd, trk);
              }
              break;
            case 0x9:
              if (b2 == 0) {
                NoteOff(p->snd, trk, b1);
              } else {                
                PlayNote(p, trk, b1, b2);
              }
              break;
            case 0x8:
              NoteOff(p->snd, trk, b1);
              break;
            case 0xB:
              switch (b1) {
                case 1: //mod wheel
                  if (trk->mod == 0) {
                    trk->lfophs = 0;
                  }

                  trk->mod = b2;
                  if (b2) {
                    trk->lfoamt = (((u16)trk->lfodep) * ((u16)trk->mod << 1)) >> 8;
                  } else {
                    trk->lfoamt = 0;
                    trk->lfo = 0;
                    TrackUpdatePitch(p->snd, trk);
                  }
                  break;
                case 2: //duty cycle
                  u8 duty = b2 >> 5;
                  if (trk->duty != duty) {
                    trk->duty = duty;
                    TrackUpdateDuty(p->snd, trk);
                  }
                  break;
                case 3: //cgb env
                  // & 0x40 >> 3 (direction)
                  // >> 3 & 7 (speed)
                  if (b2 == 0x40) { // no movement
                    trk->cgbenv = 0;
                    
                  } else if (b2 > 0x40) { //upwards
                    trk->cgbenv = 0x8 | (((0x3F - b2) >> 3) & 7);
                  } else { //downwards
                    trk->cgbenv = ((b2 >> 3) + 1) & 7;
                  }
                  break;
                case 4: //sound output
                  trk->output = b2;
                  break;
                case 7: // vol
                  if (trk->vol != b2) {
                    trk->vol = b2;
                    TrackUpdateVol(p, trk);
                  } 
                  break;
                case 10: //pan
                  if (trk->pan != b2) {
                    trk->pan = b2;
                    TrackUpdateVol(p, trk);
                  } 
                  break;
                case 11: //exp
                  if (trk->exp != b2) {
                    trk->exp = b2;
                    TrackUpdateVol(p, trk);
                  } 
                  break;
                case 20: //sappy bendr
                  trk->pbr = b2;
                  TrackUpdatePitch(p->snd, trk);
                  break;
                case 33: //sappy priority;
                  trk->priority = b2;
                  break;
                case 64: //sus
                  trk->sus = b2;
                  if (trk->sus < 0) {
                    TrackSusOff(p->snd, trk);
                  }
                  break;
                case 100: //rpn lsb
                  trk->rpnlo = b2;
                  break;
                case 101: //ron msb
                  trk->rpnhi = b2;
                  break;
                case 6: //data msb
                  trk->datahi = b2;
                  if (trk->rpnlo == 0 && trk->rpnhi == 0) {
                    trk->pbr = b2;
                    TrackUpdatePitch(p->snd, trk);
                  }
                  break;
                case 38: //data lsb
                  trk->datalo = b2;
                  break;
              }
              break;
          }
        }
        
        u32 dt = ReadVLQ(f);
        trk->wait = dt;
      }
    }


    if (p->loopend > p->loopstart && p->t >= p->loopend) {
      for (u8 i = 0; i < p->trackcount; i++) {
        TrackState *trk = &p->tracks[i];
        trk->f.ptr = trk->loopptr;
        trk->wait = trk->loopwait;
        trk->status = TRACK_STATUS_ACTIVE;
      }
      u32 dist = p->ms - p->loopstartms;
      p->t = p->loopstart;
      p->ms = p->loopstartms;
      p->nextMs -= dist;
    }

    p->nextMs += p->mspt;
  }
  
  p->ms += FRAME_MS;

//   siprintf(str,"%d %d %d", p->t, p->ms, p->nextMs);
//   dstatus(str);

  if (activeCount == 0 && p->loopend <= p->loopstart) {
    p->status = PLAYER_STATUS_INACTIVE;
  }
}
