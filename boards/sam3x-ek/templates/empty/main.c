#include <cpu/irq.h>
#include <cfg/debug.h>
#include <drv/timer.h>
#include <hw/hw_led.h>
#include <drv/lcd_hx8347.h>

static void init(void)
{
	kdbg_init();

	IRQ_ENABLE;

	timer_init();
	LED_INIT();
	lcd_hx8347_init();
}

int main(void)
{
	init();

	LED_ON(LED_RED);

	for (;;)
	{
		kprintf("Hello world!\n");
		timer_delay(500);
	}

	return 0;
}

