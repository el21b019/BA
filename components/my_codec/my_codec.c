#include "my_codec.h"

/// @brief  Inits Codec
/// @return ESP_ERROR Type
esp_err_t codec_init()
{ 
   
    // manual setup
    i2c_write_byte(I2C_CODEC, 0x2C, 0x80); // HPL Volume
    i2c_write_byte(I2C_CODEC, 0x2D, 0x80); // HPR Volume

    i2c_write_byte(I2C_CODEC, 0x45, 0x00); // shutdown device
    
    i2c_write_byte(I2C_CODEC,0x04,0x01);     //Sys clk 256Fs
    i2c_write_byte(I2C_CODEC,0x05,0x04);     //Sample Rate 44k1

    i2c_write_byte(I2C_CODEC, 0x0D, 0x30); // Analog Input Configuration: IN1 and IN2 Single Ended Enable
    i2c_write_byte(I2C_CODEC, 0x0E, 0x24); // LINE A and B internal input gain  -3dB
    
    //i2c_write_byte(I2C_CODEC, 0x11, 0x3F); // 0db coarse gain, 0db preamp gain
    //i2c_write_byte(I2C_CODEC, 0x11, 0x54); // Input enable: Guitar(MIC2) and Preamp 20dB
    i2c_write_byte(I2C_CODEC, 0x11, 0x74); // Input enable: Guitar(MIC2) and Preamp 30dB
    
    i2c_write_byte(I2C_CODEC, 0x16, 0x40); // Select MIC2 in Right ADC MIXER
    //i2c_write_byte(I2C_CODEC, 0x18, 0x03); // Right ADC Digital Gain: 0dB
    //i2c_write_byte(I2C_CODEC, 0x18, 0x0F); // Right ADC Digital Gain: -12dB
    i2c_write_byte(I2C_CODEC, 0x18, 0x00); // Right ADC Digital Gain: +3dB
    



    i2c_write_byte(I2C_CODEC, 0x25, 0x22); // Enable ADC-DAC, Enable SDOUT 
    i2c_write_byte(I2C_CODEC,0x26,0xE0);     //Enable Music Filter, Block DC in Record & Playback
    //i2c_write_byte(I2C_CODEC, 0x29, 0x0A); // Left HP mixer: LineB and DAC
    //i2c_write_byte(I2C_CODEC, 0x2A, 0x06); // right HP mixer: LineA and DAC
    i2c_write_byte(I2C_CODEC, 0x29, 0x28); // Left HP mixer: LineB and Mic2
    i2c_write_byte(I2C_CODEC, 0x2A, 0x24); // right HP mixer: LineA and Mic2
   
    i2c_write_byte(I2C_CODEC, 0x2B, 0x3C);  // Assign headphone mixers to output and -9,5dB Mixer Gain each
    i2c_write_byte(I2C_CODEC, 0x2C, 0x1A); // HPL Volume +0dB
    i2c_write_byte(I2C_CODEC, 0x2D, 0x1A); // HPR Volume +0dB
    i2c_write_byte(I2C_CODEC, 0x3E, 0x0E); // Input enable: Line A, Line B and ADCR
    i2c_write_byte(I2C_CODEC, 0x3F, 0xC2); // Ouput enable: HPR, HPL and DACR
    i2c_write_byte(I2C_CODEC, 0x40, 0x03); // Zero-Crossing Detection enabled, Enhanced Volume Smoothing and Volume Adjustment Smoothing disabled
    i2c_write_byte(I2C_CODEC, 0x44, 0x06); // ADC Oversampling rate 128xfs, ADC Dither Enable

    i2c_write_byte(I2C_CODEC, 0x45, 0x80); // startup device

    

    /*IN 1/2 -> ADC -> HP
        //i2c_codec_rgister_dump();
        i2c_write_byte(I2C_CODEC, 0x45, 0x00); // shutdown device
        i2c_write_byte(I2C_CODEC,0x04,0x01);     //Sys clk 256Fs
        i2c_write_byte(I2C_CODEC,0x05,0x04);     //Sample Rate 44k1
        i2c_write_byte(I2C_CODEC,0x06,0x00);     //no DAI
        i2c_write_byte(I2C_CODEC,0x0D,0x30);     //IN1/2 Single ended
        i2c_write_byte(I2C_CODEC,0x0E,0x24);     //LINE A/B PGA -3dB
        i2c_write_byte(I2C_CODEC,0x15,0x10);     //Left ADC MIXER LineB
        i2c_write_byte(I2C_CODEC,0x16,0x08);     //Right ADC MIXER LineA
        i2c_write_byte(I2C_CODEC,0x25,0x23);     //Enable ASD to DAC, SDO & SDI
        i2c_write_byte(I2C_CODEC,0x26,0xE0);     //Enable Music Filter, Block DC in Record & Playback
        i2c_write_byte(I2C_CODEC,0x28,0x10);     //EQ Clip detect disable

        i2c_write_byte(I2C_CODEC,0x3E,0x0F);     //Enable LineA/B and ADC L/R
        i2c_write_byte(I2C_CODEC,0x3F,0xC3);     //Enable HP L/R and DAC L/R
        i2c_write_byte(I2C_CODEC,0x44,0x04);     // ADC Oversampling 128xFs
        i2c_write_byte(I2C_CODEC, 0x45, 0x80); // startup device
        ESP_LOGI("Codec", "Initiated");
        //i2c_codec_rgister_dump();
    */
    /*INIT Self Check

    i2c_codec_read(I2C_CODEC, 0x0D, &data, 1);
    if (data != 0x30)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x0D: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x29, &data, 1);
    if (data != 0x04)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x29: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x2A, &data, 1);
    if (data != 0x08)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x2A: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x2B, &data, 1);
    if (data != 0x35)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x2B: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x2C, &data, 1);
    if (data != 0x1A)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x2C: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x2D, &data, 1);
    if (data != 0x1A)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x2D: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x3E, &data, 1);
    if (data != 0x04)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x3E: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x3F, &data, 1);
    if (data != 0xC0)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x3F: 0x", &data, 1);
        return ESP_FAIL;
    }

    data = 0xff;
    i2c_codec_read(I2C_CODEC, 0x40, &data, 1);
    if (data != 0x03)
    {
        ESP_LOG_BUFFER_HEX("Codec 0x40: 0x", &data, 1);
        return ESP_FAIL;
    }
*/
    return ESP_OK;
}

/// @brief          Sets Volume on Headphones
/// @param volume   Volumem Value - already in format needed
void setVolume_HP(uint8_t volume)
{
    /*
    //--------------------------------------------------------temp test for click on volume change
    uint8_t temp = 0xff;
    i2c_codec_read(I2C_CODEC, 0x2c,&temp,1);            //mute outputs before setting new volume level
    temp = temp | (3<<6);
    i2c_write_byte(I2C_CODEC, 0x2C, temp);
    i2c_write_byte(I2C_CODEC, 0x2D, temp);
    //--------------------------------------------------------temp test for click on volume change
  */
    // left HP
    i2c_write_byte(I2C_CODEC, 0x2C, volume);
    // right HP
    i2c_write_byte(I2C_CODEC, 0x2D, volume);
}

/// @brief          Sets Gain on Guitar
/// @param volume   Volumem Value - already in format needed
void setVolume_Guitar(uint8_t volume)
{
    if (volume == 0)
        disableGuitar();
    else
        volume--;

    if (volume < 20)
    {
        // PGA GAIN 0-19
        // Preamp Gain 0dB
        volume = 0x14 - volume;
        i2c_write_byte(I2C_CODEC, 0x11, (volume | (1 << 5)));
    }
    else if (volume >= 20 && volume < 30)
    {
        // PGA GAIN 0-9
        // Preamp Gain 20dB
        volume = 0x14 - (volume - 20);
        i2c_write_byte(I2C_CODEC, 0x11, (volume | (2 << 5)));
    }
    else if (volume >= 30)
    {
        // PGA GAIN 0-20
        // Preamp Gain 30dB
        volume = 0x14 - (volume - 30);
        i2c_write_byte(I2C_CODEC, 0x11, (volume | (3 << 5)));
    }
}

/// @brief          Sets Gain on Aux Input
/// @param volume   Volumem Value - already in format needed
void setVolume_AUX(uint8_t volume)
{

    // No Line A/B Mixer operation
    switch (volume)
    {
    case 0:
        i2c_write_byte(I2C_CODEC, 0x0E, 0x2D); //-6dB
        break;
    case 1:
        i2c_write_byte(I2C_CODEC, 0x0E, 0x24); //-3dB
        break;
    case 2:
        i2c_write_byte(I2C_CODEC, 0x0E, 0x1B); //-0dB
        break;
    default:
        break;
    }
}

void disableGuitar()
{
}
