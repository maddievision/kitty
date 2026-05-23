/* Instead of loading the demo mid and embedding it, load MIDI from fixed ROM
   location (0x30000). Intended for inserting a MIDI without needing to build. */
// #define FIXED_ADDRESS_MIDI

#include <stdio.h>
#include <tonc.h>
#include <tonc_bios.h>
#include "demobank.h"
#include "debug.h"
#include "kitty.h"

KittyState kitty;

#ifdef FIXED_ADDRESS_MIDI
void* external_mid = (void*)0x8030000;
#else
extern void* demo_mid;
#endif

int main() {
	u8 livemidi = 0;

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	DemoBankInit();
	KittyInit(&kitty, &sndbank, &drumbank, 32, 11, 4, 10, livemidi);
// 	KittyInit(&kitty, &sndbank, &drumbank, 40, 11, 4, 10, livemidi);
// 	KittyInit(&kitty, &sndbank, &drumbank, 48, 11, 3, 10, livemidi);
#ifdef FIXED_ADDRESS_MIDI
	KittyLoadSong(&kitty, (u8**) external_mid, 0);
#else
	KittyLoadSong(&kitty, (u8**) &demo_mid, 0);
#endif
	if (kitty.player.status == PLAYER_STATUS_READY) {
	} else {
		return 1;
	}

	KittyPlaySong(&kitty);
	KittySetVSync(&kitty, 1);

	while(1) {
		VBlankIntrWait();
		KittyVSync(&kitty);
		KittyMain(&kitty);
	}
	return 0;
}
