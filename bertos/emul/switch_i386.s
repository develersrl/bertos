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

/* void asm_switch_context(void ** new_sp, void ** save_sp) */
.globl asm_switch_context
asm_switch_context:
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	movl	0x24(%esp),%ebp         /* ebp = save_sp */
	movl	%esp,(%ebp)             /* *save_sp = esp */
	movl	0x20(%esp),%ebp         /* ebp = new_sp */
	movl	(%ebp),%esp             /* esp = *new_sp */
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	ret

/* int asm_switch_version(void) */
.globl asm_switch_version
asm_switch_version:
	mov	$1,%eax
	ret

