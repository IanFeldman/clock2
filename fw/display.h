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

#define LATCH_DELAY_COUNT 10000
#define SEGMENT_LED_COUNT 24
#define SEGMENT_COUNT 6

typedef enum
{
    _0 = 0, _1, _2, _3, _4, _5, _6,
    _7, _8, _9, A, B, C, D, E, F, G,
    H, I, J, K, L, M, N, O, P, Q, R,
    S, T, U, V, W, X, Y, Z, DEG, EMPTY
} seg;

static const uint32_t seg_to_dat[] =
{
    0x0069BD96, /* 0 */
    0x00622227, /* 1 */
    0x0069248F, /* 2 */
    0x00E1611E, /* 3 */
    0x00997111, /* 4 */
    0x00F8E196, /* 5 */
    0x0078E996, /* 6 */
    0x00F12222, /* 7 */
    0x00696996, /* 8 */
    0x00E9F111, /* 9 */
    0x0069F999, /* A */
    0x00E9E99E, /* B */
    0x00698896, /* C */
    0x00E9999E, /* D */
    0x00F8F88F, /* E */
    0x00F8F888, /* F */
    0x00698B96, /* G */
    0x0099F999, /* H */
    0x00722227, /* I */
    0x007222A4, /* J */
    0x0089ACA9, /* K */
    0x0088888F, /* L */
    0x009F9999, /* M */
    0x0099DB99, /* N */
    0x00699996, /* O */
    0x00E9E888, /* P */
    0x00699DC7, /* Q */
    0x00E9ECA9, /* R */
    0x00E94297, /* S */
    0x00722222, /* T */
    0x0099999F, /* U */
    0x00999996, /* V */
    0x009999F9, /* W */
    0x00996699, /* X */
    0x00999622, /* Y */
    0x00F8421F, /* Z */
    0x00757000, /* DEG */
    0x00000000  /* EMPTY */
};

void display_initialize(void);
void display_write(seg *segments, uint16_t brightness);

#endif /* DISPLAY_H */

