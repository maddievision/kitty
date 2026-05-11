#include <stdio.h>
#include <tonc.h>
#include <tonc_bios.h>
#include "debug.h"
#include "sound.h"
#include "cgbsound.h"
#include "midi.h"
#include "mixer.h"

SoundArea sndarea;
SoundBank sndbank;
SoundBank drumbank;
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
#ifdef EXTERNAL_MIDI
void* external_mid = (void*)0x8030000;
#else
extern void* hello_mid;
#endif

PlayerState player;

void SetupSoundBank() {
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
			ent->sustain = 0x10;
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
		ent->sustain = 0x10;
		ent->release = 0x30;
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

int main() {
#ifndef OUTPUT_AUDIO_ONLY
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
#endif
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

#ifndef OUTPUT_AUDIO_ONLY
	txt_init_std();
	txt_init_se(0, BG_CBB(0) | BG_SBB(31), 0, CLR_ORANGE, 0);
	pal_bg_mem[0x11] = CLR_GREEN;
#endif

	SetupSoundBank();
	MixerInit(&sndarea, MAX_VCE, 11, 9, 10);
	SoundInitCGB(&sndarea);
	PlayerInit(&player, &sndarea, &sndbank, &drumbank);	
#ifdef EXTERNAL_MIDI
	PlayerOpen(&player, (u8**) external_mid);
#else
	PlayerOpen(&player, (u8**) &hello_mid);
#endif
	if (player.status == PLAYER_STATUS_READY) {
#ifndef OUTPUT_AUDIO_ONLY
		dputs("MIDI file is valid!");
#endif
	} else {
#ifndef OUTPUT_AUDIO_ONLY
		dputs("Invalid MIDI file.");
#endif
	}

#ifdef OUTPUT_AUDIO_ONLY
	PlayerPlay(&player);
#else
#ifdef EXTERNAL_MIDI
	dputs("External MIDI ready");
#else
	dputs("hello.mid ready");
#endif
	dputs("Press A to play");
	char str[32];
#endif
	MixerVSyncOn(&sndarea);

	while(1) {
		VBlankIntrWait();
		MixerVSync(&sndarea);
		SoundMainCGB(&sndarea);
		PlayerMain(&player);
		MixerMain(&sndarea);
#ifndef OUTPUT_AUDIO_ONLY
		siprintf(str, "%ld", player.t);
		dstatus(str);
		key_poll();
		
		if (key_hit(KEY_DIR|KEY_B|KEY_A)) {
			if (key_hit(KEY_A)) {
				PlayerPlay(&player);				
			} else if (key_hit(KEY_B)) {
				PlayerStop(&player);
			}			
		}
#endif
	}
	return 0;
}
