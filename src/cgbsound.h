#ifndef _CGB_SOUND_H
#define _CGB_SOUND_H
#include <stdio.h>
#include <tonc.h>

#include "sound.h"

#define CGB_CHANNEL_PULSE_A 0
#define CGB_CHANNEL_PULSE_B 1
#define CGB_CHANNEL_WAVE 2
#define CGB_CHANNEL_NOISE 3

u32 MidiKey2FreqCGB(u8 mk, u8 fp);
void SoundInitCGB(SoundArea *snd);
void SoundMainCGB(SoundArea *snd);

#endif