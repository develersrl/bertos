#include <drv/i2s.h>
#include <cfg/macros.h>
#include <cfg/log.h>
#include <io/arm.h>

static uint8_t play_buf1[CONFIG_PLAY_BUF_LEN];
static uint8_t play_buf2[CONFIG_PLAY_BUF_LEN];

/* |x|x|CONT|SECOND|FIRST|IS_PLAYING|CUR_BUF| */
/* |7|6| 5  |  4   |  3  |     2    | 1   0 | */
static uint8_t status;
#define CURRENT_BUF     0x03
#define IS_PLAYING       2
#define FIRST_BUF_FULL   3
#define SECOND_BUF_FULL  4
#define CONTINUE_PLAY    5

INLINE bool is_buffer_full(int bv)
{
	return status & BV(bv);
}

uint8_t *i2s_getBuffer(unsigned buf_num)
{
	kprintf("getBuffer start\n");
	if (status & BV(IS_PLAYING))
		return 0;

	if ((buf_num == I2S_FIRST_BUF) && !is_buffer_full(FIRST_BUF_FULL))
	{
		status |= BV(FIRST_BUF_FULL);
		kprintf("status [0x%02X]\n", status);
		return play_buf1;
	}
	else if ((buf_num == I2S_SECOND_BUF) && !is_buffer_full(SECOND_BUF_FULL))
	{
		status |= BV(SECOND_BUF_FULL);
		kprintf("status [0x%02X]\n", status);
		return play_buf2;
	}
	else
		return 0;
}

uint8_t *i2s_getFreeBuffer(void)
{
	if (!(status & BV(IS_PLAYING)))
		return 0;

	// disable irq
	// ...
	// set continue flag
	// ...
	// set buf_full flag
	// ...
	// enable irq
	// ...
	// return the buffer

	if ((status & CURRENT_BUF) == I2S_FIRST_BUF && !is_buffer_full(SECOND_BUF_FULL))
		return play_buf2;
	else if ((status & CURRENT_BUF) == I2S_SECOND_BUF && !is_buffer_full(FIRST_BUF_FULL))
		return play_buf1;
	else
		return 0;
}

INLINE void setCurrentBuffer(int buffer)
{
	status &= ~CURRENT_BUF;
	status |= CURRENT_BUF & buffer;
}

// code irq callback
static void i2s_dma_tx_irq_handler(void) __attribute__ ((interrupt));
static void i2s_dma_tx_irq_handler(void)
{
	/*
	if (status & BV(CONTINUE_PLAY))
	{
		kprintf("irq\n");
		if ((status & CURRENT_BUF) == I2S_FIRST_BUF)
		{
			SSC_PTCR = BV(PDC_TXTDIS);
			SSC_TPR = (reg32_t)play_buf2;
			SSC_TCR = CONFIG_PLAY_BUF_LEN;
			SSC_PTCR = BV(PDC_TXTEN);

			setCurrentBuffer(I2S_SECOND_BUF);
			status &= ~BV(FIRST_BUF_FULL);
			status &= ~BV(CONTINUE_PLAY);
		}
		// TODO: refactor.
		else
		{
			SSC_PTCR = BV(PDC_TXTDIS);
			SSC_TPR = (reg32_t)play_buf1;
			SSC_TCR = CONFIG_PLAY_BUF_LEN;
			SSC_PTCR = BV(PDC_TXTEN);

			setCurrentBuffer(I2S_FIRST_BUF);
			status &= ~BV(SECOND_BUF_FULL);
			status &= ~BV(CONTINUE_PLAY);
		}
	}
	*/
	AIC_EOICR = 0;
}

bool i2s_start(void)
{
	SSC_PTCR = BV(PDC_TXTDIS);
	SSC_TPR = (reg32_t)play_buf1;
	SSC_TCR = CONFIG_PLAY_BUF_LEN / 2;
	SSC_PTCR = BV(PDC_TXTEN);
	ASSERT(SSC_PTSR & BV(PDC_TXTEN));
	/*
	kprintf("i2s_start start\n");
	if (status & (BV(FIRST_BUF_FULL) | BV(SECOND_BUF_FULL)))
	{
		setCurrentBuffer(I2S_FIRST_BUF);
		SSC_PTCR = BV(PDC_TXTDIS);
		SSC_TPR = (reg32_t)play_buf1;
		SSC_TCR = CONFIG_PLAY_BUF_LEN;

		status |= BV(IS_PLAYING);
		status |= BV(CONTINUE_PLAY);
		kprintf("start: status [0x%02X]\n", status);
		SSC_PTCR = BV(PDC_TXTEN);

		return true;
	}
	else
	{
		kprintf("start: buffers are not full\n");
		return false;
	}
	*/
	return true;
}

#define MCK_DIV 16
#define DELAY ((0 << SSC_STTDLY_SHIFT) & SSC_STTDLY)
#define PERIOD ((7 << (SSC_PERIOD_SHIFT)) & SSC_PERIOD)
/* wtf?? it seems that no 16 won't be sent with MSB first...*/
#define DATALEN (15 & SSC_DATLEN)
#define DATNB ((1 << SSC_DATNB_SHIFT) & SSC_DATNB)
#define FSLEN ((1 << SSC_FSLEN_SHIFT) & SSC_FSLEN)

#define SSC_DMA_IRQ_PRIORITY 5

void i2s_init(void)
{
	PIOA_PDR = BV(SPI1_SPCK) | BV(SPI1_MOSI) | BV(SPI1_NPCS0);
	/* reset device */
	SSC_CR = BV(SSC_SWRST);

	SSC_CMR = MCK_DIV & SSC_DIV;
	SSC_TCMR = SSC_CKS_DIV | SSC_CKO_TRAN | SSC_CKG_NONE | SSC_START_CONT | DELAY | PERIOD;
	SSC_TFMR = DATALEN | FSLEN | SSC_MSBF | SSC_FSOS_POSITIVE;

	/* Disable all irqs */
	SSC_IDR = 0xFFFFFFFF;
	/* Set the vector. */
	AIC_SVR(SSC_ID) = i2s_dma_tx_irq_handler;
	/* Initialize to edge triggered with defined priority. */
	AIC_SMR(SPI0_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED | SSC_DMA_IRQ_PRIORITY;
	/* Enable the SSC IRQ */
	AIC_IDCR = BV(SSC_ID);
	/* Enable interrupt on tx buffer empty */
	SSC_IER = BV(SSC_ENDTX);

	/* enable i2s */
	PMC_PCER = BV(SSC_ID);

	/* enable output */
	SSC_CR = BV(SSC_TXEN);

	/* set current buffer to 1 */
	status = 0x01;
	for (int i = 0; i < CONFIG_PLAY_BUF_LEN; ++i)
	{
		//uint32_t tmp = 0x5555;
		//uint32_t tmp2 = 0x9999;
		play_buf1[i] = (uint8_t)i;
		//play_buf1[i+4] = tmp2;
	}
}


