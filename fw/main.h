#ifndef MAIN_H
#define MAIN_H

#define GPIO_CLK_EN 6

#define DEBUG_PIN 3

/* Pulse per second */
#define PPS_PIN 10
#define PPS_IRQ 0    /* PININT0_IRQ */

/* X */
#define INPUT1_PIN 1
#define INPUT1_IRQ 1 /* PININT1_IRQ */

/* Y */
#define INPUT2_PIN 15
#define INPUT2_IRQ 2 /* PININT2_IRQ */

/* A + ISP */
#define INPUT3_PIN 12
#define INPUT3_IRQ 3 /* PININT3_IRQ */

/* B + RESET */
#define INPUT4_PIN 5
#define INPUT4_IRQ 4 /* PININT4_IRQ */

#define DEBOUNCE_COUNT 1000
#define MODE_COUNT 1

void PININT0_IRQHandler(void);
void PININT1_IRQHandler(void);
void PININT2_IRQHandler(void);
void PININT3_IRQHandler(void);
void PININT4_IRQHandler(void);
void UART0_IRQHandler(void);

#endif /* MAIN_H */

