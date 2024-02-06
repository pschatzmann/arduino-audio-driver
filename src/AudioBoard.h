#pragma once
#include "Driver.h"
#include "Pins.h"

namespace audio_driver {

/// @ingroup audio_driver @brief Pins need to be set up in the sketch
static DriverPins NoPins;

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

// -- Pins 
/// @ingroup audio_driver
static PinsLyrat43Class PinsLyrat43;
/// @ingroup audio_driver
static PinsLyrat42Class PinsLyrat42;
/// @ingroup audio_driver
static PinsLyratMiniClass PinsLyratMini;
/// @ingroup audio_driver
static PinsAudioKitEs8388v1Class PinsAudioKitEs8388v1;
/// @ingroup audio_driver
static PinsAudioKitEs8388v2Class PinsAudioKitEs8388v2;
/// @ingroup audio_driver
static PinsAudioKitAC101Class PinsAudioKitAC101;

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