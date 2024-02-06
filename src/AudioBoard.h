#pragma once
#include "Driver.h"
#include "DriverPins.h"

namespace audio_driver {

/**
 * @brief Defitintion for audio board pins and an audio driver
 * @ingroup audio_driver
 */
class AudioBoard {
public:
  AudioBoard(AudioDriver *driver, DriverPins pins=NoPins) {
    this->pins = pins;
    this->driver = driver;
  }

  bool begin(){
    return pins.begin() && driver->begin(codec_cfg, pins);
  }

  bool begin(CodecConfig cfg) {
    this->codec_cfg = cfg;
    return begin();
  }
  bool end(void) {
    pins.end();
    return driver->end();
  }
  bool setMute(bool enable) { return driver->setMute(enable); }
  bool setVolume(int volume) { return driver->setVolume(volume); }
  int getVolume() { return driver->getVolume(); }
  DriverPins& getPins() { return pins; }
  bool setPAPower(bool enable) { return driver->setPAPower(enable); }
  /// set volume for adc: this is only supported on some defined codecs
  bool setInputVolume(int volume) {return driver->setInputVolume(volume);}

  AudioDriver* getDriver(){
    return driver;
  }

protected:
  DriverPins pins;
  CodecConfig codec_cfg;
  AudioDriver* driver = nullptr;
};

// -- Boards
/// @ingroup audio_driver
static AudioBoard AudioKitEs8388V1{&AudioDriverES8388, PinsAudioKitEs8388v1};
/// @ingroup audio_driver
static AudioBoard AudioKitEs8388V2{&AudioDriverES8388, PinsAudioKitEs8388v2};
/// @ingroup audio_driver
static AudioBoard AudioKitAC101{&AudioDriverAC101, PinsAudioKitAC101};
/// @ingroup audio_driver
static AudioBoard LyratV43{&AudioDriverES8388, PinsLyrat43};
/// @ingroup audio_driver
static AudioBoard LyratV42{&AudioDriverES8388, PinsLyrat42};
/// @ingroup audio_driver
static AudioBoard LyratMini{&AudioDriverLyratMini, PinsLyratMini};


}