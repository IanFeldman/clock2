#ifndef MAIN_H
#define MAIN_H

#define RX_BUFFER_SIZE 32
#define STARTUP_DELAY 100000
#define WAVE_PERIOD 10000

void increment_time(void);
void UART0_IRQHandler(void);

typedef enum {
    TIME = 0,
    TEMP = 1,
    WAVE = 2
} clock_mode;

#endif /* MAIN_H */

