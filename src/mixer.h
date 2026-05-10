#ifndef _MIXER_H
#define _MIXER_H

#include "sound.h"

extern void* SoundMainRAM;	
extern void* SoundMainRAM_end;	
extern void MixerMain(SoundArea*);

void SetupSoundMainRAM();
void MixerInit(SoundArea *snd, u8 voices, u8 mvol, u8 freqMode, u8 reverb);
void MixerVSyncOn(SoundArea *snd);
void MixerVSyncOff(SoundArea *snd);
void MixerVSync(SoundArea *snd);

#endif