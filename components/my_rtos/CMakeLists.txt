idf_component_register(SRCS "u8g2_esp32_hal.c" "my_rtos.c"
                    INCLUDE_DIRS "include"
                    REQUIRES freertos my_gpio my_i2c u8g2 my_codec esp-dsp)
