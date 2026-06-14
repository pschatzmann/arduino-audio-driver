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

#include <string.h>

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"

#define ES7210_RESET_REG00 0x00      /* Reset control */
#define ES7210_CLOCK_OFF_REG01 0x01  /* Used to turn off the ADC clock */
#define ES7210_MAINCLK_REG02 0x02    /* Set ADC clock frequency division */
#define ES7210_MASTER_CLK_REG03 0x03 /* MCLK source $ SCLK division */
#define ES7210_LRCK_DIVH_REG04 0x04  /* lrck_divh */
#define ES7210_LRCK_DIVL_REG05 0x05  /* lrck_divl */
#define ES7210_POWER_DOWN_REG06 0x06 /* power down */
#define ES7210_OSR_REG07 0x07
#define ES7210_MODE_CONFIG_REG08 0x08     /* Set master/slave & channels */
#define ES7210_TIME_CONTROL0_REG09 0x09   /* Set Chip intial state period*/
#define ES7210_TIME_CONTROL1_REG0A 0x0A   /* Set Power up state period */
#define ES7210_SDP_INTERFACE1_REG11 0x11  /* Set sample & fmt */
#define ES7210_SDP_INTERFACE2_REG12 0x12  /* Pins state */
#define ES7210_ADC_AUTOMUTE_REG13 0x13    /* Set mute */
#define ES7210_ADC34_MUTERANGE_REG14 0x14 /* Set mute range */
#define ES7210_ADC34_HPF2_REG20 0x20      /* HPF */
#define ES7210_ADC34_HPF1_REG21 0x21
#define ES7210_ADC12_HPF1_REG22 0x22
#define ES7210_ADC12_HPF2_REG23 0x23
#define ES7210_ANALOG_REG40 0x40 /* ANALOG Power */
#define ES7210_MIC12_BIAS_REG41 0x41
#define ES7210_MIC34_BIAS_REG42 0x42
#define ES7210_MIC1_ES7210_GAIN_REG43 0x43
#define ES7210_MIC2_ES7210_GAIN_REG44 0x44
#define ES7210_MIC3_ES7210_GAIN_REG45 0x45
#define ES7210_MIC4_ES7210_GAIN_REG46 0x46
#define ES7210_MIC1_POWER_REG47 0x47
#define ES7210_MIC2_POWER_REG48 0x48
#define ES7210_MIC3_POWER_REG49 0x49
#define ES7210_MIC4_POWER_REG4A 0x4A
#define ES7210_MIC12_POWER_REG4B 0x4B /* MICBias & ADC & PGA Power */
#define ES7210_MIC34_POWER_REG4C 0x4C

#define I2S_DSP_MODE 0
#define MCLK_DIV_FRE 256
#define TAG_ES7210 "ES7210"

/* ES7210 address */
#define ES7210_ADDR ES7210_AD1_AD0_00 >> 1
#define FROM_PAD_PIN 0
#define FROM_CLOCK_DOUBLE_PIN 1
#define ES7210_MCLK_SOURCE                                                    \
  FROM_CLOCK_DOUBLE_PIN /* In master mode, 0 : MCLK from pad    1 : MCLK from \
                           clock doubler */

namespace audio_driver {

enum es7210_address_t {
  ES7210_AD1_AD0_00 = 0x80,
  ES7210_AD1_AD0_01 = 0x82,
  ES7210_AD1_AD0_10 = 0x84,
  ES7210_AD1_AD0_11 = 0x86
};

enum es7210_input_mics_t {
  ES7210_INPUT_MIC1 = 0x01,
  ES7210_INPUT_MIC2 = 0x02,
  ES7210_INPUT_MIC3 = 0x04,
  ES7210_INPUT_MIC4 = 0x08
};

enum es7210_gain_value_t {
  ES7210_GAIN_0DB = 0,
  ES7210_GAIN_3DB,
  ES7210_GAIN_6DB,
  ES7210_GAIN_9DB,
  ES7210_GAIN_12DB,
  ES7210_GAIN_15DB,
  ES7210_GAIN_18DB,
  ES7210_GAIN_21DB,
  ES7210_GAIN_24DB,
  ES7210_GAIN_27DB,
  ES7210_GAIN_30DB,
  ES7210_GAIN_33DB,
  ES7210_GAIN_34_5DB,
  ES7210_GAIN_36DB,
  ES7210_GAIN_37_5DB,
};

/*
 * Clock coefficient structer
 */
struct _coeff_div {
  uint32_t mclk; /* mclk frequency */
  uint32_t lrck; /* lrck */
  uint8_t ss_ds;
  uint8_t adc_div;  /* adcclk divider */
  uint8_t dll;      /* dll_bypass */
  uint8_t doubler;  /* doubler enable */
  uint8_t osr;      /* adc osr */
  uint8_t mclk_src; /* select mclk  source */
  uint32_t lrck_h;  /* The high 4 bits of lrck */
  uint32_t lrck_l;  /* The low 8 bits of lrck */
};

static i2c_bus_handle_t es7210_i2c_handle = NULL;
static es7210_input_mics_t mic_select = static_cast<es7210_input_mics_t>(
    ES7210_INPUT_MIC1 | ES7210_INPUT_MIC2); /* Number of microphones */

/* Codec hifi mclk clock divider coefficients
 *           MEMBER      REG
 *           mclk:       0x03
 *           lrck:       standard
 *           ss_ds:      --
 *           adc_div:    0x02
 *           dll:        0x06
 *           doubler:    0x02
 *           osr:        0x07
 *           mclk_src:   0x03
 *           lrckh:      0x04
 *           lrckl:      0x05
 */
static const struct _coeff_div es7210_coeff_div[] = {
    // mclk      lrck    ss_ds adc_div  dll  doubler osr  mclk_src  lrckh lrckl
    /* 8k */
    {12288000, 8000, 0x00, 0x03, 0x01, 0x00, 0x20, 0x00, 0x06, 0x00},
    {16384000, 8000, 0x00, 0x04, 0x01, 0x00, 0x20, 0x00, 0x08, 0x00},
    {19200000, 8000, 0x00, 0x1e, 0x00, 0x01, 0x28, 0x00, 0x09, 0x60},
    {4096000, 8000, 0x00, 0x01, 0x01, 0x00, 0x20, 0x00, 0x02, 0x00},

    /* 11.025k */
    {11289600, 11025, 0x00, 0x02, 0x01, 0x00, 0x20, 0x00, 0x01, 0x00},

    /* 12k */
    {12288000, 12000, 0x00, 0x02, 0x01, 0x00, 0x20, 0x00, 0x04, 0x00},
    {19200000, 12000, 0x00, 0x14, 0x00, 0x01, 0x28, 0x00, 0x06, 0x40},

    /* 16k */
    {4096000, 16000, 0x00, 0x01, 0x01, 0x01, 0x20, 0x00, 0x01, 0x00},
    {19200000, 16000, 0x00, 0x0a, 0x00, 0x00, 0x1e, 0x00, 0x04, 0x80},
    {16384000, 16000, 0x00, 0x02, 0x01, 0x00, 0x20, 0x00, 0x04, 0x00},
    {12288000, 16000, 0x00, 0x03, 0x01, 0x01, 0x20, 0x00, 0x03, 0x00},

    /* 22.05k */
    {11289600, 22050, 0x00, 0x01, 0x01, 0x00, 0x20, 0x00, 0x02, 0x00},

    /* 24k */
    {12288000, 24000, 0x00, 0x01, 0x01, 0x00, 0x20, 0x00, 0x02, 0x00},
    {19200000, 24000, 0x00, 0x0a, 0x00, 0x01, 0x28, 0x00, 0x03, 0x20},

    /* 32k */
    {12288000, 32000, 0x00, 0x03, 0x00, 0x00, 0x20, 0x00, 0x01, 0x80},
    {16384000, 32000, 0x00, 0x01, 0x01, 0x00, 0x20, 0x00, 0x02, 0x00},
    {19200000, 32000, 0x00, 0x05, 0x00, 0x00, 0x1e, 0x00, 0x02, 0x58},

    /* 44.1k */
    {11289600, 44100, 0x00, 0x01, 0x01, 0x01, 0x20, 0x00, 0x01, 0x00},

    /* 48k */
    {12288000, 48000, 0x00, 0x01, 0x01, 0x01, 0x20, 0x00, 0x01, 0x00},
    {19200000, 48000, 0x00, 0x05, 0x00, 0x01, 0x28, 0x00, 0x01, 0x90},

    /* 64k */
    {16384000, 64000, 0x01, 0x01, 0x01, 0x00, 0x20, 0x00, 0x01, 0x00},
    {19200000, 64000, 0x00, 0x05, 0x00, 0x01, 0x1e, 0x00, 0x01, 0x2c},

    /* 88.2k */
    {11289600, 88200, 0x01, 0x01, 0x01, 0x01, 0x20, 0x00, 0x00, 0x80},

    /* 96k */
    {12288000, 96000, 0x01, 0x01, 0x01, 0x01, 0x20, 0x00, 0x00, 0x80},
    {19200000, 96000, 0x01, 0x05, 0x00, 0x01, 0x28, 0x00, 0x00, 0xc8},
};

/**
 * @brief Read regs of ES7210
 *
 * @param[in] reg_addr:  reg_addr
 *
 * @return
 *     - RESULT_FAIL
 *     - RESULT_OK
 */
inline int es7210_read_reg(uint8_t reg_addr) {
  uint8_t data;
  i2c_bus_read_bytes(es7210_i2c_handle, ES7210_ADDR, &reg_addr, sizeof(reg_addr),
                     &data, sizeof(data));
  return (int)data;
}

static inline error_t es7210_write_reg(uint8_t reg_addr, uint8_t data) {
  return i2c_bus_write_bytes(es7210_i2c_handle, ES7210_ADDR, &reg_addr,
                             sizeof(reg_addr), &data, sizeof(data));
}

static inline error_t es7210_update_reg_bit(uint8_t reg_addr,
                                            uint8_t update_bits,
                                            uint8_t data) {
  uint8_t regv;
  regv = es7210_read_reg(reg_addr);
  regv = (regv & (~update_bits)) | (update_bits & data);
  return es7210_write_reg(reg_addr, regv);
}

static inline int es7210_get_coeff(uint32_t mclk, uint32_t lrck) {
  for (unsigned i = 0; i < (sizeof(es7210_coeff_div) / sizeof(es7210_coeff_div[0])); i++) {
    if (es7210_coeff_div[i].lrck == lrck && es7210_coeff_div[i].mclk == mclk) return i;
  }
  return -1;
}

inline int8_t get_es7210_mclk_src(void) { return ES7210_MCLK_SOURCE; }

inline error_t es7210_config_sample(samplerate_t sample) {
  uint8_t regv;
  int coeff;
  int sample_fre = 0;
  int mclk_fre = 0;
  error_t ret = RESULT_OK;
  switch (sample) {
    case RATE_8K:
      sample_fre = 8000;
      break;
    case RATE_11K:
      sample_fre = 11025;
      break;
    case RATE_16K:
      sample_fre = 16000;
      break;
    case RATE_22K:
      sample_fre = 22050;
      break;
    case RATE_24K:
      sample_fre = 24000;
      break;
    case RATE_32K:
      sample_fre = 32000;
      break;
    case RATE_44K:
      sample_fre = 44100;
      break;
    case RATE_48K:
      sample_fre = 48000;
      break;
    default:
      AD_LOGE("Unable to configure sample rate %dHz", sample_fre);
      break;
  }
  mclk_fre = sample_fre * MCLK_DIV_FRE;
  coeff = es7210_get_coeff(mclk_fre, sample_fre);
  if (coeff < 0) {
    AD_LOGE("Unable to configure sample rate %dHz with %dHz MCLK", sample_fre,
            mclk_fre);
    return RESULT_FAIL;
  }
  /* Set clock parammeters */
  if (coeff >= 0) {
    /* Set adc_div & doubler & dll */
    regv = es7210_read_reg(ES7210_MAINCLK_REG02) & 0x00;
    regv |= es7210_coeff_div[coeff].adc_div;
    regv |= es7210_coeff_div[coeff].doubler << 6;
    regv |= es7210_coeff_div[coeff].dll << 7;
    ret |= es7210_write_reg(ES7210_MAINCLK_REG02, regv);
    /* Set osr */
    regv = es7210_coeff_div[coeff].osr;
    ret |= es7210_write_reg(ES7210_OSR_REG07, regv);
    /* Set lrck */
    regv = es7210_coeff_div[coeff].lrck_h;
    ret |= es7210_write_reg(ES7210_LRCK_DIVH_REG04, regv);
    regv = es7210_coeff_div[coeff].lrck_l;
    ret |= es7210_write_reg(ES7210_LRCK_DIVL_REG05, regv);
  }
  return ret;
}

/**
 * @brief Select ES7210 mic
 *
 * @param[in] mic:  mics
 *
 * @return
 *     - RESULT_FAIL
 *     - RESULT_OK
 */
inline error_t es7210_mic_select(es7210_input_mics_t mic) {
  error_t ret = RESULT_OK;
  mic_select = mic;
  if (mic_select & (ES7210_INPUT_MIC1 | ES7210_INPUT_MIC2 | ES7210_INPUT_MIC3 |
                    ES7210_INPUT_MIC4)) {
    for (int i = 0; i < 4; i++) {
      ret |=
          es7210_update_reg_bit(ES7210_MIC1_ES7210_GAIN_REG43 + i, 0x10, 0x00);
    }
    ret |= es7210_write_reg(ES7210_MIC12_POWER_REG4B, 0xff);
    ret |= es7210_write_reg(ES7210_MIC34_POWER_REG4C, 0xff);
    if (mic_select & ES7210_INPUT_MIC1) {
      AD_LOGI("Enable ES7210_INPUT_MIC1");
      ret |= es7210_update_reg_bit(ES7210_CLOCK_OFF_REG01, 0x0b, 0x00);
      ret |= es7210_write_reg(ES7210_MIC12_POWER_REG4B, 0x00);
      ret |= es7210_update_reg_bit(ES7210_MIC1_ES7210_GAIN_REG43, 0x10, 0x10);
    }
    if (mic_select & ES7210_INPUT_MIC2) {
      AD_LOGI("Enable ES7210_INPUT_MIC2");
      ret |= es7210_update_reg_bit(ES7210_CLOCK_OFF_REG01, 0x0b, 0x00);
      ret |= es7210_write_reg(ES7210_MIC12_POWER_REG4B, 0x00);
      ret |= es7210_update_reg_bit(ES7210_MIC2_ES7210_GAIN_REG44, 0x10, 0x10);
    }
    if (mic_select & ES7210_INPUT_MIC3) {
      AD_LOGI("Enable ES7210_INPUT_MIC3");
      ret |= es7210_update_reg_bit(ES7210_CLOCK_OFF_REG01, 0x15, 0x00);
      ret |= es7210_write_reg(ES7210_MIC34_POWER_REG4C, 0x00);
      ret |= es7210_update_reg_bit(ES7210_MIC3_ES7210_GAIN_REG45, 0x10, 0x10);
    }
    if (mic_select & ES7210_INPUT_MIC4) {
      AD_LOGI("Enable ES7210_INPUT_MIC4");
      ret |= es7210_update_reg_bit(ES7210_CLOCK_OFF_REG01, 0x15, 0x00);
      ret |= es7210_write_reg(ES7210_MIC34_POWER_REG4C, 0x00);
      ret |= es7210_update_reg_bit(ES7210_MIC4_ES7210_GAIN_REG46, 0x10, 0x10);
    }
  } else {
    AD_LOGE("Microphone selection error");
    return RESULT_FAIL;
  }
  return ret;
}

static inline error_t set_mics_for_channels(channels_t ch) {
  error_t ret = RESULT_OK;
  switch (ch) {
    case CHANNELS2:
      mic_select = static_cast<es7210_input_mics_t>(ES7210_INPUT_MIC1 |
                                                    ES7210_INPUT_MIC2);
      break;
    case CHANNELS4:
      mic_select = static_cast<es7210_input_mics_t>(
          ES7210_INPUT_MIC1 | ES7210_INPUT_MIC2 | ES7210_INPUT_MIC3 |
          ES7210_INPUT_MIC4);
      break;
    default:
      AD_LOGE("Unsupported channels: %d", (int)ch);
      ret = RESULT_FAIL;
  }
  return ret;
}

/**
 * @brief  Set gain (Note: the enabled microphone sets the same gain)
 *
 * @param[in] gain:  gain
 *
 *       gain        :   value
 *       ES7210_GAIN_0DB    :   1
 *       ES7210_GAIN_3DB    :   2
 *       ES7210_GAIN_6DB    :   3
 *           .
 *           .
 *           .
 *       ES7210_GAIN_30DB   :   10
 *       ES7210_GAIN_33DB   :   11
 *       ES7210_GAIN_34_5DB :   12
 *       ES7210_GAIN_36DB   :   13
 *       ES7210_GAIN_37_5DB :   14
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es7210_adc_set_gain(es7210_gain_value_t gain) {
  AD_LOGD("es7210_adc_set_gain:%d", gain);
  error_t ret = RESULT_OK;
  es7210_gain_value_t max_gain_vaule = static_cast<es7210_gain_value_t>(14);
  if (gain < 0) {
    gain = (es7210_gain_value_t)0;
  } else if (gain > max_gain_vaule) {
    gain = max_gain_vaule;
  }
  if (mic_select & ES7210_INPUT_MIC1) {
    ret |= es7210_update_reg_bit(ES7210_MIC1_ES7210_GAIN_REG43, 0x0f, gain);
  }
  if (mic_select & ES7210_INPUT_MIC2) {
    ret |= es7210_update_reg_bit(ES7210_MIC2_ES7210_GAIN_REG44, 0x0f, gain);
  }
  if (mic_select & ES7210_INPUT_MIC3) {
    ret |= es7210_update_reg_bit(ES7210_MIC3_ES7210_GAIN_REG45, 0x0f, gain);
  }
  if (mic_select & ES7210_INPUT_MIC4) {
    ret |= es7210_update_reg_bit(ES7210_MIC4_ES7210_GAIN_REG46, 0x0f, gain);
  }
  return ret;
}

/*
 * @brief Initialize ES7210 ADC chip
 *
 * @param[in] codec_cfg:  configuration of ES7210
 *
 * @return
 *      - RESULT_OK
 *      - RESULT_FAIL
 */
inline error_t es7210_adc_init(codec_config_t* codec_cfg,
                               i2c_bus_handle_t i2c) {
  // select microphones
  error_t ret = set_mics_for_channels(codec_cfg->i2s.channels);

  es7210_i2c_handle = i2c;
  ret |= es7210_write_reg(ES7210_RESET_REG00, 0xff);
  ret |= es7210_write_reg(ES7210_RESET_REG00, 0x41);
  ret |= es7210_write_reg(ES7210_CLOCK_OFF_REG01, 0x1f);
  ret |= es7210_write_reg(ES7210_TIME_CONTROL0_REG09,
                          0x30); /* Set chip state cycle */
  ret |= es7210_write_reg(ES7210_TIME_CONTROL1_REG0A,
                          0x30); /* Set power on state cycle */
  ret |= es7210_write_reg(ES7210_ADC12_HPF2_REG23, 0x2a); /* Quick setup */
  ret |= es7210_write_reg(ES7210_ADC12_HPF1_REG22, 0x0a);
  ret |= es7210_write_reg(ES7210_ADC34_HPF2_REG20, 0x0a);
  ret |= es7210_write_reg(ES7210_ADC34_HPF1_REG21, 0x2a);
  /* Set master/slave audio interface */
  I2SDefinition* i2s_cfg = &(codec_cfg->i2s);
  switch (i2s_cfg->mode) {
    case MODE_MASTER: /* MASTER MODE */
      AD_LOGI("ES7210 in Master mode");
      ret |= es7210_update_reg_bit(ES7210_MODE_CONFIG_REG08, 0x01, 0x01);
      /* Select clock source for internal mclk */
      switch (get_es7210_mclk_src()) {
        case FROM_PAD_PIN:
          ret |= es7210_update_reg_bit(ES7210_MASTER_CLK_REG03, 0x80, 0x00);
          break;
        case FROM_CLOCK_DOUBLE_PIN:
          ret |= es7210_update_reg_bit(ES7210_MASTER_CLK_REG03, 0x80, 0x80);
          break;
        default:
          ret |= es7210_update_reg_bit(ES7210_MASTER_CLK_REG03, 0x80, 0x00);
          break;
      }
      break;
    case MODE_SLAVE: /* SLAVE MODE */
      AD_LOGI("ES7210 in Slave mode");
      ret |= es7210_update_reg_bit(ES7210_MODE_CONFIG_REG08, 0x01, 0x00);
      break;
    default:
      ret |= es7210_update_reg_bit(ES7210_MODE_CONFIG_REG08, 0x01, 0x00);
  }
  ret |= es7210_write_reg(ES7210_ANALOG_REG40,
                          0x43); /* Select power off analog, vdda = 3.3V, close
                                    vx20ff, VMID select 5KΩ start */
  ret |= es7210_write_reg(ES7210_MIC12_BIAS_REG41, 0x70); /* Select 2.87v */
  ret |= es7210_write_reg(ES7210_MIC34_BIAS_REG42, 0x70); /* Select 2.87v */
  ret |= es7210_write_reg(ES7210_OSR_REG07, 0x20);
  ret |= es7210_write_reg(
      ES7210_MAINCLK_REG02,
      0xc1); /* Set the frequency division coefficient and use dll except clock
                doubler, and need to set 0xc1 to clear the state */
  ret |= es7210_config_sample(i2s_cfg->rate);
  ret |= es7210_mic_select(mic_select);
  ret |= es7210_adc_set_gain(ES7210_GAIN_30DB);

  // set REG12
  uint8_t reg12 = 0;
  if (i2s_cfg->signal_type == SIGNAL_TDM) {
    reg12 = 0x02;  // TDM I2S
  }
  ret |= es7210_write_reg(ES7210_SDP_INTERFACE2_REG12, reg12);

  return RESULT_OK;
}

/**
 * @brief Deinitialize ES7210 ADC chip
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es7210_adc_deinit() { return RESULT_OK; }

inline error_t es7210_config_fmt(i2s_format_t fmt) {
  error_t ret = RESULT_OK;
  uint8_t adc_iface = 0;

  adc_iface = es7210_read_reg(ES7210_SDP_INTERFACE1_REG11);
  adc_iface &= 0xfc;

  switch (fmt) {
    case I2S_NORMAL:
      AD_LOGD("ES7210 in I2S Format");
      adc_iface |= 0x00;
      break;
    case I2S_LEFT:
    case I2S_RIGHT:
      AD_LOGD("ES7210 in LJ Format");
      adc_iface |= 0x01;
      break;
    case I2S_DSP:
      if (I2S_DSP_MODE) {
        AD_LOGD("ES7210 in DSP-A Format");
        adc_iface |= 0x13;
      } else {
        AD_LOGD("ES7210 in DSP-B Format");
        adc_iface |= 0x03;
      }
      break;
    default:
      adc_iface &= 0xfc;
      break;
  }
  ret |= es7210_write_reg(ES7210_SDP_INTERFACE1_REG11, adc_iface);
  return ret;
}

inline error_t es7210_set_bits(sample_bits_t bits) {
  error_t ret = RESULT_OK;
  uint8_t adc_iface = 0;
  adc_iface = es7210_read_reg(ES7210_SDP_INTERFACE1_REG11);
  adc_iface &= 0x1f;
  switch (bits) {
    case BIT_LENGTH_16BITS:
      adc_iface |= 0x60;
      break;
    case BIT_LENGTH_24BITS:
      adc_iface |= 0x00;
      break;
    case BIT_LENGTH_32BITS:
      adc_iface |= 0x80;
      break;
    default:
      adc_iface |= 0x60;
      break;
  }
  ret |= es7210_write_reg(ES7210_SDP_INTERFACE1_REG11, adc_iface);
  return ret;
}

/**
 * @brief Configure ES7210 ADC mode and I2S interface
 *
 * @param[in] mode:  codec mode
 * @param[in] iface:  I2S config
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es7210_adc_config_i2s(codec_mode_t mode,
                                     I2SDefinition* iface) {
  error_t ret = RESULT_OK;
  ret |= es7210_set_bits(iface->bits);
  ret |= es7210_config_fmt(iface->fmt);
  ret |= es7210_config_sample(iface->rate);
  return ret;
}

inline error_t es7210_start(uint8_t clock_reg_value) {
  error_t ret = RESULT_OK;
  ret |= es7210_write_reg(ES7210_CLOCK_OFF_REG01, clock_reg_value);
  ret |= es7210_write_reg(ES7210_POWER_DOWN_REG06, 0x00);
  ret |= es7210_write_reg(ES7210_ANALOG_REG40, 0x43);
  ret |= es7210_write_reg(ES7210_MIC1_POWER_REG47, 0x00);
  ret |= es7210_write_reg(ES7210_MIC2_POWER_REG48, 0x00);
  ret |= es7210_write_reg(ES7210_MIC3_POWER_REG49, 0x00);
  ret |= es7210_write_reg(ES7210_MIC4_POWER_REG4A, 0x00);
  ret |= es7210_mic_select(mic_select);
  return ret;
}

inline error_t es7210_stop(void) {
  error_t ret = RESULT_OK;
  ret |= es7210_write_reg(ES7210_MIC1_POWER_REG47, 0xff);
  ret |= es7210_write_reg(ES7210_MIC2_POWER_REG48, 0xff);
  ret |= es7210_write_reg(ES7210_MIC3_POWER_REG49, 0xff);
  ret |= es7210_write_reg(ES7210_MIC4_POWER_REG4A, 0xff);
  ret |= es7210_write_reg(ES7210_MIC12_POWER_REG4B, 0xff);
  ret |= es7210_write_reg(ES7210_MIC34_POWER_REG4C, 0xff);
  ret |= es7210_write_reg(ES7210_ANALOG_REG40, 0xc0);
  ret |= es7210_write_reg(ES7210_CLOCK_OFF_REG01, 0x7f);
  ret |= es7210_write_reg(ES7210_POWER_DOWN_REG06, 0x07);
  return ret;
}

/**
 * @brief Control ES7210 ADC chip
 *
 * @param[in] mode:  codec mode
 * @param[in] ctrl_state_active:  start or stop progress
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
inline error_t es7210_adc_ctrl_state_active(codec_mode_t mode,
                                            bool ctrl_state_active) {
  static uint8_t regv;
  error_t ret = RESULT_OK;
  AD_LOGW("ES7210 only supports ADC mode");
  regv = es7210_read_reg(ES7210_CLOCK_OFF_REG01);
  if ((regv != 0x7f) && (regv != 0xff)) {
    regv = es7210_read_reg(ES7210_CLOCK_OFF_REG01);
  }
  if (ctrl_state_active) {
    AD_LOGI("The ES7210_CLOCK_OFF_REG01 value before stop is %x", regv);
    ret |= es7210_start(regv);
  } else {
    AD_LOGW("The codec is about to stop");
    regv = es7210_read_reg(ES7210_CLOCK_OFF_REG01);
    ret |= es7210_stop();
  }
  return ret;
}

/**
 * @brief Get gain
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
inline error_t es7210_adc_get_gain(void) {
  int regv = 0;
  uint8_t gain_value;
  if (mic_select & ES7210_INPUT_MIC1) {
    regv = es7210_read_reg(ES7210_MIC1_ES7210_GAIN_REG43);
  } else if (mic_select & ES7210_INPUT_MIC2) {
    regv = es7210_read_reg(ES7210_MIC2_ES7210_GAIN_REG44);
  } else if (mic_select & ES7210_INPUT_MIC3) {
    regv = es7210_read_reg(ES7210_MIC3_ES7210_GAIN_REG45);
  } else if (mic_select & ES7210_INPUT_MIC4) {
    regv = es7210_read_reg(ES7210_MIC4_ES7210_GAIN_REG46);
  } else {
    AD_LOGE("No MIC selected");
    return RESULT_FAIL;
  }
  if (regv == RESULT_FAIL) {
    return regv;
  }
  gain_value = (regv & 0x0f); /* Retain the last four bits for gain */
  AD_LOGI("GET: gain_value:%d", gain_value);
  return gain_value;
}

/**
 * @brief Set volume
 *
 * @param[in] volume:  volume
 *
 * @return
 *     - RESULT_OK
 */
inline error_t es7210_adc_set_volume(int volume) {
  error_t ret = RESULT_OK;
  AD_LOGD("ADC can adjust gain");
  return ret;
}

/**
 * @brief Set ES7210 ADC mute status
 *
 * @return
 *     - RESULT_FAIL
 *     - RESULT_OK
 */
inline error_t es7210_set_mute(bool enable) {
  AD_LOGD("ES7210 SetMute :%d", enable);
  return RESULT_OK;
}

/**
 * @brief Read all regs of ES7210
 */
inline void es7210_read_all(void) {
  for (int i = 0; i <= 0x4E; i++) {
    uint8_t reg = es7210_read_reg(i);
    AD_LOGI("REG:%02x, %02x", reg, i);
  }
}

}  // namespace audio_driver
