/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in
 * which case, it is free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef _ES7243_H_
#define _ES7243_H_

#include <string.h>

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_Delay.h"
#include "Platforms/API_I2C.h"
#include "Platforms/GPIO.h"
#include "stdbool.h"

#define MCLK_PULSES_NUMBER (20)
#define ES_ASSERT(a, format, b, ...) \
  if ((a) != 0) {                    \
    AD_LOGE(format, ##__VA_ARGS__);  \
    return b;                        \
  }

namespace audio_driver {

/**
 * @brief Header-only driver class for the ES7243 ADC chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class ES7243 {
 public:
  ES7243() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) { i2c_addr = addr; }

  /// Defines the GPIO pin used to provide the MCLK signal
  void setMclkGpio(GpioPin gpio) { mclk_gpio = gpio; }

  /// @brief Initialize ES7243 adc chip
  error_t init(codec_config_t* codec_cfg) {
    error_t ret = RESULT_OK;
    mclkActive(mclk_gpio);
    ret |= writeReg(0x00, 0x01);  // slave mode, software mode
    ret |= writeReg(0x06, 0x00);
    ret |= writeReg(0x05, 0x1B);          // Mute ADC
    ret |= writeReg(0x01, audio_format);  // i2s -16bit
    ret |= writeReg(0x08, 0x43);          // enable AIN, PGA GAIN = 27DB
    ret |= writeReg(0x05, 0x13);          // un Mute ADC
    if (ret) {
      AD_LOGE("Es7243 initialize failed!");
      return RESULT_FAIL;
    }
    return ret;
  }

  /// @brief Deinitialize ES7243 adc chip
  error_t deinit(void) { return setVoiceMute(true); }

  /// @brief Control ES7243 adc chip
  error_t ctrlStateActive(codec_mode_t mode, bool ctrl_state_active) {
    return setVoiceMute(!ctrl_state_active);
  }

  /// @brief Configure ES7243 adc mode and I2S interface
  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    // master mode not supported
    if (iface->mode == MODE_MASTER) {
      AD_LOGE("configI2S: Mode must be slave");
      return RESULT_FAIL;
    }

    // set bits
    switch (iface->bits) {
      case BIT_LENGTH_16BITS:
        audio_format |= 0b011 << 2;
        break;
      case BIT_LENGTH_24BITS:
        audio_format |= 0b000 << 2;
        break;
      case BIT_LENGTH_32BITS:
        audio_format |= 0b100 << 2;
        break;
      default:
        AD_LOGE("configI2S: bits not supported %d", iface->bits);
        return RESULT_FAIL;
    }

    switch (iface->fmt) {
      case I2S_RIGHT:
      case I2S_NORMAL:
        audio_format |= 0b00;
        break;
      case I2S_LEFT:
        audio_format |= 0b01;
        break;
      case I2S_DSP:
        audio_format |= 0b11;
        break;
      default:
        AD_LOGE("configI2S: fmt not supported %d", iface->fmt);
        return RESULT_FAIL;
    }

    return writeReg(0x01, audio_format);
  }

  /// @brief  Set mute
  error_t setVoiceMute(bool mute) {
    AD_LOGI("setVoiceMute: mute = %d", mute);
    error_t ret = RESULT_OK;
    if (mute) {
      ret = writeReg(0x05, 0x1B);
    } else {
      ret = writeReg(0x05, 0x13);
    }
    return ret;
  }

  /// @brief  Set adc gain (value of gain 0~100)
  error_t setVoiceVolume(int volume) {
    AD_LOGI("setVoiceVolume: %d", volume);
    error_t ret = RESULT_OK;
    if (volume > 100) {
      volume = 100;
    }
    if (volume < 0) {
      volume = 0;
    }
    actual_volume = volume;
    switch (volume) {
      case 0 ... 12:
        ret |= writeReg(0x08, 0x11);  // 1db
        break;
      case 13 ... 25:
        ret |= writeReg(0x08, 0x13);  // 3.5db
        break;
      case 26 ... 38:
        ret |= writeReg(0x08, 0x21);  // 18db
        break;
      case 39 ... 51:
        ret |= writeReg(0x08, 0x23);  // 20.5db
        break;
      case 52 ... 65:
        ret |= writeReg(0x08, 0x05);  // 22.5db
        break;
      case 66 ... 80:
        ret |= writeReg(0x08, 0x41);  // 24.5db
        break;
      case 81 ... 90:
        ret |= writeReg(0x08, 0x07);  // 25db
        break;
      case 91 ... 100:
        ret |= writeReg(0x08, 0x43);  // 27db
        break;
      default:
        break;
    }
    return ret;
  }

  /// @brief Get adc gain (value of gain 0~100)
  error_t getVoiceVolume(int* volume) {
    *volume = actual_volume;
    return RESULT_OK;
  }
  error_t writeReg(uint8_t reg_add, uint8_t data) {
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, &reg_add, sizeof(reg_add),
                                &data, sizeof(data));
  }

  /*
      Before initializing es7243, it is necessary to output
      mclk to es7243 to activate the I2C configuration.
      So give some clocks to active es7243.
  */
  error_t mclkActive(GpioPin mclk_gpio) {
    GPIO gpio;
    gpio.pinMode(mclk_gpio, OUTPUT);
    for (int i = 0; i < MCLK_PULSES_NUMBER; ++i) {
      gpio.digitalWrite(mclk_gpio, 0);
      delayMs(1);
      gpio.digitalWrite(mclk_gpio, 1);
      delayMs(1);
    }
    return RESULT_OK;
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = 0x13;  // 0x26>>1;
  GpioPin mclk_gpio = GPIO_UNDEFINED;
  int actual_volume = 0;
  uint8_t audio_format = 0x0C;
};

}  // namespace audio_driver

#endif
