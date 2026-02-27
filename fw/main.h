#ifndef MAIN_H
#define MAIN_H

void increment_time(void);
void UART0_IRQHandler(void);

typedef enum {
    TIME,
    TEMP
} clock_mode;

#endif /* MAIN_H */

