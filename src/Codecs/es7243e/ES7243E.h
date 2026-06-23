/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#pragma once

#include "stddef.h"  // NULL

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"

namespace audio_driver {

/**
 * @brief Header-only driver class for the ES7243E ADC chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class ES7243E {
 public:
  ES7243E() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) { i2c_addr = addr; }

  /// @brief Initialize ES7243E adc chip
  error_t init(codec_config_t* codec_cfg) {
    error_t ret = RESULT_OK;
    ret |= writeReg(0x01, 0x3A);
    ret |= writeReg(0x00, 0x80);
    ret |= writeReg(0xF9, 0x00);
    ret |= writeReg(0x04, 0x02);
    ret |= writeReg(0x04, 0x01);
    ret |= writeReg(0xF9, 0x01);
    ret |= writeReg(0x00, 0x1E);
    ret |= writeReg(0x01, 0x00);

    ret |= writeReg(0x02, 0x00);
    ret |= writeReg(0x03, 0x20);
    ret |= writeReg(0x04, 0x01);
    ret |= writeReg(0x0D, 0x00);
    ret |= writeReg(0x05, 0x00);
    ret |= writeReg(0x06, 0x03);  // SCLK=MCLK/4
    ret |= writeReg(0x07, 0x00);  // LRCK=MCLK/256
    ret |= writeReg(0x08, 0xFF);  // LRCK=MCLK/256

    ret |= writeReg(0x09, 0xCA);
    ret |= writeReg(0x0A, 0x85);
    ret |= writeReg(0x0B, 0x00);
    ret |= writeReg(0x0E, 0xBF);
    ret |= writeReg(0x0F, 0x80);
    ret |= writeReg(0x14, 0x0C);
    ret |= writeReg(0x15, 0x0C);
    ret |= writeReg(0x17, 0x02);
    ret |= writeReg(0x18, 0x26);
    ret |= writeReg(0x19, 0x77);
    ret |= writeReg(0x1A, 0xF4);
    ret |= writeReg(0x1B, 0x66);
    ret |= writeReg(0x1C, 0x44);
    ret |= writeReg(0x1E, 0x00);
    ret |= writeReg(0x1F, 0x0C);
    ret |= writeReg(0x20, 0x1A);  // PGA gain +30dB
    ret |= writeReg(0x21, 0x1A);  // PGA gain +30dB

    ret |= writeReg(0x00, 0x80);  // Slave  Mode
    ret |= writeReg(0x01, 0x3A);
    ret |= writeReg(0x16, 0x3F);
    ret |= writeReg(0x16, 0x00);
    if (ret) {
      AD_LOGE("Es7243e initialize failed!");
      return RESULT_FAIL;
    }
    return ret;
  }

  /// @brief Deinitialize ES7243E adc chip
  error_t deinit(void) { return RESULT_OK; }

  /// @brief Control ES7243E adc chip
  error_t ctrlStateActive(codec_mode_t mode, bool ctrl_state_active) {
    error_t ret = RESULT_OK;
    if (ctrl_state_active) {
      ret |= writeReg(0xF9, 0x00);
      ret |= writeReg(0x04, 0x01);
      ret |= writeReg(0x17, 0x01);
      ret |= writeReg(0x20, 0x10);
      ret |= writeReg(0x21, 0x10);
      ret |= writeReg(0x00, 0x80);
      ret |= writeReg(0x01, 0x3A);
      ret |= writeReg(0x16, 0x3F);
      ret |= writeReg(0x16, 0x00);
    } else {
      AD_LOGW("The codec going to stop");
      ret |= writeReg(0x04, 0x02);
      ret |= writeReg(0x04, 0x01);
      ret |= writeReg(0xF7, 0x30);
      ret |= writeReg(0xF9, 0x01);
      ret |= writeReg(0x16, 0xFF);
      ret |= writeReg(0x17, 0x00);
      ret |= writeReg(0x01, 0x38);
      ret |= writeReg(0x20, 0x00);
      ret |= writeReg(0x21, 0x00);
      ret |= writeReg(0x00, 0x00);
      ret |= writeReg(0x00, 0x1E);
      ret |= writeReg(0x01, 0x30);
      ret |= writeReg(0x01, 0x00);
    }
    return ret;
  }

  /// @brief Configure ES7243E adc mode and I2S interface
  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    return RESULT_OK;
  }

  /// @brief  Set adc gain (value of gain 0~100)
  error_t setVoiceVolume(int volume) { return RESULT_OK; }

  /// @brief Get adc gain (value of gain 0~100)
  error_t getVoiceVolume(int* volume) { return RESULT_OK; }
  error_t writeReg(uint8_t reg_add, uint8_t data) {
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, &reg_add, sizeof(reg_add),
                                &data, sizeof(data));
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = 0x10;  // 0x20 >> 1;
};

}  // namespace audio_driver
