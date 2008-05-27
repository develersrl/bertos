/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief i386 context switch
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.4  2006/09/20 14:02:49  marco
 * C-style comment.
 *
 * Revision 1.3  2006/02/24 01:35:40  bernie
 * Update for new emulator.
 *
 * Revision 1.2  2006/02/24 01:17:05  bernie
 * Update for new emulator.
 *
 * Revision 1.1  2005/11/27 03:06:15  bernie
 * Add x86_64 task switching (to be updated to new-style scheduler).
 *
 * Revision 1.1  2005/11/14 21:06:38  bernie
 * Add x86_64 support.
 *
 * Revision 1.2  2004/06/06 16:14:12  bernie
 * Add DevLib license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

/* I know it's ugly... */
/* .intel_syntax */

/* void AsmSwitchContext(void **new_sp, void **save_sp) */
/*                       %rdi           %rsi            */
.globl asm_switch_context
asm_switch_context:
	pushq	%rax
	pushq	%rbx
	pushq	%rcx
	pushq	%rdx
	pushq	%rsi
	pushq	%rdi
	pushq	%rbp
	movq	%rsp,(%rsi)             /* *save_sp = rsp */
	movq	(%rdi),%rsp             /* rsp = *new_sp */
	popq	%rbp
	popq	%rdi
	popq	%rsi
	popq	%rdx
	popq	%rcx
	popq	%rbx
	popq	%rax
	ret

/* int asm_switch_version(void) */
.globl asm_switch_version
asm_switch_version:
	mov	$1,%rax
	ret

