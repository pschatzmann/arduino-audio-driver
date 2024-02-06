#pragma once
#include "Driver.h"
#include "Pins.h"

namespace audio_driver {

/**
 * @brief Defitintion for audio board pins and an audio driver
 * @ingroup audio_driver
 */
class AudioBoard {
public:
  AudioBoard(Pins pins, AudioDriver *driver) {
    this->pins = pins;
    this->driver =driver;
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
  Pins getPins() { return pins; }
  bool setPAPower(bool enable) { return driver->setPAPower(enable); }
  /// set volume for adc: this is only supported on some defined codecs
  bool setInputVolume(int volume) {return driver->setInputVolume(volume);}

  AudioDriver* getDriver(){
    return driver;
  }

protected:
  Pins pins;
  CodecConfig codec_cfg;
  AudioDriver* driver = nullptr;
};

// -- Drivers
/// @ingroup audio_driver
AudioDriverES8388Class AudioDriverES8388;
/// @ingroup audio_driver
AudioDriverAC101Class AudioDriverAC101;
/// @ingroup audio_driver
AudioDriverCS43l22Class AudioDriverCS43l22;
/// @ingroup audio_driver
AudioDriverES8311Class AudioDriverES8311;
/// @ingroup audio_driver
AudioDriverES7243Class AudioDriverES7243;
/// @ingroup audio_driver
AudioDriverLyratMiniClass AudioDriverLyratMini;

// -- Pins 
/// @ingroup audio_driver
PinsLyrat43Class PinsLyrat43;
/// @ingroup audio_driver
PinsLyrat42Class PinsLyrat42;
/// @ingroup audio_driver
PinsLyratMiniClass PinsLyratMini;
/// @ingroup audio_driver
PinsAudioKitEs8388v1Class PinsAudioKitEs8388v1;
/// @ingroup audio_driver
PinsAudioKitEs8388v2Class PinsAudioKitEs8388v2;
/// @ingroup audio_driver
PinsAudioKitAC101Class PinsAudioKitAC101;

// -- Boards
/// @ingroup audio_driver
AudioBoard AudioKitEs8388V1{PinsAudioKitEs8388v1, &AudioDriverES8388};
/// @ingroup audio_driver
AudioBoard AudioKitEs8388V2{PinsAudioKitEs8388v2, &AudioDriverES8388};
/// @ingroup audio_driver
AudioBoard AudioKitAC101{PinsAudioKitAC101, &AudioDriverAC101};
/// @ingroup audio_driver
AudioBoard LyratV43{PinsLyrat43, &AudioDriverES8388};
/// @ingroup audio_driver
AudioBoard LyratV42{PinsLyrat42, &AudioDriverES8388};
/// @ingroup audio_driver
AudioBoard LyratMini{PinsLyratMini, &AudioDriverLyratMini};

}