/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2000,2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief i386 context switch for WIN32
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

__declspec(naked) void AsmSwitchContext(void * new_sp, void ** save_sp)
{
	__asm
	{
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi
		push	ebp
		mov		ebp,dword ptr [esp+24h]	; ebp <- save_sp
		mov		dword ptr [ebp],esp		; *save_sp = esp
		mov		esp,dword ptr [esp+20h]	; new_sp
		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret
	}
}

__declspec(naked) void AsmReplaceContext(void * new_sp, void ** dummy)
{
	__asm
	{
		mov		esp,dword ptr [esp + 4]	; new_sp
		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret
	}
}

