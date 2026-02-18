#ifndef UART_H
#define UART_H

#include "i2c.h"

/* uart0 */
#define UART_CLK_EN 14
#define TX_PIN (uint8_t)4
#define RX_PIN (uint8_t)0
#define TX_EN_POS       0
#define RX_EN_POS       8

#define UART_EN_POS     0
#define UART_LEN_POS    2      
#define UART_TXRDY      2

#define UARTFRG_RST_N   2
#define UART0_RST_N     3

/* interrupts */
#define UART_RX_INT_EN  0
#define UART_RX_INT_RDY 0
#define UART_RX_SYNC (uint8_t)(0xAA)

#define ESC_CHAR 0x1b
#define NEW_LINE "[1E"
#define HOME_CURSOR "[H"
#define CLEAR_SCREEN "[2J"

typedef struct __attribute__((__packed__)) {
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned char year;
    unsigned char month;
    unsigned char day;
} clk_set_msg_t;

void uart_initialize(void);
void uart_print(char *str);
void uart_print_ln(char *str);
void uart_print_esc(char *code);
void uart_print_char(char c);
void uart_print_hex(char val);
void uart_handle_message(char *msg, rtc_time *time);

#endif /* UART_H */

