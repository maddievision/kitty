#include <stdio.h>
#include <tonc.h>
#include <tonc_bios.h>
#include "debug.h"
#include "sound.h"
#include "cgbsound.h"
#include "midi.h"

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
extern WaveData supersquare;
extern WaveData boogie;
extern WaveData piano_fs1;
extern WaveData piano_fs2;
extern WaveData piano_fs3;
extern WaveData piano_fs4;
extern WaveData string2;
extern WaveData crash;
extern WaveData pizz;
extern WaveData dramana;
extern WaveData poly;
extern WaveData sqld;
extern WaveData saw06;
extern void* hello_mid;
PlayerState player;

int main() {
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
	
	for (int i = 0; i < 128; i++) {
		SoundEntry* ent = &sndbank.entries[i];
		if (i < 6) {			
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
		} else if (i == 45) {
			ent->sample = &pizz;
			ent->rootnote = 60;
			ent->decay = 0x00;
			ent->sustain = 0xFF;
			ent->release = 0xFF;
		} else if (i >= 48 && i <= 52) {
			ent->rootnote = 60;
			ent->sample = &string2;
			ent->sustain = 0x40;
		} else if (i == 81) {
			ent->rootnote = 60;
			ent->sample = &poly;			
			ent->sustain = 0x40;
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
	
	toncset(pianomap.entries, 0xFFFFFFFF, 128);
	
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
		} else if (i == 42 || i == 44) {
			ent->type = SOUND_ENTRY_TYPE_SINGLE;
			ent->sample = &hhc;
		} else if (i == 46) {
			ent->type = SOUND_ENTRY_TYPE_SINGLE;
			ent->sample = &hho;
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
		}
	}

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	txt_init_std();
	txt_init_se(0, BG_CBB(0) | BG_SBB(31), 0, CLR_ORANGE, 0);
	pal_bg_mem[0x11] = CLR_GREEN;

	int octave= 0;
	SoundDriverInit(&sndarea);
	SoundDriverMode(
		(10) | //reverb
		(1 << 7) | // reverb set
		(MAX_VCE << 8) | // voices
		(11 << 8) | // master vol
		(4 << 16) | // mix freq
		0x900000  //d/a (8-bit setting)
	);
	SoundInitCGB(&sndarea);
	PlayerInit(&player, &sndarea, &sndbank, &drumbank, (u8**) &hello_mid);
	if (player.status == PLAYER_STATUS_READY) {
		dputs("MIDI file is valid!");
		PlayerPlay(&player);
	} else {
		dputs("Invalid MIDI file.");
	}

  char str[32];

	while(1) {
		VBlankIntrWait();
		SoundDriverVSync();
		SoundDriverMain();
		SoundMainCGB(&sndarea);
		PlayerMain(&player);
		siprintf(str, "%d", player.t);
		dstatus(str);

	// 	key_poll();
	// 	
	// 	if (key_hit(KEY_DIR|KEY_A)) {
	// 		if (key_hit(KEY_UP)) {
	// 		}
	// 	}
	}
	return 0;
}
