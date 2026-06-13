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
#if defined(IS_ZEPHYR)
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
#else
    // sd pins 14, 2, 15, 13
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 32, 33);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 27, 26, 25, 35);

    // add other pins
    addPin(PinFunction::KEY, 36, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 19, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 23, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 18, PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, 5, PinLogic::InputActiveLow, 6);
    addPin(PinFunction::LED, 22, PinLogic::Output, 0);
    addPin(PinFunction::LED, 19, PinLogic::Output, 1);
    addPin(PinFunction::HEADPHONE_DETECT, 5, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
#endif
};

/// @ingroup audio_driver
static PinsAudioKitAC101Class PinsAudioKitAC101;
/// @ingroup audio_driver
static AudioBoard AudioKitAC101{AudioDriverAC101, PinsAudioKitAC101};

}
