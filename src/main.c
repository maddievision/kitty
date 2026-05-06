#include <stdio.h>
#include <tonc.h>
#include <tonc_bios.h>
#include "debug.h"
#include "sound.h"
#include "midi.h"

SoundArea sndarea;
SoundBank sndbank;
SoundBank drumbank;
extern WaveData kickyou;
extern WaveData hho;
extern WaveData hhc;
extern WaveData tom;
extern WaveData orch83;
extern WaveData power;
extern WaveData duck;
extern WaveData supersquare;
extern WaveData boogie;
extern void* hello_mid;
PlayerState player;

int main() {
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
	
	for (int i = 0; i < 128; i++) {
		SoundEntry* ent = &sndbank.entries[i];
		ent->type = SOUND_ENTRY_TYPE_SINGLE;
		ent->sample = &supersquare;
		ent->rootnote = 60;
		ent->attack = 0xFF;
		ent->decay = 0xF5;
		ent->sustain = 0x10;
		ent->release = 0x30;
		
		if (i == 55) {
			ent->sample = &orch83;
			ent->rootnote = 48;			
		} else if (i == 103) {
			ent->sample = &duck;			
		} else if (i >= 32 && i < 40) {
			ent->sample = &boogie;
			ent->rootnote = 48;
			ent->attack = 0xFF;
			ent->decay = 0xD0;
			ent->sustain = 0xC0;
			ent->release = 0x10;
		}
	}

	for (int i = 0; i < 128; i++) {
		SoundEntry* ent = &drumbank.entries[i];
		ent->type = SOUND_ENTRY_TYPE_DISABLED;
		ent->sample = 0;
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
		}
	}

// 	sndbank.entries[90].sample = &duck;
// 	sndbank.entries[90].attack = 0xFF;
// 	sndbank.entries[90].decay = 0xD0;
// 	sndbank.entries[90].sustain = 0xC0;
// 	sndbank.entries[90].release = 0x10;
// 	
// 	sndbank.entries[127].sample = &kickyou;
// 	sndbank.entries[127].attack = 0xFF;
// 	sndbank.entries[127].decay = 0x00;
// 	sndbank.entries[127].sustain = 0xFF;
// 	sndbank.entries[127].release = 0xFF;

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
	PlayerInit(&player, &sndarea, &sndbank, &drumbank, (u8**) &hello_mid);
	if (player.status == PLAYER_STATUS_READY) {
		dputs("MIDI file is valid!");
		PlayerPlay(&player);
	} else {
		dputs("Invalid MIDI file.");
	}

  char str[32];
	
	dputs("press UP to play a note.");
	dputs("");

	while(1) {
		VBlankIntrWait();
		SoundDriverVSync();
		SoundDriverMain();
		PlayerMain(&player);
		key_poll();
		
		if (key_hit(KEY_DIR|KEY_A)) {
			if (key_hit(KEY_UP)) {
				u32 freq = MidiKey2Freq(&kickyou, 60, 0);
				siprintf(str, "gay C5 kickyou @ %08X",(u32)&kickyou);
				dputs(str);
				SoundChannel *chn = &sndarea.vchn[0];
				chn->status = 0;
				chn->type = 0;
				chn->volr = 0x80;
				chn->voll = 0x80;
				chn->attack = 0xFF;
				chn->decay = 0xFF;
				chn->sustain = 0xFF;
				chn->release = 0xFF;
				chn->volecho = 0;
				chn->echorem = 0;
				chn->freq = freq;
				chn->wave = &kickyou;
				chn->status = 0x80;		
				freq = MidiKey2Freq(&duck, 62, 0);
				siprintf(str, "gay D5 duck @ %08X",(u32)&duck);
				dputs(str);

				chn = &sndarea.vchn[1];
				chn->status = 0;
				chn->type = 0;
				chn->volr = 0x80;
				chn->voll = 0x80;
				chn->attack = 0xFF;
				chn->decay = 0xFF;
				chn->sustain = 0xFF;
				chn->release = 0xFF;
				chn->volecho = 0;
				chn->echorem = 0;
				chn->freq = freq;
				chn->wave = &duck;
				chn->status = 0x80;
			}
		}
	}
	return 0;
}
