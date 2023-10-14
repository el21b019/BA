#include <stdio.h>
#include "my_rtos.h"
#include "my_gpio.h"

//////////////////////////////////////////////////////////////////////////////////
// 'settings', 16x16px
const unsigned char bmp_settings[] = {
    0x00, 0x00, 0x31, 0x8c, 0x31, 0x9e, 0x31, 0x80, 0x31, 0x9e, 0x31, 0x8c, 0x79, 0x8c, 0x01, 0x8c,
    0x79, 0x8c, 0x33, 0xcc, 0x30, 0x0c, 0x33, 0xcc, 0x31, 0x8c, 0x31, 0x8c, 0x31, 0x8c, 0x00, 0x00};
// 'tuner', 16x16px
const unsigned char bmp_tuner[] = {
    0x00, 0x00, 0x0e, 0x00, 0x30, 0x00, 0x23, 0x00, 0x44, 0x40, 0x48, 0x80, 0x49, 0x10, 0x02, 0x20,
    0x04, 0x4a, 0x04, 0x8a, 0x03, 0x12, 0x08, 0x64, 0x10, 0x0c, 0x20, 0x70, 0x40, 0x00, 0x00, 0x00};
// 'volume', 16x16px
const unsigned char bmp_volume[] = {
    0x00, 0x18, 0x00, 0x0c, 0x00, 0x06, 0x02, 0x62, 0x06, 0x32, 0x0e, 0x12, 0x7e, 0x92, 0x7e, 0x52,
    0x7e, 0x52, 0x7e, 0x92, 0x0e, 0x12, 0x06, 0x32, 0x02, 0x62, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x18};
// '_aux', 16x16px
const unsigned char bmp_aux[] = {
    0x00, 0x00, 0x7f, 0x00, 0x7f, 0x68, 0x7f, 0x6a, 0x7f, 0x68, 0x7f, 0x00, 0x78, 0x00, 0x30, 0x00,
    0x78, 0x00, 0x30, 0x00, 0x78, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00};
// 'effects', 16x16px
const unsigned char bmp_effects[] = {
    0x20, 0x04, 0x03, 0xc0, 0x9c, 0x39, 0x10, 0x08, 0x20, 0x04, 0x64, 0x06, 0x4a, 0x02, 0x51, 0x12,
    0x51, 0x12, 0x40, 0xa2, 0x60, 0x46, 0x20, 0x04, 0x10, 0x08, 0x9c, 0x39, 0x03, 0xc0, 0x20, 0x04};
// 'guitar', 16x16px
const unsigned char bmp_guitar[] = {
    0x00, 0x00, 0x00, 0x06, 0x01, 0x0e, 0x02, 0x9c, 0x02, 0xb8, 0x02, 0x70, 0x0c, 0x18, 0x31, 0x04,
    0x40, 0x84, 0x44, 0x0c, 0x52, 0x70, 0x48, 0x80, 0x25, 0x00, 0x11, 0x00, 0x0e, 0x00, 0x00, 0x00};
// 'headphones', 16x16px
const unsigned char bmp_headphones[] = {
    0x00, 0x00, 0x03, 0xc0, 0x0c, 0x30, 0x10, 0x08, 0x20, 0x04, 0x60, 0x06, 0x40, 0x02, 0x40, 0x02,
    0x40, 0x02, 0x6c, 0x36, 0x6c, 0x36, 0x6c, 0x36, 0x6c, 0x36, 0x6c, 0x36, 0x2c, 0x34, 0x00, 0x00};
// 'back', 16x16px
const unsigned char bmp_back[] = {
    0x00, 0x00, 0x01, 0xe0, 0x07, 0xf8, 0x00, 0x7c, 0x00, 0x1c, 0x00, 0x0c, 0x00, 0x0c, 0x03, 0x0c,
    0x0e, 0x0c, 0x1c, 0x1c, 0x3f, 0xfc, 0x3f, 0xf8, 0x1c, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00};
// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 336)
const int bmp_allArray_LEN = 8;
const unsigned char *bmp_allArray[8] = {
    bmp_tuner,
    bmp_volume,
    bmp_effects,
    bmp_settings,
    bmp_aux,
    bmp_guitar,
    bmp_headphones,
    bmp_back};
const uint8_t numberOfMainItems = 4;
const uint8_t itemNames[][11] =
    {
        {"Tuner"},
        {"Volume"},
        {"Effects"},
        {"Settings"},
        {"AUX"},
        {"Guitar"},
        {"Headphones"}};

//////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------
//----------------------------------RTOS------------------------------------------
TaskHandle_t xMenuTask = NULL;
TaskHandle_t xButtonTask = NULL;
TaskHandle_t xTunerTask = NULL;
QueueHandle_t q_button_events = NULL;
bool tuner_flag = false;

/// @brief This Task debounces the Buttons and puts the debounced button in a queue /Rising edge
void button_task(void *arg)
{
  ESP_LOGI("BUTTON_TASK_INIT", "SETUP");
  // Array of button structures
  struct Button buttons_states[5] = {
      {BUTTON_L2, RELEASED, RELEASED, 0},
      {BUTTON_L1, RELEASED, RELEASED, 0},
      {BUTTON_DOWN, RELEASED, RELEASED, 0},
      {BUTTON_R1, RELEASED, RELEASED, 0},
      {BUTTON_R2, RELEASED, RELEASED, 0}};

  bool L2_pressed, R2_pressed;
  L2_pressed = R2_pressed = false;
  for (;;)
  {

    // set new old state
    for (size_t i = 0; i < 5; i++)
      buttons_states[i].last_state = buttons_states[i].current_state;

    buttons_states[0].current_state = gpio_get_level(BUTTON_L2);
    buttons_states[1].current_state = gpio_get_level(BUTTON_L1);
    buttons_states[2].current_state = gpio_get_level(BUTTON_DOWN);
    buttons_states[3].current_state = gpio_get_level(BUTTON_R1);
    buttons_states[4].current_state = gpio_get_level(BUTTON_R2);

    //----------------------------------------------------------------------------------------------------
    // if R2 or L2 are released - reset last change tick timer
    // and handle double event avoidance
    //----------------------------------------------------------------------------------------------------
    if ((buttons_states[0].current_state == RELEASED) && (buttons_states[0].last_state == PRESSED))
    {
      buttons_states[0].lastChangeTick = 0;
      if (L2_pressed)
        buttons_states[1].current_state = buttons_states[1].last_state = RELEASED;
      L2_pressed = false;
    }
    if ((buttons_states[4].current_state == RELEASED) && (buttons_states[4].last_state == PRESSED))
    {
      buttons_states[4].lastChangeTick = 0;
      if (R2_pressed)
        buttons_states[3].current_state = buttons_states[3].last_state = RELEASED;
      R2_pressed = false;
    }

    //----------------------------------------------------------------------------------------------------
    // if L1, Down or R1 are released - push event
    //----------------------------------------------------------------------------------------------------
    for (size_t i = 1; i < 4; i++)
    {
      if ((buttons_states[i].current_state == RELEASED) && (buttons_states[i].last_state == PRESSED))
      {
        // set tick of change
        buttons_states[i].lastChangeTick = xTaskGetTickCount();

        if (uxQueueSpacesAvailable(q_button_events) && (i >= 1 && i <= 4))
        {
          xQueueSend(q_button_events, &buttons_states[i].pin, 100);
          if (i == 0)
            ESP_LOGI("BUTTON_TASK", "Button added to Queue: L2");
          else if (i == 1)
            ESP_LOGI("BUTTON_TASK", "Button added to Queue: L1");
          else if (i == 2)
            ESP_LOGI("BUTTON_TASK", "Button added to Queue: DOWN");
          else if (i == 3)
            ESP_LOGI("BUTTON_TASK", "Button added to Queue: R1");
          else if (i == 4)
            ESP_LOGI("BUTTON_TASK", "Button added to Queue: R2");
        }
        else
        {
          ESP_LOGW("BUTTON TASK", "No space in Button Queue!");
          if (i == 0)
            ESP_LOGW("BUTTON_TASK", "Button event dropped: L2");
          else if (i == 1)
            ESP_LOGW("BUTTON_TASK", "Button event dropped: L1");
          else if (i == 2)
            ESP_LOGW("BUTTON_TASK", "Button event dropped: DOWN");
          else if (i == 3)
            ESP_LOGW("BUTTON_TASK", "Button event dropped: R1");
          else if (i == 4)
            ESP_LOGW("BUTTON_TASK", "Button event dropped: R2");
        }
      }
    }
    //----------------------------------------------------------------------------------------------------
    // if R2 or L2 are held - push event in queue
    //----------------------------------------------------------------------------------------------------
    if (((buttons_states[0].current_state == PRESSED) && (buttons_states[0].last_state == PRESSED)) &&
        ((xTaskGetTickCount() - buttons_states[0].lastChangeTick) > 50))
    {
      if (uxQueueSpacesAvailable(q_button_events))
      {
        L2_pressed = true;
        buttons_states[0].lastChangeTick = xTaskGetTickCount();
        xQueueSend(q_button_events, &buttons_states[0].pin, 100);
        ESP_LOGI("BUTTON_TASK", "Button added to Queue from Hold: L2");
      }
      else
      {
        ESP_LOGW("BUTTON TASK", "No space in Button Queue!");
      }
    }

    if (((buttons_states[4].current_state == PRESSED) && (buttons_states[4].last_state == PRESSED)) &&
        ((xTaskGetTickCount() - buttons_states[4].lastChangeTick) > 50))
    {
      if (uxQueueSpacesAvailable(q_button_events))
      {
        R2_pressed = true;
        buttons_states[4].lastChangeTick = xTaskGetTickCount();
        xQueueSend(q_button_events, &buttons_states[4].pin, 100);
        ESP_LOGI("BUTTON_TASK", "Button added to Queue from Hold: R2");
      }
      else
      {
        ESP_LOGW("BUTTON TASK", "No space in Button Queue!");
      }
    }

    //----------------------------------------------------------------------------------------------------
    // if R2 or L2 are pressed down - start timing
    //----------------------------------------------------------------------------------------------------
    if ((buttons_states[0].current_state == PRESSED) && (buttons_states[0].last_state == RELEASED))
      buttons_states[0].lastChangeTick = xTaskGetTickCount();
    if ((buttons_states[4].current_state == PRESSED) && (buttons_states[4].last_state == RELEASED))
      buttons_states[4].lastChangeTick = xTaskGetTickCount();

    //----------------------------------------------------------------------------------------------------

    vTaskDelay(BUTTON_POLL_RATE / portTICK_PERIOD_MS);
  }
}
/// @brief Handels the Main Menu, controlls Display, and sends Commands to Codec - according to Menu state and Button input
void menu_task(void *arg)
{
  u8g2_t display; // a structure which will contain all the data for one display
  display_init(&display);

  enum MenuStates menu_state = INIT;
  gpio_num_t new_button = -1;
  //-------------------------------------------MAIN MENU
  uint8_t currentItem, previousItem, nextItem;
  previousItem = 0;
  currentItem = 1;
  nextItem = 2;
  //-------------------------------------------VOLUME MENU
  uint8_t currentVol = 0;
  enum VolumeStates volume_state = VOL_MENU;
  uint8_t volume_hp_out = 0x17;
  uint8_t volume_aux_in = 2 - 1;      //-3dB
  uint8_t volume_guitar_in = 50 - 30; //+20dB PreAmp
  //-------------------------------------------TUNER
  uint16_t targetFreq = 0;
  uint16_t actualFreq = 0;
  for (;;)
  {
    new_button = -1;

    switch (menu_state)
    {
    case INIT:
      //----------------------------------------------------------------------------------------------------
      // init display in this state
      //----------------------------------------------------------------------------------------------------
      if (uxQueueMessagesWaiting(q_button_events))
      {
        xQueueReceive(q_button_events, &new_button, 500);

        menu_state = MENU;
        new_button = -1;
        draw_main_menu(&display, &previousItem, &currentItem, &nextItem);
      }

      break;
    case MENU:
      //----------------------------------------------------------------------------------------------------
      // MAIN MENU                                 MAIN MENU                                 MAIN MENU
      //----------------------------------------------------------------------------------------------------
      if (uxQueueMessagesWaiting(q_button_events))
      {
        xQueueReceive(q_button_events, &new_button, 500);

        if ((new_button == BUTTON_L1) || (new_button == BUTTON_L2))
        {
          nextItem = currentItem;
          currentItem = previousItem;
          if (previousItem > 0)
            previousItem--;
          else
            previousItem = numberOfMainItems - 1;
        }
        else if ((new_button == BUTTON_R1) || (new_button == BUTTON_R2))
        {
          previousItem = currentItem;
          currentItem = nextItem;
          nextItem++;
          if (nextItem >= numberOfMainItems)
            nextItem = 0;
        }
        else if (new_button == BUTTON_DOWN)
        {
          if (currentItem == 0)
            menu_state = TUNER;
          else if (currentItem == 1)
            menu_state = VOLUME;
          else if (currentItem == 2)
            menu_state = EFFECTS;
          else if (currentItem == 3)
            menu_state = SETTINGS;
          ESP_LOGI("MENU-TASK_MAIN-MENU", "New Menu State is: %s", itemNames[currentItem]);
        }
        new_button = -1;
        draw_main_menu(&display, &previousItem, &currentItem, &nextItem);
      }
      break;
    case TUNER:
      //----------------------------------------------------------------------------------------------------
      // TUNER                                 TUNER                                 TUNER
      //----------------------------------------------------------------------------------------------------
      if (uxQueueMessagesWaiting(q_button_events))
      {
        xQueueReceive(q_button_events, &new_button, 500);
        if (new_button == BUTTON_DOWN)
        {
          menu_state = MENU;
          ESP_LOGI("MENU-TASK_TUNER", "New Menu State is: MENU");
          new_button = -1;
          draw_main_menu(&display, &previousItem, &currentItem, &nextItem);
          break;
        }
      }

      // start tuner task
      tuner_flag = true;
      // xTaskCreate(tuner_task, "TunerTask", TUNER_TASK_STACK_SIZE, NULL, TUNER_TASK_PRIORITY, &xTunerTask);
      //  start task
      //  input for chromatic and Guitar and Bass
      //  get fft data from tuner task queue
      //  draw tuner interface
      //  end tuner task when back to menu
      //  end i2s when back to menu

      draw_tuner(&display, targetFreq, actualFreq);

      break;
    case VOLUME:
      //----------------------------------------------------------------------------------------------------
      // VOLUME                                 VOLUME                                 VOLUME
      //----------------------------------------------------------------------------------------------------
      if (uxQueueMessagesWaiting(q_button_events))
        xQueueReceive(q_button_events, &new_button, 500);
      if (new_button != -1)
      {
        if ((new_button == BUTTON_L1) || (new_button == BUTTON_L2))
        {
          if (volume_state == VOL_MENU)
          {
            if (currentVol > 0)
              currentVol--;
            else
              currentVol = 3;
          }
          else if (volume_state == VOL_AUX)
          {
            // volume up
            if (volume_aux_in < 2)
            {
              volume_aux_in++;
              ESP_LOGI("BUTTON-TASK-Volume", "New AUX Volume:%d", volume_aux_in);
              // call volume up function
              setVolume_AUX(volume_aux_in);
            }
          }
          else if (volume_state == VOL_HEADPHONE)
          {
            if (new_button == BUTTON_L2)
            {
              // volume up
              if (volume_hp_out < 31)
              {
                if (volume_hp_out < 29)
                  volume_hp_out += 3;
                else
                  volume_hp_out++;

                ESP_LOGI("BUTTON-TASK-Volume", "New HP Volume:%d", volume_hp_out);
                // call volume up function
                setVolume_HP(volume_hp_out);
              }
            }
            else if (new_button == BUTTON_L1)
            {
              // volume up
              if (volume_hp_out < 0x1F)
              {
                volume_hp_out++;
                ESP_LOGI("BUTTON-TASK-Volume", "New HP Volume:%d", volume_hp_out);
                // call volume up function
                setVolume_HP(volume_hp_out);
              }
            }
          }
          else if (volume_state == VOL_GUITAR)
          {
            if (new_button == BUTTON_L2)
            {
              // volume up
              if (volume_guitar_in < 50)
              {
                if (volume_guitar_in < 48)
                  volume_guitar_in += 3;
                else
                  volume_guitar_in++;

                ESP_LOGI("BUTTON-TASK-Volume", "New Guitar Volume:%d", volume_guitar_in);
                // call volume up function
                setVolume_Guitar(volume_guitar_in);
              }
            }
            else if (new_button == BUTTON_L1)
            {
              // volume up
              if (volume_guitar_in < 50)
              {
                volume_guitar_in++;
                ESP_LOGI("BUTTON-TASK-Volume", "New Guitar Volume:%d", volume_guitar_in);
                // call volume up function
                setVolume_Guitar(volume_guitar_in);
              }
            }
          }
        }
        else if ((new_button == BUTTON_R1) || (new_button == BUTTON_R2))
        {
          if (volume_state == VOL_MENU)
          {
            if (currentVol == 3)
              currentVol = 0;
            else
              currentVol++;
          }
          else if (volume_state == VOL_AUX)
          {
            // volume down
            if (volume_aux_in > 0)
            {
              volume_aux_in--;
              ESP_LOGI("BUTTON-TASK-Volume", "New AUX Volume:%d", volume_aux_in);
              // call volume down function
              setVolume_AUX(volume_aux_in);
            }
          }
          else if (volume_state == VOL_HEADPHONE)
          {
            if (new_button == BUTTON_R2)
            {
              // volume down
              if (volume_hp_out > 0)
              {
                if (volume_hp_out > 2)
                  volume_hp_out -= 3;
                else
                  volume_hp_out--;

                ESP_LOGI("BUTTON-TASK-Volume", "New HP Volume:%d", volume_hp_out);
                // call volume down function
                setVolume_HP(volume_hp_out);
              }
            }

            else if (new_button == BUTTON_R1)
            {
              // volume down
              if (volume_hp_out > 0)
              {
                volume_hp_out--;
                ESP_LOGI("BUTTON-TASK-Volume", "New HP Volume:%d", volume_hp_out);
                // call volume down function
                setVolume_HP(volume_hp_out);
              }
            }
          }
          else if (volume_state == VOL_GUITAR)
          {
            if (new_button == BUTTON_R2)
            {
              // volume down
              if (volume_guitar_in > 0)
              {
                if (volume_guitar_in > 2)
                  volume_guitar_in -= 3;
                else
                  volume_guitar_in--;

                ESP_LOGI("BUTTON-TASK-Volume", "New Guitar Volume:%d", volume_guitar_in);
                // call volume down function
                setVolume_Guitar(volume_guitar_in);
              }
            }

            else if (new_button == BUTTON_R1)
            {
              // volume down
              if (volume_guitar_in > 0)
              {
                volume_guitar_in--;
                ESP_LOGI("BUTTON-TASK-Volume", "New Guitar Volume:%d", volume_guitar_in);
                // call volume down function
                setVolume_Guitar(volume_guitar_in);
              }
            }
          }
        }
        else if (new_button == BUTTON_DOWN)
        {
          if (volume_state == VOL_MENU)
          {
            if (currentVol == 0)
              volume_state = VOL_HEADPHONE;
            else if (currentVol == 1)
              volume_state = VOL_AUX;
            else if (currentVol == 2)
              volume_state = VOL_GUITAR;
            else if (currentVol == 3)
            {
              menu_state = MENU;
              currentVol = 0;
              draw_main_menu(&display, &previousItem, &currentItem, &nextItem);
              new_button = -1;
              break;
            }
          }
          else
            volume_state = VOL_MENU;
        }

        new_button = -1;
      }

      draw_volume_menu(&display, &currentVol, &volume_state, &volume_hp_out, &volume_aux_in, &volume_guitar_in);

      break;
    case EFFECTS:
      //----------------------------------------------------------------------------------------------------
      // EFFECTS                                 EFFECTS                               EFFECTS
      //----------------------------------------------------------------------------------------------------
      if (uxQueueMessagesWaiting(q_button_events))
      {
        xQueueReceive(q_button_events, &new_button, 500);
        if ((new_button == BUTTON_L1) || (new_button == BUTTON_L2))
        {
        }
        else if ((new_button == BUTTON_R1) || (new_button == BUTTON_R2))
        {
        }
        else if (new_button == BUTTON_DOWN)
        {
          menu_state = MENU;
          draw_main_menu(&display, &previousItem, &currentItem, &nextItem);
          new_button = -1;
          break;
        }
      }

      u8g2_ClearBuffer(&display);
      u8g2_SetFont(&display, u8g2_font_7x14B_mr);
      u8g2_DrawStr(&display, 2, 15, "Effects Menu");
      u8g2_SendBuffer(&display);
      break;
    case SETTINGS:
      //----------------------------------------------------------------------------------------------------
      // SETTINGS                                 SETTINGS                               SETTINGS
      //----------------------------------------------------------------------------------------------------
      if (uxQueueMessagesWaiting(q_button_events))
      {
        xQueueReceive(q_button_events, &new_button, 500);
        if ((new_button == BUTTON_L1) || (new_button == BUTTON_L2))
        {
        }
        else if ((new_button == BUTTON_R1) || (new_button == BUTTON_R2))
        {
        }
        else if (new_button == BUTTON_DOWN)
        {
          menu_state = MENU;
          draw_main_menu(&display, &previousItem, &currentItem, &nextItem);
          new_button = -1;
          break;
        }
      }

      u8g2_ClearBuffer(&display);
      u8g2_SetFont(&display, u8g2_font_7x14B_mr);
      u8g2_DrawStr(&display, 2, 15, "Settings Menu");
      u8g2_SendBuffer(&display);
      break;

    default:
      menu_state = MENU;
      break;
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
/// @brief Set up the Codec for I2S, fft samples and calculates string deviation from tone
void tuner_task(void *arg)
{
  static const char *TAG = "TUNER-TASK";
  i2s_chan_handle_t i2s_rx = NULL;

  codec_i2s_start();
  i2s_std(&i2s_rx);

  float max_magnitude = 0;
  int max_index = 0;
  float *fft_table_buff = NULL;
  esp_err_t init_result;

  // Use calloc to allocate memory and initialize to zero
  float *input_data = (float *)calloc(FFT_SIZE, sizeof(float)); // Buffer for FFT input (zero-padded)

  init_result = dsps_fft2r_init_fc32(fft_table_buff, FFT_SIZE);
  if (init_result != ESP_OK)
  {
    ESP_LOGE(TAG, "DSP FFT INIT Fail: %d", (int)init_result);
    return;
  }
  for (;;)
  {

    for (uint16_t i = 0; i < SAMPLE_SIZE; i++)
    {
      // queue pop to input_data[i];
      // now sample is zero padded
    }

    esp_err_t fft_result = dsps_fft2r_fc32_ae32_(input_data, FFT_SIZE, fft_table_buff);
    if (fft_result != ESP_OK)
    {
      ESP_LOGE(TAG, "DSP FFT Calculation Fail: %d", (int)fft_result);
      return;
    }

    // Find Frequency with Highest Amplitude
    for (int i = 0; i < FFT_SIZE / 2; i++)
    {
      float magnitude = sqrt(input_data[2 * i] * input_data[2 * i] + input_data[2 * i + 1] * input_data[2 * i + 1]);
      if (magnitude > max_magnitude)
      {
        max_magnitude = magnitude;
        max_index = i;
      }
    }

    // Convert index to frequency
    float frequency = (float)max_index * SAMPLE_RATE / FFT_SIZE;

    tune(frequency);
    if (!tuner_flag)
      break;

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  dsps_fft2r_deinit_fc32();
  free(input_data);
  /*{

    // set up i2s

    i2s_chan_handle_t rx_handle;
    i2s_init_std(& rx_handle);
    //i2s_init_legcy(& rx_handle);
    //i2s_start(I2S_PORT);

    int16_t *i2s_buf_r = (int16_t *)calloc(WAVE_BUFF_SIZE, sizeof(int16_t));
    // Before reading data, start the RX channel first
    i2s_channel_enable(rx_handle);

    size_t i2s_bytes_read = 0;

    // Window coefficients
    __attribute__((aligned(16))) float wind[WAVE_BUFF_SIZE];
    //int16_t *spectrum = (int16_t *)calloc(WAVE_BUFF_SIZE, sizeof(int16_t));
    unsigned int start_fft;
    unsigned int end_fft;
    esp_err_t ret;

    ESP_LOGI(TAG, "Start FFT init.");
    ret = dsps_fft2r_init_sc16(NULL, WAVE_BUFF_SIZE >> 1);
    if (ret != ESP_OK)
    {
      ESP_LOGE(TAG, "Not possible to initialize FFT2R. Error = %i", ret);
      return;
    }
    // Generate hann window
    dsps_wind_hann_f32(wind, WAVE_BUFF_SIZE);

    for (;;)
    {
      i2s_channel_read(rx_handle, i2s_buf_r, WAVE_BUFF_SIZE, &i2s_bytes_read, 510);
      // perform fft on i2s buffer
      // apply window on signal
      for (uint16_t i = 0; i < WAVE_BUFF_SIZE; i++)
        i2s_buf_r[i] = i2s_buf_r[i] * wind[i];

      // FFT Radix 2
      start_fft = dsp_get_cpu_cycle_count();
      dsps_fft2r_sc16_ae32(i2s_buf_r, WAVE_BUFF_SIZE >> 1);
      // Bit reverse
      dsps_bit_rev_sc16_ansi(i2s_buf_r, WAVE_BUFF_SIZE >> 1);
      // Convert one complex vector with length N/2 to one real spectrum vector with length N/2
      dsps_cplx2reC_sc16(i2s_buf_r, WAVE_BUFF_SIZE >> 1);
      end_fft = dsp_get_cpu_cycle_count();

      for (int i = 0 ; i < WAVE_BUFF_SIZE/2 ; i++) {
          i2s_buf_r[i] = 10 * log10f((i2s_buf_r[i * 2 + 0] * i2s_buf_r[i * 2 + 0] + i2s_buf_r[i * 2 + 1] * i2s_buf_r[i * 2 + 1] + 0.0000001)/WAVE_BUFF_SIZE);
      ESP_LOGW(TAG, "Signal x1");
      dsps_view(&i2s_buf_r, WAVE_BUFF_SIZE/2, 64, 10,  -60, 40, '|');

      // communicate strongest frequency to other taskda
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    // Have to stop the channel before deleting it
    i2s_channel_disable(rx_handle);
    // If the handle is not needed any more, delete it to release the channel resources
    i2s_del_channel(rx_handle);*/
}
//----------------------------------------------------------------------------------------------------
/// @brief Assign Queues, Buffers, Semaphores, etc. to their handle pointer
/// @return ESP_ERROR Type
esp_err_t rtos_init()
{
  vTaskDelay(pdMS_TO_TICKS(300));

  q_button_events = xQueueCreate(BUTTON_QUEUE_SIZE, sizeof(gpio_num_t));
  if (q_button_events == NULL)
    return ESP_FAIL;

  xTaskCreate(menu_task, "MenuTask", MENU_TASK_STACK_SIZE, NULL, MENU_TASK_PRIORITY, &xMenuTask);
  xTaskCreate(button_task, "ButtonTask", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, &xButtonTask);

  return ESP_OK;
}

//----------------------------------------------------------------------------------------------------

/// @brief              Draws Main Menu
/// @param display      Dispaly Handle
/// @param previousItem Index of previous Item in itemNames and bmp_allArray
/// @param currentItem  index of current Item in itemNames and bmp_allArray
/// @param nextItem     Index of next Item in itemNames and bmp_allArray
void draw_main_menu(u8g2_t *display, uint8_t *previousItem, uint8_t *currentItem, uint8_t *nextItem)
{
  u8g2_ClearBuffer(display);
  // draw previous item
  u8g2_SetFont(display, u8g2_font_7x14_mr);
  u8g2_DrawStr(display, 26, 15, &itemNames[*previousItem]);
  u8g2_DrawBitmap(display, 5, 2, 16 / 8, 16, bmp_allArray[*previousItem]);

  // draw current item
  u8g2_SetFont(display, u8g2_font_8x13B_mr);
  u8g2_DrawStr(display, 26, 37, &itemNames[*currentItem]);
  u8g2_DrawBitmap(display, 5, 24, 16 / 8, 16, bmp_allArray[*currentItem]);

  // draw top line
  u8g2_DrawLine(display, 1, 22, 90, 22);
  // draw bottom lines
  u8g2_DrawLine(display, 1, 41, 91, 41);
  u8g2_DrawLine(display, 2, 42, 90, 42);
  // draw left lines
  u8g2_DrawLine(display, 0, 23, 0, 40);
  // draw right lines
  u8g2_DrawLine(display, 91, 23, 91, 40);
  u8g2_DrawLine(display, 92, 24, 92, 40);

  // draw next item
  u8g2_SetFont(display, u8g2_font_7x14_mr);
  u8g2_DrawStr(display, 26, 59, &itemNames[*nextItem]);
  u8g2_DrawBitmap(display, 5, 46, 16 / 8, 16, bmp_allArray[*nextItem]);

  u8g2_SendBuffer(display);
}

/// @brief         Initializes HAL for U8G2 Library and draws INIT Screen
/// @param display Display Handle
void display_init(u8g2_t *display)
{
  ESP_LOGI("MENU_TASK_INIT", "SETUP");
  u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
  u8g2_esp32_hal.sda = I2C_MASTER_SDA_IO;
  u8g2_esp32_hal.scl = I2C_MASTER_SCL_IO;
  u8g2_esp32_hal_init(u8g2_esp32_hal);
  u8g2_Setup_ssd1306_i2c_102x64_ea_oleds102_f(
      display,
      // U8G2_R0,
      U8G2_R2,
      u8g2_esp32_i2c_byte_cb,
      u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
  u8g2_SetI2CAddress(display, 0x78);

  ESP_LOGI("MENU_TASK_INIT", "u8g2_InitDisplay");
  gpio_set_level(GPIO_NUM_27, 1);
  u8g2_InitDisplay(display); // send init sequence to the display, display is in sleep mode after this,
  ESP_LOGI("MENU_TASK_INIT", "u8g2_SetPowerSave");
  u8g2_SetPowerSave(display, 0); // wake up display
  ESP_LOGI("MENU_TASK_INIT", "u8g2_ClearBuffer");
  u8g2_ClearBuffer(display);
  ESP_LOGI("MENU_TASK_INIT", "u8g2_SetFont");
  u8g2_SetFont(display, u8g2_font_lubBI12_tr);
  ESP_LOGI("MENU_TASK_INIT", "u8g2_DrawStr");
  u8g2_DrawStr(display, 20, 20, "Guitar");
  u8g2_DrawStr(display, 12, 48, "Amplifier");
  ESP_LOGI("MENU_TASK_INIT", "u8g2_SendBuffer");
  u8g2_SendBuffer(display);
}

/// @brief
/// @param display      Display Handle
/// @param currentVol   Current volume cursor on
/// @param volume_state Sets if and what volume is changable
/// @param vol_hp       Current Headphone Volume
/// @param vol_aux      Current Aux Gain
/// @param vol_guitar   Current Guitar Gain
void draw_volume_menu(u8g2_t *display, uint8_t *currentVolsel, enum VolumeStates *volume_state, uint8_t *vol_hp, uint8_t *vol_aux, uint8_t *vol_guitar)
{
  u8g2_ClearBuffer(display);

  if (*currentVolsel == 3)
  {
    // draw AUX volume
    u8g2_DrawFrame(display, 18, 3, 64, 12);
    u8g2_DrawBox(display, 20, 5, *vol_aux * 30, 8);
    u8g2_DrawBitmap(display, 1, 1, 16 / 8, 16, bmp_allArray[4]);

    // draw Guitar volume
    u8g2_DrawFrame(display, 18, 23, 55, 12);
    u8g2_DrawBox(display, 20, 25, *vol_guitar, 8);
    u8g2_DrawBitmap(display, 1, 21, 16 / 8, 16, bmp_allArray[5]);
    // draw Back to menu
    u8g2_SetFont(display, u8g2_font_7x14B_mr);
    u8g2_DrawStr(display, 2, 55, "Back to Menu");
    u8g2_DrawBitmap(display, 86, 41, 16 / 8, 16, bmp_allArray[7]);

    // u8g2_DrawFrame(display, 18, 43, 79, 12);
    // u8g2_DrawBox(display, 20, 45, volume_guitar_in * 3, 8);
  }
  else
  {
    // draw Headphone volume
    u8g2_DrawFrame(display, 18, 3, 66, 12);
    u8g2_DrawBox(display, 20, 5, *vol_hp * 2, 8);
    u8g2_DrawBitmap(display, 1, 1, 16 / 8, 16, bmp_allArray[6]);

    // draw AUX volume
    u8g2_DrawFrame(display, 18, 23, 64, 12);
    u8g2_DrawBox(display, 20, 25, *vol_aux * 30, 8);
    u8g2_DrawBitmap(display, 1, 21, 16 / 8, 16, bmp_allArray[4]);
    // draw guitar volume
    u8g2_DrawFrame(display, 18, 43, 55, 12);
    u8g2_DrawBox(display, 20, 45, *vol_guitar, 8);
    u8g2_DrawBitmap(display, 1, 41, 16 / 8, 16, bmp_allArray[5]);
  }
  // selection Frame
  if (*currentVolsel == 0)
    u8g2_DrawFrame(display, 0, 0, 102, 18);
  else if (*currentVolsel == 1)
    u8g2_DrawFrame(display, 0, 20, 102, 18);
  else if (*currentVolsel == 2 || *currentVolsel == 3)
    u8g2_DrawFrame(display, 0, 40, 102, 18);

  // draw selection identifier
  if (*volume_state == VOL_HEADPHONE)
    u8g2_DrawBox(display, 99, 5, 2, 8);
  else if (*volume_state == VOL_AUX)
    u8g2_DrawBox(display, 99, 25, 2, 8);
  else if (*volume_state == VOL_GUITAR)
    u8g2_DrawBox(display, 99, 45, 2, 8);

  u8g2_SendBuffer(display);
}

/// @brief
/// @param display    Display Handle
/// @param note       Target Frequency
/// @param deviation  Current Frequency
void draw_tuner(u8g2_t *display, char note, int deviation)
{
  u8g2_ClearBuffer(display);
  u8g2_SetFont(display, u8g2_font_7x14B_mr);
  u8g2_DrawStr(display, 26, 15, "TUNER");
  u8g2_DrawBitmap(display, 5, 2, 16 / 8, 16, bmp_allArray[0]);

  u8g2_SetFont(display, u8g2_font_unifont_t_76);
  u8g2_DrawGlyph(display, 5, 59, 0x266D);  // ♭
  u8g2_DrawGlyph(display, 93, 59, 0x266F); // ♯
  u8g2_DrawFrame(display, 10, 25, 82, 20);
  u8g2_DrawBox(display, 49, 27, 3, 2);
  u8g2_DrawBox(display, 49, 41, 3, 2);
  u8g2_SetFont(display, u8g2_font_7x14B_mr);

  u8g2_DrawGlyph(display, 48, 59, note);
  //center is 50
  u8g2_SendBuffer(display);

  //  [Nearest Note] [x Hz(in smaller font)]
  //          |        .
  //          |        .            [Deviation in Cent]
  //          |        .
  //          |        .
}

/// @brief  Start I2S transmission
void codec_i2s_start()
{
  uint8_t inen = 0;
  uint8_t outen = 0;

  i2c_codec_read(I2C_CODEC, 0x3E, &inen, 1);
  i2c_codec_read(I2C_CODEC, 0x3F, &outen, 1);
  inen |= 1;
  outen |= 1;

  i2c_write_byte(I2C_CODEC, 0x45, 0x00); // shutdown device
                                         // i2c_write_byte(I2C_CODEC, 0x06, 0x01); // Sets up DAI for I2S slave mode operation.
  i2c_write_byte(I2C_CODEC, 0x05, 0x08); // set Sample Rate to 48k
  i2c_write_byte(I2C_CODEC, 0x15, 0x40); // Left ADC Mixer to MIC2
  i2c_write_byte(I2C_CODEC, 0x22, 0x04); // DLY on, 16Bit Word Size
  i2c_write_byte(I2C_CODEC, 0x25, 0x2B); // SDOEN, SDIEN, LBEN, LT OFF
  i2c_write_byte(I2C_CODEC, 0x26, 0xE0); // Filter conf: music mode, DC BLOCk Filter Record and Playback,
  i2c_write_byte(I2C_CODEC, 0x3E, inen);
  i2c_write_byte(I2C_CODEC, 0x3F, outen);
  i2c_write_byte(I2C_CODEC, 0x44, 0x06); // ADC Performance: OSR 128xfs, ADC Quantizer Dither
  i2c_write_byte(I2C_CODEC, 0x45, 0x80); // startup device
}

/// @brief Set up ESP to I2S reception
/// @param rx_handle  i2s_chan_handle_t I2S Channel Handle
void i2s_std(i2s_chan_handle_t *rx_handle)
{
  // Get the default channel configuration by helper macro.
  // This helper macro is defined in 'i2s_common.h' and shared by all the I2S communication modes.
  // It can help to specify the I2S role and port ID
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT, I2S_ROLE_MASTER);
  // Allocate a new RX channel and get the handle of this channel
  i2s_new_channel(&chan_cfg, NULL, rx_handle);

  // Setting the configurations, the slot configuration and clock configuration can be generated by the macros
  // These two helper macros are defined in 'i2s_std.h' which can only be used in STD mode.
  // They can help to specify the slot and clock configurations for initialization or updating

  i2s_std_config_t std_cfg = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
      .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
      .gpio_cfg = {
          .mclk = I2S_MCLK,
          .bclk = I2S_BCLK,
          .ws = I2S_LRCLK,
          .dout = I2S_DOUT,
          .din = I2S_DIN,
          .invert_flags = {
              .mclk_inv = false,
              .bclk_inv = false,
              .ws_inv = false,
          },
      },
  };
  // Initialize the channel
  i2s_channel_init_std_mode(rx_handle, &std_cfg);
  i2s_channel_enable(rx_handle);
}

/*void i2s_legacy(i2s_chan_handle_t *rx_handle)
{
  const i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = 16,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = BUF_LEN,
      .use_apll = false};
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t i2s_pin_config = {
    .bck_io_num = I2S_BCLK,
    .data_in_num= I2S_DIN,
    .data_out_num = I2S_DOUT,
    .mck_io_num = I2S_MCLK,
    .ws_io_num = I2S_WS
  };
  i2s_set_pin(I2S_PORT, &i2s_pin_config);

  i2s_start(I2S_PORT);
}
*/

/// @brief find nearest string frequency and put it and deviatino in queue
/// @param frequency
void tune(float frequency)
{
}