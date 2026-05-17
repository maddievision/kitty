/* Instead of loading the demo mid and embedding it, load MIDI from fixed ROM
   location (0x30000). Intended for inserting a MIDI without needing to build. */
// #define FIXED_ADDRESS_MIDI

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
	u8 livemidi = 0;

	irq_init(NULL);
	irq_add(II_VBLANK, NULL);

	DemoBankInit();
	s4aInit(&sappy, &sndbank, &drumbank, 32, 11, 4, 10, livemidi);
// 	s4aInit(&sappy, &sndbank, &drumbank, 40, 11, 4, 10, livemidi);
// 	s4aInit(&sappy, &sndbank, &drumbank, 48, 11, 3, 10, livemidi);
#ifdef FIXED_ADDRESS_MIDI
	s4aLoadSong(&sappy, (u8**) external_mid, 0);
#else
	s4aLoadSong(&sappy, (u8**) &demo_mid, 0);
#endif
	if (sappy.player.status == PLAYER_STATUS_READY) {
	} else {
		return 1;
	}

	s4aPlaySong(&sappy);
	s4aSetVSync(&sappy, 1);

	while(1) {
		VBlankIntrWait();
		s4aVSync(&sappy);
		s4aMain(&sappy);
	}
	return 0;
}
