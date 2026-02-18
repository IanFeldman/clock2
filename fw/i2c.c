#include "i2c.h"
#include "LPC8xx.h"
#include "display.h"

/* Initialize I2C peripheral as master at 100kHz */
void i2c_initialize(void)
{
    /* enable clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << I2C_CLK_EN);

    /* assert i2c reset */
    LPC_SYSCON->PRESETCTRL |= (1 << I2C_RST_N);

    /* switch matrix */
    uint32_t reg7 = ~(0xFF << SDA_EN_POS);
    uint32_t reg8 = ~(0xFF << SCL_EN_POS);
    LPC_SWM->PINASSIGN7 = reg7 | (SDA_PIN << SDA_EN_POS);
    LPC_SWM->PINASSIGN8 = reg8 | (SCL_PIN << SCL_EN_POS);

    /* clock speed */
    /* 48 MHz / 480 = 100kHz (standard mode) */
    LPC_I2C->DIV = 480;

    /* set as master */
    LPC_I2C->CFG = (1 << I2C_MSTEN);
}


/* Perform I2C write to address */
void i2c_tx(uint8_t chip_addr, uint8_t data_addr, uint8_t *data, int size)
{
    /* write slave address with read/~write bit set to 0 */
    chip_addr &= ~(1 << I2C_READWRITE);
    LPC_I2C->MSTDAT = chip_addr;
    /* start the transmission */
    LPC_I2C->MSTCTL = (1 << I2C_MSTSTART);
    /* wait for pending status to be set */
    while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));
    /* write 8 bits of data address */
    LPC_I2C->MSTDAT = data_addr;
    /* indicate to continue */
    LPC_I2C->MSTCTL = (1 << I2C_MSTCONT);
    /* wait for pending status to be set */
    while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));

    /* write remaining data */
    for (int i = 0; i < size; i++)
    {
        /* write 8 bits of data */
        LPC_I2C->MSTDAT = data[i];
        /* indicate to continue */
        LPC_I2C->MSTCTL = (1 << I2C_MSTCONT);
        /* wait for pending status to be set */
        while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));
    }

    /* stop transaction */
    LPC_I2C->MSTCTL = (1 << I2C_MSTSTOP);
}


/* Perform I2C read from address */
void i2c_rx(uint8_t chip_addr, uint8_t data_addr, uint8_t *data, int size)
{
    /* ===== write ===== */

    /* write slave address with read/~write bit set to 0 */
    chip_addr &= ~(1 << I2C_READWRITE);
    LPC_I2C->MSTDAT = chip_addr;
    /* start the transmission */
    LPC_I2C->MSTCTL = (1 << I2C_MSTSTART);
    /* wait for pending status to be set */
    while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));
    /* write 8 bits of data address */
    LPC_I2C->MSTDAT = data_addr;
    /* indicate to continue */
    LPC_I2C->MSTCTL = (1 << I2C_MSTCONT);
    /* wait for pending status to be set */
    while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));

    /* ===== read ===== */
    /* write slave address with read/~write bit set to 1 */
    chip_addr |= (1 << I2C_READWRITE);
    LPC_I2C->MSTDAT = chip_addr;
    /* restart the transmission */
    LPC_I2C->MSTCTL = (1 << I2C_MSTSTART);

    /* read data */
    for (int i = 0; i < size; i++)
    {
        /* wait for pending status to be set */
        while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));
        /* read byte of data */
        data[i] = LPC_I2C->MSTDAT;
        while (!(LPC_I2C->STAT & (1 << I2C_MSTPENDING)));

        /* continue (not on last byte) */
        if (i < size - 1)
        {
            LPC_I2C->MSTCTL = (1 << I2C_MSTCONT);
        }
    }

    /* stop transaction */
    LPC_I2C->MSTCTL = (1 << I2C_MSTSTOP);
}


/* Set RTC multi-function pin to square wave output and configure clock trim */
void i2c_rtc_output_config(void)
{
    /* SQWEN = 1 */
    /* todo: trim */
    uint8_t config = (1 << 6);
    i2c_tx(RTC_CHIP_ADDR, RTC_CTRL_ADDR, &config, sizeof(config));
}


/* Set RTC time, 24-hour mode, oscillator enabled */
void i2c_rtc_set_time(rtc_time_t time)
{
    uint8_t hours_data =
        (time.hours_ones & 0x0F) | (time.hours_tens << 4);
    hours_data &= ~(1 << 6); /* set to 24-hour time */
    i2c_tx(RTC_CHIP_ADDR, RTC_HRS_ADDR, &hours_data, sizeof(hours_data));

    uint8_t minutes_data =
        (time.minutes_ones & 0x0F) |  (time.minutes_tens << 4);
    i2c_tx(RTC_CHIP_ADDR, RTC_MIN_ADDR, &minutes_data, sizeof(minutes_data));

    uint8_t seconds_data =
        (time.seconds_ones & 0x0F) |  (time.seconds_tens << 4);
    seconds_data |= (1 << 7); /* enable oscillator */
    i2c_tx(RTC_CHIP_ADDR, RTC_SEC_ADDR, &seconds_data, sizeof(hours_data));
}


/* Get current seconds, minutes, hours from RTC */
rtc_time_t i2c_rtc_get_time(void)
{
    rtc_time_t time;

    /* get seconds */
    uint8_t seconds_data;
    i2c_rx(RTC_CHIP_ADDR, RTC_SEC_ADDR, &seconds_data, sizeof(seconds_data));
    time.seconds_ones = seconds_data & 0x0F;
    time.seconds_tens = (seconds_data & 0x70) >> 4;

    /* get minutes */
    uint8_t minutes_data;
    i2c_rx(RTC_CHIP_ADDR, RTC_MIN_ADDR, &minutes_data, sizeof(minutes_data));
    time.minutes_ones = minutes_data & 0x0F;
    time.minutes_tens = (minutes_data & 0x70) >> 4;

    /* get hours */
    uint8_t hours_data;
    i2c_rx(RTC_CHIP_ADDR, RTC_HRS_ADDR, &hours_data, sizeof(hours_data));
    time.hours_ones = hours_data & 0x0F;
    time.hours_tens = (hours_data & 0x30) >> 4;

    return time;
}


/* Configure temperature sensor settings */
void i2c_tmp_output_config(void)
{
    /* set resolution */
    uint8_t resolution = (0x03); /* 0.0625 C/bit */
    i2c_tx(TMP_CHIP_ADDR, TMP_RES_ADDR, &resolution, sizeof(resolution));
    /* disable alerts */
    uint8_t config[2];
    config[0] = 0x00;
    config[1] = 0x00;
    i2c_tx(TMP_CHIP_ADDR, TMP_CNFG_ADDR, (uint8_t *)&config, sizeof(config));
}


/* Get temperature and display it */
tmp_temp_t i2c_tmp_get_temp(int unit)
{
    /* read temp register */
    uint8_t temp_raw[2];
    i2c_rx(TMP_CHIP_ADDR, TMP_TEMP_ADDR, temp_raw, sizeof(temp_raw));
    uint16_t temp_full = (temp_raw[0] << 8) | temp_raw[1];
    /* mask out upper bits */
    temp_full = temp_full & 0x0FFF;

    /* multiply by resolution and 100 to get integer milli-degrees celcius */
    int temp_m = 0;
    if (unit == DEG_F)
    {
        /* (0.0625 * 9/5 + 32) * 100 */
        temp_m = (float)temp_full * 11.25f + 3200;
    }
    else if (unit == DEG_C)
    {
        /* 0.0625 * 100 */
        temp_m = (float)temp_full * 6.25f;
    }

    /* abcd */
    tmp_temp_t temp;
    temp.deg_tens = temp_m / 1000;
    temp_m -= (temp.deg_tens * 1000);
    /* bcd */
    temp.deg_ones = temp_m / 100;
    temp_m -= (temp.deg_ones * 100);
    /* cd */
    temp.deg_tenths = temp_m / 10;
    temp_m -= (temp.deg_tenths * 10);
    /* d */
    temp.deg_hundredths = temp_m;
    /* celcius */
    temp.deg_symbol = DEG;
    temp.deg_unit = unit;
    return temp;
}

