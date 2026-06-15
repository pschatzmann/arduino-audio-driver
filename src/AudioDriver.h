#pragma once
#include "Codecs/AllCodecs.h"
#include "ConfigAudioDriver.h"
#include "DriverCommon.h"
#include "DriverDeviceInfo.h"
#include "Platforms/API_Delay.h"
#include "Platforms/API_GPIO.h"

namespace audio_driver {

const int rate_num[14] = {8000,  11025, 16000, 22050, 24000,  32000,  44100,
                          48000, 64000, 88200, 96000, 128000, 176400, 192000};
const samplerate_t rate_code[14] = {
    RATE_8K,  RATE_11K, RATE_16K, RATE_22K, RATE_24K,  RATE_32K,  RATE_44K,
    RATE_48K, RATE_64K, RATE_88K, RATE_96K, RATE_128K, RATE_176K, RATE_192K};

/**
 * @brief I2S configuration and definition of input and output with default
 * values
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
    i2s.channels = CHANNELS2;
    i2s.fmt = I2S_NORMAL;
    // codec is slave - microcontroller is master
    i2s.mode = MODE_SLAVE;
  }

  /// Compare all attributes but ignore sample rate
  bool equalsExRate(CodecConfig alt) {
    return (input_device == alt.input_device &&
            output_device == alt.output_device && i2s.bits == alt.i2s.bits &&
            i2s.channels == alt.i2s.channels && i2s.fmt == alt.i2s.fmt &&
            i2s.mode == alt.i2s.mode);
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

  /// Sets the bits per sample with a numeric value
  bool setBitsNumeric(int bits) {
    switch (bits) {
      case 16:
        i2s.bits = BIT_LENGTH_16BITS;
        return true;
      case 18:
        i2s.bits = BIT_LENGTH_18BITS;
        return true;
      case 20:
        i2s.bits = BIT_LENGTH_20BITS;
        return true;
      case 24:
        i2s.bits = BIT_LENGTH_24BITS;
        return true;
      case 32:
        i2s.bits = BIT_LENGTH_32BITS;
        return true;
    }
    AD_LOGE("bits not supported: %d", bits);
    return false;
  }

  /// Get the sample rate as number
  int getRateNumeric() {
    for (int j = 0; j < 14; j++) {
      if (rate_code[j] == i2s.rate) {
        AD_LOGD("-> %d", rate_num[j]);
        return rate_num[j];
      }
    }
    return 0;
  }

  /// Returns the number of channels as number
  int getChannelsNumeric() { return i2s.channels; }

  /// Defines the number of channels
  bool setChannelsNumeric(int channels) {
    switch (channels) {
      case CHANNELS2:
        i2s.channels = (channels_t)channels;
        return true;
      case CHANNELS4:
        i2s.channels = (channels_t)channels;
        return true;
      case CHANNELS8:
        i2s.channels = (channels_t)channels;
        return true;
      case CHANNELS16:
        i2s.channels = (channels_t)channels;
        return true;
      default:
        AD_LOGE("Channels not supported: %d - using %d", channels, 2);
        i2s.channels = CHANNELS2;
        return false;
    }
  }

  /// Sets the sample rate as number: returns the effectively set rate
  int setRateNumeric(int requestedRate) {
    int diff = 99999;
    int result = 0;
    for (int j = 0; j < 14; j++) {
      if (rate_num[j] == requestedRate) {
        AD_LOGD("-> %d", rate_num[j]);
        i2s.rate = rate_code[j];
        return requestedRate;
      } else {
        int new_diff = abs(rate_num[j] - requestedRate);
        if (new_diff < diff) {
          result = j;
          diff = new_diff;
        }
      }
    }
    AD_LOGE("Sample Rate not supported: %d - using %d", requestedRate,
            rate_num[result]);
    i2s.rate = rate_code[result];
    return rate_num[result];
  }

  /// Determines the codec_mode_t dynamically based on the input and output
  codec_mode_t get_mode() {
    // codec_mode_t mode;
    bool is_input = false;
    bool is_output = false;

    if (input_device != ADC_INPUT_NONE) is_input = true;

    if (output_device != DAC_OUTPUT_NONE) is_output = true;

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
  bool sd_active = false;
  bool sdmmc_active = false;
};

/**
 * @brief Abstract Driver API for codec chips
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriver {
 public:
  /// Starts the processing
  virtual bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    AD_LOGI("AudioDriver::begin");
    p_pins = &pins;

    // start GPIO
    getGPIO().begin(pins);

    // Store default i2c address to pins
    setupI2CAddress();

    AD_LOGI("sd_active: %d", codecCfg.sd_active);
    p_pins->setSPIActiveForSD(codecCfg.sd_active);
    AD_LOGI("sdmmc_active: %d", codecCfg.sdmmc_active);
    p_pins->setSDMMCActive(codecCfg.sdmmc_active);
    if (!p_pins->begin()) {
      AD_LOGE("AudioBoard::pins::begin failed");
      return false;
    }

    if (!setConfig(codecCfg)) {
      AD_LOGE("setConfig has failed");
      return false;
    }
    setPAPower(true);
    // setup default volume
    setVolume(DRIVER_DEFAULT_VOLUME);
    return true;
  }

  /// changes the configuration
  virtual bool setConfig(CodecConfig codecCfg) {
    AD_LOGI("AudioDriver::setConfig");
    codec_cfg = codecCfg;
    if (!init(codec_cfg)) {
      AD_LOGE("AudioDriver init failed");
      return false;
    }
    codec_mode_t codec_mode = codec_cfg.get_mode();
    if (!controlState(codec_mode)) {
      AD_LOGE("AudioDriver controlState failed");
      return false;
    }
    bool result = configInterface(codec_mode, codec_cfg.i2s);
    if (!result) {
      AD_LOGE("AudioDriver configInterface failed");
      return false;
    }
    return result;
  }
  /// Ends the processing: shut down dac and adc
  virtual bool end(void) { return deinit(); }
  /// Mutes all output lines
  virtual bool setMute(bool enable) = 0;
  /// Mute individual lines: only supported for some rare DACs
  virtual bool setMute(bool mute, int line) {
    AD_LOGE("setMute not supported on line level");
    return false;
  }

  /// Defines the Volume (in %) if volume is 0, mute is enabled,range is 0-100.
  virtual bool setVolume(int volume) = 0;
  /// Determines the actual volume (range: 0-100)
  virtual int getVolume() = 0;
  /// Defines the input volume (range: 0-100) if supported
  virtual bool setInputVolume(int volume) { return false; }
  /// Determines if setVolume() is suppored
  virtual bool isVolumeSupported() { return true; }
  /// Determines if setInputVolume() is supported
  virtual bool isInputVolumeSupported() { return false; }
  /// Provides the pin information
  virtual DriverDeviceInfo& pins() { return *p_pins; }

  void setPins(DriverDeviceInfo& pins) { p_pins = &pins; }

  /// Sets the PA Power pin to active or inactive
  bool setPAPower(bool enable) {
    if (p_pins == nullptr) {
      AD_LOGI("pins are null");
      return false;
    }
    GpioPin pin = pins().getPinID(PinFunction::PA);
    if (pin == GPIO_UNDEFINED) {
      AD_LOGI("PinFunction::PA not defined");
      return false;
    }
    AD_LOGI("setPAPower pin %d -> %d", pin, enable);
    getGPIO().digitalWrite(pin, enable ? HIGH : LOW);
    return true;
  }

  operator bool() { return p_pins != nullptr; }

  /// Defines the i2c address
  virtual bool setI2CAddress(uint16_t adr) {
    if (p_pins == nullptr) return false;
    // look it up from the pin definition
    auto i2c = pins().getI2CPins(PinFunction::CODEC);
    if (i2c) {
      AD_LOGI("==> Updating address: 0x%x", adr);
      InfoI2C val = i2c.value();
      val.address = adr;
      pins().setI2C(val);
      return true;
    } else {
      // we should have a codec defined!
      AD_LOGW("No I2C Pins defined");
    }
    return false;
  }

  /// Provides the i2c address
  virtual int getI2CAddress() {
    if (p_pins == nullptr) return i2c_default_address;
    // look it up from the pin definition
    auto i2c = pins().getI2CPins(PinFunction::CODEC);
    if (i2c) {
      auto value = i2c.value();
      auto addr = value.address;
      if (addr > -1) return addr;
    }
    return i2c_default_address;
  }

  /// If no address is defined in the pins we provide it here
  void setupI2CAddress() {
    AD_LOGI("setupI2CAddress: 0x%x", i2c_default_address);
    int adr = getI2CAddress();
    setI2CAddress(adr);
  }

  API_GPIO& getGPIO() {
    assert(p_pins != nullptr);
    return p_pins->getGPIO();
  }

 protected:
  CodecConfig codec_cfg;
  DriverDeviceInfo* p_pins = nullptr;
  int i2c_default_address = -1;

  int mapVolume(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  /// Determine the TwoWire object from the I2C config or use Wire
  virtual i2c_bus_handle_t getI2C() {
    if (p_pins == nullptr) return DEFAULT_WIRE;
    auto i2c = pins().getI2CPins(PinFunction::CODEC);
    if (!i2c) {
      return DEFAULT_WIRE;
    }
    i2c_bus_handle_t result = (i2c_bus_handle_t)i2c.value().p_wire;
    return result;
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
    if (volume > max) volume = max;
    if (volume < min) volume = min;
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
  virtual bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    // codec_cfg = codecCfg;
    // p_pins = &pins;
    return true;
  }
  virtual bool end(void) { return true; }
  virtual bool setMute(bool enable) { return false; }
  virtual bool setVolume(int volume) { return false; }
  virtual int getVolume() { return 100; }
  virtual bool setInputVolume(int volume) { return false; }
  virtual bool isVolumeSupported() { return false;}
  virtual bool isInputVolumeSupported() { return false; }
};

/**
 * @brief Driver API for AC101 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverAC101Class : public AudioDriver {
 public:
  AudioDriverAC101Class() { i2c_default_address = 0x1A; }
  bool setMute(bool mute) { return ac101.setVoiceMute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    return ac101.setVoiceVolume(limitValue(volume, 0, 100)) == RESULT_OK;
  };
  int getVolume() {
    int vol;
    ac101.getVoiceVolume(&vol);
    return vol;
  };

  /// Provides access to the wrapped AC101 driver instance
  AC101& driver() { return ac101; }

 protected:
  AC101 ac101;

  bool init(codec_config_t codec_cfg) {
    ac101.setWire(getI2C());
    ac101.setAddress(getI2CAddress());
    return ac101.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return ac101.deinit() == RESULT_OK; }
  bool controlState(codec_mode_t mode) {
    return ac101.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return ac101.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for the CS43l22 codec chip on 0x94 (0x4A<<1)
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverCS43l22Class : public AudioDriver {
 public:
  AudioDriverCS43l22Class(uint16_t deviceAddr = 0x4A) {
    i2c_default_address = deviceAddr;
  }

  virtual bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    AD_LOGD("AudioDriverCS43l22Class::begin");
    p_pins = &pins;
    codec_cfg = codecCfg;
    // manage reset pin -> acive high
    setPAPower(true);
    // Setup enable pin for codec
    delayMs(100);
    cs43l22.setWire(getI2C());
    cs43l22.setAddress(getI2CAddress());
    uint32_t freq = getFrequency(codec_cfg.i2s.rate);
    uint16_t outputDevice = getOutput(codec_cfg.output_device);
    AD_LOGD("cs43l22.init");
    bool result = cs43l22.init(outputDevice, volume, freq) == 0;
    if (!result) {
      AD_LOGE("error: cs43l22.init");
    }
    cs43l22.play(nullptr, 0);
    return result;
  }

  virtual bool setConfig(CodecConfig codecCfg) {
    codec_cfg = codecCfg;
    uint32_t freq = getFrequency(codec_cfg.i2s.rate);
    uint16_t outputDevice = getOutput(codecCfg.output_device);
    return cs43l22.init(outputDevice, this->volume, freq) == 0;
  }

  bool setMute(bool mute) {
    uint32_t rc = mute ? cs43l22.pause() : cs43l22.resume();
    return rc == 0;
  }

  bool setVolume(int volume) {
    this->volume = volume;
    return cs43l22.setVolume(volume) == 0;
  }
  int getVolume() { return volume; }

  /// Provides access to the wrapped CS43L22 driver instance
  CS43L22& driver() { return cs43l22; }

 protected:
  CS43L22 cs43l22;
  int volume = 100;

  bool deinit() {
    int cnt = cs43l22.stop(AUDIO_MUTE_ON);
    cnt += cs43l22.reset();
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
      default:
        break;
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
      default:
        break;
    }
    return OUTPUT_DEVICE_BOTH;
  }
};

/**
 * @brief Driver API for CS42448 TDS DAC/ADC
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverCS42448Class : public AudioDriver {
 public:
  AudioDriverCS42448Class() { i2c_default_address = 0x48; }
  bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) override {
    cfg = codecCfg;
    // setup pins
    pins.begin();
    // setup cs42448
    cs42448.begin(cfg, getI2C(), getI2CAddress());
    cs42448.setMute(false);
    return true;
  }
  virtual bool setConfig(CodecConfig codecCfg) {
    bool result = true;
    if (codecCfg.equalsExRate(cfg)) {
      // just update the rate
      if (cfg.i2s.rate != codecCfg.getRateNumeric()) {
        cs42448.setMute(true);
        cs42448.setSampleRate(codecCfg.getRateNumeric());
        cs42448.setMute(false);
      }
    } else {
      assert(p_pins != nullptr);
      result = begin(codecCfg, *p_pins);
    }
    return result;
  }
  bool end(void) override { return cs42448.end(); }
  bool setMute(bool enable) override { return cs42448.setMute(enable); }
  bool setMute(bool enable, int line) {
    return cs42448.setMuteDAC(line, enable);
  }
  /// Defines the Volume (in %) if volume is 0, mute is enabled,range is 0-100.
  bool setVolume(int volume) override {
    this->volume = volume;
    return cs42448.setVolumeDAC(2.55f * volume);
  }
  bool setVolume(int dac, int volume) {
    return cs42448.setVolumeDAC(dac, 2.55f * volume);
  }

  int getVolume() override { return volume; }
  bool setInputVolume(int volume) override {
    int vol = mapVolume(volume, 0, 100, -128, 127);
    return cs42448.setVolumeADC(vol);
  }
  bool isVolumeSupported() override { return true; }
  bool isInputVolumeSupported() override { return true; }

  DriverDeviceInfo& pins() { return *p_pins; }
  CS42448& driver() { return cs42448; }

 protected:
  CS42448 cs42448;
  int volume = 100;
  CodecConfig cfg;
};

/**
 * @brief Driver API for ES7210 codec chip. This chip supports only input!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES7210Class : public AudioDriver {
 public:
  AudioDriverES7210Class() { i2c_default_address = ES7210_AD1_AD0_00 >> 1; }
  bool setMute(bool mute) { return es7210.setMute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    this->volume = volume;
    return es7210.setVolume(limitValue(volume, 0, 100)) == RESULT_OK;
  }
  int getVolume() { return volume; }
  bool setInputVolume(int volume) { return setVolume(volume); }
  bool isInputVolumeSupported() { return true; }
  ES7210& driver() { return es7210; }

 protected:
  ES7210 es7210;
  int volume;

  bool init(codec_config_t codec_cfg) {
    es7210.setWire(getI2C());
    es7210.setAddress(getI2CAddress());
    return es7210.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return es7210.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es7210.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es7210.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for Lyrat ES7243 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES7243Class : public AudioDriver {
 public:
  AudioDriverES7243Class() { i2c_default_address = 0x13; }
  bool setMute(bool mute) {
    return es7243.setVoiceMute(mute) == RESULT_OK;
  }
  bool setVolume(int volume) {
    return es7243.setVoiceVolume(limitValue(volume, 0, 100)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es7243.getVoiceVolume(&vol);
    return vol;
  }
  ES7243& driver() { return es7243; }

 protected:
  ES7243 es7243;

  bool init(codec_config_t codec_cfg) {
    es7243.setWire(getI2C());
    es7243.setAddress(getI2CAddress());
    return es7243.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return es7243.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es7243.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es7243.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES7243e codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class AudioDriverES7243eClass : public AudioDriver {
 public:
  AudioDriverES7243eClass() { i2c_default_address = 0x10; }
  bool setMute(bool mute) {
    return mute ? setVolume(0) == RESULT_OK : setVolume(volume) == RESULT_OK;
  }
  bool setVolume(int volume) {
    this->volume = volume;
    return es7243e.setVoiceVolume(limitValue(volume, 0, 100)) ==
           RESULT_OK;
  }
  int getVolume() {
    int vol = 0;
    es7243e.getVoiceVolume(&vol);
    return vol;
  }
  ES7243E& driver() { return es7243e; }

 protected:
  ES7243E es7243e;
  int volume = 0;

  bool init(codec_config_t codec_cfg) {
    es7243e.setWire(getI2C());
    es7243e.setAddress(getI2CAddress());
    return es7243e.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return es7243e.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es7243e.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es7243e.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES8156 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8156Class : public AudioDriver {
 public:
  AudioDriverES8156Class() { i2c_default_address = ES8156_ADDR; }
  bool setMute(bool mute) {
    return es8156.setVoiceMute(mute) == RESULT_OK;
  }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return es8156.setVoiceVolume(limitValue(volume, 0, 100)) ==
           RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8156.getVoiceVolume(&vol);
    return vol;
  }
  ES8156& driver() { return es8156; }

 protected:
  ES8156 es8156;

  bool init(codec_config_t codec_cfg) {
    es8156.setWire(getI2C());
    es8156.setAddress(getI2CAddress());
    return es8156.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return es8156.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8156.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8156.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for Lyrat  ES8311 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8311Class : public AudioDriver {
 public:
  AudioDriverES8311Class() { i2c_default_address = ES8311_ADDR; }
  bool setMute(bool mute) { return es8311.setVoiceMute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    return es8311.setVoiceVolume(limitValue(volume, 0, 100)) ==
           RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8311.getVoiceVolume(&vol);
    return vol;
  }

  ///
  void setMasterClockSource(int source) { master_clock_source = source; }

  ES8311& driver() { return es8311; }

 protected:
  ES8311 es8311;
  int master_clock_source = -1;

  bool init(codec_config_t codec_cfg) {
    int mclk_src = master_clock_source;
    if (mclk_src == -1) {
      mclk_src = 0;  // = FROM_MCLK_PIN;
    }
    AD_LOGI("MCLK_SOURCE: %d", mclk_src);

    assert(getI2C() != nullptr);
    es8311.setWire(getI2C());
    es8311.setAddress(getI2CAddress());
    es8311.setMclkSrc(mclk_src);
    return es8311.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return es8311.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8311.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8311.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES8374 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8374Class : public AudioDriver {
 public:
  AudioDriverES8374Class() { i2c_default_address = ES8374_ADDR; }
  bool setMute(bool mute) { return es8374.setVoiceMute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return es8374.setVoiceVolume(limitValue(volume, 0, 100)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8374.getVoiceVolume(&vol);
    return vol;
  }

  /// Provides access to the ES8374 driver
  ES8374& driver() { return es8374; }

 protected:
  ES8374 es8374;

  bool init(codec_config_t codec_cfg) {
    auto codec_mode = this->codec_cfg.get_mode();
    es8374.setWire(getI2C());
    es8374.setAddress(getI2CAddress());
    return es8374.init(&codec_cfg, codec_mode) == RESULT_OK;
  }
  bool deinit() { return es8374.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8374.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8374.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for ES8388 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverES8388Class : public AudioDriver {
 public:
  AudioDriverES8388Class(int volumeHack) {
    i2c_default_address = ES8388_ADDR;
    volume_hack = volumeHack;
  }
  AudioDriverES8388Class() { i2c_default_address = ES8388_ADDR; }
  bool setMute(bool mute) {
    line_active[0] = !mute;
    line_active[1] = !mute;
    return es8388.setVoiceMute(mute) == RESULT_OK;
  }
  // mute individual line: lines start at 0 (valid range 0:1)
  bool setMute(bool mute, int line) {
    if (line > 1) {
      AD_LOGD("invalid line %d", line);
      return false;
    }
    // mute is managed on line level, so deactivate global mute
    line_active[line] = !mute;
    if (line_active[0] && line_active[1]) {
      return es8388.configOutputDevice(DAC_OUTPUT_ALL) == RESULT_OK;
    } else if (!line_active[0] && !line_active[1]) {
      return es8388.configOutputDevice(DAC_OUTPUT_NONE) == RESULT_OK;
    } else if (line_active[0]) {
      return es8388.configOutputDevice(DAC_OUTPUT_LINE1) == RESULT_OK;
    } else if (line_active[1]) {
      return es8388.configOutputDevice(DAC_OUTPUT_LINE2) == RESULT_OK;
    }
    return false;
  }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return es8388.setVoiceVolume(limitValue(volume, 0, 100)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    es8388.getVoiceVolume(&vol);
    return vol;
  }

  /// Provides access to the ES8388 driver
  ES8388& driver() { return es8388; }

  bool setInputVolume(int volume) {
    // mapVolume values from 0 - 100 to 0 to 8

    // es_mic_gain_t: MIC_GAIN_MIN = -1, 0,3,6,9,12,15,18,21,24 MIC_GAIN_MAX =
    // 25

    // Vol:     0, 12.5, 25, 37.5, 50, 62.5, 75, 87.5, 100
    // idx:     0,    1,  2,    3,  4,    5,  6,    7,   8
    // dB/gain: 0,    3,  6,    9, 12,   15, 18,   21,  24
    // factor:  1,    2,  4,    8, 16,   32, 63,  126, 252

    // es8388 Register 9 – ADC Control 1
    // dB MicL MicR
    // 0 0000 0000
    // 3 0001 0001
    // 6 0010 0010
    // 9 0011 0011
    // 12 0100 0100
    // 15 0101 0101
    // 18 0110 0110
    // 21 0111 0111
    // 24 1000 1000

    es_mic_gain_t gains[] = {MIC_GAIN_0DB,  MIC_GAIN_3DB,  MIC_GAIN_6DB,
                             MIC_GAIN_9DB,  MIC_GAIN_12DB, MIC_GAIN_15DB,
                             MIC_GAIN_18DB, MIC_GAIN_21DB, MIC_GAIN_24DB};

    int vol = limitValue(volume, 0, 100);
    int idx = mapVolume(vol, 0, 100, 0, 8);

    es_mic_gain_t gain = gains[idx];
    AD_LOGD("input volume: %d -> gain %d [dB] (idx: %d of 0..8)", volume, gain,
            idx);
    return setMicrophoneGain(gain);
  }

  bool setMicrophoneGain(es_mic_gain_t gain) {
    return es8388.setMicGain(gain) == RESULT_OK;
  }

  bool isInputVolumeSupported() { return true; }

  void setVolumeHack(int volume_hack) {
    this->volume_hack = volume_hack;
    es8388.setVolumeHack(volume_hack);
  }
  int getVolumeHack() { return volume_hack; }

 protected:
  ES8388 es8388;
  bool line_active[2] = {true, true};
  int volume_hack = AI_THINKER_ES8388_VOLUME_HACK;

  bool init(codec_config_t codec_cfg) {
    es8388.setWire(getI2C());
    es8388.setAddress(getI2CAddress());
    return es8388.init(&codec_cfg, volume_hack) == RESULT_OK;
  }
  bool deinit() { return es8388.deinit() == RESULT_OK; }

  bool controlState(codec_mode_t mode) {
    return es8388.ctrlStateActive(mode, true) == RESULT_OK;
  }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    return es8388.configI2S(mode, &iface) == RESULT_OK;
  }
};

/**
 * @brief Driver API for TAS5805M codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverTAS5805MClass : public AudioDriver {
 public:
  AudioDriverTAS5805MClass() { i2c_default_address = TAS5805M_ADDR; }
  bool setMute(bool mute) { return tas5805m.setMute(mute) == RESULT_OK; }
  bool setVolume(int volume) {
    AD_LOGD("volume %d", volume);
    return tas5805m.setVolume(limitValue(volume, 0, 100)) == RESULT_OK;
  }
  int getVolume() {
    int vol;
    tas5805m.getVolume(&vol);
    return vol;
  }

  /// Provides access to the TAS5805M driver
  TAS5805M& driver() { return tas5805m; }

 protected:
  TAS5805M tas5805m;

  bool init(codec_config_t codec_cfg) {
    tas5805m.setWire(getI2C());
    tas5805m.setAddress(getI2CAddress());
    return tas5805m.init(&codec_cfg) == RESULT_OK;
  }
  bool deinit() { return tas5805m.deinit() == RESULT_OK; }
};

/**
 * @brief Driver API for WM8990 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverWM8960Class : public AudioDriver {
 public:
  AudioDriverWM8960Class() { i2c_default_address = WM8960_I2C_ADDRESS; }

  /// Provides access to the WM8960 driver
  WM8960& driver() { return wm8960; }

  bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    codec_cfg = codecCfg;

    // define wire object
    wm8960.setWire(getI2C());
    wm8960.setAddress(getI2CAddress());
    wm8960.setWriteRetryCount(i2c_retry_count);

    // setup wm8960
    int features = getFeatures(codecCfg);
    if (!wm8960.init(features)) {
      AD_LOGE("wm8960.init");
      return false;
    }
    setVolume(DRIVER_DEFAULT_VOLUME);
    if (!wm8960.activate()) {
      AD_LOGE("wm8960.activate");
      return false;
    }
    if (!configure_clocking()) {
      AD_LOGE("configure_clocking");
      return false;
    }
    return true;
  }
  bool end(void) {
    wm8960.deactivate();
    wm8960.deinit();
    return true;
  }
  virtual bool setConfig(CodecConfig codecCfg) {
    codec_cfg = codecCfg;
    return configure_clocking();
  }

  bool setMute(bool enable) { return setVolume(enable ? 0 : volume_out); }

  /// Defines the Volume (in %) if volume is 0, mute is enabled,range is 0-100.
  bool setVolume(int volume) {
    volume_out = limitValue(volume, 0, 100);
    int vol_int =
        volume_out == 0.0 ? 0 : mapVolume(volume_out, 0, 100, 30, 0x7F);
    return wm8960.setOutputVolume(vol_int);
  }

  int getVolume() { return volume_out; }

  bool setInputVolume(int volume) {
    volume_in = limitValue(volume, 0, 100);
    int vol_int = mapVolume(volume_in, 0, 100, 0, 30);
    return wm8960.adjustInputVolume(vol_int);
  }
  bool isVolumeSupported() { return true; }

  bool isInputVolumeSupported() { return true; }

  /// Configuration: define retry count (default : 0)
  void setI2CRetryCount(int cnt) { i2c_retry_count = cnt; }

  /// Configuration: enable/disable PLL (active by default)
  void setEnablePLL(bool active) { vs1053_enable_pll = active; }

  /// Configuration: define master clock frequency (default: 0)
  void setMclkHz(uint32_t hz) { vs1053_mclk_hz = hz; }

  void dumpRegisters() { wm8960.dump(); }

 protected:
  WM8960 wm8960;
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
    if (!wm8960.configureClocking(
            vs1053_mclk_hz, vs1053_enable_pll,
            sampleRate(codec_cfg.getRateNumeric()),
            wordLength(codec_cfg.getBitsNumeric()),
            modeMasterSlave(codec_cfg.i2s.mode == MODE_MASTER))) {
      AD_LOGE("wm8960.configureClocking");
      return false;
    }
    return true;
  }

  wm8960_adc_dac_sample_rate_t sampleRate(int rate) {
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

  wm8960_word_length_t wordLength(int bits) {
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
  wm8960_mode_t modeMasterSlave(bool is_master) {
    return is_master ? WM8960_MODE_MASTER : WM8960_MODE_SLAVE;
  }
};

/**
 * @brief Driver API for the wm8978 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverWM8978Class : public AudioDriver {
 public:
  AudioDriverWM8978Class() = default;

  bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) override {
    bool rc = true;
    rc = wm8078.begin(getI2C(), getI2CAddress());
    setConfig(codecCfg);

    // setup initial default volume
    setVolume(DRIVER_DEFAULT_VOLUME);

    return rc;
  }

  bool setConfig(CodecConfig codecCfg) override {
    codec_cfg = codecCfg;
    bool is_dac = codec_cfg.output_device != DAC_OUTPUT_NONE;
    bool is_adc = codec_cfg.input_device != ADC_INPUT_NONE;
    wm8078.cfgADDA(is_dac, is_adc);

    bool is_mic = codec_cfg.input_device == ADC_INPUT_LINE1 ||
                  codec_cfg.input_device == ADC_INPUT_ALL;
    bool is_linein = codec_cfg.input_device == ADC_INPUT_LINE2 ||
                     codec_cfg.input_device == ADC_INPUT_ALL;
    bool is_auxin = codec_cfg.input_device == ADC_INPUT_LINE3 ||
                    codec_cfg.input_device == ADC_INPUT_ALL;
    wm8078.cfgInput(is_mic, is_linein, is_auxin);
    wm8078.cfgOutput(is_dac, false);

    int bits = toBits(codecCfg.i2s.bits);
    if (bits < 0) return false;
    int i2s = toI2S(codecCfg.i2s.fmt);
    if (i2s < 0) return false;
    wm8078.cfgI2S(i2s, bits);
    return true;
  }

  bool end() override {
    setVolume(0);
    wm8078.cfgADDA(false, false);
    return true;
  }

  /// Mute line 0 = speaker, line 1 = headphones
  bool setMute(bool mute, int line) override {
    int scaled = mute ? 0 : mapVolume(volume, 0, 100, 0, 63);
    switch (line) {
      case 0:
        wm8078.setSPKvol(scaled);
        return true;
      case 1:
        wm8078.setHPvol(scaled, scaled);
        return true;
      default:
        return false;
    }
    return false;
  }

  bool setMute(bool mute) override {
    if (mute) {
      // set volume to 0
      wm8078.setSPKvol(0);
      wm8078.setHPvol(0, 0);
    } else {
      // restore volume
      setVolume(volume);
    }
    return true;
  }

  bool setVolume(int volume) override {
    this->volume = volume;
    int scaled = mapVolume(volume, 0, 100, 0, 63);
    wm8078.setSPKvol(scaled);
    wm8078.setHPvol(scaled, scaled);
    return true;
  }

  int getVolume() override { return volume; }

  bool setInputVolume(int volume) override {
    int scaled = mapVolume(volume, 0, 100, 0, 63);
    wm8078.setMICgain(scaled);
    wm8078.setAUXgain(scaled);
    wm8078.setLINEINgain(scaled);
    return true;
  }

  bool isVolumeSupported() override { return true; }

  bool isInputVolumeSupported() override { return true; }

  WM8978& driver() { return wm8078; }

 protected:
  WM8978 wm8078;
  int volume = 0;

  /// fmt:0,LSB(right-aligned);1,MSB(left-aligned);2,Philips standard,
  /// I2S;3,PCM/DSP;
  int toI2S(i2s_format_t fmt) {
    switch (fmt) {
      case I2S_NORMAL:
        return 2;
      case I2S_LEFT:
        return 1;
      case I2S_RIGHT:
        return 0;
      case I2S_DSP:
        return 3;
      default:
        break;
    }
    return -1;
  }

  // len: 0, 16 digits; 1, 20 digits; 2, 24 digits; 3, 32 digits;
  int toBits(sample_bits_t bits) {
    switch (bits) {
      case BIT_LENGTH_16BITS:
        return 0;
      case BIT_LENGTH_20BITS:
        return 1;
      case BIT_LENGTH_24BITS:
        return 2;
      case BIT_LENGTH_32BITS:
        return 3;
      default:
        break;
    }
    return -1;
  }
};

/**
 * @brief Driver API for the wm8994 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverWM8994Class : public AudioDriver {
 public:
  AudioDriverWM8994Class(uint16_t deviceAddr = WM8994_ADDR) {
    this->i2c_default_address = deviceAddr;
  }

  /// Provides access to the WM8994 driver
  WM8994& driver() { return wm8994; }

  virtual bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    codec_cfg = codecCfg;
    // manage reset pin -> active high
    setPAPower(true);
    delayMs(10);
    p_pins = &pins;
    int vol = mapVolume(volume, 0, 100, DEFAULT_VOLMIN, DEFAULT_VOLMAX);
    uint32_t freq = codecCfg.getRateNumeric();
    uint16_t outputDevice = getOutput(codec_cfg.output_device);

    wm8994.setWire(getI2C());
    wm8994.setAddress(getI2CAddress());
    return wm8994.init(outputDevice, vol, freq) == 0;
  }

  bool setMute(bool mute) {
    uint32_t rc = mute ? wm8994.pause() : wm8994.resume();
    return rc == 0;
  }

  bool setVolume(int volume) {
    this->volume = volume;
    int vol = mapVolume(volume, 0, 100, DEFAULT_VOLMIN, DEFAULT_VOLMAX);
    return wm8994.setVolume(vol) == 0;
  }
  int getVolume() { return volume; }

 protected:
  int volume = 100;
  WM8994 wm8994;

  bool deinit() {
    int cnt = wm8994.stop(AUDIO_MUTE_ON);
    cnt += wm8994.reset();
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
 * @brief Driver API for the CS43l22 codec chip on 0x94 (0x4A<<1)
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverPCM3168Class : public AudioDriver {
 public:
  AudioDriverPCM3168Class() { i2c_default_address = 0x44; };

  bool setMute(bool mute) { return driver.setMute(mute); }

  bool setMute(bool mute, int line) { return driver.setMute(line, mute); }

  bool setVolume(int vol) {
    volume = vol;
    return driver.setVolume(100.0 * vol);
  }
  int getVolume() { return volume; }

 protected:
  int volume;
  PCM3168 driver;

  bool init(codec_config_t codec_cfg) {
    driver.setWire(getI2C());
    driver.setAddress(getI2CAddress());
    return true;
  }
  bool deinit() { return driver.end(); }
  bool controlState(codec_mode_t mode) { return true; }
  bool configInterface(codec_mode_t mode, I2SDefinition iface) {
    if (iface.mode == MODE_MASTER) {
      AD_LOGE("Only slave is supported: MCU must be master");
      return false;
    }
    PCM3168::FMT fmt = PCM3168::FMT::I2SHighSpeedTDM24bit;
    switch (iface.bits) {
      case BIT_LENGTH_16BITS:
        if (iface.fmt != I2S_RIGHT) {
          AD_LOGW("Only I2S_RIGHT is supported for 16 bits");
        }
        fmt = PCM3168::FMT::Right16bit;
        break;
      case BIT_LENGTH_32BITS:
      case BIT_LENGTH_24BITS:
        if (iface.signal_type == SIGNAL_TDM) {
          switch (iface.fmt) {
            case I2S_NORMAL:
              fmt = PCM3168::FMT::I2STDM24bit;
              break;
            case I2S_LEFT:
              fmt = PCM3168::FMT::LeftTDM24bit;
              break;
            default:
              AD_LOGE("Unsupported fmt for tdm");
              return false;
          }
        } else {
          switch (iface.fmt) {
            case I2S_NORMAL:
              fmt = PCM3168::FMT::I2S24bit;
              break;
            case I2S_LEFT:
              fmt = PCM3168::FMT::Left24bit;
              break;
            case I2S_RIGHT:
              fmt = PCM3168::FMT::Right24bit;
              break;
            case I2S_DSP:
              fmt = PCM3168::FMT::LeftDSP24bit;
              break;
            default:
              AD_LOGE("Unsupported fmt");
              return false;
          }
        }
        break;
      default:
        AD_LOGE("Unsupported bits");
        return false;
    }

    return driver.begin(fmt);
  }
};

/**
 * @brief Driver API for Lyrat Mini with a ES8311 and a ES7243 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverLyratMiniClass : public AudioDriver {
 public:
  bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    AD_LOGI("AudioDriverLyratMiniClass::begin");
    p_pins = &pins;
    codec_cfg = codecCfg;

    // setup SPI for SD
    // pins.setSPIActiveForSD(codecCfg.sd_active);

    // Start ES8311
    AD_LOGI("starting ES8311");
    pins.begin();
    dac.setPins(this->pins());
    if (!dac.setConfig(codecCfg)) {
      AD_LOGE("setConfig failed");
      return false;
    }
    setPAPower(true);
    setVolume(DRIVER_DEFAULT_VOLUME);

    // Start ES7243
    if (codecCfg.input_device != ADC_INPUT_NONE) {
      AD_LOGI("starting ES7243");
      adc.setPins(this->pins());
      if (!adc.setConfig(codecCfg)) {
        AD_LOGE("adc.begin failed");
        return false;
      }
    }

    return true;
  }
  bool end(void) {
    int rc = 0;
    rc += dac.end();
    rc += adc.end();
    return rc == 2;
  }
  bool setMute(bool enable) override { return dac.setMute(enable); }
  bool setVolume(int volume) override { return dac.setVolume(volume); }
  int getVolume() override { return dac.getVolume(); }
  bool setInputVolume(int volume) override { return adc.setVolume(volume); }
  int getInputVolume() { return adc.getVolume(); }
  bool isInputVolumeSupported() override { return true; }

 protected:
  AudioDriverES8311Class dac;
  AudioDriverES7243Class adc;
};

/**
 * @brief Generic Driver API for boards with a different DAC and ADC codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverCombined : public AudioDriver {
 public:
  AudioDriverCombined(AudioDriver& dac, AudioDriver& adc, bool sdActive = true)
      : p_dac(&dac), p_adc(&adc) {}

  bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) {
    AD_LOGI("AudioDriverCombined::begin");
    p_pins = &pins;
    codec_cfg = codecCfg;

    assert(p_dac != nullptr);
    assert(p_adc != nullptr);

    AD_LOGI("sd_active: %d", codecCfg.sd_active);
    p_pins->setSPIActiveForSD(codecCfg.sd_active);
    AD_LOGI("sdmmc_active: %d", codecCfg.sdmmc_active);
    p_pins->setSDMMCActive(codecCfg.sdmmc_active);

    // Start
    AD_LOGI("starting DAC");
    pins.begin();
    p_dac->setPins(this->pins());
    if (!p_dac->setConfig(codecCfg)) {
      AD_LOGE("setConfig failed");
      return false;
    }
    setPAPower(true);
    setVolume(DRIVER_DEFAULT_VOLUME);

    // Start ADC
    if (codecCfg.input_device != ADC_INPUT_NONE) {
      AD_LOGI("starting ADC");
      p_adc->setPins(this->pins());
      if (!p_adc->setConfig(codecCfg)) {
        AD_LOGE("adc.begin failed");
        return false;
      }
    }
    return true;
  }
  bool end(void) {
    int rc = 0;
    rc += p_dac->end();
    rc += p_adc->end();
    return rc == 2;
  }
  bool setMute(bool enable) override { return p_dac->setMute(enable); }
  bool setVolume(int volume) override { return p_dac->setVolume(volume); }
  int getVolume() override { return p_dac->getVolume(); }
  bool setInputVolume(int volume) override { return p_adc->setVolume(volume); }
  int getInputVolume() { return p_adc->getVolume(); }
  bool isInputVolumeSupported() override { return true; }

 protected:
  AudioDriver* p_dac = nullptr;
  AudioDriver* p_adc = nullptr;
};

/**
 * @brief Generic Driver API that wraps a codec driver class which has been
 * ported from the Zephyr RTOS (see Codecs/<chip>/*.h, e.g. WM8904, DA7212,
 * TAS2563, ...). It provides the I2C wire/address from the pin configuration
 * to the driver and calls its begin()/setVolume()/setMute() (or
 * setOutputVolume()/setOutputMute()) methods, so that the chip can be used
 * via the regular AudioDriver API.
 *
 * Some chips require additional parameters (e.g. the master clock
 * frequency) for their begin() method: in this case begin() is not called
 * automatically and driver() must be used to set up the chip manually before
 * AudioDriver::begin() is called.
 *
 * @tparam T the Zephyr ported codec driver class (e.g. WM8904)
 * @tparam DefaultI2CAddress the default I2C address of the chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
template <class T, uint8_t DefaultI2CAddress = 0>
class AudioDriverZephyrT : public AudioDriver {
 public:
  AudioDriverZephyrT(uint8_t i2cAddress = DefaultI2CAddress) {
    if (i2cAddress != 0) i2c_default_address = i2cAddress;
  }

  /// Provides access to the wrapped Zephyr driver instance
  T& driver() { return zephyr_driver; }

  bool setMute(bool enable) override { return zephyr_driver.setMute(enable); }

  bool setVolume(int volume) override {
    volume_value = limitValue(volume, 0, 100);
    return zephyr_driver.setVolume(volume_value);
  }

  int getVolume() override { return volume_value; }

  bool setInputVolume(int volume) override {
    return zephyr_driver.setInputVolume(limitValue(volume, 0, 100));
  }

  int getInputVolume() { return zephyr_driver.getInputVolume(); }

  bool isInputVolumeSupported() override {
    return zephyr_driver.isInputVolumeSupported();
  }

 protected:
  T zephyr_driver;
  int volume_value = 100;

  bool init(codec_config_t codecCfg) override {
    zephyr_driver.setWire(getI2C());
    zephyr_driver.setAddress((uint8_t)getI2CAddress());
    return true;
  }
  /// Stops the input and output and (if supported) shuts down the codec
  bool deinit() override { return zephyr_driver.setMute(true); }
  bool controlState(codec_mode_t mode) override { return true; }
  /// Configures the codec (sample rate, bits per sample) for the given I2S setup
  bool configInterface(codec_mode_t mode, I2SDefinition iface) override {
    CodecConfig cfg;
    cfg.i2s = iface;
    return zephyr_driver.begin((uint32_t)cfg.getRateNumeric(),
                                (uint8_t)cfg.getBitsNumeric());
  }
};

/*  -- NAU8325 Driver Class---  */
class AudioDriverNAU8325Class : public AudioDriver {
 public:
  PCBCUPID_NAU8325* nau8325 = nullptr;  // pointer to construct later

  ~AudioDriverNAU8325Class() {
    if (nau8325) delete nau8325;
  }

  bool begin(CodecConfig cfg, DriverDeviceInfo& pins) override {
    AD_LOGI("AudioDriverNAU8325Class::begin");

    this->p_pins = &pins;

    // Get I2C config
    auto i2c_opt = pins.getI2CPins(PinFunction::CODEC);
    if (!i2c_opt) {
      AD_LOGE("No I2C pins defined for codec");
      return false;
    }
    InfoI2C val = i2c_opt.value();

    // Create instance with required params (only TwoWire&)
    nau8325 = new PCBCUPID_NAU8325(val.p_wire);

    // Set MCLK if available
    auto mclk = pins.getPinID(PinFunction::MCLK_SOURCE);
    if (mclk != GPIO_UNDEFINED) {
      getGPIO().pinMode(mclk, OUTPUT);
      getGPIO().digitalWrite(mclk, HIGH);
      delayMs(10);  // optional small delay to stabilize
    }

    // Begin your driver using known public methods
    int fs = cfg.getRateNumeric();
    int bits = cfg.getBitsNumeric();

    AD_LOGI("Calling nau8325->begin(fs=%d, bits=%d)", fs, bits);
    if (!nau8325->begin(fs, bits)) {
      AD_LOGE("NAU8325 begin failed");
      return false;
    }

    return true;
  }

  bool end() override {
    if (nau8325) {
      nau8325->powerOff();
    }
    return true;
  }

  bool setMute(bool enable) override {
    if (!nau8325) return false;
    nau8325->softMute(enable);
    return true;
  }

  bool setVolume(int volume) override {
    if (!nau8325) return false;
    // Mapping volume (0–100) to 0–255 (NAU range)
    uint8_t val = mapVolume(volume, 0, 100, 0, 255);
    nau8325->setVolume(val, val);  // assuming stereo
    return true;
  }

  int getVolume() override {
    // NAU8325 doesn't have a getVolume, so return last set or dummy
    return 100;
  }
};

#ifdef ARDUINO
// currently only supported in Arduino because we do not have a platform
// independent SPI API
/**
 * @brief Driver API for AD1938 TDS DAC/ADC
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioDriverAD1938Class : public AudioDriver {
 public:
  bool begin(CodecConfig codecCfg, DriverDeviceInfo& pins) override {
    int clatch = pins.getPinID(PinFunction::LATCH);
    if (clatch < 0) return false;
    int reset = pins.getPinID(PinFunction::RESET);
    if (reset < 0) return false;
    auto spi_opt = pins.getSPIPins(PinFunction::CODEC);
    SPIClass* p_spi = nullptr;
    if (spi_opt) {
      p_spi = (SPIClass*)spi_opt.value().p_spi;
    } else {
      p_spi = &SPI;
      p_spi->begin();
    }
    // setup pins
    pins.begin();
    // setup ad1938
    ad1938.begin(getGPIO(), codecCfg, clatch, reset, *p_spi);
    ad1938.enable();
    ad1938.setMute(false);
    return true;
  }
  virtual bool setConfig(CodecConfig codecCfg) {
    assert(p_pins != nullptr);
    bool result = begin(codecCfg, *p_pins);
    return result;
  }
  bool end(void) override { return ad1938.end(); }
  bool setMute(bool mute) override { return ad1938.setMute(mute); }
  // mutes an individual DAC: valid range (0:7)
  bool setMute(bool mute, int line) {
    if (line < 0 || line > 7) return false;
    return ad1938.setVolume(
        line, mute ? 0.0f : (static_cast<float>(volumes[line]) / 100.0f));
  }

  /// Defines the Volume (in %) if volume is 0, mute is enabled,range is 0-100.
  bool setVolume(int volume) override {
    this->volume = volume;
    for (int j = 0; j < 8; j++) {
      volumes[j] = volume;
    }
    return ad1938.setVolume(static_cast<float>(volume) / 100.0f);
  }
  /// Defines the Volume per DAC (in %) if volume is 0, mute is enabled,range is
  /// 0-100.
  bool setVolume(int volume, int line) {
    if (line < 0 || line > 7) return false;
    volumes[line] = volume;
    return ad1938.setVolume(line, static_cast<float>(volume) / 100.0f);
  }

  int getVolume() override { return volume; }
  bool setInputVolume(int volume) override { return false; }
  bool isVolumeSupported() override { return true; }
  bool isInputVolumeSupported() override { return false; }

  DriverDeviceInfo& pins() { return *p_pins; }
  AD1938& driver() { return ad1938; }

 protected:
  AD1938 ad1938;
  int volume = 100;
  int volumes[8] = {100, 100, 100, 100, 100, 100, 100, 100};
};

#endif

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
static AudioDriverES8388Class AudioDriverES8388H0{0};
/// @ingroup audio_driver
static AudioDriverES8388Class AudioDriverES8388H1{1};
/// @ingroup audio_driver
static AudioDriverES8388Class AudioDriverES8388H2{2};
/// @ingroup audio_driver
static AudioDriverES8388Class AudioDriverES8388H3{3};
/// @ingroup audio_driver
static AudioDriverWM8960Class AudioDriverWM8960;
/// @ingroup audio_driver
static AudioDriverWM8978Class AudioDriverWM8978;
/// @ingroup audio_driver
static AudioDriverWM8994Class AudioDriverWM8994;
/// @ingroup audio_driver
static AudioDriverLyratMiniClass AudioDriverLyratMini;
/// @ingroup audio_driver
static NoDriverClass NoDriver;
/// @ingroup audio_driver
static AudioDriverCS42448Class AudioDriverCS42448;
/// @ingroup audio_driver
static AudioDriverPCM3168Class AudioDriverPCM3168;
/// @ingroup audio_driver
static AudioDriverNAU8325Class AudioDriverNAU8325;
/// @ingroup audio_driver
static AudioDriverCombined AudioDriverES8311_ES7210(AudioDriverES8311,
                                                    AudioDriverES7210);
/// @ingroup audio_driver
static AudioDriverCombined AudioDriverES8311_ES7243(AudioDriverES8311,
                                                    AudioDriverES7243);

// -- Zephyr ported Drivers
/// @ingroup audio_driver
static AudioDriverZephyrT<WM8904, 0x1A> AudioDriverWM8904;
/// @ingroup audio_driver
static AudioDriverZephyrT<WM8962, 0x1A> AudioDriverWM8962;
/// @ingroup audio_driver
static AudioDriverZephyrT<MAX98091, 0x10> AudioDriverMAX98091;
/// @ingroup audio_driver
static AudioDriverZephyrT<AW88298, 0x36> AudioDriverAW88298;
/// @ingroup audio_driver
static AudioDriverZephyrT<PCM1681, 0x4C> AudioDriverPCM1681;
/// @ingroup audio_driver
static AudioDriverZephyrT<TAS2563, 0x4C> AudioDriverTAS2563;
/// @ingroup audio_driver
static AudioDriverZephyrT<TAS6422DAC, 0x6C> AudioDriverTAS6422DAC;
/// @ingroup audio_driver
static AudioDriverZephyrT<TLV320DAC310x, 0x18> AudioDriverTLV320DAC310x;
/// @ingroup audio_driver
static AudioDriverZephyrT<DA7212, 0x1A> AudioDriverDA7212;
/// @ingroup audio_driver
static AudioDriverZephyrT<TLV320AIC3110, 0x18> AudioDriverTLV320AIC3110;

#ifdef ARDUINO
/// @ingroup audio_driver
static AudioDriverAD1938Class AudioDriverAD1938;
#endif

}  // namespace audio_driver
