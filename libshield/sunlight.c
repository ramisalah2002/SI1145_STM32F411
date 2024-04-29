#include "sunlight.h"
#include "lib/i2c.h"
#include "lib/uart.h"
#include "lib/io.h"
#include "lib/timer.h"

#include "sunlight.h"

#define DELAY_1_SECOND      1000000
#define DELAY_10_SECONDS    10000000

#define delay_us(us)        timer_wait_us(_TIM3, us)

void SI114X_DeInit(void) {
    // ENABLE UV reading
    // these reg must be set to the fixed value
    SI114X_WriteByte(SI114X_UCOEFF0, 0x29);
    SI114X_WriteByte(SI114X_UCOEFF1, 0x89);
    SI114X_WriteByte(SI114X_UCOEFF2, 0x02);
    SI114X_WriteByte(SI114X_UCOEFF3, 0x00);
    SI114X_WriteParamData(SI114X_CHLIST, SI114X_CHLIST_ENUV | SI114X_CHLIST_ENALSIR | SI114X_CHLIST_ENALSVIS |
                   SI114X_CHLIST_ENPS1);
    //
    // set LED1 CURRENT(22.4mA)(It is a normal value for many LED)
    //
    SI114X_WriteParamData(SI114X_PS1_ADCMUX, SI114X_ADCMUX_LARGE_IR);
    SI114X_WriteByte(SI114X_PS_LED21, SI114X_LED_CURRENT_22MA);
    SI114X_WriteParamData(SI114X_PSLED12_SELECT, SI114X_PSLED12_SELECT_PS1_LED1); //
    //
    // PS ADC SETTING
    //
    SI114X_WriteParamData(SI114X_PS_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
    SI114X_WriteParamData(SI114X_PS_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
    SI114X_WriteParamData(SI114X_PS_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE | SI114X_ADC_MISC_ADC_RAWADC);
    //
    // VIS ADC SETTING
    //
    SI114X_WriteParamData(SI114X_ALS_VIS_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
    SI114X_WriteParamData(SI114X_ALS_VIS_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
    SI114X_WriteParamData(SI114X_ALS_VIS_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE);
    //
    // IR ADC SETTING
    //
    SI114X_WriteParamData(SI114X_ALS_IR_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
    SI114X_WriteParamData(SI114X_ALS_IR_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
    SI114X_WriteParamData(SI114X_ALS_IR_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE);
    //
    // interrupt enable
    //
    SI114X_WriteByte(SI114X_INT_CFG, SI114X_INT_CFG_INTOE);
    SI114X_WriteByte(SI114X_IRQ_ENABLE, SI114X_IRQEN_ALS);
    //
    // AUTO RUN
    //
    SI114X_WriteByte(SI114X_MEAS_RATE0, 0xFF);
    SI114X_WriteByte(SI114X_COMMAND, SI114X_PSALS_AUTO);
}

bool SI114X_Begin(void) {
    i2c_master_init(_I2C1); // Initialize I2C
    //
    // Init IIC  and reset si1145
    //
    if (SI114X_ReadByte(SI114X_PART_ID) != 0X45) {
        return false;
    }
    SI114X_Reset();
    //
    // INIT
    //
    SI114X_DeInit();
    return true;
}

void SI114X_Reset(void) {
    SI114X_WriteByte(SI114X_MEAS_RATE0, 0);
    SI114X_WriteByte(SI114X_MEAS_RATE1, 0);
    SI114X_WriteByte(SI114X_IRQ_ENABLE, 0);
    SI114X_WriteByte(SI114X_IRQ_MODE1, 0);
    SI114X_WriteByte(SI114X_IRQ_MODE2, 0);
    SI114X_WriteByte(SI114X_INT_CFG, 0);
    SI114X_WriteByte(SI114X_IRQ_STATUS, 0xFF);

    SI114X_WriteByte(SI114X_COMMAND, SI114X_RESET);
     delay_us(10);
    SI114X_WriteByte(SI114X_HW_KEY, 0x17);
     delay_us(10);
}

void SI114X_WriteByte(uint8_t Reg, uint8_t Value) {
    uint8_t buf[2] = {Reg, Value};
    i2c_write(_I2C1, SI114X_ADDR, buf, 2);
}

uint8_t SI114X_ReadByte(uint8_t Reg) {
    uint8_t buf[1];
    i2c_write(_I2C1, SI114X_ADDR, &Reg, 1);
    i2c_read(_I2C1, SI114X_ADDR, buf, 1);
    return buf[0];
}

uint16_t SI114X_ReadHalfWord(uint8_t Reg) {
    uint8_t buf[2];
    i2c_write(_I2C1, SI114X_ADDR, &Reg, 1);
    i2c_read(_I2C1, SI114X_ADDR, buf, 2);
    return ((uint16_t)buf[0] << 8) | buf[1];
}

uint8_t SI114X_ReadParamData(uint8_t Reg) {
    SI114X_WriteByte(SI114X_COMMAND, Reg | SI114X_QUERY);
    return SI114X_ReadByte(SI114X_RD);
}

uint8_t SI114X_WriteParamData(uint8_t Reg, uint8_t Value) {
    // write Value into PARAMWR reg first
    SI114X_WriteByte(SI114X_WR, Value);
    SI114X_WriteByte(SI114X_COMMAND, Reg | SI114X_SET);
    // SI114X writes value out to PARAM_RD, read and confirm its right
    return SI114X_ReadByte(SI114X_RD);
}

uint16_t SI114X_ReadVisible(void) {
    return SI114X_ReadHalfWord(SI114X_ALS_VIS_DATA0);
}

uint16_t SI114X_ReadIR(void) {
    return SI114X_ReadHalfWord(SI114X_ALS_IR_DATA0);
}

uint16_t SI114X_ReadProximity(uint8_t PSn) {
    return SI114X_ReadHalfWord(PSn);
}

uint16_t SI114X_ReadUV(void) {
    return (SI114X_ReadHalfWord(SI114X_AUX_DATA0_UVINDEX0));
}
