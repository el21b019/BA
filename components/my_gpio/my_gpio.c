#include <stdio.h>
#include "my_gpio.h"

static const char *TAG = "GPIO";

/// @brief Inits Button Inputs with PullUp and Reset Pin for Display
/// @return ESP_OK / ESP_FAIL
esp_err_t gpio_init()
{
  esp_err_t ret;
  
  ESP_LOGI(TAG, "Starting GPIO initializtion:");

  //--------------------------------------------------------------------------------
  // For Display
  // Display reset IO 27 active low
  //--------------------------------------------------------------------------------
  gpio_config_t gpio_conf_display;
  gpio_conf_display.pin_bit_mask = (uint64_t)(1 << GPIO_NUM_27);
  gpio_conf_display.mode = GPIO_MODE_OUTPUT;
  gpio_conf_display.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_conf_display.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_conf_display.intr_type = GPIO_INTR_DISABLE;
  ret = gpio_config(&gpio_conf_display);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "gpio_setup failure Display !\n");
    return ESP_FAIL;
  }
  ret = gpio_set_level(GPIO_NUM_27, 1);

  //--------------------------------------------------------------------------------
  // For Button
  //--------------------------------------------------------------------------------
  gpio_config_t gpio_conf_button;
  gpio_conf_button.intr_type = GPIO_INTR_DISABLE;
  gpio_conf_button.mode = GPIO_MODE_INPUT;
  gpio_conf_button.pin_bit_mask = (uint64_t)(1 << BUTTON_L1) | (uint64_t)(1 << BUTTON_L2) |
                                  (uint64_t)(1 << BUTTON_R1) | (uint64_t)(1 << BUTTON_R2) |
                                  (uint64_t)(1 << BUTTON_DOWN);
  gpio_conf_button.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_conf_button.pull_up_en = GPIO_PULLUP_ENABLE;
  ret = gpio_config(&gpio_conf_button);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "gpio_setup failure Buttons !\n");
    return ESP_FAIL;
  }

  return ret;
}
