#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for Lyrat 4.2 - use the PinsLyrat42 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyrat42Class : public DriverDeviceInfoZephyr {
 public:
  PinsLyrat42Class() {
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));

    // add other pins
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_1), gpios), PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_2), gpios), PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_3), gpios), PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_4), gpios), PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_5), gpios), PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_6), gpios), PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, GPIO_DT_SPEC_GET(DT_ALIAS(auxin_detect), gpios), PinLogic::InputActiveLow);
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(pa), gpios), PinLogic::Output);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios), PinLogic::Output, 1);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios), PinLogic::Output, 2);
  }
};

/// @ingroup audio_driver
static PinsLyrat42Class PinsLyrat42;
/// @ingroup audio_driver
static AudioBoard LyratV42{AudioDriverES8388H3, PinsLyrat42};

}  // namespace audio_driver
