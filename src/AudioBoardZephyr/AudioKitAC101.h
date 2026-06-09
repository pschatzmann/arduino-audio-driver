#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for alt AC101 AudioDriver - use the PinsAudioKitAC101 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsAudioKitAC101Class : public DriverDeviceInfoZephyr {
 public:
  PinsAudioKitAC101Class() {
    // add i2c codec pins: device, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c0)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c0)));

    // add other pins
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key1), gpios), PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key2), gpios), PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key3), gpios), PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key4), gpios), PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key5), gpios), PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key6), gpios), PinLogic::InputActiveLow, 6);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios), PinLogic::Output, 0);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios), PinLogic::Output, 1);
    addPin(PinFunction::HEADPHONE_DETECT, GPIO_DT_SPEC_GET(DT_ALIAS(headphone_detect), gpios), PinLogic::InputActiveLow);
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(pa), gpios), PinLogic::Output);
  }
};

/// @ingroup audio_driver
static PinsAudioKitAC101Class PinsAudioKitAC101;
/// @ingroup audio_driver
static AudioBoard AudioKitAC101{AudioDriverAC101, PinsAudioKitAC101};

}
