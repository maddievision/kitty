/* Flashcart example was mostly derived from the example code https://github.com/afska/gba-flashcartio/blob/master/example/src/main.cpp */

/* Enabled reading MIDI buffer from RAM (0x3004000). Intended for use with
   scripts/mesen/udpmidi.lua and https://codeberg.org/roses/midi2udp */
#define LIVE_MIDI_INPUT

#include <tonc.h>
#include <string>
#include <vector>
#include <algorithm>

extern "C" {
#define FLASHCARTIO_DISABLE_DMA 1
#include "../../shared/vendor/gba-flashcartio/lib/flashcartio.h"
#include "debug.h"
#include "demobank.h"
#include "s4a.h"
}

#define MAX_SIZE 0x28000
#define MAX_ENTRIES 20

SappyState sappy;
__attribute__((section(".ewram"))) char external_mid[MAX_SIZE];

void listDir(std::vector<FILINFO> items,
						 u32 selected,
						 u32 offset = 0,
						 u32 count = MAX_ENTRIES);
std::vector<FILINFO> readDir(std::string path);
void log(std::string text);
void halt(std::string text);
u16 waitFor(u16 key);

const std::string names[] = {"-", "Everdrive", "EZ Flash"};

void init() {
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	REG_KEYCNT = 0b1100000000001111;
	irq_add(II_KEYPAD, SoftReset);

	// (1) Initialize the library
	log("Activating...");
	if (!flashcartio_activate())
		halt("No flashcart detected!");
}

int main() {
	char error[256];
	init();
	DemoBankInit();

	// (2) Use FatFs functions
	FATFS fatfs;
	FRESULT fr = f_mount(&fatfs, "", 1);
	if (fr > 0)
		halt("mount failed!");
		
	std::string path = "midi";
	u32 selected = 0;
	std::vector<FILINFO> files = readDir(path);
	listDir(files, selected);
	
	while (true) {
		u16 keys = waitFor(KEY_DIR | KEY_A | KEY_B);
		if (keys & KEY_DOWN) {
			// Cursor down
			selected = (selected + 1) % files.size();
		} else if (keys & KEY_UP) {
			// Cursor up
			selected = selected == 0 ? files.size() - 1 : selected - 1;
		} else if (keys & KEY_LEFT) {
			if (selected < 10) {
				selected = 0;
			} else {
				selected -= 10;
			}
		} else if (keys & KEY_RIGHT) {
			if ((selected + 10) >= files.size() - 1) {
				selected = files.size() - 1;
			} else {
				selected += 10;
			}
		} else if (keys & KEY_A) {
			// Read file/directory
			auto selectedItem = files[selected];
			if (selectedItem.fattrib & AM_DIR) {
				// Navigate to directory
				path += "/" + std::string(selectedItem.fname);
				files = readDir(path);
				selected = 0;
			} else {
				// Read file
				if (selectedItem.fsize < MAX_SIZE) {
					auto filePath = path + "/" + selectedItem.fname;
					FIL fil;
					f_open(&fil, filePath.c_str(), FA_READ);
					if (fr > 0)
						halt("open failed!");
	
					std::string content = "";
					size_t amt = selectedItem.fsize;
					size_t bufsize = 128;
					size_t read = 0;
					char* ptr = external_mid;
					while (amt > 0) {
						if (bufsize > amt) {
							bufsize = amt;
						}
						f_read(&fil, ptr, bufsize, &read);
						ptr += read;
						amt -= read;
					}

					u8 livemidi =
						#ifdef LIVE_MIDI_INPUT
								1;
						#else
								0;
						#endif		
					s4aInit(&sappy, &sndbank, &drumbank, 12, 11, 9, 10, livemidi);

					s4aLoadSong(&sappy, (u8**) external_mid, error);					
					if (sappy.player.status == PLAYER_STATUS_READY) {
						log("Playing " + std::string(selectedItem.fname) + "!\n" + std::to_string(sappy.player.trackcount) + " tracks.\nPress B to stop.");
						s4aPlaySong(&sappy);
						s4aSetVSync(&sappy, 1);						
						while(1) {
							VBlankIntrWait();
							s4aVSync(&sappy);
							s4aMain(&sappy);
							// siprintf(str, "%ld", sappy.player.t);
							// dstatus(str);
							key_poll();
							
							if (key_hit(KEY_B)) {
								s4aStopSong(&sappy);
								s4aSetVSync(&sappy, 0);
								break;								
							}
						}
					} else {
						log(error);
						waitFor(KEY_B);
					}
				} else {
					log("Nah, too big!");
					waitFor(KEY_B);
				}	
			}
		} else if (keys & KEY_B) {
			// Go back
			if (path != "/") {
				path = "/";
				files = readDir(path);
				selected = 0;
			}
		}
	
		VBlankIntrWait();
		listDir(files, selected, max(selected - MAX_ENTRIES / 2, 0));
	}
	
	return 0;
}


void listDir(std::vector<FILINFO> items, u32 selected, u32 offset, u32 count) {
	std::string output = "";
	for (u32 i = offset; i < items.size(); i++) {
		auto item = items[i];
		output += std::string(selected == i ? ">> " : "   ") +
							(item.fattrib & AM_DIR ? "[" : "") + std::string(item.fname) +
							(item.fattrib & AM_DIR ? "]" : "") + "\n";
		if (i - offset + 1 >= count)
			break;
	}
	log(output);
}

bool nameCompare(const FILINFO& a, const FILINFO& b) {
		std::string sa = a.fname;
		std::string sb = b.fname;
		return std::lexicographical_compare(
				sa.begin(), sa.end(),
				sb.begin(), sb.end(),
				[](char a, char b) {
						return std::tolower(a) < std::tolower(b);
				}
		);
}

std::vector<FILINFO> readDir(std::string path) {
	auto items = std::vector<FILINFO>{};

	DIR dir;
	FRESULT fr = f_opendir(&dir, path.c_str());
	if (fr > 0)
		halt("opendir failed!");
	FILINFO fno;
	std::string ext = ".mid";
	while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0) {
		std::string fname = fno.fname;
		if ((!(fno.fattrib & AM_DIR) && !std::equal(ext.rbegin(), ext.rend(), fname.rbegin())) || fname.rfind("._", 0) == 0) {
			continue;
		}
		items.push_back(fno);
	}
	
	std::sort(items.begin(), items.end(), nameCompare);
	
	fr = f_closedir(&dir);
	if (fr > 0)
		halt("closedir failed!");

	return items;
}

void log(std::string text) {
	tte_erase_screen();
	tte_write("#{P:0,0}");
	tte_write(text.c_str());
}

void halt(std::string text) {
	log(text);
	while (true)
		;
}

u16 waitFor(u16 key) {
	u32 hits;

	do {
		VBlankIntrWait();
		key_poll();
		hits = key_hit(key);
	} while (hits == 0);

	return hits;
}