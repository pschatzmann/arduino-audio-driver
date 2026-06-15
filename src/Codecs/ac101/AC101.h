// code from
// https://github.com/donny681/esp-adf/blob/master/components/audio_Codecs/AC101/AC101.h

#pragma once

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"
#include <string.h>

#define AC_ASSERT(a, format, b, ...)         \
  if ((a) != 0) {                            \
    AD_LOGE("AC101", format, ##__VA_ARGS__); \
    return b;                                \
  }

#define I2C_MASTER_WRITE 0
#define I2C_MASTER_READ 1

#define AC101_ADDR 0x1A /*!< Device address 0x1a/0x34 */

#undef READ_BIT
#undef WRITE_BIT

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0 /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0       /*!< I2C ack value */
#define NACK_VAL 0x1      /*!< I2C nack value */

#define CHIP_AUDIO_RS 0x00
#define PLL_CTRL1 0x01
#define PLL_CTRL2 0x02
#define SYSCLK_CTRL 0x03
#define MOD_CLK_ENA 0x04
#define MOD_RST_CTRL 0x05
#define I2S_SR_CTRL 0x06
#define I2S1LCK_CTRL 0x10
#define I2S1_SDOUT_CTRL 0x11
#define I2S1_SDIN_CTRL 0x12
#define I2S1_MXR_SRC 0x13
#define I2S1_VOL_CTRL1 0x14
#define I2S1_VOL_CTRL2 0x15
#define I2S1_VOL_CTRL3 0x16
#define I2S1_VOL_CTRL4 0x17
#define I2S1_MXR_GAIN 0x18
#define ADC_DIG_CTRL 0x40
#define ADC_VOL_CTRL 0x41
#define HMIC_CTRL1 0x44
#define HMIC_CTRL2 0x45
#define HMIC_STATUS 0x46
#define DAC_DIG_CTRL 0x48
#define DAC_VOL_CTRL 0x49
#define DAC_MXR_SRC 0x4c
#define DAC_MXR_GAIN 0x4d
#define ADC_APC_CTRL 0x50
#define ADC_SRC 0x51
#define ADC_SRCBST_CTRL 0x52
#define OMIXER_DACA_CTRL 0x53
#define OMIXER_SR 0x54
#define OMIXER_BST1_CTRL 0x55
#define HPOUT_CTRL 0x56
#define SPKOUT_CTRL 0x58
#define AC_DAC_DAPCTRL 0xa0
#define AC_DAC_DAPHHPFC 0xa1
#define AC_DAC_DAPLHPFC 0xa2
#define AC_DAC_DAPLHAVC 0xa3
#define AC_DAC_DAPLLAVC 0xa4
#define AC_DAC_DAPRHAVC 0xa5
#define AC_DAC_DAPRLAVC 0xa6
#define AC_DAC_DAPHGDEC 0xa7
#define AC_DAC_DAPLGDEC 0xa8
#define AC_DAC_DAPHGATC 0xa9
#define AC_DAC_DAPLGATC 0xaa
#define AC_DAC_DAPHETHD 0xab
#define AC_DAC_DAPLETHD 0xac
#define AC_DAC_DAPHGKPA 0xad
#define AC_DAC_DAPLGKPA 0xae
#define AC_DAC_DAPHGOPA 0xaf
#define AC_DAC_DAPLGOPA 0xb0
#define AC_DAC_DAPOPT 0xb1
#define DAC_DAP_ENA 0xb5

namespace audio_driver {

enum ac_adda_fs_i2s1_t {
  SAMPLE_RATE_8000 = 0x0000,
  SAMPLE_RATE_11052 = 0x1000,
  SAMPLE_RATE_12000 = 0x2000,
  SAMPLE_RATE_16000 = 0x3000,
  SAMPLE_RATE_22050 = 0x4000,
  SAMPLE_RATE_24000 = 0x5000,
  SAMPLE_RATE_32000 = 0x6000,
  SAMPLE_RATE_44100 = 0x7000,
  SAMPLE_RATE_48000 = 0x8000,
  SAMPLE_RATE_96000 = 0x9000,
  SAMPLE_RATE_192000 = 0xa000,
};

enum ac_i2s1_bclk_div_t {
  BCLK_DIV_1 = 0x0,
  BCLK_DIV_2 = 0x1,
  BCLK_DIV_4 = 0x2,
  BCLK_DIV_6 = 0x3,
  BCLK_DIV_8 = 0x4,
  BCLK_DIV_12 = 0x5,
  BCLK_DIV_16 = 0x6,
  BCLK_DIV_24 = 0x7,
  BCLK_DIV_32 = 0x8,
  BCLK_DIV_48 = 0x9,
  BCLK_DIV_64 = 0xa,
  BCLK_DIV_96 = 0xb,
  BCLK_DIV_128 = 0xc,
  BCLK_DIV_192 = 0xd,

};

enum ac_i2s1_lrck_div_t {
  LRCK_DIV_16 = 0x0,
  LRCK_DIV_32 = 0x1,
  LRCK_DIV_64 = 0x2,
  LRCK_DIV_128 = 0x3,
  LRCK_DIV_256 = 0x4,
};

enum ac_bits_length_t {
  BIT_LENGTH_8_BITS = 0x00,
  BIT_LENGTH_16_BITS = 0x01,
  BIT_LENGTH_20_BITS = 0x02,
  BIT_LENGTH_24_BITS = 0x03,
};

enum ac_mode_sm_t {
  AC_MODE_MIN = -1,
  AC_MODE_SLAVE = 0x00,
  AC_MODE_MASTER = 0x01,
  AC_MODE_MAX,
};

enum ac_module_t {
  AC_MODULE_MIN = -1,
  AC_MODULE_ADC = 0x01,
  AC_MODULE_DAC = 0x02,
  AC_MODULE_ADC_DAC = 0x03,
  AC_MODULE_LINE = 0x04,
  AC_MODULE_MAX
};

enum ac_output_mixer_source_t {
  SRC_MIC1 = 1,
  SRC_MIC2 = 2,
  SRC_LINEIN = 3,
};

enum ac_output_mixer_gain_t {
  GAIN_N45DB = 0,
  GAIN_N30DB = 1,
  GAIN_N15DB = 2,
  GAIN_0DB = 3,
  GAIN_15DB = 4,
  GAIN_30DB = 5,
  GAIN_45DB = 6,
  GAIN_60DB = 7,
};

/**
 * @brief Configure AC101 clock
 */
struct ac_i2s_clock_t {
  ac_i2s1_bclk_div_t bclk_div; /*!< bits clock divide */
  ac_i2s1_lrck_div_t lclk_div; /*!< WS clock divide */
};

/**
 * @brief Header-only driver class for the AC101 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AC101 {
 public:
  AC101() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) {
    if (addr > 0) i2c_addr = addr;
  }

  error_t init(codec_config_t* codec_cfg) {
    error_t res = RESULT_OK;

    res = writeReg(CHIP_AUDIO_RS, 0x123);
    delayMs(1000);
    if (res != RESULT_OK) {
      AD_LOGE("reset failed!");
      return res;
    } else {
      AD_LOGI("reset");
    }
    res |= writeReg(SPKOUT_CTRL, 0xe880);

    // Enable the PLL from 256*44.1KHz MCLK source
    res |= writeReg(PLL_CTRL1, 0x014f);
    // res |= writeReg(PLL_CTRL2, 0x83c0);
    res |= writeReg(PLL_CTRL2, 0x8600);

    // Clocking system
    res |= writeReg(SYSCLK_CTRL, 0x8b08);
    res |= writeReg(MOD_CLK_ENA, 0x800c);
    res |= writeReg(MOD_RST_CTRL, 0x800c);
    res |= writeReg(I2S_SR_CTRL, 0x7000);  // sample rate
    // AIF config
    res |= writeReg(I2S1LCK_CTRL, 0x8850);     // BCLK/LRCK
    res |= writeReg(I2S1_SDOUT_CTRL, 0xc000);  //
    res |= writeReg(I2S1_SDIN_CTRL, 0xc000);
    res |= writeReg(I2S1_MXR_SRC, 0x2200);  //

    res |= writeReg(ADC_SRCBST_CTRL, 0xccc4);

    res |= writeReg(ADC_SRC, getSrcValue(codec_cfg->input_device));
    res |= writeReg(ADC_DIG_CTRL, 0x8000);
    res |= writeReg(ADC_APC_CTRL, 0xbbc3);

    // Path Configuration
    res |= writeReg(DAC_MXR_SRC, 0xcc00);
    res |= writeReg(DAC_DIG_CTRL, 0x8000);
    res |= writeReg(OMIXER_SR, 0x0081);
    res |= writeReg(OMIXER_DACA_CTRL, 0xf080);  //}

    //* Enable Speaker output
    res |= writeReg(0x58, 0xeabd);

    AD_LOGI("init done");
    return res;
  }

  error_t deinit() {
    return writeReg(CHIP_AUDIO_RS, 0x123);  // soft reset
  }

  error_t ctrlStateActive(codec_mode_t mode, bool ctrlStateActive) {
    int res = 0;
    int es_mode_t = 0;

    switch (mode) {
      case CODEC_MODE_ENCODE:
        es_mode_t = AC_MODULE_ADC;
        break;
      case CODEC_MODE_LINE_IN:
        es_mode_t = AC_MODULE_LINE;
        break;
      case CODEC_MODE_DECODE:
        es_mode_t = AC_MODULE_DAC;
        break;
      case CODEC_MODE_BOTH:
        es_mode_t = AC_MODULE_ADC_DAC;
        break;
      default:
        es_mode_t = AC_MODULE_DAC;
        AD_LOGW("Codec mode not support, default is decode mode");
        break;
    }
    if (!ctrlStateActive) {
      res = stop((ac_module_t)es_mode_t);
    } else {
      res = start((ac_module_t)es_mode_t);
    }
    return res;
  }

  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    error_t res = 0;
    int bits = 0;
    int fmat = 0;
    int sample_fre = 0;
    uint16_t regval;
    switch (iface->bits)  // 0x10
    {
      // case BIT_LENGTH_8BITS:
      // 	bits = BIT_LENGTH_8_BITS;
      // 	break;
      case BIT_LENGTH_16BITS:
        bits = BIT_LENGTH_16_BITS;
        break;
      case BIT_LENGTH_24BITS:
        bits = BIT_LENGTH_24_BITS;
        break;
      default:
        bits = BIT_LENGTH_16_BITS;
    }

    switch (iface->fmt)  // 0x10
    {
      case I2S_NORMAL:
        fmat = 0x0;
        break;
      case I2S_LEFT:
        fmat = 0x01;
        break;
      case I2S_RIGHT:
        fmat = 0x02;
        break;
      case I2S_DSP:
        fmat = 0x03;
        break;
      default:
        fmat = 0x00;
        break;
    }

    switch (iface->rate) {
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
        sample_fre = 44100;
    }
    regval = readReg(I2S1LCK_CTRL);
    regval &= 0xffc3;
    regval |= (iface->mode << 15);
    regval |= (bits << 4);
    regval |= (fmat << 2);
    res |= writeReg(I2S1LCK_CTRL, regval);
    res |= writeReg(I2S_SR_CTRL, sample_fre);
    return res;
  }

  error_t i2sConfigClock(ac_i2s_clock_t* cfg) {
    error_t res = 0;
    uint16_t regval = 0;
    regval = readReg(I2S1LCK_CTRL);
    regval &= 0xe03f;
    regval |= (cfg->bclk_div << 9);
    regval |= (cfg->lclk_div << 6);
    res = writeReg(I2S1LCK_CTRL, regval);
    return res;
  }

  int getSpkVolume() {
    int res;
    res = readReg(SPKOUT_CTRL);
    res &= 0x1f;
    return res * 2;
  }

  error_t setSpkVolume(uint8_t volume) {
    if (volume > 0x3f) volume = 0x3f;
    volume = volume / 2;

    uint16_t res;
    error_t ret;

    res = readReg(SPKOUT_CTRL);
    res &= (~0x1f);
    volume &= 0x1f;
    res |= volume;
    ret = writeReg(SPKOUT_CTRL, res);
    return ret;
  }

  int getEarphVolume() {
    int res;
    res = readReg(HPOUT_CTRL);
    return (res >> 4) & 0x3f;
  }

  error_t setEarphVolume(uint8_t volume) {
    if (volume > 0x3f) volume = 0x3f;

    uint16_t res, tmp;
    error_t ret;
    res = readReg(HPOUT_CTRL);
    tmp = ~(0x3f << 4);
    res &= tmp;
    volume &= 0x3f;
    res |= (volume << 4);
    ret = writeReg(HPOUT_CTRL, res);
    return ret;
  }

  error_t setOutputMixerGain(ac_output_mixer_gain_t gain,
                              ac_output_mixer_source_t source) {
    uint16_t regval, temp, clrbit;
    error_t ret;
    regval = readReg(OMIXER_BST1_CTRL);
    switch (source) {
      case SRC_MIC1:
        temp = (gain & 0x7) << 6;
        clrbit = ~(0x7 << 6);
        break;
      case SRC_MIC2:
        temp = (gain & 0x7) << 3;
        clrbit = ~(0x7 << 3);
        break;
      case SRC_LINEIN:
        temp = (gain & 0x7);
        clrbit = ~0x7;
        break;
      default:
        return -1;
    }
    regval &= clrbit;
    regval |= temp;
    ret = writeReg(OMIXER_BST1_CTRL, regval);
    return ret;
  }

  error_t setVoiceMute(bool enable) {
    error_t res = 0;

    if (enable) {
      res = setEarphVolume(0);
      res |= setSpkVolume(0);
    }
    return res;
  }

  error_t setVoiceVolume(int volume) {
    error_t res;
    res = setEarphVolume(volume);
    res |= setSpkVolume(volume);
    return res;
  }

  error_t getVoiceVolume(int* volume) {
    *volume = getEarphVolume();
    return 0;
  }
  error_t writeReg(uint8_t reg_add, uint16_t data) {
    uint8_t send_buff[2];
    send_buff[0] = (data >> 8) & 0xff;
    send_buff[1] = data & 0xff;
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, &reg_add,
                                sizeof(reg_add), (uint8_t*)send_buff,
                                sizeof(send_buff));
  }

  error_t readI2C(uint8_t devAddr, uint8_t reg_add, uint8_t* p_data,
                  size_t size) {
    return i2c_bus_read_bytes(i2c_handle, devAddr, &reg_add, sizeof(reg_add),
                               p_data, size);
  }

  uint16_t readReg(uint8_t reg_addr) {
    uint16_t val = 0;
    uint8_t data_rd[2];
    readI2C(i2c_addr, reg_addr, data_rd, 2);
    val = (data_rd[0] << 8) + data_rd[1];
    return val;
  }

  void setCodecClk(samplerate_t sampledata) {
    uint16_t sample_fre;
    switch (sampledata) {
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
        sample_fre = 44100;
    }
    writeReg(I2S_SR_CTRL, sample_fre);
  }

  /// Determines the value for the SRC register based on the selected input_device
  uint16_t getSrcValue(input_device_t input_device) {
    uint16_t src_value = 0;
    switch (input_device) {
      // microphone
      case ADC_INPUT_LINE1:
        src_value = 0x2020;
        break;
      // linein
      case ADC_INPUT_LINE2:
        src_value = 0x0408;
        break;
      // right mic in & left line in
      case ADC_INPUT_LINE3:
        src_value = 0x0420;
        break;
      // both
      case ADC_INPUT_ALL:
      default:
        src_value = 0x0408 | 0x2020;
        break;
    }
    return src_value;
  }

  error_t start(ac_module_t mode) {
    error_t res = 0;
    if (mode == AC_MODULE_LINE) {
      res |= writeReg(0x51, 0x0408);
      res |= writeReg(0x40, 0x8000);
      res |= writeReg(0x50, 0x3bc0);
    }
    if (mode == AC_MODULE_ADC || mode == AC_MODULE_ADC_DAC ||
        mode == AC_MODULE_LINE) {
      // I2S1_SDOUT_CTRL
      // res |= writeReg(PLL_CTRL2, 0x8120);
      res |= writeReg(0x04, 0x800c);
      res |= writeReg(0x05, 0x800c);
      // res |= writeReg(0x06, 0x3000);
    }
    if (mode == AC_MODULE_DAC || mode == AC_MODULE_ADC_DAC ||
        mode == AC_MODULE_LINE) {
      //* Enable Headphoe output
      res |= writeReg(OMIXER_DACA_CTRL, 0xff80);
      res |= writeReg(HPOUT_CTRL, 0xc3c1);
      res |= writeReg(HPOUT_CTRL, 0xcb00);
      delayMs(100);
      res |= writeReg(HPOUT_CTRL, 0xfbc0);

      //* Enable Speaker output
      res |= writeReg(SPKOUT_CTRL, 0xeabd);
      delayMs(10);
      setVoiceVolume(30);
    }

    return res;
  }

  error_t stop(ac_module_t mode) {
    error_t res = 0;
    res |= writeReg(HPOUT_CTRL, 0x01);     // disable earphone
    res |= writeReg(SPKOUT_CTRL, 0xe880);  // disable speaker
    return res;
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = AC101_ADDR;
};

}  // namespace audio_driver
