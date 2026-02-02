#ifndef DISPLAY_H
#define DISPLAY_H

#include "i2c.h"

#define GPIO_CLK_EN 6
#define DELAY_CNTS 1

#define SRCLR_PIN 12  /* active low clear */
#define RCLK_PIN   3  /* shift clock */
#define SRCLK_PIN  6  /* overall clock */

#define SER1_PIN 13
#define SER2_PIN 14
#define SER3_PIN 15
#define SER4_PIN 16
#define SER5_PIN 17
#define SER6_PIN  7

#define LSB_MASK 0x01
#define POINT_MASK 0xFE

/* indices of non-numerical display symbols */
#define DSP_CLR 10
#define DSP_DEG 11
#define DSP_E   14
#define DSP_R   15
#define DSP_O   16

static const uint8_t display_table[17] =
{
    0x03, /* 0 */
    0x9F, /* 1 */
    0x25, /* 2 */
    0x0D, /* 3 */
    0x99, /* 4 */
    0x49, /* 5 */
    0x41, /* 6 */
    0x1F, /* 7 */
    0x01, /* 8 */
    0x19, /* 9 */
    0xFF, /* clear   (10) */
    0x39, /* degrees (11) */
    0x71, /* f       (12) */
    0x63, /* c       (13) */
    0x61, /* e       (14) */
    0xF5, /* r       (15) */
    0xC5, /* o       (16) */
};

void display_initialize(void);
void display_clear(void);
void display_error(void);
void display_time(rtc_time_t *time);
void display_temp(tmp_temp_t *temp);

#endif /* DISPLAY_H */

