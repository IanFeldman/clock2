#include "LPC8xx.h"
#include "display.h"
#include "input.h"
#include "i2c.h"
#include "main.h"
#include "uart.h"

/* isr variables */
volatile rtc_time time = { 0, 0, 0, 0, 0 };
volatile int rx_flag = 0;
volatile char rx_buffer[RX_BUFFER_SIZE];

int main(void)
{
    input_initialize();
    uart_initialize();
    i2c_initialize();
    display_initialize();

    /* reset terminal */
    uart_print_esc(CLEAR_SCREEN);
    uart_print_esc(HOME_CURSOR);

    /* initialize light sensor */
    i2c_light_output_config();

    /* initalize time */
    i2c_rtc_output_config();
    i2c_rtc_set_time(time);

    /* initialize temp */
    i2c_tmp_output_config();

    /* delay before starting program */
    for (int i = 0; i < STARTUP_DELAY; i++);

    clock_mode mode = TIME;

    while (1)
    {
        /* check incoming message */
        if (rx_flag)
        {
            uart_handle_message((char *)rx_buffer, (rtc_time *)&time);
            i2c_rtc_set_time(time);
            rx_flag = 0;
        }

        /* check inputs */
        if (input_debounce(INPUT1_PIN, (int *)&input1_flag))
        {
            if (++mode > WAVE)
            {
                mode = TIME;
            }
            pps_flag = 1;
            input_toggle_debug();
        }

        /* service 1hz pps */
        if (pps_flag || mode == WAVE)
        {
            switch(mode)
            {
                case TIME:
                {
                    uint16_t light = i2c_light_get_als();
                    display_write((seg *)&time, light);
                    break;
                }
                case TEMP:
                {
                    uint16_t light = i2c_light_get_als();
                    tmp_temp temp_f = i2c_tmp_get_temp(DEG_F);
                    display_write((seg *)&temp_f, light);
                    break;
                }
                case WAVE:
                {
                    /* update wave animation every WAVE_PERIOD loops */
                    static int cnt = 0;
                    if (++cnt > WAVE_PERIOD) {
                        display_wave();
                        cnt = 0;
                    }
                    break;
                }
                default:
                    break;
            }
            pps_flag = 0;
        }
    }

    return 1;
}


/* Increment current time by one second */
void increment_time(void)
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

