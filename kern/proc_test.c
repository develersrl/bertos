
#include <kern/proc.h>
#include <kern/monitor.h>
#include <drv/timer.h>

/**
 * Proc scheduling test subthread 1
 */
static void NORETURN proc_test_thread1(void)
{
	for (;;)
	{
		kputs(">task 1\n");
		timer_delay(50);
		proc_switch();
	}
}

/**
 * Proc scheduling test subthread 2
 */
static void NORETURN proc_test_thread2(void)
{
	for (;;)
	{
		kputs(">task 2\n");
		timer_delay(75);
		proc_switch();
	}
}

static cpustack_t proc_test_stack1[256];//CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];
static cpustack_t proc_test_stack2[256];//CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];
static cpustack_t monitor_stack[256];//CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];

/**
 * Proc scheduling test
 */
void NORETURN proc_test(void)
{
	monitor_start(sizeof(monitor_stack), monitor_stack);
	proc_new(proc_test_thread1, NULL, sizeof(proc_test_stack1), proc_test_stack1);
	proc_new(proc_test_thread2, NULL, sizeof(proc_test_stack2), proc_test_stack2);
	kputs("Created tasks\n");

	kputs("stack1:\n");
//	#warning FIXME
	kdump(proc_test_stack1+sizeof(proc_test_stack1)-64, 64);
	kputs("stack2:\n");
//	#warning FIXME
	kdump(proc_test_stack2+sizeof(proc_test_stack1)-64, 64);

	for (;;)
	{
		kputs(">main task\n");
		timer_delay(93);
		proc_switch();
	}

	ASSERT(false);
}
