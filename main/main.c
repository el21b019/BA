#include "main.h"


static const char *TAG = "MAIN";

void app_main(void)
{
	ESP_LOGI(TAG, "Starting:");

	if (gpio_init() != ESP_OK)
	{
		ESP_LOGE(TAG, "GPIO init FAILED");
		vTaskDelay(pdMS_TO_TICKS(2000));
		esp_restart();
	}
	ESP_LOGI(TAG, "GPIO Setup success");
	vTaskDelay(pdMS_TO_TICKS(100));

	if (i2c_init() != ESP_OK)
	{
		ESP_LOGE(TAG, "I2C init FAILED");
		vTaskDelay(pdMS_TO_TICKS(2000));
		esp_restart();
	}
	ESP_LOGI(TAG, "I2C Setup success");
	vTaskDelay(pdMS_TO_TICKS(100));

	if (codec_init() != ESP_OK)
	{
		ESP_LOGI(TAG, "Codec Setup FAILED");
		vTaskDelay(pdMS_TO_TICKS(2000));
		esp_restart();
	}
	ESP_LOGI(TAG, "Codec Setup success");
	vTaskDelay(pdMS_TO_TICKS(100));
/*
	if (tps_init() != ESP_OK)
	{
		ESP_LOGI(TAG, "TPS Setup FAILED");
		vTaskDelay(pdMS_TO_TICKS(2000));
		esp_restart();
	}
	ESP_LOGI(TAG, "TPS Setup success");
	vTaskDelay(pdMS_TO_TICKS(100));
*/
	if (rtos_init() != ESP_OK)
	{
		ESP_LOGI(TAG, "RTOS Setup FAILED");
		vTaskDelay(pdMS_TO_TICKS(2000));
		esp_restart();
	}
	ESP_LOGI(TAG, "RTOS Setup success");
	vTaskDelay(pdMS_TO_TICKS(100));

}
