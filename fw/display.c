#include "LPC8xx.h"
#include "display.h"
#include "util.h"

static void display_delay(void);
static void display_write_data(uint32_t data, uint16_t brightness);

/* Setup output pins and spi pins */
void display_initialize(void)
{
    /* disable SWDIO function (bit 3) so PIO0_2 can be used */
    LPC_SWM->PINENABLE0 |= (1 << 3);

    /* configure latch and blank as outputs, low by default */
    LPC_GPIO_PORT->DIR0 |= (1 << XLATCH_PIN)  | (1 << BLANK_PIN);
    LPC_GPIO_PORT->CLR0 |= (1 << XLATCH_PIN)  | (1 << BLANK_PIN);
    /* set blank high */
    LPC_GPIO_PORT->SET0 |= (1 << BLANK_PIN);

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

    /* set tx length, disable slave sel, ignore rx */
    LPC_SPI0->TXCTRL = (1 << SPI_TXSSEL_N) | (SPI_TXLEN << SPI_TXLEN_POS) |
                       (1 << SPI_RXIGNORE);

    /* set config: enabled as master */
    LPC_SPI0->CFG = (1 << SPI_EN) | (1 << SPI_MSTEN);

    seg clear[6] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    display_write(clear, 0x0000);
    /* set blank low */
    LPC_GPIO_PORT->CLR0 |= (1 << BLANK_PIN);
}


/* Short delay for toggling latch */
static void display_delay(void)
{
    for (int i = 0; i < LATCH_DELAY_COUNT; i++);
}


/* Write a single segment of 24 leds all at the same brightness */
/* data = 24 bits, brightness = 12 bits */
static void display_write_data(uint32_t data, uint16_t brightness)
{
    /* for each led */
    for (int i = 0; i < SEGMENT_LED_COUNT; i++)
    {
        uint16_t write_val = 0x0000;
        /* if led is on */
        if (data & (1 << i))
        {
            write_val = brightness;
        }

        /* wait for tx ready */
        while (!(LPC_SPI0->STAT & (1 << SPI_TXRDY)));
        LPC_SPI0->TXDAT = write_val;
    }

    /* wait for tx ready and transmission complete */
    while (!(LPC_SPI0->STAT & (1 << SPI_TXRDY)));

}


/* Write to all 6 segments with the same brightness */
void display_write(uint8_t *segments, uint16_t brightness)
{
    for (int i = SEGMENT_COUNT - 1; i >= 0; i--)
    {
        uint32_t data = seg_to_dat[segments[i]];
        display_write_data(data, brightness);
    }

    /* toggle latch */
    display_delay();
    LPC_GPIO_PORT->SET0 |= (1 << XLATCH_PIN);
    display_delay();
    LPC_GPIO_PORT->CLR0 |= (1 << XLATCH_PIN);
}


/* Write scrolling sine wave to display */
void display_wave(void)
{
    /* current sine index */
    static int sine_idx = 0;
    /* amount each column index is offset by */
    const int column_idx_offset = 2;
    /* amount of bits to decrease overall brightness by */
    const int brightness_shift = 5;
    /* amount to add to overall brightness */
    const int brightness_adder = 4;

    /* track index for each column */
    int column_sine_idx = sine_idx;

    /* iterate over each segment (right to left) */
    for (int i = 0; i < SEGMENT_COUNT; i++)
    {
        /* track brightness of each column */
        uint16_t col_data[SEGMENT_COLUMN_COUNT];

        /* iterate over each column (right to left) */
        for (int j = 0; j < SEGMENT_COLUMN_COUNT; j++)
        {
            /* set brightness of this column */
            column_sine_idx += column_idx_offset;
            if (column_sine_idx >= SINE_LUT_SIZE)
            {
                column_sine_idx -= SINE_LUT_SIZE;
            }
            /* store brightness */
            col_data[j] =
                (sine_lut[column_sine_idx] >> brightness_shift) + brightness_adder;
        }

        /* write brightness for this segment */
        for (int k = 0; k < SEGMENT_ROW_COUNT; k++)
        {
            for (int l = 0; l < SEGMENT_COLUMN_COUNT; l++)
            {
                while (!(LPC_SPI0->STAT & (1 << SPI_TXRDY)));
                LPC_SPI0->TXDAT = col_data[l];
            }
        }
    }

    /* toggle latch to update display */
    /* wait for tx ready and transmission complete */
    while (!(LPC_SPI0->STAT & (1 << SPI_TXRDY)));
    display_delay();
    LPC_GPIO_PORT->SET0 |= (1 << XLATCH_PIN);
    display_delay();
    LPC_GPIO_PORT->CLR0 |= (1 << XLATCH_PIN);

    /* increment sine index */
    sine_idx += column_idx_offset;
    if (sine_idx >= SINE_LUT_SIZE)
    {
        sine_idx = 0;
    }
}

