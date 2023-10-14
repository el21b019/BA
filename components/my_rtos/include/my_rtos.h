#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "my_i2c.h"
#include "my_codec.h"
#include "driver/i2s_std.h"

#include "esp_dsp.h"
#include <math.h>

#define MENU_TASK_STACK_SIZE 4096
#define MENU_TASK_PRIORITY 1
#define BUTTON_TASK_STACK_SIZE 2048
#define BUTTON_TASK_PRIORITY 1
#define BUTTON_QUEUE_SIZE   2
#define TUNER_TASK_STACK_SIZE 4096
#define TUNER_TASK_PRIORITY 2

#define FFT_SIZE    96000
#define SAMPLE_SIZE 24000
#define BUF_LEN 64  //FOR LEGACY INIT
#define WAVE_BUFF_SIZE 24000
#define SAMPLE_RATE 48000
#define I2S_MCLK GPIO_NUM_26
#define I2S_BCLK GPIO_NUM_25
#define I2S_LRCLK GPIO_NUM_33
#define I2S_WS I2S_LRCLK
#define I2S_DOUT GPIO_NUM_32
#define I2S_DIN GPIO_NUM_34
#define I2S_PORT I2S_NUM_0


enum MenuStates
{
    INIT,
    MENU,
    TUNER,
    VOLUME,
    EFFECTS,
    SETTINGS,

    CLEAR,
    AUX,
    ROUTE
};
enum VolumeStates
{
    VOL_MENU,
    VOL_HEADPHONE,
    VOL_AUX,
    VOL_GUITAR
};

esp_err_t rtos_init();
void button_task(void *arg);
void menu_task(void *arg);
void tuner_task(void *arg);
void draw_main_menu(u8g2_t *display, uint8_t *previousItem, uint8_t *currentItem, uint8_t *nextItem);
void display_init(u8g2_t *display);
void draw_volume_menu(u8g2_t *display, uint8_t *currentVolsel, enum VolumeStates *volume_state, uint8_t *vol_hp, uint8_t *vol_aux, uint8_t *vol_guitar);
void draw_tuner(u8g2_t *display, char note, int deviation);
void codec_i2s_start();
void i2s_std(i2s_chan_handle_t *rx_handle);
//void i2s_legacy(i2s_chan_handle_t *rx_handle);
void tune(float frequency);