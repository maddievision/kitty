#include "bank.h"

SoundBank sndbank;
SoundBank drumbank;
SoundBank drumkit;
SoundBank pianomulti;
SoundMap pianomap;
extern WaveData kickyou;
extern WaveData hho;
extern WaveData hhc;
extern WaveData tom;
extern WaveData orch83;
extern WaveData power;
extern WaveData duck;
extern WaveData drm_clap;
extern WaveData supersquare;
extern WaveData boogie;
extern WaveData piano_fs1;
extern WaveData piano_fs2;
extern WaveData piano_fs3;
extern WaveData piano_fs4;
extern WaveData string2;
extern WaveData crash;
extern WaveData pizz;
extern WaveData organ;
extern WaveData dramana;
extern WaveData poly;
extern WaveData sqld;
extern WaveData saw06;
extern WaveData brass;
extern WaveData ep2;
extern WaveData marimba;

void DemoBankInit() {
  for (int i = 0; i < 128; i++) {
    SoundEntry* ent = &sndbank.entries[i];
    if (i < 4) {			
      ent->type = SOUND_ENTRY_TYPE_MULTI;
      SoundEntryMulti* m = (SoundEntryMulti*)((void*) ent);
      m->basenote = 0;
      m->group = &pianomulti;
      m->map = &pianomap;
      continue;
    }
  
  
    ent->type = SOUND_ENTRY_TYPE_SINGLE;
    ent->sample = &sqld;
    ent->rootnote = 60;
    ent->attack = 0xFF;
    ent->decay = 0xF5;
    ent->sustain = 0x20;
    ent->release = 0x30;
    ent->amp = 0x80;
    if (i >= 29 && i < 32) {
      ent->sample = &saw06;
      ent->rootnote = 48;			
      ent->sustain = 0x40;
    } else if (i == 4 || i == 5) {
      ent->sample = &ep2;
      ent->rootnote = 84;
      ent->sustain = 0x01;
      ent->release = 0xC0;
    } else if (i == 45) {
      ent->sample = &pizz;
      ent->rootnote = 60;
      ent->decay = 0x00;
      ent->sustain = 0xFF;
      ent->release = 0xFF;
    } else if (i >= 15 && i <= 23) {
      ent->rootnote = 60;
      ent->rootnote = 72;
      ent->sample = &organ;
      ent->sustain = 0x40;
    } else if ((i == 44) || (i >= 48 && i <= 52)) {
      ent->rootnote = 60;
      ent->sample = &string2;
      ent->sustain = 0x40;
      ent->release = 0xC0;
    } else if ((i >= 56 && i <= 61) || (i >= 64 && i <= 68)) {
      ent->rootnote = 60;
      ent->sample = &brass;			
      ent->amp = 0xC0;
      ent->sustain = 0x40;
    } else if (i == 81 || i == 62 || i == 63) {
      ent->rootnote = 60;
      ent->sample = &poly;			
      ent->sustain = 0x40;
    } else if (i == 85) {
      ent->rootnote = 60;
      ent->sample = &duck;			
      ent->sustain = 0x40;
    } else if (i >= 69 && i <= 78) {
      ent->sample = &supersquare;
      ent->sustain = 0x40;
      ent->rootnote = 72;
      ent->amp = 0xC0;
    } else if (i == 55) {
      ent->sample = &orch83;
      ent->rootnote = 48;		
      ent->release = 0xF8;	
    } else if (i >= 8 && i <= 14) {
      ent->sample = &marimba;
      ent->release = 0xF8;	
    } else if (i >=96 && i <= 103) {
      ent->rootnote = 72;
      ent->sample = &dramana;
      ent->decay = 0xFC;
      ent->release = 0xF0;
      ent->sustain = 0x0;
    }	else if (i >=116) {
      ent->rootnote = 48;
      ent->sample = &tom;
      ent->release = 0xF0;
    } else if (i == 103) {
      ent->rootnote = 60;
      ent->sample = &duck;			
      ent->sustain = 0x40;
  
    } else if (i >= 32 && i < 40) {
      ent->sample = &boogie;
      ent->amp = 0xFF;
      ent->rootnote = 48;
      ent->attack = 0xFF;
      ent->decay = 0xD0;
      ent->sustain = 0xC0;
      ent->release = 0x10;
    }
  }

  for (int i = 0; i < 128; i++) {
    SoundEntry* ent = &pianomulti.entries[i];
    ent->type = SOUND_ENTRY_TYPE_SINGLE;
    ent->rootnote = 60;
    ent->attack = 0xFF;
    ent->decay = 0xF5;
    ent->sustain = 0x01;
    ent->release = 0xC0;
    ent->amp = 0x80;
    switch (i) {
      case 0:
        ent->sample = &piano_fs1;
        break;
      case 1:
        ent->sample = &piano_fs2;
        break;
      case 2:
        ent->sample = &piano_fs3;
        break;
      case 3:
        ent->sample = &piano_fs4;
        break;
      default:
        ent->sample = &piano_fs4;
    }
  }
  
  for (int i = 0; i <= 53; i++) {
    pianomap.entries[i] = 0;
  }
  for (int i = 54; i <= 65; i++) {
    pianomap.entries[i] = 1;
  }
  for (int i = 66; i <= 77; i++) {
    pianomap.entries[i] = 2;
  }
  for (int i = 78; i <= 127; i++) {
    pianomap.entries[i] = 3;
  }
  
    
  for (int i = 0; i < 128; i++) {
    SoundEntry* ent = &drumbank.entries[i];
    ent->type = SOUND_ENTRY_TYPE_KIT;
    ent->sample = (void*) &drumkit;
  }
  
  
  for (int i = 0; i < 128; i++) {
    SoundEntry* ent = &drumkit.entries[i];
    ent->type = SOUND_ENTRY_TYPE_DISABLED;
    ent->sample = 0;
    ent->amp = 0x80;
    ent->rootnote = 60;
    ent->attack = 0xFF;
    ent->decay = 0x00;
    ent->sustain = 0xFF;
    ent->release = 0xFF;
    if (i == 36 || i == 35) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &kickyou;
    } else if (i == 38 || i == 40) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &power;
    } else if (i == 37 || i == 39) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &drm_clap;
    } else if (i == 42 || i == 44) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &hhc;
    } else if (i == 46) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &hho;
    } else if (i == 51) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &hho;
      ent->rootnote = 62;
    } else if (i == 53) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &hho;
      ent->amp = 0x40;
      ent->rootnote = 64;
    } else if (i == 59) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &hho;
      ent->rootnote = 63;
    } else if (i == 41) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &tom;
      ent->rootnote = 56;
    } else if (i == 43) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &tom;
      ent->rootnote = 59;
    } else if (i == 45) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &tom;
      ent->rootnote = 62;
    } else if (i == 47) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &tom;
      ent->rootnote = 65;
    } else if (i == 48) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &tom;
      ent->rootnote = 67;
    } else if (i == 50) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &tom;
      ent->rootnote = 68;
    } else if (i == 49) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &crash;
    } else if (i == 57) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &crash;
      ent->rootnote = 65;
    } else if (i == 55) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &crash;
      ent->rootnote = 62;
    } else if (i == 52) {
      ent->type = SOUND_ENTRY_TYPE_SINGLE;
      ent->sample = &crash;
      ent->rootnote = 60;
    }
  }
}
