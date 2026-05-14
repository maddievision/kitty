/* Instead of loading the demo mid and embedding it, load MIDI from fixed ROM
   location (0x30000). Intended for inserting a MIDI without needing to build. */
// #define FIXED_ADDRESS_MIDI

/* Enabled reading MIDI buffer from RAM (0x3004000). Intended for use with
   scripts/mesen/udpmidi.lua and https://codeberg.org/roses/midi2udp */
#define LIVE_MIDI_INPUT

#include <stdio.h>
#include <tonc.h>
#include <tonc_bios.h>
#include "demobank.h"
#include "debug.h"
#include "s4a.h"

SappyState sappy;

#ifdef FIXED_ADDRESS_MIDI
void* external_mid = (void*)0x8030000;
#else
extern void* demo_mid;
#endif

int main() {
	char str[32];
	u8 livemidi =
#ifdef LIVE_MIDI_INPUT
		1;
#else
		0;
#endif		

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	txt_init_std();
	txt_init_se(0, BG_CBB(0) | BG_SBB(31), 0, CLR_ORANGE, 0);
	pal_bg_mem[0x11] = CLR_GREEN;

	DemoBankInit();
	s4aInit(&sappy, &sndbank, &drumbank, 12, 11, 9, 10, livemidi);
#ifdef FIXED_ADDRESS_MIDI
	s4aLoadSong(&sappy, (u8**) external_mid);
#else
	s4aLoadSong(&sappy, (u8**) &demo_mid);
#endif
	if (sappy.player.status == PLAYER_STATUS_READY) {
		dputs("MIDI file is valid!");
	} else {
		dputs("Invalid MIDI file.");
	}

#ifdef FIXED_ADDRESS_MIDI
	siprintf(str, "MIDI from fixed address %p ready", external_mid);
	dputs(str);
#else
	dputs("Embedded MIDI ready");
#endif
	dputs("Press A to play");
#ifdef LIVE_MIDI_INPUT
	dputs("Live MIDI enabled");
#endif

	s4aSetVSync(&sappy, 1);

	while(1) {
		VBlankIntrWait();
		s4aVSync(&sappy);
		s4aMain(&sappy);
		siprintf(str, "%ld", sappy.player.t);
		dstatus(str);
		key_poll();
		
		if (key_hit(KEY_DIR|KEY_B|KEY_A)) {
			if (key_hit(KEY_A)) {
				s4aPlaySong(&sappy);
			} else if (key_hit(KEY_B)) {
				s4aStopSong(&sappy);
			}			
		}
	}
	return 0;
}
