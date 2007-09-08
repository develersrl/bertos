/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief C++-like structured exception handling for C programs
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */
#ifndef MWARE_EXCEPT_H
#define MWARE_EXCEPT_H

#include <cfg/debug.h>

#include <setjmp.h>

#define EXCEPT_CONTEXTS 8

/**
 * A stack of jump buffers used to record try sites
 * so they can be reached from throw sites.
 *
 * The stack contains return points for each nested
 * context. jmp_buf's are pushed into the stack at
 * try points and popped out when the try block ends
 * normally or when an exception is thrown.
 */
extern jmp_buf except_stack[EXCEPT_CONTEXTS];
extern int except_top;

#ifdef _DEBUG
#	define PUSH_EXCEPT	ASSERT(except_top < EXCEPT_CONTEXTS), setjmp(except_stack[except_top++]))
#	define POP_EXCEPT	(ASSERT(except_top > 0), --except_top)
#	define DO_EXCEPT	(ASSERT(except_top > 0), longjmp(except_stack[--except_top], true))
#else
#	define PUSH_EXCEPT	(setjmp(except_stack[except_top++]))
#	define POP_EXCEPT	(--except_top)
#	define DO_EXCEPT	(longjmp(except_stack[--except_top], true))
#endif

/**
 * Jump buffer to use when throwing an exception or aborting an operation
 *
 * User code can throw exceptions like this:
 *
 * \code
 *   void a_function_throwing_exceptions(void)
 *   {
 *       if (some_error_condition)
 *          THROW;
 *   }
 * \endcode
 *
 * Catching exceptions (brackets are optional):
 *
 * \code
 *    EXCEPT_DEFINE;
 *
 *    void a_function_catching_an_exception(void)
 *    {
 *        TRY
 *        {
 *            printf("Entered try block\n");
 *            a_function_throwing_exceptions();
 *            printf("Survived execution of critical code\n");
 *        }
 *        CATCH
 *        {
 *            printf("Exception caught!\n");
 *        }
 *        CATCH_END
 *    }
 * \endcode
 *
 * Simple syntax when you don't need to do anything when catching an excaption:
 *
 * \code
 *    TRY
 *        printf("Entered try block\n");
 *        a_function_throwing_exceptions();
 *        printf("Survived execution of critical code\n");
 *    TRY_END
 * \endcode
 *
 * You also need to declare the exception stack once in
 * your global declarations:
 * \code
 *    EXCEPT_DEFINE;
 * \endcode
 */
#define TRY          if (PUSH_EXCEPT) { {
#define TRY_END      } POP_EXCEPT; }
#define CATCH        } POP_EXCEPT; } else {
#define CATCH_END    }
#define THROW        DO_EXCEPT


#define EXCEPT_DEFINE \
	jmp_buf except_stack[EXCEPT_CONTEXTS]; \
	int except_top;

#endif /* MWARE_EXCEPT_H */
