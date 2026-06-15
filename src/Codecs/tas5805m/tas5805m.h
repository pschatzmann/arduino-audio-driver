/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#include "stdbool.h"
#include "DriverCommon.h"
#include "Codecs/CodecConstants.h"
#include "Platforms/API_I2C.h"
#include "Platforms/API_Delay.h"
#include "Platforms/GPIO.h"
#include "tas5805m_reg_cfg.h"
#include <stdint.h>

#define TAS5805M_REG_00      0x00
#define TAS5805M_REG_02      0x02
#define TAS5805M_REG_03      0x03
#define TAS5805M_REG_24      0x24
#define TAS5805M_REG_25      0x25
#define TAS5805M_REG_26      0x26
#define TAS5805M_REG_27      0x27
#define TAS5805M_REG_28      0x28
#define TAS5805M_REG_29      0x29
#define TAS5805M_REG_2A      0x2a
#define TAS5805M_REG_2B      0x2b
#define TAS5805M_REG_35      0x35
#define TAS5805M_REG_7E      0x7e
#define TAS5805M_REG_7F      0x7f

#define TAS5805M_PAGE_00     0x00
#define TAS5805M_PAGE_2A     0x2a

#define TAS5805M_BOOK_00     0x00
#define TAS5805M_BOOK_8C     0x8c

#define  MASTER_VOL_REG_ADDR    0X4C
#define  MUTE_TIME_REG_ADDR     0X51

#define  TAS5805M_DAMP_MODE_BTL      0x0
#define  TAS5805M_DAMP_MODE_PBTL     0x04

// 0x5c>>1 = 0x2E
#define TAS5805M_ADDR 0x2E
#define TAS5805M_VOLUME_MAX 100
#define TAS5805M_VOLUME_MIN 0

#define TAS5805M_ASSERT(a, format, b, ...) \
  if ((a) != 0) {                          \
    AD_LOGE(format, ##__VA_ARGS__);        \
    return b;                              \
  }

namespace audio_driver {

/**
 * @brief Header-only driver class for the TAS5805M amplifier chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class TAS5805M {
 public:
  TAS5805M() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) {
    if (addr > 0) i2c_addr = addr;
  }

  /// Defines the GPIO pin used to power on the amplifier
  void setPowerPin(GpioPin pin) { power_pin = pin; }

  /// @brief Initialize TAS5805 codec chip
  error_t init(codec_config_t* codec_cfg) {
    GPIO gpio;
    error_t ret = RESULT_OK;
    AD_LOGI("Power ON CODEC with GPIO %d", power_pin);
    gpio.pinMode(power_pin, OUTPUT);
    gpio.digitalWrite(power_pin, 0);
    delayMs(20);
    gpio.digitalWrite(power_pin, 1);
    delayMs(20);

    ret |= transmitRegisters(
        tas5805m_registers,
        sizeof(tas5805m_registers) / sizeof(tas5805m_registers[0]));

    TAS5805M_ASSERT(ret, "Fail to iniitialize tas5805m PA", RESULT_FAIL);
    return ret;
  }

  /// @brief Deinitialize TAS5805 codec chip
  error_t deinit(void) {
    // TODO
    return RESULT_OK;
  }

  /// @brief  Set voice volume (0~100)
  error_t setVolume(int vol) {
    int vol_idx = 0;

    if (vol < TAS5805M_VOLUME_MIN) {
      vol = TAS5805M_VOLUME_MIN;
    }
    if (vol > TAS5805M_VOLUME_MAX) {
      vol = TAS5805M_VOLUME_MAX;
    }
    vol_idx = vol / 5;

    uint8_t cmd[2] = {0, 0};
    error_t ret = RESULT_OK;

    cmd[0] = MASTER_VOL_REG_ADDR;
    cmd[1] = tas5805m_volume[vol_idx];
    ret = i2c_bus_write_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);
    AD_LOGW("volume = 0x%x", cmd[1]);
    return ret;
  }

  /// @brief Get voice volume (0~100)
  error_t getVolume(int* value) {
    /// FIXME: Got the digit volume is not right.
    uint8_t cmd[2] = {MASTER_VOL_REG_ADDR, 0x00};
    error_t ret =
        i2c_bus_read_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);
    TAS5805M_ASSERT(ret, "Fail to get volume", RESULT_FAIL);
    unsigned i;
    for (i = 0; i < sizeof(tas5805m_volume); i++) {
      if (cmd[1] >= tas5805m_volume[i]) break;
    }
    AD_LOGI("Volume is %d", i * 5);
    *value = 5 * i;
    return ret;
  }

  /**
   * @brief Set TAS5805 mute or not
   *        Continuously call should have an interval time determined by
   *        setMuteFade()
   */
  error_t setMute(bool enable) {
    error_t ret = RESULT_OK;
    uint8_t cmd[2] = {TAS5805M_REG_03, 0x00};
    ret |= i2c_bus_read_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);

    if (enable) {
      cmd[1] |= 0x8;
    } else {
      cmd[1] &= (~0x08);
    }
    ret |= i2c_bus_write_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);

    TAS5805M_ASSERT(ret, "Fail to set mute", RESULT_FAIL);
    return ret;
  }

  /**
   * @brief Mute gradually by (value)ms
   * @param value  Time for mute with millisecond.
   */
  error_t setMuteFade(int value) {
    error_t ret = 0;
    unsigned char cmd[2] = {MUTE_TIME_REG_ADDR, 0x00};
    /* Time for register value
     *   000: 11.5 ms
     *   001: 53 ms
     *   010: 106.5 ms
     *   011: 266.5 ms
     *   100: 0.535 sec
     *   101: 1.065 sec
     *   110: 2.665 sec
     *   111: 5.33 sec
     */
    if (value <= 12) {
      cmd[1] = 0;
    } else if (value <= 53) {
      cmd[1] = 1;
    } else if (value <= 107) {
      cmd[1] = 2;
    } else if (value <= 267) {
      cmd[1] = 3;
    } else if (value <= 535) {
      cmd[1] = 4;
    } else if (value <= 1065) {
      cmd[1] = 5;
    } else if (value <= 2665) {
      cmd[1] = 6;
    } else {
      cmd[1] = 7;
    }
    cmd[1] |= (cmd[1] << 4);

    ret |= i2c_bus_write_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);
    TAS5805M_ASSERT(ret, "Fail to set mute fade", RESULT_FAIL);
    AD_LOGI("Set mute fade, value:%d, 0x%x", value, cmd[1]);
    return ret;
  }

  /// @brief Get TAS5805 mute status
  error_t getMute(int* value) {
    error_t ret = RESULT_OK;
    uint8_t cmd[2] = {TAS5805M_REG_03, 0x00};
    ret |= i2c_bus_read_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);

    TAS5805M_ASSERT(ret, "Fail to get mute", RESULT_FAIL);
    *value = (cmd[1] & 0x08) >> 3;
    AD_LOGI("Get mute value: 0x%x", *value);
    return ret;
  }

  /**
   * @brief Set DAMP mode
   * @param value  TAS5805M_DAMP_MODE_BTL or TAS5805M_DAMP_MODE_PBTL
   */
  error_t setDampMode(int value) {
    unsigned char cmd[2] = {0};
    cmd[0] = TAS5805M_REG_02;
    cmd[1] = 0x10 | value;
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, &cmd[0], 1, &cmd[1], 1);
  }

  /// @brief Control TAS5805 codec chip
  error_t ctrlStateActive(codec_mode_t mode, bool ctrl_state_active) {
    // TODO
    return RESULT_OK;
  }

  /// @brief Configure TAS5805 codec mode and I2S interface
  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    // TODO
    return RESULT_OK;
  }

  error_t transmitRegisters(const tas5805m_cfg_reg_t* conf_buf, int size) {
    int i = 0;
    error_t ret = RESULT_OK;
    while (i < size) {
      switch (conf_buf[i].offset) {
        case CFG_META_SWITCH:
          // Used in legacy applications.  Ignored here.
          break;
        case CFG_META_DELAY:
          delayMs(conf_buf[i].value);
          break;
        case CFG_META_BURST:
          ret = i2c_bus_write_bytes(i2c_handle, i2c_addr,
                                    (unsigned char*)(&conf_buf[i + 1].offset), 1,
                                    (unsigned char*)(&conf_buf[i + 1].value),
                                    conf_buf[i].value);
          i += (conf_buf[i].value / 2) + 1;
          break;
        case CFG_END_1:
          if (CFG_END_2 == conf_buf[i + 1].offset &&
              CFG_END_3 == conf_buf[i + 2].offset) {
            AD_LOGI("End of tms5805m reg: %d", i);
          }
          break;
        default:
          ret = i2c_bus_write_bytes(i2c_handle, i2c_addr,
                                    (unsigned char*)(&conf_buf[i].offset), 1,
                                    (unsigned char*)(&conf_buf[i].value), 1);
          break;
      }
      i++;
    }
    if (ret != RESULT_OK) {
      AD_LOGE("Fail to load configuration to tas5805m");
      return RESULT_FAIL;
    }
    AD_LOGI("%s:  write %d reg done", __FUNCTION__, i);
    return ret;
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = TAS5805M_ADDR;
  GpioPin power_pin{};
};

} // namespace audio_driver
