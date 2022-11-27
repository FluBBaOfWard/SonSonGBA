#ifdef __arm__

#include "Shared/gba_asm.h"
#include "Shared/EmuSettings.h"
#include "ARM6809/ARM6809mac.h"
#include "SonSonVideo/SonSonVideo.i"

	.global machineInit
	.global loadCart
	.global m6809Mapper
	.global emuFlags
	.global romNum
	.global cartFlags
	.global romStart
	.global vromBase0
	.global vromBase1
	.global promsBase

	.global soundCpuRam
	.global EMU_RAM
	.global ROM_Space
	.global testState



	.syntax unified
	.arm

	.section .rodata
	.align 2

rawRom:
ROM_Space:

// Son Son
// Main cpu
	.incbin "sonson/ss.01e"
	.incbin "sonson/ss.02e"
	.incbin "sonson/ss.03e"
// Sound cpu
	.incbin "sonson/ss_6.c11"
// Chars
	.incbin "sonson/ss_7.b6"
	.incbin "sonson/ss_8.b5"
// Sprites
	.incbin "sonson/ss_9.m5"
	.incbin "sonson/ss_10.m6"
	.incbin "sonson/ss_11.m3"
	.incbin "sonson/ss_12.m4"
	.incbin "sonson/ss_13.m1"
	.incbin "sonson/ss_14.m2"
// Proms
	.incbin "sonson/ssb4.b2"
	.incbin "sonson/ssb5.b1"
	.incbin "sonson/ssb2.c4"
	.incbin "sonson/ssb3.h7"
	.incbin "sonson/ssb1.k11"

/*
// Son Son (Japan)
// Main cpu
	.incbin "sonson/ss_0.l9"
	.incbin "sonson/ss_1.j9"
	.incbin "sonson/ss_2.l8"
	.incbin "sonson/ss_3.j8"
	.incbin "sonson/ss_4.l7"
	.incbin "sonson/ss_5.j7"
// Sound cpu
	.incbin "sonson/ss_6.c11"
// Chars
	.incbin "sonson/ss_7.b6"
	.incbin "sonson/ss_8.b5"
// Sprites
	.incbin "sonson/ss_9.m5"
	.incbin "sonson/ss_10.m6"
	.incbin "sonson/ss_11.m3"
	.incbin "sonson/ss_12.m4"
	.incbin "sonson/ss_13.m1"
	.incbin "sonson/ss_14.m2"
// Proms
	.incbin "sonson/ssb4.b2"
	.incbin "sonson/ssb5.b1"
	.incbin "sonson/ssb2.c4"
	.incbin "sonson/ssb3.h7"
	.incbin "sonson/ssb1.k11"
*/
	.section .ewram,"ax"
	.align 2
;@----------------------------------------------------------------------------
machineInit: 	;@ Called from C
	.type   machineInit STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}
	mov r0,#0x0014				;@ 3/1 wait state
	ldr r1,=REG_WAITCNT
	strh r0,[r1]

	bl gfxInit
//	bl ioInit
	bl soundInit
//	bl cpuInit

	ldmfd sp!,{lr}
	bx lr

;@----------------------------------------------------------------------------
loadCart: 		;@ Called from C:  r0=rom number, r1=emuflags
	.type   loadCart STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r11,lr}
	str r0,romNum
	str r1,emuFlags

	ldr r3,=rawRom
//	ldr r3,=ROM_Space
								;@ r3=rombase til end of loadcart so DON'T FUCK IT UP
	str r3,romStart				;@ Set rom base
	add r0,r3,#0xC000			;@ 0xC000
//	str r0,cpu2Base				;@ Sound cpu rom
	add r0,r0,#0x2000			;@ 0x2000
	str r0,vromBase0			;@ Chars
	add r0,r0,#0x4000
	str r0,vromBase1			;@ Sprites
	add r0,r0,#0xC000
	str r0,promsBase			;@ proms

	ldr r4,=MEMMAPTBL_
	ldr r5,=RDMEMTBL_
	ldr r6,=WRMEMTBL_
	ldr r7,=mem6809R0
	ldr r8,=rom_W
	mov r0,#0
tbLoop1:
	add r1,r3,r0,lsl#13
	str r1,[r4,r0,lsl#2]
	str r7,[r5,r0,lsl#2]
	str r8,[r6,r0,lsl#2]
	add r0,r0,#1
	cmp r0,#0x07
	bne tbLoop1

	ldr r7,=empty_R
	ldr r8,=empty_W
tbLoop3:
	str r3,[r4,r0,lsl#2]
	str r7,[r5,r0,lsl#2]
	str r8,[r6,r0,lsl#2]
	add r0,r0,#1
	cmp r0,#0x100
	bne tbLoop3

	ldr r1,=EMU_RAM
	ldr r7,=mem6809R0
	ldr r8,=ram6809W0
	mov r0,#0xF8				;@ RAM
	str r1,[r4,r0,lsl#2]		;@ MemMap
	str r7,[r5,r0,lsl#2]		;@ RdMem
	str r8,[r6,r0,lsl#2]		;@ WrMem

	ldr r1,=EMU_RAM
	ldr r7,=IO_R
	ldr r8,=IO_W
	mov r0,#0xFF				;@ IO, gfx
	str r1,[r4,r0,lsl#2]		;@ MemMap
	str r7,[r5,r0,lsl#2]		;@ RdMem
	str r8,[r6,r0,lsl#2]		;@ WrMem


	bl gfxReset
	bl ioReset
	bl soundReset
	bl cpuReset

	ldmfd sp!,{r4-r11,lr}
	bx lr


;@----------------------------------------------------------------------------
//	.section itcm
;@----------------------------------------------------------------------------

;@----------------------------------------------------------------------------
m6809Mapper:		;@ Rom paging..
;@----------------------------------------------------------------------------
	ands r0,r0,#0xFF			;@ Safety
	bxeq lr
	stmfd sp!,{r3-r8,lr}
	ldr r5,=MEMMAPTBL_
	ldr r2,[r5,r1,lsl#2]!
	ldr r3,[r5,#-1024]			;@ RDMEMTBL_
	ldr r4,[r5,#-2048]			;@ WRMEMTBL_

	mov r5,#0
	cmp r1,#0xF9
	movmi r5,#12

	add r6,m6809optbl,#m6809ReadTbl
	add r7,m6809optbl,#m6809WriteTbl
	add r8,m6809optbl,#m6809MemTbl
	b m6809MemAps
m6809MemApl:
	add r6,r6,#4
	add r7,r7,#4
	add r8,r8,#4
m6809MemAp2:
	add r3,r3,r5
	sub r2,r2,#0x2000
m6809MemAps:
	movs r0,r0,lsr#1
	bcc m6809MemApl				;@ C=0
	strcs r3,[r6],#4			;@ readmem_tbl
	strcs r4,[r7],#4			;@ writemem_tb
	strcs r2,[r8],#4			;@ memmap_tbl
	bne m6809MemAp2

;@------------------------------------------
m6809Flush:		;@ Update cpu_pc & lastbank
;@------------------------------------------
	reEncodePC

	ldmfd sp!,{r3-r8,lr}
	bx lr


;@----------------------------------------------------------------------------

romNum:
	.long 0						;@ RomNumber
romInfo:						;@ Keep emuflags/BGmirror together for savestate/loadstate
emuFlags:
	.byte 0						;@ EmuFlags      (label this so Gui.c can take a peek) see EmuSettings.h for bitfields
	.byte SCALED				;@ (display type)
	.byte 0,0					;@ (sprite follow val)
cartFlags:
	.byte 0 					;@ CartFlags
	.space 3

romStart:
	.long 0
vromBase0:
	.long 0
vromBase1:
	.long 0
promBase:
	.long 0
promsBase:
	.long 0
	.pool

	.section .sbss
WRMEMTBL_:
	.space 256*4
RDMEMTBL_:
	.space 256*4
MEMMAPTBL_:
	.space 256*4
soundCpuRam:
	.space 0x0800
EMU_RAM:
	.space 0x2060				;@ Actually 0x1800 + 0x60
testState:
	.space 0x1864+0x800+0x10+0x10+0x28+0x28

;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
