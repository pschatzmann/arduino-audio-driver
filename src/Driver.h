
#pragma once
#include "Common.h"
#include "Driver/ac101/ac101.h"
#include "Driver/cs43l22/cs43l22.h"
#include "Driver/es7243/es7243.h"
#include "Driver/es8311/es8311.h"
#include "Driver/es8388/es8388.h"
#include "DriverPins.h"

namespace audio_driver {

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
    adc_input = ADC_INPUT_LINE1;
    dac_output = DAC_OUTPUT_ALL;
    i2s.bits = BIT_LENGTH_16BITS;
    i2s.rate = RATE_44K;
    i2s.fmt = I2S_NORMAL;
    // codec is slave - microcontroller is master
    i2s.mode = MODE_SLAVE;
  }
  codec_mode_t get_mode() {
    codec_mode_t mode;
    bool is_input = false;
    bool is_output = false;

    if (adc_input != ADC_INPUT_NONE)
      is_input = true;

    if (dac_output != DAC_OUTPUT_NONE)
      is_output = true;

    if (is_input && is_output) {
      AUDIODRIVER_LOGD("CODEC_MODE_BOTH");
      return CODEC_MODE_BOTH;
    }

    if (is_output)
      AUDIODRIVER_LOGD("CODEC_MODE_DECODE");
      return CODEC_MODE_DECODE;

    if (is_input)
      AUDIODRIVER_LOGD("CODEC_MODE_ENCODE");
      return CODEC_MODE_ENCODE;

    AUDIODRIVER_LOGD("CODEC_MODE_NONE");
    return CODEC_MODE_NONE;
  }
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
    codec_cfg = codecCfg;
    p_pins = &pins;
    if (!init(codec_cfg)){
      AUDIODRIVER_LOGE("init failed");
      return false;
    }
    codec_mode_t codec_mode = codec_cfg.get_mode();
    if(!controlState(codec_mode)){
      AUDIODRIVER_LOGE("controlState failed");
      return false;
    }
    bool result = configInterface(codec_mode, codec_cfg.i2s);
    if(!result){
      AUDIODRIVER_LOGE("configInterface failed");
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
    Pin pin = pins().getPin(PA);
    if (pin == -1)
      return false;
    AUDIODRIVER_LOGI("setPAPower pin %d -> %d", pin, enable);
    digitalWrite(pin, enable ? HIGH : LOW);
    return true;
  }

protected:
  CodecConfig codec_cfg;
  DriverPins *p_pins = nullptr;

  virtual bool init(codec_config_t codec_cfg) { return false; }
  virtual bool deinit() { return false; }
  virtual bool controlState(codec_mode_t mode) { return false; };
  virtual bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return false;
  };

  /// make sure that value is in range
  /// @param volume 
  /// @return  
  int limitVolume(int volume, int min=0, int max=100) {
    if (volume > max)
      volume = max;
    if (volume < min)
      volume = min;
    return volume;
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
    AUDIODRIVER_LOGD("volume %d", volume);
    return es8388_set_voice_volume(limitVolume(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8388_get_voice_volume(&vol);
    return vol;
  }

  bool setInputVolume(int volume) {
    // map values from 0 - 100 to 0 to 10
    es_mic_gain_t gain = (es_mic_gain_t)(limitVolume(volume) / 10);
    AUDIODRIVER_LOGD("input volume: %d -> gain %d", volume, gain);
    return setMicrophoneGain(gain);
  }

  bool setMicrophoneGain(es_mic_gain_t gain) {
    return es8388_set_mic_gain(gain) == RESULT_OK;
  }

  bool isInputVolumeSupported() { return true; }

protected:
  bool init(codec_config_t codec_cfg) {
    auto i2c = pins().getI2CPins(CODEC);
    if (!i2c) {
      AUDIODRIVER_LOGE("i2c pins not defined");
      return false;
    }

    return es8388_init(&codec_cfg, i2c.value().p_wire) == RESULT_OK;
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
 * @brief Driver API for Lyrat  ES8311 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8311Class : public AudioDriver {
public:
  bool setMute(bool mute) { return es8311_set_voice_mute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    return es8311_codec_set_voice_volume(limitVolume(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8311_codec_get_voice_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    auto i2c = pins().getI2CPins(CODEC);
    if (!i2c) {
      AUDIODRIVER_LOGE("i2c pins not defined");
      return false;
    }
    int mclk_src = pins().getPin(MCLK_SOURCE);
    if (mclk_src == -1)
      return false;
    return es8311_codec_init(&codec_cfg, i2c.value().p_wire, mclk_src) ==
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
 * @brief Driver API for Lyrat  ES7243 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES7243Class : public AudioDriver {
public:
  bool setMute(bool mute) {
    return es7243_adc_set_voice_mute(mute) == RESULT_OK;
  }
  bool setVolume(int volume) {
    return es7243_adc_set_voice_volume(limitVolume(volume)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es7243_adc_get_voice_volume(&vol);
    return vol;
  }

protected:
  bool init(codec_config_t codec_cfg) {
    auto i2c = pins().getI2CPins(CODEC);
    if (!i2c) {
      AUDIODRIVER_LOGE("i2c pins not defined");
      return false;
    }
    return es7243_adc_init(&codec_cfg, i2c.value().p_wire) == RESULT_OK;
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
 * @brief Driver API for Lyrat Mini with a ES8311 and a ES7243 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverLyratMiniClass : public AudioDriver {
public:
  bool begin(CodecConfig codecCfg, DriverPins &pins) {
    int rc = 0;
    if (codecCfg.dac_output != DAC_OUTPUT_NONE)
      rc += !dac.begin(codecCfg, pins);
    if (codecCfg.adc_input != ADC_INPUT_NONE)
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

/**
 * @brief Driver API for AC101 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverAC101Class : public AudioDriver {
public:
  bool setMute(bool mute) { return ac101_set_voice_mute(mute); }
  bool setVolume(int volume) { return ac101_set_voice_volume(limitVolume(volume)); };
  int getVolume() {
    int vol;
    ac101_get_voice_volume(&vol);
    return vol;
  };

protected:
  bool init(codec_config_t codec_cfg) {
    auto i2c = pins().getI2CPins(CODEC);
    if (!i2c) {
      AUDIODRIVER_LOGE("i2c pins not defined");
      return false;
    }
    ac101_set_i2c_handle(i2c.value().p_wire);
    return ac101_init(&codec_cfg) == RESULT_OK;
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
 * @brief Driver API for the CS43l22 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverCS43l22Class : public AudioDriver {
public:
  AudioDriverCS43l22Class(uint16_t deviceAddr = 0x94) {
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
    uint32_t freq = getFrequency(codec_cfg.i2s.rate);
    uint16_t outputDevice = getOutput(codec_cfg.dac_output);
    return cs43l22_Init(deviceAddr, outputDevice, vol, freq) == 0;
  }

  bool setMute(bool mute) {
    uint32_t rc = mute ? cs43l22_Pause(deviceAddr) : cs43l22_Resume(deviceAddr);
    return rc == 0;
  }

  bool setVolume(int volume) {
    this->volume = volume;
    int vol = map(volume, 0, 100, DEFAULT_VOLMIN, DEFAULT_VOLMAX);
    return cs43l22_SetVolume(deviceAddr, vol) == 0;
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

  uint32_t getFrequency(samplerate_t sample_rate) {
    switch (sample_rate) {
    case RATE_08K:
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

  uint16_t getOutput(dac_output_t dac_output) {
    switch (dac_output) {
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

// -- Drivers
/// @ingroup audio_driver
static AudioDriverES8388Class AudioDriverES8388;
/// @ingroup audio_driver
static AudioDriverAC101Class AudioDriverAC101;
/// @ingroup audio_driver
static AudioDriverCS43l22Class AudioDriverCS43l22;
/// @ingroup audio_driver
static AudioDriverES8311Class AudioDriverES8311;
/// @ingroup audio_driver
static AudioDriverES7243Class AudioDriverES7243;
/// @ingroup audio_driver
static AudioDriverLyratMiniClass AudioDriverLyratMini;



} // namespace audio_driver