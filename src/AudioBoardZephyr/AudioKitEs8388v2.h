#pragma once
#include "AudioBoard.h"

namespace audio_driver {
/**
 * @brief Pins for alt Es8388 AudioDriver - use the PinsAudioKitEs8388v2 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsAudioKitEs8388v2Class : public DriverDeviceInfo {
 public:
  PinsAudioKitEs8388v2Class() {
    // add i2c
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));

    // add other pins
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_1), gpios), PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_2), gpios), PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_3), gpios), PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_4), gpios), PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_5), gpios), PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_6), gpios), PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, GPIO_DT_SPEC_GET(DT_ALIAS(auxin_detect), gpios), PinLogic::InputActiveLow);
    addPin(PinFunction::HEADPHONE_DETECT, GPIO_DT_SPEC_GET(DT_ALIAS(headphone_detect), gpios), PinLogic::InputActiveLow);
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(pa), gpios), PinLogic::Output);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led), gpios), PinLogic::Output);
  }
};

/// @ingroup audio_driver
static PinsAudioKitEs8388v2Class PinsAudioKitEs8388v2;
/// @ingroup audio_driver
static AudioBoard AudioKitEs8388V2{AudioDriverES8388, PinsAudioKitEs8388v2};

}
