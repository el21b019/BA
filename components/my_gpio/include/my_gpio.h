#pragma once
#include <freertos/FreeRTOS.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#define BUTTON_L2 GPIO_NUM_17
#define BUTTON_L1 GPIO_NUM_16
#define BUTTON_DOWN GPIO_NUM_5
#define BUTTON_R1 GPIO_NUM_19
#define BUTTON_R2 GPIO_NUM_18
#define PIN_RELEASED    1
#define PIN_PRESSED     0
#define BUTTON_POLL_RATE    100



// Button state enumeration
typedef enum ButtonState
{
    PRESSED,
    RELEASED
};
// Button structure to store button information
typedef struct Button
{
    gpio_num_t pin;
    enum ButtonState current_state;
    enum ButtonState last_state;
    TickType_t lastChangeTick;
} ;

esp_err_t gpio_init();