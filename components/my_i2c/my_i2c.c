#include "my_i2c.h"

//----------------------------------I2C-------------------------------------------
/// @brief Set up I2C Controller 0 in Master Mode and installs it
/// @return ESP_OK / ESP_FAIL
esp_err_t i2c_init()
{

  esp_err_t ret;

  i2c_config_t con;
  con.mode = I2C_MODE_MASTER;
  con.sda_io_num = I2C_MASTER_SDA_IO; // select SDA GPIO specific to your project
  con.sda_pullup_en = GPIO_PULLUP_ENABLE;
  con.scl_io_num = I2C_MASTER_SCL_IO; // select SCL GPIO specific to your project
  con.scl_pullup_en = GPIO_PULLUP_ENABLE;
  con.master.clk_speed = I2C_MASTER_FREQ_HZ; // select frequency specific to your project
  con.clk_flags = 0;                         // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here

  ret = i2c_param_config(I2C_NUM_0, &con);
  if (ret != ESP_OK)
  {
    ESP_LOGE("MAIN_I2C_INIT", "I2C Parameter config Fail");
    return ESP_FAIL;
  }

  ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_LEVEL1);
  if (ret != ESP_OK)
  {
    ESP_LOGE("MAIN_I2C_INIT", "I2C Parameter config Fail");
    return ESP_FAIL;
  }
  //--------------------------------------------------------------------------------
  //--------------------------------------Power-ON I2C Selftest---------------------
  //--------------------------------------------------------------------------------
  vTaskDelay(pdMS_TO_TICKS(100));
  uint8_t data = 0xff;
 
 /* i2c_reg_read(I2C_POWER, 0x0E, &data, 1);
  if (data != 0x88)
  {
    ESP_LOG_BUFFER_HEX("Power register 0x0E not responding Correct (POR:0x88)", &data, 1);
    return ESP_FAIL;
  }
*/
  i2c_codec_read(I2C_CODEC, 0xff, &data, 1);
  if (data != 0x43)
  {
    ESP_LOG_BUFFER_HEX("Codec revision ID incorrect (POR:0x43)", &data, 1);
    return ESP_FAIL;
  }
  return ret;
}

/// @brief            Reads Data from a Slave
/// @param slave_addr I2C Adress of the Slave to adress - gets shifted one to left 
/// @param reg_addr   Adress of the register to read from 
/// @param data       Pointer to the receiving data
/// @param len        How many bytes to be read. 
///                   *Size of the data variable must not be smaller
/// @return           ESP_ERROR Type
esp_err_t i2c_reg_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
  esp_err_t ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_addr << 1), ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  if (ret != ESP_OK)
  {
    return ret;
  }
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_addr << 1) | 1, ACK_CHECK_EN);
  i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return ret;
}

/// @brief            Reads Data from the codec - second start bit
/// @param slave_addr I2C Adress of the Slave to adress - gets shifted one to left 
/// @param reg_addr   Adress of the register to read from 
/// @param data       Pointer to the receiving data
/// @param len        How many bytes to be read. 
///                   *Size of the data variable must not be smaller
/// @return           ESP_ERROR Type
esp_err_t i2c_codec_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
  esp_err_t ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_addr << 1), ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_addr << 1) | 1, ACK_CHECK_EN);
  i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return ret;
}

/// @brief            Wiretes one Byte to a register on a slave
/// @param slave_addr I2C Adress of the Slave to adress - gets shifted one to left 
/// @param reg_addr   Adress of the register to write to 
/// @param data       Data to be written
/// @return           ESP_ERROR Type
esp_err_t i2c_write_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
  esp_err_t ret = ESP_FAIL;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_addr << 1), ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return ret;
}

/// @brief                Wiretes multiple Bytes to a register on Codec a slave 
///                       The Register Adress will increment on codec automatically
/// @param slave_addr     I2C Adress of the Slave to adress - gets shifted one to left 
/// @param reg_addr       Adress of the register to write to 
/// @param data           Array of Data
/// @param numberOfBytes  Number of Bytes in Array to be written
/// @return               ESP_ERROR Type
esp_err_t i2c_codec_write_n_bytes(uint8_t slave_addr, uint8_t reg_addr, uint8_t data[], uint8_t numberOfBytes)
{
  esp_err_t ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_addr << 1), ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);
  for (size_t i = 0; i < numberOfBytes; i++)
  {
    i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
  }
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return ret;
}

/// @brief    Initializes the TPS Chip - Sets VDLO to 3.25V
/// @return   ESP_ERROR Type
esp_err_t tps_init()
{
  //set VDCDC1 to standard and UVLO to 3.25 to trigger recharge faster
  i2c_write_byte(I2C_POWER, 0x0C, (0x33 | (0b11<<5)));
  uint8_t data = 0xFF;
  if (i2c_reg_read(I2C_POWER, 0x0C, &data,1) == (0x33 | (0b11<<5)) )
    return ESP_OK;
  else
    return ESP_FAIL;
  
}

//--------------------------------------------------------------------------------
/// @brief Reads the register form 0x00 to 0xBD from Codec
void i2c_codec_rgister_dump()
{
  uint8_t data = 0xff;
  for (size_t i = 0; i <= 0xBD; i++)
  {
    i2c_codec_read(I2C_CODEC, i, &data, 1);
    ESP_LOGW("Codec", "Register 0x%02x: 0x%02x", i, data);
    data = 0xff;
  }
}

