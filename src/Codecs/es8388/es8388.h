/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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
#include <string.h>

// ES8388 address  0x22:CE=1;0x20:CE=0
#define ES8388_ADDR 0x10
// ES8388 register
#define ES8388_CONTROL1 0x00
#define ES8388_CONTROL2 0x01

#define ES8388_CHIPPOWER 0x02

#define ES8388_ADCPOWER 0x03
#define ES8388_DACPOWER 0x04

#define ES8388_CHIPLOPOW1 0x05
#define ES8388_CHIPLOPOW2 0x06

#define ES8388_ANAVOLMANAG 0x07

#define ES8388_MASTERMODE 0x08
/* ADC */
#define ES8388_ADCCONTROL1 0x09
#define ES8388_ADCCONTROL2 0x0a
#define ES8388_ADCCONTROL3 0x0b
#define ES8388_ADCCONTROL4 0x0c
#define ES8388_ADCCONTROL5 0x0d
#define ES8388_ADCCONTROL6 0x0e
#define ES8388_ADCCONTROL7 0x0f
#define ES8388_ADCCONTROL8 0x10
#define ES8388_ADCCONTROL9 0x11
#define ES8388_ADCCONTROL10 0x12
#define ES8388_ADCCONTROL11 0x13
#define ES8388_ADCCONTROL12 0x14
#define ES8388_ADCCONTROL13 0x15
#define ES8388_ADCCONTROL14 0x16
/* DAC */
#define ES8388_DACCONTROL1 0x17
#define ES8388_DACCONTROL2 0x18
#define ES8388_DACCONTROL3 0x19
#define ES8388_DACCONTROL4 0x1a
#define ES8388_DACCONTROL5 0x1b
#define ES8388_DACCONTROL6 0x1c
#define ES8388_DACCONTROL7 0x1d
#define ES8388_DACCONTROL8 0x1e
#define ES8388_DACCONTROL9 0x1f
#define ES8388_DACCONTROL10 0x20
#define ES8388_DACCONTROL11 0x21
#define ES8388_DACCONTROL12 0x22
#define ES8388_DACCONTROL13 0x23
#define ES8388_DACCONTROL14 0x24
#define ES8388_DACCONTROL15 0x25
#define ES8388_DACCONTROL16 0x26
#define ES8388_DACCONTROL17 0x27
#define ES8388_DACCONTROL18 0x28
#define ES8388_DACCONTROL19 0x29
#define ES8388_DACCONTROL20 0x2a
#define ES8388_DACCONTROL21 0x2b
#define ES8388_DACCONTROL22 0x2c
#define ES8388_DACCONTROL23 0x2d
#define ES8388_DACCONTROL24 0x2e
#define ES8388_DACCONTROL25 0x2f
#define ES8388_DACCONTROL26 0x30
#define ES8388_DACCONTROL27 0x31
#define ES8388_DACCONTROL28 0x32
#define ES8388_DACCONTROL29 0x33
#define ES8388_DACCONTROL30 0x34

namespace audio_driver {

typedef enum {
  ES8388_OUTPUT_MIN = -1,
  ES8388_OUTPUT_LOUT1 = 0x04,
  ES8388_OUTPUT_ROUT1 = 0x08,
  ES8388_OUTPUT_SPK = 0x09,
  ES8388_OUTPUT_LOUT2 = 0x10,
  ES8388_OUTPUT_ROUT2 = 0x20,
  ES8388_OUTPUT_ALL = 0x3c,
  ES8388_OUTPUT_MAX,
} es_output_device_t;

typedef enum {
  ESP8388_INPUT_MIN = -1,
  ESP8388_INPUT_LINPUT1_RINPUT1 = 0x00,
  ESP8388_INPUT_MIC1 = 0x05,
  ESP8388_INPUT_MIC2 = 0x06,
  ESP8388_INPUT_LINPUT2_RINPUT2 = 0x50,
  ESP8388_INPUT_DIFFERENCE = 0xf0,
  ESP8388_INPUT_MAX,
} es8388_input_device_t;

static i2c_bus_handle_t es8388_i2c_handle = NULL;
static int dac_power = 0x3c;
static int address = ES8388_ADDR;
static int esp8388_volume_hack = AI_THINKER_ES8388_VOLUME_HACK;

#define ES_ASSERT(a, format, b, ...) \
  if ((a) != 0) {                    \
    AD_LOGE(format, ##__VA_ARGS__);  \
    return b;                        \
  }

static error_t es8388_write_reg(uint8_t slave_addr, uint8_t reg_add, uint8_t data) {
  return i2c_bus_write_bytes(es8388_i2c_handle, slave_addr, &reg_add, sizeof(reg_add),
                             &data, sizeof(data));
}


static int es8388_set_adc_dac_volume(int mode, int volume, int dot) {
  AD_LOGD("es8388_set_adc_dac_volume: %d.%d", volume, dot);
  int res = 0;
  if (volume < -96 || volume > 0) {
    AD_LOGW("Warning: volume < -96! or > 0!");
    if (volume < -96)
      volume = -96;
    else
      volume = 0;
  }
  dot = (dot >= 5 ? 1 : 0);
  volume = (-volume << 1) + dot;
  if (mode == CODEC_MODE_ENCODE || mode == CODEC_MODE_BOTH) {
    res |= es8388_write_reg(address, ES8388_ADCCONTROL8, volume);
    res |= es8388_write_reg(address, ES8388_ADCCONTROL9,
                        volume);  // ADC Right Volume=0db
  }
  if (mode == CODEC_MODE_DECODE || mode == CODEC_MODE_BOTH) {
    res |= es8388_write_reg(address, ES8388_DACCONTROL5, volume);
    res |= es8388_write_reg(address, ES8388_DACCONTROL4, volume);
  }
  return res;
}

inline error_t es8388_write_reg(uint8_t reg_add, uint8_t data) {
  return es8388_write_reg(address, reg_add, data);
}

inline error_t es8388_read_reg(uint8_t reg_add, uint8_t* p_data) {
  return i2c_bus_read_bytes(es8388_i2c_handle, address, &reg_add, sizeof(reg_add),
                            p_data, 1);
}

inline void es8388_read_all() {
  AD_TRACED();
  for (int i = 0; i < 50; i++) {
    uint8_t reg = 0;
    es8388_read_reg(i, &reg);
    AD_LOGI("%x: %x", i, reg);
  }
}

inline error_t es8388_set_voice_mute(bool enable) {
  AD_TRACED();
  error_t res = RESULT_OK;
  uint8_t reg = 0;
  res = es8388_read_reg(ES8388_DACCONTROL3, &reg);
  reg = reg & 0xC3;               // keep 11000011
  int value = enable ? 0x3C : 0;  // enable is 00111100 / disable is 00000000
  res |= es8388_write_reg(address, ES8388_DACCONTROL3, value | reg);
  return res;
}

inline error_t es8388_set_mic_gain(es_mic_gain_t gain) {
  AD_LOGD("es8388_set_mic_gain: %d", gain);
  error_t res, gain_n;
  gain_n = (int)gain / 3;
  gain_n = (gain_n << 4) + gain_n;
  res = es8388_write_reg(address, ES8388_ADCCONTROL1, gain_n);  // MIC PGA
  return res;
}

static inline error_t es8388_set_voice_volume_0(int volume) {
  AD_LOGD("es8388_set_voice_volume: %d", volume);
  error_t res = RESULT_OK;
  if (volume < 0)
    volume = 0;
  else if (volume > 100)
    volume = 100;
  volume /= 3;
  // ROUT1VOL LOUT1VOL 0 -> -45dB; 33 -> – 4.5dB
  res = es8388_write_reg(address, ES8388_DACCONTROL24, volume);
  res |= es8388_write_reg(address, ES8388_DACCONTROL25, volume);
  // DAC LDACVOL RDACVOL default 0 = 0DB; Default value 192 = – -96 dB
  res |= es8388_write_reg(address, ES8388_DACCONTROL26, 0);
  res |= es8388_write_reg(address, ES8388_DACCONTROL27, 0);
  return res;
}

static inline error_t es8388_set_voice_volume_1(int volume) {
  AD_LOGD("es8388_set_voice_volume (HACK 1): %d", volume);
  error_t res = RESULT_OK;
  if (volume < 0)
    volume = 0;
  else if (volume > 100)
    volume = 100;
  volume /= 3;
  // DAC LDACVOL RDACVOL default 0 = 0DB; Default value 192 = – -96 dB
  res = es8388_write_reg(address, ES8388_DACCONTROL4, 0);
  res |= es8388_write_reg(address, ES8388_DACCONTROL5, 0);
  // LOUT1 LOUT1 volume: dataheet says only 6 bits
  res |= es8388_write_reg(address, ES8388_DACCONTROL24, volume);
  res |= es8388_write_reg(address, ES8388_DACCONTROL25, volume);
  // LOUT2 ROUT2 volume: datasheet says only 6 bits
  res |= es8388_write_reg(address, ES8388_DACCONTROL26, volume);
  res |= es8388_write_reg(address, ES8388_DACCONTROL27, volume);
  return res;
}

static inline error_t es8388_set_voice_volume_2(int volume) {
  AD_LOGD("es8388_set_voice_volume (HACK 2): %d", volume);
  error_t res = RESULT_OK;
  if (volume < 0)
    volume = 0;
  else if (volume > 100)
    volume = 100;
  volume = (volume * 63) / 100;
  // LOUT1 RLOUT1 volume: dataheet says only 6 bits
  res = es8388_write_reg(address, ES8388_DACCONTROL24, volume);
  res |= es8388_write_reg(address, ES8388_DACCONTROL25, volume);
  // DAC LDACVOL RDACVOL default 0 = 0DB; Default value 192 = – -96 dB
  res |= es8388_write_reg(address, ES8388_DACCONTROL26, 0);
  res |= es8388_write_reg(address, ES8388_DACCONTROL27, 0);
  // 30-bit a coefficient for shelving filter
  res |= es8388_write_reg(address, ES8388_DACCONTROL8, 192 >> 2);
  res |= es8388_write_reg(address, ES8388_DACCONTROL9, 192 >> 2);
  return res;
}

/**
 * @brief  Set voice volume
 *
 * @param volume:  voice volume (0~100)
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_set_voice_volume(int volume) {
  switch (esp8388_volume_hack) {
    case 0:
      return es8388_set_voice_volume_0(volume);
    case 1:
      return es8388_set_voice_volume_1(volume);
    case 2:
      return es8388_set_voice_volume_2(volume);
    default:
      return es8388_set_voice_volume_0(volume);
  }
}

/**
 * @brief Get voice volume
 *
 * @param[out] *volume:  voice volume (0~100)
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_get_voice_volume(int* volume) {
  AD_TRACED();
  error_t res = RESULT_OK;
  uint8_t reg = 0;
  res = es8388_read_reg(ES8388_DACCONTROL24, &reg);
  if (res == RESULT_FAIL) {
    *volume = 0;
  } else {
    *volume = reg;
    *volume *= 3;
    if (*volume == 99) *volume = 100;
  }
  return res;
}

/**
 * @brief  Start ES8388 codec chip
 *
 * @param mode:  set ADC or DAC or both
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_start(codec_mode_t mode) {
  AD_TRACED();
  error_t res = RESULT_OK;
  uint8_t prev_data = 0, data = 0;
  es8388_read_reg(ES8388_DACCONTROL21, &prev_data);
  if (mode == CODEC_MODE_LINE_IN) {
    res |= es8388_write_reg(
        address, ES8388_DACCONTROL16,
        0x09);  // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2 by pass enable
    res |=
        es8388_write_reg(address, ES8388_DACCONTROL17,
                     0x50);  // left DAC to left mixer enable  and  LIN signal
                             // to left mixer enable 0db  : bupass enable
    res |=
        es8388_write_reg(address, ES8388_DACCONTROL20,
                     0x50);  // right DAC to right mixer enable  and  LIN signal
                             // to right mixer enable 0db : bupass enable
    res |= es8388_write_reg(address, ES8388_DACCONTROL21, 0xC0);  // enable adc
  } else {
    res |= es8388_write_reg(address, ES8388_DACCONTROL21, 0x80);  // enable dac
  }
  es8388_read_reg(ES8388_DACCONTROL21, &data);
  if (prev_data != data) {
    res |= es8388_write_reg(address, ES8388_CHIPPOWER,
                        0xF0);  // start state machine
    // res |= es8388_write_reg(address, ES8388_CONTROL1, 0x16);
    // res |= es8388_write_reg(address, ES8388_CONTROL2, 0x50);
    res |= es8388_write_reg(address, ES8388_CHIPPOWER,
                        0x00);  // start state machine
  }
  if (mode == CODEC_MODE_ENCODE || mode == CODEC_MODE_BOTH ||
      mode == CODEC_MODE_LINE_IN) {
    res |= es8388_write_reg(address, ES8388_ADCPOWER,
                        0x00);  // power up adc and line in
    AD_LOGD("es8388_start default is mode:%d", mode);
  }
  if (mode == CODEC_MODE_DECODE || mode == CODEC_MODE_BOTH ||
      mode == CODEC_MODE_LINE_IN) {
    res |= es8388_write_reg(address, ES8388_DACPOWER,
                        dac_power);  // power up dac and line out
    res |= es8388_set_voice_mute(false);
    AD_LOGD("es8388_start default is mode:%d", mode);
  }

  return res;
}

/**
 * @brief  Stop ES8388 codec chip
 *
 * @param mode:  set ADC or DAC or both
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_stop(codec_mode_t mode) {
  AD_TRACED();
  error_t res = RESULT_OK;
  if (mode == CODEC_MODE_LINE_IN) {
    res |= es8388_write_reg(address, ES8388_DACCONTROL21, 0x80);  // enable dac
    res |= es8388_write_reg(address, ES8388_DACCONTROL16,
                        0x00);  // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
    res |= es8388_write_reg(address, ES8388_DACCONTROL17,
                        0x90);  // only left DAC to left mixer enable 0db
    res |= es8388_write_reg(address, ES8388_DACCONTROL20,
                        0x90);  // only right DAC to right mixer enable 0db
    return res;
  }
  if (mode == CODEC_MODE_DECODE || mode == CODEC_MODE_BOTH) {
    res |= es8388_write_reg(address, ES8388_DACPOWER, 0x00);
    res |= es8388_set_voice_mute(true);
    // res |= Es8388SetAdcDacVolume(CODEC_MODE_DECODE, -96, 5);      // 0db
    // res |= es8388_write_reg(address, ES8388_DACPOWER, 0xC0);  //power down
    // dac and line out
  }
  if (mode == CODEC_MODE_ENCODE || mode == CODEC_MODE_BOTH) {
    // res |= Es8388SetAdcDacVolume(CODEC_MODE_ENCODE, -96, 5);      // 0db
    res |= es8388_write_reg(address, ES8388_ADCPOWER,
                        0xFF);  // power down adc and line in
  }
  if (mode == CODEC_MODE_BOTH) {
    res |= es8388_write_reg(address, ES8388_DACCONTROL21, 0x9C);  // disable
                                                              // mclk
    //        res |= es8388_write_reg(address, ES8388_CONTROL1, 0x00);
    //        res |= es8388_write_reg(address, ES8388_CONTROL2, 0x58);
    //        res |= es8388_write_reg(address, ES8388_CHIPPOWER, 0xF3);  //stop
    //        state machine
  }

  return res;
}

/**
 * @brief Configure ES8388 I2S format
 *
 * @param mod:  set ADC or DAC or both
 * @param cfg:   ES8388 I2S format
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_config_fmt(codec_mode_t mod, i2s_format_t cfg) {
  AD_TRACED();
  error_t res = RESULT_OK;
  uint8_t reg = 0;
  if (mod == CODEC_MODE_ENCODE || mod == CODEC_MODE_BOTH) {
    res = es8388_read_reg(ES8388_ADCCONTROL4, &reg);
    reg = reg & 0xfc;
    res |= es8388_write_reg(address, ES8388_ADCCONTROL4, reg | cfg);
  }
  if (mod == CODEC_MODE_DECODE || mod == CODEC_MODE_BOTH) {
    res = es8388_read_reg(ES8388_DACCONTROL1, &reg);
    reg = reg & 0xf9;
    res |= es8388_write_reg(address, ES8388_DACCONTROL1, reg | (cfg << 1));
  }
  return res;
}

/**
 * @brief Configure I2s clock in MSATER mode
 *
 * @param cfg:  set bits clock and WS clock
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_i2s_config_clock(es_i2s_clock_t cfg) {
  AD_TRACED();
  error_t res = RESULT_OK;
  res |= es8388_write_reg(address, ES8388_MASTERMODE, cfg.sclk_div);
  res |= es8388_write_reg(address, ES8388_ADCCONTROL5,
                      cfg.lclk_div);  // ADCFsMode,singel SPEED,RATIO=256
  res |= es8388_write_reg(address, ES8388_DACCONTROL2,
                      cfg.lclk_div);  // ADCFsMode,singel SPEED,RATIO=256
  return res;
}

/**
 * @brief Deinitialize ES8388 codec chip
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_deinit(void) {
  AD_TRACED();
  int res = 0;
  res = es8388_write_reg(address, ES8388_CHIPPOWER,
                     0xFF);  // reset and stop es8388
#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
  headphone_detect_deinit();
#endif

  return res;
}

/**
 * @brief Initialize ES8388 codec chip
 *
 * @param cfg configuration of ES8388
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_init(codec_config_t* cfg, i2c_bus_handle_t handle,
                           int addr, int volumeHack) {
  AD_TRACED();
  es8388_i2c_handle = handle;
  esp8388_volume_hack = volumeHack;
  // set address
  if (addr > 0) address = addr;

  int res = 0;

  res |= es8388_write_reg(address, ES8388_DACCONTROL3,
                      0x04);  // 0x04 mute/0x00 unmute&ramp;DAC unmute and
                              // disabled digital volume control soft ramp
  /* Chip Control and Power Management */
  res |= es8388_write_reg(address, ES8388_CONTROL2, 0x50);
  res |= es8388_write_reg(address, ES8388_CHIPPOWER,
                      0x00);  // normal all and power up all

  // Disable the internal DLL to improve 8K sample rate
  res |= es8388_write_reg(address, 0x35, 0xA0);
  res |= es8388_write_reg(address, 0x37, 0xD0);
  res |= es8388_write_reg(address, 0x39, 0xD0);

  res |= es8388_write_reg(address, ES8388_MASTERMODE,
                      cfg->i2s.mode);  // CODEC IN I2S SLAVE MODE

  /* dac */
  res |= es8388_write_reg(address, ES8388_DACPOWER,
                      0xC0);  // disable DAC and disable Lout/Rout/1/2
  res |= es8388_write_reg(address, ES8388_CONTROL1,
                      0x12);  // Enfr=0,Play&Record Mode,(0x17-both of mic&paly)
  //    res |= es8388_write_reg(address, ES8388_CONTROL2, 0);
  //    //LPVrefBuf=0,Pdn_ana=0
  res |= es8388_write_reg(address, ES8388_DACCONTROL1,
                      0x18);  // 1a 0x18:16bit iis , 0x00:24
  res |= es8388_write_reg(address, ES8388_DACCONTROL2,
                      0x02);  // DACFsMode,SINGLE SPEED; DACFsRatio,256
  res |= es8388_write_reg(address, ES8388_DACCONTROL16,
                      0x00);  // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
  res |= es8388_write_reg(address, ES8388_DACCONTROL17,
                      0x90);  // only left DAC to left mixer enable 0db
  res |= es8388_write_reg(address, ES8388_DACCONTROL20,
                      0x90);  // only right DAC to right mixer enable 0db
  res |= es8388_write_reg(address, ES8388_DACCONTROL21,
                      0x80);  // set internal ADC and DAC use the same LRCK
                              // clock, ADC LRCK as internal LRCK
  res |= es8388_write_reg(address, ES8388_DACCONTROL23, 0x00);    // vroi=0
  res |= es8388_set_adc_dac_volume(CODEC_MODE_DECODE, 0, 0);  // 0db
  dac_power = 0;
  AD_LOGI("output_device: %d", cfg->output_device);
  if (DAC_OUTPUT_LINE1 == cfg->output_device) {
    dac_power = ES8388_OUTPUT_LOUT1 | ES8388_OUTPUT_ROUT1;
  } else if (DAC_OUTPUT_LINE2 == cfg->output_device) {
    dac_power = ES8388_OUTPUT_LOUT2 | ES8388_OUTPUT_ROUT2;
  } else if (DAC_OUTPUT_ALL == cfg->output_device) {
    dac_power = ES8388_OUTPUT_LOUT1 | ES8388_OUTPUT_LOUT2 |
                ES8388_OUTPUT_ROUT1 | ES8388_OUTPUT_ROUT2;
  }
  res |= es8388_write_reg(address, ES8388_DACPOWER,
                      dac_power);  // 0x3c Enable DAC and Enable Lout/Rout/1/2
  /* adc */
  res |= es8388_write_reg(address, ES8388_ADCPOWER, 0xFF);

  // // AudioDriver: WORKAROUND_MIC_LINEIN_MIXED
  es_mic_gain_t mic_gain = (es_mic_gain_t)ES8388_DEFAULT_INPUT_GAIN;
  // TODO
  // if (WORKAROUND_MIC_LINEIN_MIXED ){
  //     // logic for Audiokit: line 1 does not work, so we also use line2 but
  //     with a different gain if (ADC_INPUT_LINE1 == cfg->input_device){
  //         mic_gain = (es_mic_gain_t)WORKAROUND_ES8388_LINE1_GAIN;
  //         cfg->input_device= ADC_INPUT_LINE2;
  //     }
  // }
  // res |= es8388_write_reg(address, ES8388_ADCCONTROL1, 0xbb); // MIC Left and
  // Right channel PGA gain res |= es8388_write_reg(address, ES8388_ADCCONTROL1,
  // ES8388_DEFAULT_INPUT_GAIN); // MIC Left and Right channel PGA gain
  res |= es8388_set_mic_gain(mic_gain);
  int tmp = 0;
  if (ADC_INPUT_LINE1 == cfg->input_device) {
    tmp = ESP8388_INPUT_LINPUT1_RINPUT1;
  } else if (ADC_INPUT_LINE2 == cfg->input_device) {
    tmp = ESP8388_INPUT_LINPUT2_RINPUT2;
  } else {
    tmp = ESP8388_INPUT_DIFFERENCE;
  }
  res |= es8388_write_reg(address, ES8388_ADCCONTROL2,
                      tmp);  // 0x00 LINSEL & RINSEL, LIN1/RIN1 as ADC Input;
                             // DSSEL,use one DS Reg11; DSR, LINPUT1-RINPUT1
  res |= es8388_write_reg(address, ES8388_ADCCONTROL3, 0x02);
  res |= es8388_write_reg(address, ES8388_ADCCONTROL4,
                      0x0d);  // Left/Right data, Left/Right justified mode,
                              // Bits length, I2S format
  res |= es8388_write_reg(address, ES8388_ADCCONTROL5,
                      0x02);  // ADCFsMode,singel SPEED,RATIO=256
  // ALC for Microphone
  res |= es8388_set_adc_dac_volume(CODEC_MODE_ENCODE, 0, 0);  // 0db
  res |= es8388_write_reg(address, ES8388_ADCPOWER,
                      0x09);  // Power on ADC, Enable LIN&RIN, Power off
                              // MICBIAS, set int1lp to low power mode
  // es8388_pa_power(cfg->_DAC_OUTPUT!=ES8388_OUTPUT_LINE2);
  //  AD_LOGI("init,out:%02x, in:%02x", cfg->_DAC_OUTPUT, cfg->input_device);
  return res;
}

/**
 * @brief Configure ES8388 data sample bits
 *
 * @param mode:  set ADC or DAC or both
 * @param bit_per_sample:  bit number of per sample
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es8388_set_bits_per_sample(codec_mode_t mode,
                                          sample_bits_t bit_per_sample) {
  AD_TRACED();
  error_t res = RESULT_OK;
  uint8_t reg = 0;
  int bits = (int)bit_per_sample;

  if (mode == CODEC_MODE_ENCODE || mode == CODEC_MODE_BOTH) {
    res = es8388_read_reg(ES8388_ADCCONTROL4, &reg);
    reg = reg & 0xe3;
    res |= es8388_write_reg(address, ES8388_ADCCONTROL4, reg | (bits << 2));
  }
  if (mode == CODEC_MODE_DECODE || mode == CODEC_MODE_BOTH) {
    res = es8388_read_reg(ES8388_DACCONTROL1, &reg);
    reg = reg & 0xc7;
    res |= es8388_write_reg(address, ES8388_DACCONTROL1, reg | (bits << 3));
  }
  return res;
}

/**
 * @brief Configure ES8388 DAC mute or not. Basically you can use this function
 * to mute the output or unmute
 *
 * @param enable enable(1) or disable(0)
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */

/**
 * @brief Get ES8388 DAC mute status
 *
 *  @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es8388_get_voice_mute(void) {
  AD_TRACED();
  error_t res = RESULT_OK;
  uint8_t reg = 0;
  res = es8388_read_reg(ES8388_DACCONTROL3, &reg);
  if (res == RESULT_OK) {
    reg = (reg & 0x04) >> 2;
  }
  return res == RESULT_OK ? reg : res;
}

/**
 * @brief Set ES8388 mic gain
 *
 * @param gain db of mic gain
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */

/**
 * @brief Set ES8388 adc input mode
 *
 * @param input adc input mode
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es8388_config_input_device(es8388_input_device_t input) {
  AD_TRACED();
  error_t res;
  uint8_t reg = 0;
  res = es8388_read_reg(ES8388_ADCCONTROL2, &reg);
  reg = reg & 0x0f;
  res |= es8388_write_reg(address, ES8388_ADCCONTROL2, reg | input);
  return res;
}

/**
 * @brief Set ES8388 dac output mode
 *
 * @param output dac output mode
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es8388_config_output_device(output_device_t output_device) {
  AD_TRACED();
  AD_LOGI("output_device: %d", output_device);

  uint8_t reg = 0;
  error_t res = es8388_read_reg(ES8388_DACPOWER, &reg);
  reg = reg & 0xC3;  // keep 11000011

  uint8_t value = 0;
  switch (output_device) {
    case DAC_OUTPUT_LINE1:
      value = ES8388_OUTPUT_LOUT1 | ES8388_OUTPUT_ROUT1;
      AD_LOGI("DAC_OUTPUT_LINE2:  0x%x", reg | value);
      break;
    case DAC_OUTPUT_LINE2:
      value = ES8388_OUTPUT_LOUT2 | ES8388_OUTPUT_ROUT2;
      AD_LOGI("DAC_OUTPUT_LINE1: 0x%x", reg | value);
      break;
    case DAC_OUTPUT_ALL:
      value = ES8388_OUTPUT_LOUT1 | ES8388_OUTPUT_LOUT2 | ES8388_OUTPUT_ROUT1 |
              ES8388_OUTPUT_ROUT2;
      AD_LOGI("DAC_OUTPUT_ALL: 0x%x", reg | value);
      break;
    case DAC_OUTPUT_NONE:
      value = 0;
      AD_LOGI("DAC_OUTPUT_NONE: 0x%x", reg | value);
      break;
  }
  res |= es8388_write_reg(address, ES8388_DACPOWER, reg | value);
  return res;
}

/**
 * @brief Write ES8388 register
 *
 * @param reg_add address of register
 * @param data data of register
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */

/**
 * @brief Reads ES8388 register
 *
 * @param reg_add address of register
 * @param data data of register
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */

/**
 * @brief Print all ES8388 registers
 *
 * @return
 *     - void
 */

/**
 * @brief Configure ES8388 codec mode and I2S interface
 *
 * @param mode codec mode
 * @param iface I2S config
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es8388_config_i2s(codec_mode_t mode, I2SDefinition* iface) {
  AD_TRACED();
  error_t res = RESULT_OK;
  int tmp = 0;
  res |= es8388_config_fmt(CODEC_MODE_BOTH, iface->fmt);
  if (iface->bits == BIT_LENGTH_16BITS) {
    tmp = BIT_LENGTH_16BITS;
  } else if (iface->bits == BIT_LENGTH_24BITS) {
    tmp = BIT_LENGTH_24BITS;
  } else {
    tmp = BIT_LENGTH_32BITS;
  }
  res |= es8388_set_bits_per_sample(CODEC_MODE_BOTH, (sample_bits_t)tmp);
  return res;
}

/**
 * @brief Control ES8388 codec chip
 *
 * @param mode codec mode
 * @param ctrl_state_active start or stop decode or encode progress
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es8388_ctrl_state_active(codec_mode_t mode,
                                        bool ctrl_state_active) {
  AD_LOGD("es8388_ctrl_state_active: %d, %d", mode, ctrl_state_active);
  int res = 0;
  int es_mode_t = 0;
  switch (mode) {
    case CODEC_MODE_ENCODE:
      es_mode_t = CODEC_MODE_ENCODE;
      break;
    case CODEC_MODE_LINE_IN:
      es_mode_t = CODEC_MODE_LINE_IN;
      break;
    case CODEC_MODE_DECODE:
      es_mode_t = CODEC_MODE_DECODE;
      break;
    case CODEC_MODE_BOTH:
      es_mode_t = CODEC_MODE_BOTH;
      break;
    default:
      es_mode_t = CODEC_MODE_DECODE;
      AD_LOGW("Codec mode not support, default is decode mode");
      break;
  }
  if (!ctrl_state_active) {
    res = es8388_stop((codec_mode_t)es_mode_t);
  } else {
    res = es8388_start((codec_mode_t)es_mode_t);
    AD_LOGD("start default is decode mode:%d", es_mode_t);
  }
  return res;
}

}  // namespace audio_driver
