#include "LPC8xx.h"
#include "display.h"

/* Setup output pins and spi pins */
void display_initialize(void)
{
    /* disable SWDIO function (bit 3) so PIO0_2 can be used */
    LPC_SWM->PINENABLE0 |= (1 << 3);

    /* configure latch and blank as outputs, low by default */
    LPC_GPIO_PORT->DIR0 |= (1 << XLATCH_PIN)  | (1 << BLANK_PIN);
    LPC_GPIO_PORT->CLR0 |= (1 << XLATCH_PIN)  | (1 << BLANK_PIN);

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
    /* 48 MHz / 48 = 1MHz */
    LPC_SPI0->DIV = 48;

    /* set config: enabled as master */
    LPC_SPI0->CFG = (1 << SPI_EN) | (1 << SPI_MSTEN);
}

