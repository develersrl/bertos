/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernie Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief AVR context switch
 *
 * \version $Id$
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include <avr/io.h>

/*
 * NOTE: At each change of this function affecting proc.c
 * (i.e. arguments, data stored in the stack) bump up version
 * number in asm_switch_version().
 */

/* void asm_switch_context(void **new_sp, void **save_sp) */
.globl asm_switch_context
asm_switch_context:

;	r0 is the TEMP REG and can be used freely.
;	r1 is the ZERO REG and must always contain 0.
;
;	Stack frame is 19 byte, remember to update
;	CPU_SAVED_REGS_CNT if you change pushed regs.

	in	r0,SREG-__SFR_OFFSET
	push	r0
;	push	r1	;zero-reg
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r16
	push	r17
;	push	r18	;caller-save
;	push	r19	;caller-save
;	push	r20	;caller-save
;	push	r21	;caller-save
;	push	r22	;caller-save
;	push	r23	;caller-save
;	push	r24	;caller-save
;	push	r25	;caller-save
;	push	r26	;caller-save
;	push	r27	;caller-save
	push	r28
	push	r29
;	push	r30	;caller-save
;	push	r31	;caller-save

;	First parameter (new_sp) is in r24:r25, second (save_sp) in r22:r23

	in	r18,SPL-__SFR_OFFSET	; r18:r19 = SP
	in	r19,SPH-__SFR_OFFSET
	movw	r26,r22			; X = save_sp
	st	X+,r18			; *save_sp = SP
	st	X,r19
	movw	r26,r24			; X = new_sp
	ld	r18,X+
	ld	r19,X

; 	Set new stack pointer.
;	AVR is an 8 bit processor so
;	care must be taken when updating
;	SP that is a 16 bit reg.
;	Two instructions are required to update SP
;	so an IRQ can sneak in between them.
;	So IRQ *MUST* be disabled and then restored.
	cli				; Disable interrupt
	out	SPL-__SFR_OFFSET,r18	; SP = *new_sp
	out	SPH-__SFR_OFFSET,r19
	out	SREG-__SFR_OFFSET,r0	; Restore previous IRQ state

;	pop	r31	;caller-save
;	pop	r30	;caller-save
	pop	r29
	pop	r28
;	pop	r27	;caller-save
;	pop	r26	;caller-save
;	pop	r25	;caller-save
;	pop	r24	;caller-save
;	pop	r23	;caller-save
;	pop	r22	;caller-save
;	pop	r21	;caller-save
;	pop	r20	;caller-save
;	pop	r19	;caller-save
;	pop	r18	;caller-save
	pop	r17
	pop	r16
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
;	pop	r1	;zero-reg
	pop	r0
	out	SREG-__SFR_OFFSET,r0

	ret


/* int asm_switch_version(void) */
.globl asm_switch_version
asm_switch_version:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	ret

