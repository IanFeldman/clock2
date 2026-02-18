#include "LPC8xx.h"
#include "display.h"
#include "input.h"
#include "i2c.h"
#include "main.h"
#include "uart.h"

static void increment_time(void);

/* isr variables */
volatile rtc_time_t time = { 0, 0, 0, 0, 0 };
volatile int rx_flag = 0;
volatile char rx_buffer[32];

int main(void)
{
    input_initialize();
    uart_initialize();
    i2c_initialize();
    display_initialize();

    /* reset terminal */
    uart_print_esc(CLEAR_SCREEN);
    uart_print_esc(HOME_CURSOR);

    /* initalize time */
    i2c_rtc_output_config();
    i2c_rtc_set_time(time);

    int x = 0;
    seg test_1[6] = {A, B, C, D, E, F};
    seg test_2[6] = {G, H, I, J, K, L};
    seg test_3[6] = {M, N, O, P, Q, R};
    seg test_4[6] = {S, T, U, V, W, X};
    seg test_5[6] = {Y, Z, _0, _1, _2, _3};
    seg test_6[6] = {_4, _5, _6, _7, _8, _9};

    while (1)
    {
        /* check incoming message */
        if (rx_flag)
        {
            uart_handle_message((char *)rx_buffer, (rtc_time_t *)&time);
            i2c_rtc_set_time(time);
            rx_flag = 0;
        }

        /* check inputs */
        if (input_debounce(INPUT1_PIN, (int *)&input1_flag))
        {
            switch(x) {
                case 0:
                    display_write(test_1, 256);
                    break;
                case 1:
                    display_write(test_2, 256);
                    break;
                case 2:
                    display_write(test_3, 256);
                    break;
                case 3:
                    display_write(test_4, 256);
                    break;
                case 4:
                    display_write(test_5, 256);
                    break;
                case 5:
                    display_write(test_6, 256);
                    break;
                default:
                    break;
            }
            if (++x > 5) {
                x = 0;
            }
        }

        /* service 1hz pps */
        if (pps_flag)
        {
            input_toggle_debug();
            pps_flag = 0;
        }
    }

    return 1;
}


/* Increment current time by one second */
static void increment_time(void)
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

