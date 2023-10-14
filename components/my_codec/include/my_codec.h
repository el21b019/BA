#pragma once
#include "my_i2c.h"

esp_err_t codec_init();
void setVolume_HP(uint8_t volume);
void setVolume_Guitar(uint8_t volume);
void setVolume_AUX(uint8_t volume);
void disableGuitar();