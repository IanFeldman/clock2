#include "LPC8xx.h"
#include "display.h"

static void display_delay(void);

/* Setup output pins and spi pins */
void display_initialize(void)
{
    /* disable SWDIO function (bit 3) so PIO0_2 can be used */
    LPC_SWM->PINENABLE0 |= (1 << 3);

    /* configure latch and blank as outputs, low by default */
    LPC_GPIO_PORT->DIR0 |= (1 << XLATCH_PIN)  | (1 << BLANK_PIN);
    LPC_GPIO_PORT->CLR0 |= (1 << XLATCH_PIN)  | (1 << BLANK_PIN);
    /* toggle blank */
    LPC_GPIO_PORT->SET0 |= (1 << BLANK_PIN);
    display_delay();
    LPC_GPIO_PORT->CLR0 |= (1 << BLANK_PIN);

    /* enable clock for spi0 */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << SPI_CLK_EN);

    /* assert spi0 reset */
    LPC_SYSCON->PRESETCTRL |= (1 << SPI_RST_N);

    /* configure spi0 pins */
    uint32_t reg3 = ~(0xFF << SCK_EN_POS);
    uint32_t reg4 = ~(0xFF << MOSI_EN_POS);
    LPC_SWM->PINASSIGN3 = reg3 | (LED_CLOCK_PIN << SCK_EN_POS);
    LPC_SWM->PINASSIGN4 = reg4 | (LED_DATA_PIN << MOSI_EN_POS);

    /* clock speed */
    /* 48 MHz / 480 = 100KHz */
    LPC_SPI0->DIV = 480;

    /* set tx length, disable slave sel, ignore rx */
    LPC_SPI0->TXCTRL = (1 << SPI_TXSSEL_N) | (SPI_TXLEN << SPI_TXLEN_POS) |
                       (1 << SPI_RXIGNORE);

    /* set config: enabled as master */
    LPC_SPI0->CFG = (1 << SPI_EN) | (1 << SPI_MSTEN);
}


/* Short delay for toggling latch */
static void display_delay(void)
{
    for (int i = 0; i < LATCH_DELAY_COUNT; i++);
}


/* Write a single segment of 24 leds all at the same brightness */
/* Data = 24 bits, brightness = 12 bits */
void display_write_segment(uint32_t data, uint16_t brightness)
{
    /* for each led */
    for (int i = 0; i < SEGMENT_LED_COUNT; i++)
    {
        uint16_t data = 0x0000;
        /* if led is on */
        if (data & (1 << i))
        {
            data = brightness;
        }

        /* wait for tx ready */
        while (!(LPC_SPI0->STAT & (1 << SPI_TXRDY)));
        LPC_SPI0->TXDAT = brightness;
    }

    /* wait for tx ready and transmission complete */
    while (!(LPC_SPI0->STAT & (1 << SPI_TXRDY)));

    /* toggle latch */
    display_delay();
    LPC_GPIO_PORT->SET0 |= (1 << XLATCH_PIN);
    display_delay();
    LPC_GPIO_PORT->CLR0 |= (1 << XLATCH_PIN);
}

