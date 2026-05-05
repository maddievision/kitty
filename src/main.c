#include <stdio.h>
#include <tonc.h>
#include <tonc_bios.h>
#include "debug.h"
#include "sound.h"
#include "midi.h"

SoundArea sndarea;
SoundBank sndbank;
extern WaveData kickyou;
extern WaveData duck;
extern void* hello_mid;
PlayerState player;

int main() {
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
	
	sndbank.insts[0] = &duck;
	sndbank.insts[1] = &kickyou;

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
	PlayerInit(&player, &sndarea, &sndbank, (u8**) &hello_mid);
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
