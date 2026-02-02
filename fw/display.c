#include "LPC8xx.h"
#include "display.h"
#include "util.h"

/* Delay for some time. */
static void display_delay(int counts)
{
    for (int i = 0; i < counts; i++);
}


/* Pulse SRCLK and RCLK to latch serial values. */
static void display_pulse_clk(void)
{
    LPC_GPIO_PORT->SET0 |= (1 << SRCLK_PIN);
    LPC_GPIO_PORT->CLR0 |= (1 << RCLK_PIN);
    display_delay(DELAY_CNTS);
    LPC_GPIO_PORT->CLR0 |= (1 << SRCLK_PIN);
    LPC_GPIO_PORT->SET0 |= (1 << RCLK_PIN);
    display_delay(DELAY_CNTS);
}


/* Clock value into shift register. */
static void display_set(uint8_t *values)
{
    const int shift_cnt = 8;
    for (int i = 0; i < shift_cnt; i++)
    {
        /* set data */
        LPC_GPIO_PORT->B0[SER1_PIN] = (values[0] >> i) & LSB_MASK;
        LPC_GPIO_PORT->B0[SER2_PIN] = (values[1] >> i) & LSB_MASK;
        LPC_GPIO_PORT->B0[SER3_PIN] = (values[2] >> i) & LSB_MASK;
        LPC_GPIO_PORT->B0[SER4_PIN] = (values[3] >> i) & LSB_MASK;
        LPC_GPIO_PORT->B0[SER5_PIN] = (values[4] >> i) & LSB_MASK;
        LPC_GPIO_PORT->B0[SER6_PIN] = (values[5] >> i) & LSB_MASK;

        display_pulse_clk();
    }

    /* reset pins */
    LPC_GPIO_PORT->CLR0 |= (1 << SRCLK_PIN);
    LPC_GPIO_PORT->SET0 |= (1 << RCLK_PIN);
}


/* Initialize pins and shift registers to zero. */
void display_initialize(void)
{   
    /* enable clock for gpio */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << GPIO_CLK_EN);

    /* disable SWCLK function (bit 2) so PIO0_3 can be used as GPIO */
    LPC_SWM->PINENABLE0 |= (1 << 2);

    /* set pins as outputs */
    LPC_GPIO_PORT->DIR0 |= (1 << SRCLR_PIN) | (1 << RCLK_PIN) | (1 << SRCLK_PIN) |
                           (1 << SER1_PIN)  | (1 << SER2_PIN) | (1 << SER3_PIN)  |
                           (1 << SER4_PIN)  | (1 << SER5_PIN) | (1 << SER6_PIN);

    LPC_GPIO_PORT->CLR0 |= (1 << SRCLR_PIN) | (1 << RCLK_PIN) | (1 << SRCLK_PIN) |
                           (1 << SER1_PIN)  | (1 << SER2_PIN) | (1 << SER3_PIN)  |
                           (1 << SER4_PIN)  | (1 << SER5_PIN) | (1 << SER6_PIN);

    /* reset registers */
    display_delay(DELAY_CNTS);
    LPC_GPIO_PORT->SET0 |= (1 << SRCLR_PIN);
    display_delay(DELAY_CNTS);

    const uint8_t values[6] = {0, 0, 0, 0, 0, 0};
    display_set((uint8_t *)values);
}


/* Clear display */
void display_clear(void)
{
    uint8_t values[6];
    memset(values, display_table[DSP_CLR], 6);
    display_set(values);
}


/* Display error */
void display_error(void)
{
    uint8_t values[6] = {
        display_table[DSP_E],
        display_table[DSP_R],
        display_table[DSP_R],
        display_table[DSP_O],
        display_table[DSP_R],
        display_table[DSP_CLR]
    };
    display_set(values);
}


/* Update display with time */
void display_time(rtc_time_t *time)
{
    uint8_t values[6] = {
        display_table[time->hours_tens],
        display_table[time->hours_ones],
        display_table[time->minutes_tens],
        display_table[time->minutes_ones],
        display_table[time->seconds_tens],
        display_table[time->seconds_ones]
    };
    display_set(values);
}


/* Update display with time */
void display_temp(tmp_temp_t *temp)
{
    uint8_t values[6] = {
        display_table[temp->deg_tens],
        display_table[temp->deg_ones] & POINT_MASK,
        display_table[temp->deg_tenths],
        display_table[temp->deg_hundredths],
        display_table[DSP_DEG],
        display_table[temp->deg_unit]
    };
    display_set(values);
}

