/*!
 * \file
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief DSP5680x task switching support
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

void asm_switch_context(void ** new_sp/*R2*/, void ** save_sp/*R3*/);
asm void asm_switch_context(void ** new_sp, void ** save_sp)
{
	lea   (SP)+
	move  n,x:(SP)+
	move  x0,x:(SP)+
	move  y0,x:(SP)+
	move  y1,x:(SP)+
	move  a0,x:(SP)+
	move  a1,x:(SP)+
	move  a2,x:(SP)+
	move  b0,x:(SP)+
	move  b1,x:(SP)+
	move  b2,x:(SP)+
	move  r0,x:(SP)+
	move  r1,x:(SP)+
	move  r2,x:(SP)+
	move  r3,x:(SP)+

	move  omr,x:(SP)+
	move  la,x:(SP)+
	move  m01,x:(SP)+
	move  lc,x:(SP)+

	;
	; save hardware stack
	;
	move  hws,x:(SP)+
	move  hws,x:(SP)+

	; From the manual:
	; The compiler uses page 0 address locations X: 0x0030 - 0x003F as register
	; variables. Frequently accessed local variables are assigned to the page 0
	; registers instead of to stack locations so that load and store instructions
	; are shortened. Addresses X: 0x0030 - 0x0037 (page 0 registers MR0-MR7) are
	; volatile registers and can be overwritten. The remaining registers (page 0
	; registers MR8-MR15) are treated as non-volatile and, if used by a routine,
	; must be saved on entry and restored on exit.
	;
	; So, register 0x30-0x37 are caller-save, while 0x38-0x3F are callee-save.
	move  x:<$38,y1
	move  y1,x:(SP)+
	move  x:<$39,y1
	move  y1,x:(SP)+
	move  x:<$3A,y1
	move  y1,x:(SP)+
	move  x:<$3B,y1
	move  y1,x:(SP)+
	move  x:<$3C,y1
	move  y1,x:(SP)+
	move  x:<$3D,y1
	move  y1,x:(SP)+
	move  x:<$3E,y1
	move  y1,x:(SP)+
	move  x:<$3F,y1
	move  y1,x:(SP)
	
	; 
	; 28 words have been pushed on the stack.
	nop
	move SP, x:(R3)
	nop
	move x:(R2), SP
	nop

	pop   y1
	move  y1,x:<$3F
	pop   y1
	move  y1,x:<$3E
	pop   y1
	move  y1,x:<$3D
	pop   y1
	move  y1,x:<$3C
	pop   y1
	move  y1,x:<$3B
	pop   y1
	move  y1,x:<$3A
	pop   y1
	move  y1,x:<$39
	pop   y1
	move  y1,x:<$38

	;
	; restore hardware stack
	;
	move  hws,la  ; Clear HWS to ensure proper reload
	move  hws,la
	pop   HWS
	pop   HWS
	
	;
	; restore all saved registers
	;
	pop   lc
	pop   m01
	pop   la
	pop   omr

	pop   r3
	pop   r2
	pop   r1
	pop   r0
	pop   b2
	pop   b1
	pop   b0
	pop   a2
	pop   a1
	pop   a0

	pop   y1
	pop   y0
	pop   x0

	pop   n
	
	rts
}
