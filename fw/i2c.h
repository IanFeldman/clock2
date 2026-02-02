#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define SDA_PIN (uint8_t)11
#define SCL_PIN (uint8_t)10
#define SDA_EN_POS       24
#define SCL_EN_POS        0
#define I2C_CLK_EN        5
#define I2C_RST_N         6
#define I2C_MSTEN         0
#define I2C_MSTCONT       0
#define I2C_MSTSTART      1
#define I2C_MSTSTOP       2
#define I2C_MSTPENDING    0
#define I2C_READWRITE     0

/* Real time clock */
#define RTC_CHIP_ADDR 0xDE
#define RTC_CTRL_ADDR 0x07
#define RTC_SEC_ADDR 0x00
#define RTC_MIN_ADDR 0x01
#define RTC_HRS_ADDR 0x02

/* Temperature sensor */
#define TMP_CHIP_ADDR 0x30
#define TMP_CNFG_ADDR 0x01
#define TMP_TEMP_ADDR 0x05
#define TMP_RES_ADDR 0x08
#define DEG_C 13
#define DEG_F 12

typedef struct {
    uint8_t hours_tens;
    uint8_t hours_ones;
    uint8_t minutes_tens;
    uint8_t minutes_ones;
    uint8_t seconds_tens;
    uint8_t seconds_ones;
} rtc_time_t;

typedef struct {
    uint8_t deg_tens;
    uint8_t deg_ones;
    uint8_t deg_tenths;
    uint8_t deg_hundredths;
    uint8_t deg_unit; /* 13 = C, 12 = F */
} tmp_temp_t;

void i2c_initialize(void);
void i2c_tx(uint8_t chip_addr, uint8_t addr, uint8_t *data, int size);
void i2c_rx(uint8_t chip_addr, uint8_t addr, uint8_t *data, int size);

/* rtc functions */
void i2c_rtc_output_config(void);
rtc_time_t i2c_rtc_get_time(void);
void i2c_rtc_set_time(rtc_time_t time);

/* tmp functions */
void i2c_tmp_output_config(void);
tmp_temp_t i2c_tmp_get_temp(int unit);

#endif /* I2C_H */

