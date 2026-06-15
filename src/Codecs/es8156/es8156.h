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

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"
#include <math.h>
#include "string.h"

#ifndef BIT
#define BIT(nr) (1 << (nr))
#endif
// 0x8 = 0x10>>1
#define ES8156_ADDR 0x8
#define BOARD_PA_GAIN (0) /* Power amplifier gain defined by board (dB) */

#define ES8156_DAC_VOL_CFG_DEFAULT()  \
  {                                   \
      .max_dac_volume = 32,           \
      .min_dac_volume = -95.5,        \
      .board_pa_gain = BOARD_PA_GAIN, \
      .volume_accuracy = 0.5,         \
      .dac_vol_symbol = 1,            \
      .zero_volume_reg = 0xBF,        \
      .reg_value = 0,                 \
      .user_volume = 0,               \
      .offset_conv_volume = NULL,     \
  }

/* ES8156 register space */
/*
 * RESET Control
 */
#define ES8156_RESET_REG00 0x00
/*
 * Clock Managerment
 */
#define ES8156_MAINCLOCK_CTL_REG01 0x01
#define ES8156_SCLK_MODE_REG02 0x02
#define ES8156_LRCLK_DIV_H_REG03 0x03
#define ES8156_LRCLK_DIV_L_REG04 0x04
#define ES8156_SCLK_DIV_REG05 0x05
#define ES8156_NFS_CONFIG_REG06 0x06
#define ES8156_MISC_CONTROL1_REG07 0x07
#define ES8156_CLOCK_ON_OFF_REG08 0x08
#define ES8156_MISC_CONTROL2_REG09 0x09
#define ES8156_TIME_CONTROL1_REG0A 0x0a
#define ES8156_TIME_CONTROL2_REG0B 0x0b
/*
 * System Control
 */
#define ES8156_CHIP_STATUS_REG0C 0x0c
#define ES8156_P2S_CONTROL_REG0D 0x0d
#define ES8156_DAC_OSR_COUNTER_REG10 0x10
/*
 * SDP Control
 */
#define ES8156_DAC_SDP_REG11 0x11
#define ES8156_AUTOMUTE_SET_REG12 0x12
#define ES8156_DAC_MUTE_REG13 0x13
#define ES8156_VOLUME_CONTROL_REG14 0x14

/*
 * ALC Control
 */
#define ES8156_ALC_CONFIG1_REG15 0x15
#define ES8156_ALC_CONFIG2_REG16 0x16
#define ES8156_ALC_CONFIG3_REG17 0x17
#define ES8156_MISC_CONTROL3_REG18 0x18
#define ES8156_EQ_CONTROL1_REG19 0x19
#define ES8156_EQ_CONTROL2_REG1A 0x1a
/*
 * Analog System Control
 */
#define ES8156_ANALOG_SYS1_REG20 0x20
#define ES8156_ANALOG_SYS2_REG21 0x21
#define ES8156_ANALOG_SYS3_REG22 0x22
#define ES8156_ANALOG_SYS4_REG23 0x23
#define ES8156_ANALOG_LP_REG24 0x24
#define ES8156_ANALOG_SYS5_REG25 0x25
/*
 * Chip Information
 */
#define ES8156_I2C_PAGESEL_REGFC 0xFC
#define ES8156_CHIPID1_REGFD 0xFD
#define ES8156_CHIPID0_REGFE 0xFE
#define ES8156_CHIP_VERSION_REGFF 0xFF

namespace audio_driver {

/**
 * @brief Header-only driver class for the ES8156 DAC chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class ES8156 {
 public:
  ES8156() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) { i2c_addr = addr; }

  /// @brief Initialize ES8156 codec chip
  error_t init(codec_config_t* cfg) {
    if (initialized) {
      AD_LOGW("The es8156 DAC has been already initialized");
      return RESULT_OK;
    }
    initialized = true;

    writeReg(0x02, 0x04);
    writeReg(0x20, 0x2A);
    writeReg(0x21, 0x3C);
    writeReg(0x22, 0x00);
    writeReg(0x24, 0x07);
    writeReg(0x23, 0x00);

    writeReg(0x0A, 0x01);
    writeReg(0x0B, 0x01);
    writeReg(0x11, 0x00);
    writeReg(0x14, 179);  // volume 70%

    writeReg(0x0D, 0x14);
    writeReg(0x18, 0x00);
    writeReg(0x08, 0x3F);
    writeReg(0x00, 0x02);
    writeReg(0x00, 0x03);
    writeReg(0x25, 0x20);

    // codec_dac_volume_config_t vol_cfg = ES8156_DAC_VOL_CFG_DEFAULT();
    // dac_vol_handle = audio_codec_volume_init(&vol_cfg);
    return RESULT_OK;
  }

  /// @brief Deinitialize ES8156 codec chip
  error_t deinit(void) {
    initialized = false;
    // audio_codec_volume_deinit(dac_vol_handle);
    return RESULT_OK;
  }

  /// @brief Control ES8156 codec chip
  error_t ctrlStateActive(codec_mode_t mode, bool ctrl_state_active) {
    error_t ret = RESULT_OK;
    if (ctrl_state_active) {
      ret = resume();
    } else {
      AD_LOGW("The codec going to stop");
      ret = standby();
    }
    return ret;
  }

  /// @brief Configure ES8156 codec mode and I2S interface
  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    return RESULT_OK;
  }

  /// @brief Configure ES8156 DAC mute or not.
  error_t setVoiceMute(bool enable) {
    int regv = readReg(ES8156_DAC_MUTE_REG13);
    if (enable) {
      regv = regv | BIT(1) | BIT(2);
    } else {
      regv = regv & (~(BIT(1) | BIT(2)));
    }
    writeReg(ES8156_DAC_MUTE_REG13, regv);
    return RESULT_OK;
  }

  /**
   * @brief Set voice volume
   * @note Register values. 0x00: -95.5 dB, 0x5B: -50 dB, 0xBF: 0 dB, 0xFF: 32 dB
   * @note Accuracy of gain is 0.5 dB
   * @param volume: voice volume (0~100)
   */
  error_t setVoiceVolume(int volume) {
    int ret = 0;
    // TODO
    // uint8_t reg = 0;
    // reg = audio_codec_get_dac_reg_value(dac_vol_handle, volume);
    // ret = writeReg(ES8156_VOLUME_CONTROL_REG14, reg);
    // AD_LOGD( "Set volume:%.2d reg_value:0x%.2x dB:%.1f",
    // dac_vol_handle->user_volume, reg,
    //         audio_codec_cal_dac_volume(dac_vol_handle));
    return ret;
  }

  /// @brief Get voice volume (0~100)
  error_t getVoiceVolume(int* volume) {
    int ret = 0;
    int regv = 0;
    *volume = 0;
    regv = readReg(ES8156_VOLUME_CONTROL_REG14);
    if (regv == RESULT_FAIL) {
      ret = RESULT_FAIL;
    } else {
      if (regv == dac_vol_handle->reg_value) {
        *volume = dac_vol_handle->user_volume;
      } else {
        *volume = 0;
        ret = RESULT_FAIL;
      }
    }
    AD_LOGD("Get volume:%.2d reg_value:0x%.2x", *volume, regv);
    return ret;
  }
  error_t writeReg(uint8_t reg_addr, uint8_t data) {
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, &reg_addr,
                                sizeof(reg_addr), &data, sizeof(data));
  }

  int readReg(uint8_t reg_addr) {
    uint8_t data;
    i2c_bus_read_bytes(i2c_handle, i2c_addr, &reg_addr, sizeof(reg_addr),
                        &data, sizeof(data));
    return (int)data;
  }

  error_t standby(void) {
    error_t ret = 0;
    ret = writeReg(0x14, 0x00);
    ret |= writeReg(0x19, 0x02);
    ret |= writeReg(0x21, 0x1F);
    ret |= writeReg(0x22, 0x02);
    ret |= writeReg(0x25, 0x21);
    ret |= writeReg(0x25, 0xA1);
    ret |= writeReg(0x18, 0x01);
    ret |= writeReg(0x09, 0x02);
    ret |= writeReg(0x09, 0x01);
    ret |= writeReg(0x08, 0x00);
    return ret;
  }

  error_t resume(void) {
    error_t ret = 0;
    ret |= writeReg(0x08, 0x3F);
    ret |= writeReg(0x09, 0x00);
    ret |= writeReg(0x18, 0x00);

    ret |= writeReg(0x25, 0x20);
    ret |= writeReg(0x22, 0x00);
    ret |= writeReg(0x21, 0x3C);
    ret |= writeReg(0x19, 0x20);
    ret |= writeReg(0x14, 179);
    return ret;
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = ES8156_ADDR;
  bool initialized = false;
  codec_dac_volume_config_t* dac_vol_handle;
};

}  // namespace audio_driver
