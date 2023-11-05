#ifdef __arm__

#include "ARM6809/ARM6809.i"
#include "SonSonVideo/SonSonVideo.i"

	.global joyCfg
	.global EMUinput
	.global g_dipSwitch0
	.global g_dipSwitch1
	.global g_dipSwitch2
	.global g_dipSwitch3
	.global coinCounter0
	.global coinCounter1

	.global ioReset
	.global refreshEMUjoypads
	.global IO_R
	.global IO_W

	.syntax unified
	.arm

	.section .ewram,"ax"
	.align 2
;@----------------------------------------------------------------------------
ioReset:
;@----------------------------------------------------------------------------
	bx lr
;@----------------------------------------------------------------------------
refreshEMUjoypads:			;@ Call every frame
;@----------------------------------------------------------------------------
		ldr r4,=frameTotal
		ldr r4,[r4]
		movs r0,r4,lsr#2		;@ C=frame&2 (autofire alternates every other frame)
	ldr r4,EMUinput
	and r0,r4,#0xf0
		ldr r2,joyCfg
		andcs r4,r4,r2
		tstcs r4,r4,lsr#10		;@ L?
		andcs r4,r4,r2,lsr#16
	adr r1,rlud2lrud
	ldrb r0,[r1,r0,lsr#4]

	tst r4,#0x02				;@ B
	orrne r0,r0,#0x01			;@ Shoot
//	tst r4,#0x01				;@ A
//	orrne r0,r0,#0x20			;@ Button 2

	mov r1,#0
	mov r3,#0
	tst r4,#0x4					;@ Select
	orrne r3,r3,#0x10			;@ Coin
	tst r4,#0x8					;@ Start
	orrne r3,r3,#0x01			;@ Start
	tst r2,#0x20000000			;@ Player2?
	movne r1,r0
	movne r0,#0
	movne r3,r3,lsl#1

	strb r0,joy0State
	strb r1,joy1State
	strb r3,joy2State
	bx lr

joyCfg: .long 0x00ff01ff	;@ byte0=auto mask, byte1=(saves R), byte2=R auto mask
							;@ bit 31=single/multi, 30,29=1P/2P, 27=(multi) link active, 24=reset signal received
nrPlayers:	.long 0			;@ Number of players in multilink.
joySerial:	.byte 0
joy0State:	.byte 0
joy1State:	.byte 0
joy2State:	.byte 0
rlud2lrud:		.byte 0x00,0x08,0x04,0x0C, 0x10,0x18,0x14,0x1C, 0x20,0x28,0x24,0x2C, 0x30,0x38,0x34,0x3C
rlud2lrud180:	.byte 0x00,0x04,0x04,0x0C, 0x20,0x24,0x28,0x2C, 0x10,0x14,0x18,0x1C, 0x30,0x34,0x38,0x3C
rlud2lrud90:	.byte 0x00,0x08,0x04,0x0c, 0x02,0x0a,0x06,0x0e, 0x01,0x09,0x05,0x0d, 0x03,0x0b,0x07,0x0f
rlud2lrud270:	.byte 0x00,0x04,0x08,0x0c, 0x01,0x05,0x09,0x0d, 0x02,0x06,0x0a,0x0e, 0x03,0x07,0x0b,0x0f
g_dipSwitch0:	.byte 0
g_dipSwitch1:	.byte 0x85		;@ Lives, cabinet & demo sound.
g_dipSwitch2:	.byte 0
g_dipSwitch3:	.byte 0
coinCounter0:	.long 0
coinCounter1:	.long 0

EMUinput:			;@ This label here for main.c to use
	.long 0			;@ EMUjoypad (this is what Emu sees)

;@----------------------------------------------------------------------------
Input0_R:		;@ Player 1
;@----------------------------------------------------------------------------
;@	mov r11,r11					;@ No$GBA breakpoint
	ldrb r0,joy0State
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
Input1_R:		;@ Player 2
;@----------------------------------------------------------------------------
;@	mov r11,r11					;@ No$GBA breakpoint
	ldrb r0,joy1State
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
Input2_R:		;@ Coins, Start & Service
;@----------------------------------------------------------------------------
;@	mov r11,r11					;@ No$GBA breakpoint
	ldrb r0,joy2State
	eor r0,r0,#0xFF
	bx lr

#ifdef GBA
	.section .iwram, "ax", %progbits	;@ For the GBA
	.align 2
#endif
;@----------------------------------------------------------------------------
Input3_R:
;@----------------------------------------------------------------------------
	ldr r0,=g_dipSwitch1
	ldrb r0,[r0]
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
Input4_R:
;@----------------------------------------------------------------------------
	ldr r0,=g_dipSwitch2
	ldrb r0,[r0]
	eor r0,r0,#0xFF
	bx lr
;@----------------------------------------------------------------------------
Input5_R:
;@----------------------------------------------------------------------------
	ldr r0,=g_dipSwitch0
	ldrb r0,[r0]
	eor r0,r0,#0xFF
	bx lr

;@----------------------------------------------------------------------------
IO_R:				;@ I/O read, 0x2000-0x3FFFF
;@----------------------------------------------------------------------------
	mov r1,addy,lsr#8
;@	cmp r1,#0x20
;@	beq SpriteMem_R
	cmp r1,#0x30
	bne empty_IO_R
	and r1,addy,#0xFF
	cmp r1,#0x07
	ldrmi pc,[pc,r1,lsl#2]
;@---------------------------
	b empty_IO_R
;@ io_read_tbl
	.long empty_IO_R		;@ 0x3000
	.long empty_IO_R		;@ 0x3001
	.long Input0_R			;@ 0x3002
	.long Input1_R			;@ 0x3003
	.long Input2_R			;@ 0x3004
	.long Input3_R			;@ 0x3005
	.long Input4_R			;@ 0x3006
;@----------------------------------------------------------------------------
;@SpriteMem_R
;@----------------------------------------------------------------------------
//	and r1,addy,#0xFF
//	subs r1,r1,#0x20
//	bmi empty_W
//	cmp r1,#0x60
//	bpl empty_W
//	ldr r2,=SPR_RAM
//	ldrb r0,[r2,r1]
//	bx lr
;@----------------------------------------------------------------------------
Scrl_R:
;@----------------------------------------------------------------------------
//	ldrb r0,scrollX
	bx lr

;@----------------------------------------------------------------------------
IO_W:				;@ I/O write, 0x2000-0x3FFFF
;@----------------------------------------------------------------------------
	mov r1,addy,lsr#8
	cmp r1,#0x20
	beq SpriteMem_W
	cmp r1,#0x30
	bne empty_IO_W
	ands r1,addy,#0xFF
	beq Scrl_W
	cmp r1,#0x08
	beq WatchDog_W
	cmp r1,#0x10
	beq soundLatchWrite
	cmp r1,#0x18
	beq Flip_W
	cmp r1,#0x19
	beq IRQTrig_W
	cmp r1,#0x1E
	beq CoinB_W
	cmp r1,#0x1F
	beq CoinA_W

	b empty_IO_W
;@----------------------------------------------------------------------------
SpriteMem_W:
;@----------------------------------------------------------------------------
	and r1,addy,#0xFF
	subs r1,r1,#0x20
	bmi empty_W
	cmp r1,#0x60
	bpl empty_W

	ldr r2,=EMU_RAM+0x2000
	strb r0,[r2,r1]
	bx lr

;@----------------------------------------------------------------------------
Scrl_W:
;@----------------------------------------------------------------------------
	stmfd sp!,{addy,lr}
	ldr sonptr,=sonVideo_0
	bl sonScrollXW
	ldmfd sp!,{addy,pc}
;@----------------------------------------------------------------------------
soundLatchWrite:
;@----------------------------------------------------------------------------
	stmfd sp!,{addy,lr}
	ldr sonptr,=sonVideo_0
	bl sonLatchW
	ldmfd sp!,{addy,pc}
;@----------------------------------------------------------------------------
WatchDog_W:
;@----------------------------------------------------------------------------
//	mov r11,r11					;@ No$GBA breakpoint
	bx lr
;@----------------------------------------------------------------------------
Flip_W:
;@----------------------------------------------------------------------------
	and r0,r0,#1				;@ Bit 0=Flipscreen
;@	strb r0,irqControl

	bx lr
;@----------------------------------------------------------------------------
IRQTrig_W:					;@ Trig IRQ on sound cpu, only on low to high.
;@----------------------------------------------------------------------------
//	mov r11,r11					;@ No$GBA breakpoint
	and r0,r0,#1				;@ Bit 0=firq
//	b soundCpuSetFIRQ
	bx lr
;@----------------------------------------------------------------------------
CoinA_W:
;@----------------------------------------------------------------------------
	tst r0,#1
	ldrne r1,=coinCounter0
	ldrne r0,[r1]
	addne r0,r0,#1
	strne r0,[r1]
	bx lr
;@----------------------------------------------------------------------------
CoinB_W:
;@----------------------------------------------------------------------------
	tst r0,#1
	ldrne r1,=coinCounter1
	ldrne r0,[r1]
	addne r0,r0,#1
	strne r0,[r1]
	bx lr

;@----------------------------------------------------------------------------
soundIO_R:				;@ I/O read, 0xA000
;@----------------------------------------------------------------------------
	cmp addy,#0xA000
	bne empty_IO_R
//	mov r11,r11					;@ No$GBA breakpoint
	stmfd sp!,{addy,lr}
	mov r0,#0
//	bl soundCpuSetFIRQ
	ldr sonptr,=sonVideo_0
	bl sonLatchR
	ldmfd sp!,{addy,pc}

;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
