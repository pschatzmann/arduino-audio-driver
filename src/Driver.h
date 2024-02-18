
#pragma once
#include "Common.h"
#include "Driver/ac101/ac101.h"
#include "Driver/cs43l22/cs43l22.h"
#include "Driver/es7210/es7210.h"
#include "Driver/es7243/es7243.h"
#include "Driver/es7243e/es7243e.h"
#include "Driver/es8156/es8156.h"
#include "Driver/es8311/es8311.h"
#include "Driver/es8374/es8374.h"
#include "Driver/es8388/es8388.h"
#include "Driver/tas5805m/tas5805m.h"
#include "Driver/wm8960/mtb_wm8960.h"
#include "Driver/wm8994/wm8994.h"

#include "DriverPins.h"

namespace audio_driver {

const int rate_num[8] = {8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000};
const samplerate_t rate_code[8] = {RATE_8K, RATE_11K, RATE_16K, RATE_22K,
                                   RATE_24K, RATE_32K, RATE_44K, RATE_48K};

/**
 * @brief I2S configuration and defition of input and output with default values
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class CodecConfig : public codec_config_t {
public:

  /// @brief setup default values
  CodecConfig() {
    input_device = ADC_INPUT_LINE1;
    output_device = DAC_OUTPUT_ALL;
    i2s.bits = BIT_LENGTH_16BITS;
    i2s.rate = RATE_44K;
    i2s.fmt = I2S_NORMAL;
    // codec is slave - microcontroller is master
    i2s.mode = MODE_SLAVE;
  }

  /// Returns bits per sample as number
  int getBitsNumeric() {
    switch (i2s.bits) {
    case BIT_LENGTH_16BITS:
      return 16;
    case BIT_LENGTH_24BITS:
      return 24;
    case BIT_LENGTH_32BITS:
      return 32;
    default:
      return 0;
    }
    return 0;
  }

  // sets the bits per sample with a numeric value
  int setBitsNumeric(int bits) {
    switch (bits) {
    case 16:
      i2s.bits = BIT_LENGTH_16BITS;
      return bits;
    case 18:
      i2s.bits = BIT_LENGTH_18BITS;
      return bits;
    case 20:
      i2s.bits = BIT_LENGTH_20BITS;
      return bits;
    case 24:
      i2s.bits = BIT_LENGTH_24BITS;
      return bits;
    case 32:
      i2s.bits = BIT_LENGTH_32BITS;
      return bits;
    }
    return 0;
  }

  /// get the sample rate as number
  int getRateNumeric() {
    for (int j = 0; j < 8; j++) {
      if (rate_code[j] == i2s.rate) {
        AD_LOGD("-> %d", rate_num[j]);
        return rate_num[j];
      }
    }
    return 0;
  }

  /// sets the sample rate as number
  int setRateNumeric(int rateNum) {
    int diff = 99999;
    int result = 0;
    for (int j = 0; j < 8; j++) {
      if (rate_num[j] == rateNum) {
        AD_LOGD("-> %d", rate_num[j]);
        i2s.rate = rate_code[j];
        return rateNum;
      } else {
        int new_diff = abs(rate_code[j] - rateNum);
        if (new_diff < diff) {
          result = j;
          diff = new_diff;
        }
      }
    }
    AD_LOGE("Sample Rate not supported: %d - using %d", rateNum,
            rate_num[result]);
    i2s.rate = rate_code[result];
    return rate_num[result];
  }

  /// determines the codec_mode_t dynamically based on the input and output
  codec_mode_t get_mode() {
    // codec_mode_t mode;
    bool is_input = false;
    bool is_output = false;

    if (input_device != ADC_INPUT_NONE)
      is_input = true;

    if (output_device != DAC_OUTPUT_NONE)
      is_output = true;

    if (is_input && is_output) {
      AD_LOGD("mode->CODEC_MODE_BOTH");
      return CODEC_MODE_BOTH;
    }

    if (is_output) {
      AD_LOGD("mode->CODEC_MODE_DECODE");
      return CODEC_MODE_DECODE;
    }

    if (is_input) {
      AD_LOGD("mode->CODEC_MODE_ENCODE");
      return CODEC_MODE_ENCODE;
    }

    AD_LOGD("mode->CODEC_MODE_NONE");
    return CODEC_MODE_NONE;
  }
  // if sd active we setup SPI for the SD
  bool sd_active = true;

};

/**
 * @brief Abstract Driver API for codec chips
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriver {
public:
  virtual bool begin(CodecConfig codecCfg, DriverPins &pins) {
    AD_LOGD("AudioDriver::begin");
    codec_cfg = codecCfg;
    p_pins = &pins;
    pins.setSPIActiveForSD(codecCfg.sd_active);
    if (!init(codec_cfg)) {
      AD_LOGE("init failed");
      return false;
    }
    codec_mode_t codec_mode = codec_cfg.get_mode();
    if (!controlState(codec_mode)) {
      AD_LOGE("controlState failed");
      return false;
    }
    bool result = configInterface(codec_mode, codec_cfg.i2s);
    if (!result) {
      AD_LOGE("configInterface failed");
    }
    setPAPower(true);
    return result;
  }
  virtual bool end(void) { return deinit(); }
  virtual bool setMute(bool enable) = 0;
  /// Defines the Volume (in %) if volume is 0, mute is enabled,range is 0-100.
  virtual bool setVolume(int volume) = 0;
  virtual int getVolume() = 0;
  virtual bool setInputVolume(int volume) { return false; }
  virtual bool isVolumeSupported() { return true; }
  virtual bool isInputVolumeSupported() { return false; }

  DriverPins &pins() { return *p_pins; }

  /// Sets the PA Power pin to active or inactive
  bool setPAPower(bool enable) {
    GpioPin  pin = pins().getPinID(PinFunction::PA);
    if (pin == -1)
      return false;
    AD_LOGI("setPAPower pin %d -> %d", pin, enable);
    digitalWrite(pin, enable ? HIGH : LOW);
    return true;
  }

protected:
  CodecConfig codec_cfg;
  DriverPins *p_pins = nullptr;

  /// Detemine the TwoWire object from the I2C config or use Wire
  TwoWire* getI2C() {
    if (p_pins == nullptr) return &Wire;
    auto i2c = pins().getI2CPins(PinFunction::CODEC);
    if (!i2c) {
      return &Wire;
    }
    return i2c.value().p_wire;
  }

  virtual bool init(codec_config_t codec_cfg) { return false; }
  virtual bool deinit() { return false; }
  virtual bool controlState(codec_mode_t mode) { return false; };
  virtual bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return false;
  };

  /// make sure that value is in range
  /// @param volume
  /// @return
  int limitValue(int volume, int min = 0, int max = 100) {
    if (volume > max)
      volume = max;
    if (volume < min)
      volume = min;
    return volume;
  }
};

/**
 * @brief Dummy Driver which does nothing.
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class NoDriverClass : public AudioDriver {
public:
  virtual bool begin(CodecConfig codecCfg, DriverPins &pins) {
    codec_cfg = codecCfg;
    p_pins = &pins;
    return true;
  }
  virtual bool end(void) { return true; }
  virtual bool setMute(bool enable) { return false;}
  virtual bool setVolume(int volume) { return false;}
  virtual int getVolume() { return 100;}
  virtual bool setInputVolume(int volume) { return false; }
  virtual bool isVolumeSupported() { { return false;} }
  virtual bool isInputVolumeSupported() { return false; }
};


/**
 * @brief Driver API for AC101 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverAC101Class : public AudioDriver {
public:
  bool setMute(bool mute) { return ac101_set_voice_mute(mute); }
  bool setVolume(int volume) {
    return ac101_set_voice_volume(limitValue(volume));
  };
  int getVolume() {
    int vol;
    ac101_get_voice_volume(&vol);
    return vol;
  };

protected:
  bool init(codec_config_t codec_cfg) {
    return ac101_init(&codec_cfg, getI2C()) == RESULT_OK;
  };
  bool deinit() { return ac101_deinit() == RESULT_OK; }
  bool controlState(codec_mode_t mode) {
    return ac101_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return ac101_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for the CS43l22 codec chip on 0x94 (0x4A<<1)
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverCS43l22Class : public AudioDriver {
public:
  AudioDriverCS43l22Class(uint16_t deviceAddr = 0x94 ) {
    this->deviceAddr = deviceAddr;
  }

  void setI2CAddress(uint16_t adr) { deviceAddr = adr; }

  virtual bool begin(CodecConfig codecCfg, DriverPins &pins) {
    AD_LOGD("AudioDriverCS43l22Class::begin");
    p_pins = &pins;
    codec_cfg = codecCfg;
    // manage reset pin -> acive high
    setPAPower(true);
  // Setup enable pin for codec
    delay(100);
    uint32_t freq = getFrequency(codec_cfg.i2s.rate);
    uint16_t outputDevice = getOutput(codec_cfg.output_device);
    AD_LOGD("cs43l22_Init");
    bool result = cs43l22_Init(deviceAddr, outputDevice, volume, freq, getI2C()) == 0;
    if (!result){
      AD_LOGE("error: cs43l22_Init");
    }
    cs43l22_Play(deviceAddr, nullptr, 0);
    return result;
  }

  bool setMute(bool mute) {
    uint32_t rc = mute ? cs43l22_Pause(deviceAddr) : cs43l22_Resume(deviceAddr);
    return rc == 0;
  }

  bool setVolume(int volume) {
    this->volume = volume;
    return cs43l22_SetVolume(deviceAddr, volume) == 0;
  }
  int getVolume() { return volume; }

protected:
  uint16_t deviceAddr;
  int volume = 100;

  bool deinit() {
    int cnt = cs43l22_Stop(deviceAddr, AUDIO_MUTE_ON);
    cnt += cs43l22_Reset(deviceAddr);
    setPAPower(false);
    return cnt == 0;
  }

  uint32_t getFrequency(samplerate_t rateNum) {
    switch (rateNum) {
    case RATE_8K:
      return 8000; /*!< set to  8k samples per second */
    case RATE_11K:
      return 11024; /*!< set to 11.025k samples per second */
    case RATE_16K:
      return 16000; /*!< set to 16k samples in per second */
    case RATE_22K:
      return 22050; /*!< set to 22.050k samples per second */
    case RATE_24K:
      return 24000; /*!< set to 24k samples in per second */
    case RATE_32K:
      return 32000; /*!< set to 32k samples in per second */
    case RATE_44K:
      return 44100; /*!< set to 44.1k samples per second */
    case RATE_48K:
      return 48000; /*!< set to 48k samples per second */
    }
    return 44100;
  }

  uint16_t getOutput(output_device_t output_device) {
    switch (output_device) {
    case DAC_OUTPUT_NONE:
      return 0;
    case DAC_OUTPUT_LINE1:
      return OUTPUT_DEVICE_SPEAKER;
    case DAC_OUTPUT_LINE2:
      return OUTPUT_DEVICE_HEADPHONE;
    case DAC_OUTPUT_ALL:
      return OUTPUT_DEVICE_BOTH;
    }
    return OUTPUT_DEVICE_BOTH;
  }
};

/**
 * @brief Driver API for ES7210 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES7210Class : public AudioDriver {
public:
  bool setMute(bool mute) { return es7210_set_mute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    this->volume = volume;
    return es7210_adc_set_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() { return volume; }

protected:
  int volume;

  bool init(codec_config_t codec_cfg) {
    return es7210_adc_init(&codec_cfg, getI2C()) == RESULT_OK;
  }
  bool deinit() { return es7210_adc_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es7210_adc_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es7210_adc_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for Lyrat ES7243 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES7243Class : public AudioDriver {
public:
  bool setMute(bool mute) {
    return es7243_adc_set_voice_mute(mute) == RESULT_OK;
  }
  bool setVolume(int volume) {
    return es7243_adc_set_voice_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es7243_adc_get_voice_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    return es7243_adc_init(&codec_cfg, getI2C()) == RESULT_OK;
  }
  bool deinit() { return es7243_adc_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es7243_adc_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es7243_adc_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES7243e codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class AudioDriverES7243eClass : public AudioDriver {
public:
  bool setMute(bool mute) {
    return mute ? setVolume(0) == RESULT_OK : setVolume(volume) == RESULT_OK;
  }
  bool setVolume(int volume) {
    this->volume = volume;
    return es7243e_adc_set_voice_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es7243e_adc_get_voice_volume(&vol);
    return vol;
  }

protected:
  int volume = 0;

  bool init(codec_config_t codec_cfg) {
    return es7243e_adc_init(&codec_cfg, getI2C()) == RESULT_OK;
  }
  bool deinit() { return es7243e_adc_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es7243e_adc_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es7243e_adc_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES8388 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8156Class : public AudioDriver {
public:
  bool setMute(bool mute) {
    return es8156_codec_set_voice_mute(mute) == RESULT_OK;
  }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return es8156_codec_set_voice_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8156_codec_get_voice_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    return es8156_codec_init(&codec_cfg, getI2C()) == RESULT_OK;
  }
  bool deinit() { return es8156_codec_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8156_codec_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8156_codec_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for Lyrat  ES8311 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8311Class : public AudioDriver {
public:
  bool setMute(bool mute) { return es8311_set_voice_mute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    return es8311_codec_set_voice_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8311_codec_get_voice_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    int mclk_src = pins().getPinID(PinFunction::MCLK_SOURCE);
    if (mclk_src == -1)
      return false;
    return es8311_codec_init(&codec_cfg, getI2C(), mclk_src) ==
           RESULT_OK;
  }
  bool deinit() { return es8311_codec_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8311_codec_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8311_codec_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES8374 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8374Class : public AudioDriver {
public:
  bool setMute(bool mute) { return es8374_set_voice_mute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return es8374_codec_set_voice_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8374_codec_get_voice_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    auto codec_mode = this->codec_cfg.get_mode();
    return es8374_codec_init(&codec_cfg, codec_mode, getI2C()) ==
           RESULT_OK;
  }
  bool deinit() { return es8374_codec_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8374_codec_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8374_codec_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES8388 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8388Class : public AudioDriver {
public:
  bool setMute(bool mute) { return es8388_set_voice_mute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return es8388_set_voice_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8388_get_voice_volume(&vol);
    return vol;
  }

  bool setInputVolume(int volume) {
    // map values from 0 - 100 to 0 to 10
    es_mic_gain_t gain = (es_mic_gain_t)(limitValue(volume) / 10);
    AD_LOGD("input volume: %d -> gain %d", volume, gain);
    return setMicrophoneGain(gain);
  }

  bool setMicrophoneGain(es_mic_gain_t gain) {
    return es8388_set_mic_gain(gain) == RESULT_OK;
  }

  bool isInputVolumeSupported() { return true; }

protected:
  bool init(codec_config_t codec_cfg) {
    return es8388_init(&codec_cfg, getI2C()) == RESULT_OK;
  }
  bool deinit() { return es8388_deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8388_ctrl_state_active(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8388_config_i2s(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for TAS5805M codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverTAS5805MClass : public AudioDriver {
public:
  bool setMute(bool mute) { return tas5805m_set_mute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return tas5805m_set_volume(limitValue(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    tas5805m_get_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    return tas5805m_init(&codec_cfg, getI2C()) == RESULT_OK;
  }
  bool deinit() { return tas5805m_deinit() == RESULT_OK; }
};

/**
 * @brief Driver API for WM8990 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverWM8960Class : public AudioDriver {
public:
  bool begin(CodecConfig codecCfg, DriverPins &pins) {
    codec_cfg = codecCfg;

    // define wire object
    mtb_wm8960_set_wire(getI2C());
    mtb_wm8960_set_write_retry_count(i2c_retry_count);

    // setup wm8960
    int features = getFeatures(codecCfg);
    if (!mtb_wm8960_init(features)) {
      AD_LOGE("mtb_wm8960_init");
      return false;
    }
    setVolume(DRIVER_DEFAULT_VOLUME);
    if (!mtb_wm8960_activate()) {
      AD_LOGE("mtb_wm8960_activate");
      return false;
    }
    if (!configure_clocking()) {
      AD_LOGE("configure_clocking");
      return false;
    }
    return true;
  }
  bool end(void) {
    mtb_wm8960_deactivate();
    mtb_wm8960_free();
    return true;
  }

  bool setMute(bool enable) { return setVolume(enable ? 0 : volume_out); }

  /// Defines the Volume (in %) if volume is 0, mute is enabled,range is 0-100.
  bool setVolume(int volume) {
    volume_out = limitValue(volume, 0, 100);
    int vol_int = volume_out == 0.0 ? 0 : map(volume_out, 0, 100, 30, 0x7F);
    return mtb_wm8960_set_output_volume(vol_int);
  }

  int getVolume() { return volume_out; }

  bool setInputVolume(int volume) {
    volume_in = limitValue(volume, 0, 100);
    int vol_int = map(volume_in, 0, 100, 0, 30);
    return mtb_wm8960_adjust_input_volume(vol_int);
  }
  bool isVolumeSupported() { return true; }

  bool isInputVolumeSupported() { return true; }

  /// Configuration: define retry count (default : 0)
  void setI2CRetryCount(int cnt) { i2c_retry_count = cnt; }

  /// Configuration: enable/diable PLL (active by default)
  void setEnablePLL(bool active) { vs1053_enable_pll = active; }

  /// Configuration: define master clock frequency (default: 0)
  void setMclkHz(uint32_t hz) { vs1053_mclk_hz = hz; }

  void dumpRegisters() { mtb_wm8960_dump(); }

protected:
  int volume_in = 100;
  int volume_out = 100;
  int i2c_retry_count = 0;
  uint32_t vs1053_mclk_hz = 0;
  bool vs1053_enable_pll = true;

  int getFeatures(CodecConfig cfg) {
    int features = 0;
    switch (cfg.output_device) {
    case DAC_OUTPUT_LINE1:
      features = features | WM8960_FEATURE_SPEAKER;
      break;
    case DAC_OUTPUT_LINE2:
      features = features | WM8960_FEATURE_HEADPHONE;
      break;
    case DAC_OUTPUT_ALL:
      features = features | WM8960_FEATURE_SPEAKER | WM8960_FEATURE_HEADPHONE;
      break;
    default:
      break;
    }
    switch (cfg.input_device) {
    case ADC_INPUT_LINE1:
      features = features | WM8960_FEATURE_MICROPHONE1;
      break;
    case ADC_INPUT_LINE2:
      features = features | WM8960_FEATURE_MICROPHONE2;
      break;
    case ADC_INPUT_ALL:
      features = features | WM8960_FEATURE_MICROPHONE1 |
                 WM8960_FEATURE_MICROPHONE2 | WM8960_FEATURE_MICROPHONE3;
      break;
    default:
      break;
    }
    AD_LOGI("features: %d", features);
    return features;
  }

  bool configure_clocking() {
    if (vs1053_mclk_hz == 0) {
      // just pick a multiple of the sample rate
      vs1053_mclk_hz = 512 * codec_cfg.getRateNumeric();
    }
    if (!mtb_wm8960_configure_clocking(
            vs1053_mclk_hz, vs1053_enable_pll,
            sampleRate(codec_cfg.getRateNumeric()),
            wordLength(codec_cfg.getBitsNumeric()),
            modeMasterSlave(codec_cfg.i2s.mode == MODE_MASTER))) {
      AD_LOGE("mtb_wm8960_configure_clocking");
      return false;
    }
    return true;
  }

  mtb_wm8960_adc_dac_sample_rate_t sampleRate(int rate) {
    switch (rate) {
    case 48000:
      return WM8960_ADC_DAC_SAMPLE_RATE_48_KHZ;
    case 44100:
      return WM8960_ADC_DAC_SAMPLE_RATE_44_1_KHZ;
    case 32000:
      return WM8960_ADC_DAC_SAMPLE_RATE_32_KHZ;
    case 24000:
      return WM8960_ADC_DAC_SAMPLE_RATE_24_KHZ;
    case 22050:
      return WM8960_ADC_DAC_SAMPLE_RATE_22_05_KHZ;
    case 16000:
      return WM8960_ADC_DAC_SAMPLE_RATE_16_KHZ;
    case 12000:
      return WM8960_ADC_DAC_SAMPLE_RATE_12_KHZ;
    case 11025:
      return WM8960_ADC_DAC_SAMPLE_RATE_11_025_KHZ;
    case 8018:
      return WM8960_ADC_DAC_SAMPLE_RATE_8_018_KHZ;
    case 8000:
      return WM8960_ADC_DAC_SAMPLE_RATE_8_KHZ;
    default:
      AD_LOGE("Unsupported rate: %d", rate);
      return WM8960_ADC_DAC_SAMPLE_RATE_44_1_KHZ;
    }
  }

  mtb_wm8960_word_length_t wordLength(int bits) {
    switch (bits) {
    case 16:
      return WM8960_WL_16BITS;
    case 20:
      return WM8960_WL_20BITS;
    case 24:
      return WM8960_WL_24BITS;
    case 32:
      return WM8960_WL_32BITS;
    default:
      AD_LOGE("Unsupported bits: %d", bits);
      return WM8960_WL_16BITS;
    }
  }

  /// if microcontroller is master then module is slave
  mtb_wm8960_mode_t modeMasterSlave(bool is_master) {
    return is_master ? WM8960_MODE_MASTER : WM8960_MODE_SLAVE;
  }
};

/**
 * @brief Driver API for the wm8994 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverWM8994Class : public AudioDriver {
public:
  AudioDriverWM8994Class(uint16_t deviceAddr = 0x34) {
    this->deviceAddr = deviceAddr;
  }

  void setI2CAddress(uint16_t adr) { deviceAddr = adr; }

  virtual bool begin(CodecConfig codecCfg, DriverPins &pins) {
    codec_cfg = codecCfg;
    // manage reset pin -> acive high
    setPAPower(true);
    delay(10);
    p_pins = &pins;
    int vol = map(volume, 0, 100, DEFAULT_VOLMIN, DEFAULT_VOLMAX);
    uint32_t freq = codecCfg.getRateNumeric();
    uint16_t outputDevice = getOutput(codec_cfg.output_device);

    return wm8994_Init(deviceAddr, outputDevice, vol, freq,
                       getI2C()) == 0;
  }

  bool setMute(bool mute) {
    uint32_t rc = mute ? wm8994_Pause(deviceAddr) : wm8994_Resume(deviceAddr);
    return rc == 0;
  }

  bool setVolume(int volume) {
    this->volume = volume;
    int vol = map(volume, 0, 100, DEFAULT_VOLMIN, DEFAULT_VOLMAX);
    return wm8994_SetVolume(deviceAddr, vol) == 0;
  }
  int getVolume() { return volume; }

protected:
  uint16_t deviceAddr;
  int volume = 100;

  bool deinit() {
    int cnt = wm8994_Stop(deviceAddr, AUDIO_MUTE_ON);
    cnt += wm8994_Reset(deviceAddr);
    setPAPower(false);
    return cnt == 0;
  }

  uint16_t getOutput(output_device_t output_device) {
    switch (output_device) {
    case DAC_OUTPUT_NONE:
      return 0;
    case DAC_OUTPUT_LINE1:
      return OUTPUT_DEVICE_SPEAKER;
    case DAC_OUTPUT_LINE2:
      return OUTPUT_DEVICE_HEADPHONE;
    case DAC_OUTPUT_ALL:
      return OUTPUT_DEVICE_BOTH;
    }
    return OUTPUT_DEVICE_BOTH;
  }
};

/**
 * @brief Driver API for Lyrat Mini with a ES8311 and a ES7243 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverLyratMiniClass : public AudioDriver {
public:
  bool begin(CodecConfig codecCfg, DriverPins &pins) {
    int rc = 0;
    if (codecCfg.output_device != DAC_OUTPUT_NONE)
      rc += !dac.begin(codecCfg, pins);
    if (codecCfg.input_device != ADC_INPUT_NONE)
      rc += !adc.begin(codecCfg, pins);
    return rc == 0;
  }
  bool end(void) {
    int rc = 0;
    rc += dac.end();
    rc += adc.end();
    return rc == 0;
  }
  bool setMute(bool enable) { return dac.setMute(enable); }
  bool setVolume(int volume) { return dac.setVolume(volume); }
  int getVolume() { return dac.getVolume(); }
  bool setInputVolume(int volume) { return adc.setVolume(volume); }
  int getInputVolume() { return adc.getVolume(); }
  bool isInputVolumeSupported() { return true; }

protected:
  AudioDriverES8311Class dac;
  AudioDriverES7243Class adc;
};

// -- Drivers
/// @ingroup audio_driver
static AudioDriverAC101Class AudioDriverAC101;
/// @ingroup audio_driver
static AudioDriverCS43l22Class AudioDriverCS43l22;
/// @ingroup audio_driver
static AudioDriverES7210Class AudioDriverES7210;
/// @ingroup audio_driver
static AudioDriverES7243Class AudioDriverES7243;
/// @ingroup audio_driver
static AudioDriverES7243eClass AudioDriverES7243e;
/// @ingroup audio_driver
static AudioDriverES8156Class AudioDriverES8156;
/// @ingroup audio_driver
static AudioDriverES8311Class AudioDriverES8311;
/// @ingroup audio_driver
static AudioDriverES8374Class AudioDriverES8374;
/// @ingroup audio_driver
static AudioDriverES8388Class AudioDriverES8388;
/// @ingroup audio_driver
static AudioDriverWM8960Class AudioDriverWM8960;
/// @ingroup audio_driver
static AudioDriverWM8994Class AudioDriverWM8994;
/// @ingroup audio_driver
static AudioDriverLyratMiniClass AudioDriverLyratMini;
/// @ingroup audio_driver
static NoDriverClass NoDriver;

} // namespace audio_driver