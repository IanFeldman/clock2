#include "LPC8xx.h"
#include "i2c.h"
#include "uart.h"
#include "main.h"

static inline void increment_time(void);
static void gpio_int_initialize(void);
static int debounce(void);

/* isr variables */
volatile rtc_time_t time = { 0, 0, 0, 0, 0 };
volatile int pps_flag = 0;
volatile int mode_flag = 0;
volatile int rx_flag = 0;
volatile char rx_buffer[32];

int main(void)
{
    // enable clock for gpio
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << GPIO_CLK_EN);
    // disable SWCLK function (bit 2) so PIO0_3 can be used as GPIO */
    LPC_SWM->PINENABLE0 |= (1 << 2);
    // set debug pin as output
    LPC_GPIO_PORT->DIR0 |= (1 << DEBUG_PIN);
    LPC_GPIO_PORT->SET0 |= (1 << DEBUG_PIN);

    // display_initialize();
    uart_initialize();
    i2c_initialize();

    /* reset terminal */
    uart_print_esc(CLEAR_SCREEN);
    uart_print_esc(HOME_CURSOR);

    /* initialize pin interrupts */
    // gpio_int_initialize();

    /* initalize time */
    i2c_rtc_output_config();
    i2c_rtc_set_time(time);

    /* 0: off, 1: time, ... */
    int mode = 1;
    int update_display = 0;

    while (1)
    {
        /* check incoming message */
        if (rx_flag)
        {
            uart_handle_message((char *)rx_buffer, (rtc_time_t *)&time);
            i2c_rtc_set_time(time);
            update_display = 1;
            rx_flag = 0;
        }

        /* check mode */
        if (mode_flag && debounce())
        {
            mode++;
            if (mode >= MODE_COUNT)
            {
                mode = 0;
                // display_clear();
            }
            else
            {
                update_display = 1;
            }
        }

        /* service 1hz pps */
        if (pps_flag || update_display)
        {
            switch(mode)
            {
                /* normal time display */
                case 1:
                    // display_time((rtc_time_t *)&time);
                    break;

                /* off */
                default:
                    break;
            }
            pps_flag = 0;
            update_display = 0;
        }
    }

    return 1;
}


/* Enable external interrupts for gpio pins:
 *  - Rising edge PININT0 for PPS (PIO0_2)
 *  - Falling edge PININT1 for INPUT1 (PIO0_1)
 *  - Falling edge PININT2 for INPUT2 (PIO0_15)
 *  - Falling edge PININT3 for INPUT3 (PIO0_12)
 *  - Falling edge PININT4 for INPUT4 (PIO0_5)
 */
static void gpio_int_initialize(void)
{
    /* disable SWDIO function (bit 3) so PIO0_2 can be used as GPIO */
    /* disable ACMP_I2 function (bit 1) so PIO0_1 can be used as GPIO */
    LPC_SWM->PINENABLE0 |= (1 << 3) | (1 << 1);
    /* set pins as inputs */
    LPC_GPIO_PORT->DIR0 &= ~((1 << PPS_PIN) | (1 << INPUT1_PIN));
    /* set external interrupts */
    LPC_SYSCON->PINTSEL[PPS_IRQ] = PPS_PIN;
    LPC_SYSCON->PINTSEL[INPUT1_IRQ] = INPUT1_PIN;
    /* set edge sensitive */
    LPC_PIN_INT->ISEL &= ~((1 << PPS_IRQ) | (1 << INPUT1_IRQ));
    /* PPS detect rising edge */
    LPC_PIN_INT->IENR |= (1 << PPS_IRQ);
    /* MODE detect falling edge */
    LPC_PIN_INT->IENF |= (1 << INPUT1_IRQ);
    /* clear any pending/leftover flags */
    LPC_PIN_INT->IST = 0xFF;
    /* enable interrupts */
    NVIC_EnableIRQ(PININT0_IRQn);
    NVIC_EnableIRQ(PININT1_IRQn);
}


/* Increment current time by one second */
static inline void increment_time(void)
{
    time.seconds_ones++;
    /* check seconds */
    if (time.seconds_ones > 9)
    {
        time.seconds_ones = 0;
        time.seconds_tens++;

        if (time.seconds_tens > 5)
        {
            time.seconds_tens = 0;
            time.minutes_ones++;
        }
    }

    /* check minutes */
    if (time.minutes_ones > 9)
    {
        time.minutes_ones = 0;
        time.minutes_tens++;

        if (time.minutes_tens > 5)
        {
            time.minutes_tens = 0;
            time.hours_ones++;
        }
    }

    /* check hours */
    if (time.hours_ones > 9)
    {
        time.hours_ones = 0;
        time.hours_tens++;
    }

    /* reset hours to 0 at 24 */
    if (time.hours_tens > 1 && time.hours_ones > 3)
    {
        time.hours_tens = 0;
        time.hours_ones = 0;
    }
}


/* Debounce MODE pin input. Return 1 if pressed, 0 otherwise */
static int debounce(void)
{
    static int counter = 0;

    counter++;
    /* timeout */
    if (counter > DEBOUNCE_COUNT)
    {
        /* active low */
        int press = !(LPC_GPIO_PORT->B0[INPUT1_PIN]);
        counter = 0; 
        mode_flag = 0;
        return press;
    }
    return 0;
}


/* RTC PPS external interrrupt handler */
void PININT0_IRQHandler(void)
{
    /* check interrupt flag */
    if (LPC_PIN_INT->IST & (1 << PPS_IRQ))
    {
        pps_flag = 1;
        increment_time();
        LPC_PIN_INT->IST = (1 << PPS_IRQ);
    }
}


/* MODE pin external interrrupt handler */
void PININT1_IRQHandler(void)
{
    /* check interrupt flag and set mode flag */
    if (LPC_PIN_INT->IST & (1 << INPUT1_IRQ))
    {
        mode_flag = 1;
        LPC_PIN_INT->IST = (1 << INPUT1_IRQ);
    }
}


/* Serial interrupt handler */
void UART0_IRQHandler(void)
{
    static int rx_idx = 0;

    /* rx ready */
    if (LPC_USART0->INTSTAT & (1 << UART_RX_INT_RDY)) {
        /* get byte */
        char rx_byte = LPC_USART0->RXDATA;
        if (!rx_idx && (rx_byte != UART_RX_SYNC)) return;
        rx_buffer[rx_idx++] = rx_byte;

        /* reset index and set flag if we received full message */
        int full_msg_size = (sizeof(clk_set_msg_t) + sizeof(UART_RX_SYNC));
        if (rx_idx == full_msg_size) {
            rx_idx = 0;
            rx_flag = 1;
        }
    }
}

