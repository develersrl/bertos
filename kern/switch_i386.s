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

 *
 * $Log$
 * Revision 1.2  2004/06/06 16:14:12  bernie
 * Add DevLib license information.
 *
 * Revision 1.1  2004/05/23 17:27:00  bernie
 * Import kern/ subdirectory.
 *
 */

!!!!!! THIS FILE HAS NOT BEEN REVISED FOR THE NEW SCHEDULER API !!!!!!

/* I know it's ugly... */
#.intel_syntax

/* void AsmSwitchContext(void * new_sp, void ** save_sp) */
.globl AsmSwitchContext
AsmSwitchContext:
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	movl	0x24(%esp),%ebp		/* ebp = save_sp */
	movl	%esp,(%ebp)			/* *save_sp = esp */
	movl	0x20(%esp),%esp		/* esp = new_sp */
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	ret

/* void AsmReplaceContext(void * new_sp, void ** dummy) */
.globl AsmReplaceContext
AsmReplaceContext:
	movl	4(%esp),%esp		/* esp = new_sp */
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	ret

