#pragma once
#include "AudioBoard.h"

#define DEV(dev) GPIO_DT_SPEC_GET(DT_ALIAS(dev))
#define KEY(key) GPIO_DT_SPEC_GET(DT_ALIAS(key), gpios)

namespace audio_driver {

/**
 * @brief Pins for Es8388 AudioDriver - use the PinsAudioKitEs8388v1 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsAudioKitEs8388v1Class : public DriverDeviceInfoZephyr {
 public:
  PinsAudioKitEs8388v1Class() {
#if defined(IS_ZEPHYR)
    // add i2c
    addI2C(PinFunction::CODEC, DEV(i2c_1));
    // add i2s
    addI2S(PinFunction::CODEC, DEV(i2s_1));

    // add other pins
    addPin(PinFunction::KEY, KEY(key_1), PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, KEY(key_2), PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, KEY(key_3), PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, KEY(key_4), PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, KEY(key_5), PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, KEY(key_6), PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, KEY(auxin_detect), PinLogic::InputActiveLow);
    addPin(PinFunction::HEADPHONE_DETECT, KEY(headphone_detect), PinLogic::InputActiveLow);
    addPin(PinFunction::PA, KEY(pa), PinLogic::Output);
    addPin(PinFunction::LED, KEY(led), PinLogic::Output);
#else
#endif
  }
};

/// @ingroup audio_driver
static PinsAudioKitEs8388v1Class PinsAudioKitEs8388v1;
/// @ingroup audio_driver
static AudioBoard AudioKitEs8388V1{AudioDriverES8388, PinsAudioKitEs8388v1};

}  // namespace audio_driver$
