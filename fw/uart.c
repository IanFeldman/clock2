#include "uart.h"
#include "LPC8xx.h"
#include "util.h"

/* Initialize uart peripheral at 9600 baud with rx interrupts */
void uart_initialize(void)
{
    /* enable clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << UART_CLK_EN);
    
    /* clear resets */
    LPC_SYSCON->PRESETCTRL |= (1 << UARTFRG_RST_N); // clear frg reset
    LPC_SYSCON->PRESETCTRL |= (1 << UART0_RST_N); // clear uart reset

    /* switch matrix */
    uint32_t reg = ~((0xFF << TX_EN_POS) | (0xFF << RX_EN_POS));
    LPC_SWM->PINASSIGN0 = reg | ((TX_PIN << TX_EN_POS) | (RX_PIN << RX_EN_POS));

    /* configure baud for 9600 */
    /* U_PCLK = UARTCLKDIV/(1 + MULT/DIV) */

    /* main uart peripheral clock */
    LPC_SYSCON->UARTCLKDIV = 4;

    /* fractional generator */
    // LPC_SYSCON->UARTFRGDIV = 0xFF; /* 0xFF + 0x01 = 256 */
    // LPC_SYSCON->UARTFRGMULT = 0;

    /* baud rate = U_PCLK/16 x BRGVAL */
    LPC_USART0->BRG = 78;

    /* configure control register */
    LPC_USART0->CTRL = 0;
    /* clear any pending flags */
    LPC_USART0->STAT = 0xFFFF;

    /* enable rx interrupts */
    LPC_USART0->INTENSET = (1 << UART_RX_INT_EN);
    NVIC_EnableIRQ(UART0_IRQn);

    /* configure for 8 bits, no parity, 1 stop bit */
    LPC_USART0->CFG = (1 << UART_EN_POS) | (1 << UART_LEN_POS);
}


/* Print string over serial. */
void uart_print(char *str)
{
    if (!str) { return; }

    while (*str)
    {
        uart_print_char(*str++);
    }
}


/* Print string with new line appended. */
void uart_print_ln(char *str)
{
    uart_print(str);
    uart_print_esc(NEW_LINE);
}


/* Print an ANSI escape code over serial. */
void uart_print_esc(char *code)
{
    if (!code) { return; }
    uart_print_char(ESC_CHAR);
    uart_print(code);
}


/* Print a single character over serial */
void uart_print_char(char c)
{
    /* wait until tx ready */
    while (!(LPC_USART0->STAT & (1 << UART_TXRDY)));
    LPC_USART0->TXDATA = c;
}


/* Print value as hex */
void uart_print_hex(char val)
{
    uart_print_char("0123456789ABCDEF"[val >> 4]);
    uart_print_char("0123456789ABCDEF"[val & 0x0F]);
}


/* Convert message payload to bcd and store in rtc_time_t
 * Todo: Add rtc_date_t
 */
void uart_handle_message(char *msg, rtc_time_t *time)
{
    clk_set_msg_t clk_set;
    memcpy(&clk_set, (void *)msg + sizeof(UART_RX_SYNC), sizeof(clk_set));

    /* convert to time */
    time->hours_tens = clk_set.hours / 10;
    time->hours_ones = clk_set.hours - (time->hours_tens * 10);
    time->minutes_tens = clk_set.minutes / 10;
    time->minutes_ones = clk_set.minutes - (time->minutes_tens * 10);
    time->seconds_tens = clk_set.seconds / 10;
    time->seconds_ones = clk_set.seconds - (time->seconds_tens * 10);

    /* todo: check if numbers are invalid */

    /* todo: convert to date */
}

