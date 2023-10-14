#pragma once 
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"


#define ACK_CHECK_EN 0x1 /*!< I2C master will check ack from slave*/

#define I2C_MASTER_SDA_IO GPIO_NUM_23
#define I2C_MASTER_SCL_IO GPIO_NUM_22
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 325000
#define I2C_DISPLAY 0x3C
#define I2C_CODEC 0x10
#define I2C_POWER 0x48


esp_err_t i2c_init();
esp_err_t i2c_reg_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
esp_err_t i2c_write_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
esp_err_t i2c_codec_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
esp_err_t i2c_write_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
esp_err_t i2c_codec_write_n_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t data[], uint8_t numberOfBytes);
void i2c_codec_rgister_dump();
esp_err_t tps_init();