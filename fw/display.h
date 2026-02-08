#ifndef DISPLAY_H
#define DISPLAY_H

#define SPI_CLK_EN 11
#define SPI_RST_N 0
#define SCK_EN_POS 24
#define MOSI_EN_POS 0

#define SPI_EN 0
#define SPI_MSTEN 2

#define LED_DATA_PIN 2
#define LED_CLOCK_PIN 17
#define XLATCH_PIN 13
#define BLANK_PIN 14

void display_initialize(void);

#endif /* DISPLAY_H */

