#ifndef DISPLAY_H
#define DISPLAY_H

#define SPI_CLK_EN 11
#define SPI_RST_N 0
#define SCK_EN_POS 24
#define MOSI_EN_POS 0

#define SPI_EN 0
#define SPI_MSTEN 2
#define SPI_TXRDY 1

#define SPI_TXSSEL_N 16 /* deassert slave select */
#define SPI_TXLEN_POS 24
#define SPI_TXLEN 0xB /* 12 bit */
#define SPI_RXIGNORE 22

#define LED_DATA_PIN 2
#define LED_CLOCK_PIN 17
#define XLATCH_PIN 13
#define BLANK_PIN 14

#define LATCH_DELAY_COUNT 100
#define SEGMENT_LED_COUNT 24

void display_initialize(void);

#endif /* DISPLAY_H */

