/* Adapter for gba-hq-mixer */

.syntax unified
.align 2
.global MixerMain
.thumb
.thumb_func
.type MixerMain, %function

.equ MixerID, 0x65736F52 @ Rose
.equ SoundMainRAM, 0x3005000
.equ PcmBufferSize, 0x630

/* SoundArea struct */
.equ SoundArea_ident, 0
.equ SoundArea_DmaCount, 4
.equ SoundArea_DmaPeriod, 0xB
.equ SoundArea_SamplesPerFrame, 0x10
.equ SoundArea_pcmbuf, 0xFD0

/* HQ-Mixer args */
.equ ARG_BUFFER_POS, 0x8
.equ ARG_BUFFER_POS_INDEX_HINT, 0x14

MixerMain:
  ldr r2, l_MixerID
  ldr r3, [r0, SoundArea_ident]
  cmp r2, r3
  beq MixerMain_Prep
  bx lr
MixerMain_Prep:
  adds r3, 1
  str r3, [r0, SoundArea_ident]
  push {r4-r7, lr}
  mov r1, r8
  mov r2, r9
  mov r3, r10
  mov r4, r11
  push {r0-r4}
  sub sp, 0x18
  movs r1, 0
  str r1, [sp, ARG_BUFFER_POS_INDEX_HINT]
  ldr r3, [r0, SoundArea_SamplesPerFrame]
  mov r8, r3
  ldr r5, l_SoundArea_pcmbuf
  adds r5, r0
  ldrb r4, [r0, SoundArea_DmaCount]
  subs r7, r4, 1
  bls MainMixer_NoOffset
  ldrb r1, [r0, SoundArea_DmaPeriod]
  subs r1, r7
  mov r2, r8
  muls r2, r1
  adds r5, r2
MainMixer_NoOffset:
  str r5, [sp, ARG_BUFFER_POS]
  ldr r6, l_PcmBufferSize
  ldr r3, l_SoundMainRAM
  bx r3
  
.align 2, 0
  l_MixerID: .word MixerID
  l_SoundMainRAM: .word SoundMainRAM + 1
  l_PcmBufferSize: .word PcmBufferSize
  l_SoundArea_pcmbuf: .word SoundArea_pcmbuf
