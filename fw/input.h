#ifndef INPUT_H
#define INPUT_H

#define GPIO_CLK_EN 6

/* Debug output */
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

extern volatile int pps_flag;
extern volatile int input1_flag;
extern volatile int input2_flag;
extern volatile int input3_flag;
extern volatile int input4_flag;

void input_initialize(void);
int input_debounce(int input_pin, int *flag);
void input_toggle_debug(void);
void PININT0_IRQHandler(void);
void PININT1_IRQHandler(void);
void PININT2_IRQHandler(void);
void PININT3_IRQHandler(void);
void PININT4_IRQHandler(void);

#endif /* INPUT_H */

