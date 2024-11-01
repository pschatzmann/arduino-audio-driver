#pragma once
#include "Driver.h"
#include "DriverPins.h"

namespace audio_driver {

/**
 * @brief Defitintion for audio board pins and an audio driver
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AudioBoard {
public:
  AudioBoard(AudioDriver *driver, DriverPins* pins=&NoPins) {
    this->pins = pins;
    this->driver = driver;
  }
  
  AudioBoard(AudioDriver &driver, DriverPins& pins=NoPins) {
    this->pins = &pins;
    this->driver = &driver;
  }

  bool begin(){
    AD_LOGD("AudioBoard::begin");
    pins->setSPIActiveForSD(codec_cfg.sd_active);
    if (!pins->begin()){
      AD_LOGE("AudioBoard::pins::begin failed");
      return false;
    }
    if (!driver->begin(codec_cfg, *pins)){
      AD_LOGE("AudioBoard::driver::begin failed");
      return false;
    }
    setVolume(DRIVER_DEFAULT_VOLUME);
    is_active = true;
    return true;
  }

  /// Starts the processing
  bool begin(CodecConfig cfg) {
    this->codec_cfg = cfg;
    return begin();
  }

  /// Updates the CodecConfig values -> reconfigures the codec only
  bool setConfig(CodecConfig cfg) {
    this->codec_cfg = cfg;
    return driver->setConfig(cfg);
  }

  bool end(void) {
    pins->end();
    is_active = false;
    return driver->end();
  }
  bool setMute(bool enable) { return driver->setMute(enable); }
  bool setMute(bool enable, int line) { 
    if (line == power_amp_line) setPAPower(!enable);
    return driver->setMute(enable, line); 
  }
  bool setVolume(int volume) {
    AD_LOGD("setVolume: %d", volume);
    // when we get the volume we make sure that we report the same value
    // w/o rounding issues 
    this->volume = volume; 
    return (is_active) ? driver->setVolume(volume) : false; 
  }
  int getVolume() { 
#if DRIVER_REPORT_DRIVER_VOLUME
    return driver->getVolume(); }
#else
    return volume >= 0 ? volume : driver->getVolume(); }
#endif
  DriverPins& getPins() { return *pins; }
  bool setPAPower(bool enable) { return driver->setPAPower(enable); }
  /// set volume for adc: this is only supported on some defined codecs
  bool setInputVolume(int volume) {return driver->setInputVolume(volume);}

  AudioDriver* getDriver(){
    return driver;
  }

protected:
  DriverPins* pins;
  CodecConfig codec_cfg;
  AudioDriver* driver = nullptr;
  int power_amp_line = ES8388_PA_LINE;
  int volume = -1;
  bool is_active = false;
};

// -- Boards
/// @ingroup audio_driver
static AudioBoard AudioKitEs8388V1{AudioDriverES8388, PinsAudioKitEs8388v1};
/// @ingroup audio_driver
static AudioBoard AudioKitEs8388V2{AudioDriverES8388, PinsAudioKitEs8388v2};
/// @ingroup audio_driver
static AudioBoard AudioKitAC101{AudioDriverAC101, PinsAudioKitAC101};
/// @ingroup audio_driver
static AudioBoard LyratV43{AudioDriverES8388, PinsLyrat43};
/// @ingroup audio_driver
static AudioBoard LyratV42{AudioDriverES8388, PinsLyrat42};
/// @ingroup audio_driver
static AudioBoard LyratMini{AudioDriverLyratMini, PinsLyratMini};
/// @ingroup audio_driver
static AudioBoard NoBoard{NoDriver, NoPins};
/// @ingroup audio_driver
static AudioBoard GenericWM8960{AudioDriverWM8960, NoPins};
/// @ingroup audio_driver
static AudioBoard GenericCS43l22{AudioDriverCS43l22, NoPins};
#if defined(ARDUINO_GENERIC_F411VETX)
/// @ingroup audio_driver
static AudioBoard STM32F411Disco{AudioDriverCS43l22, PinsSTM32F411Disco};
#endif

}