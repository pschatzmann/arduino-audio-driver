#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for M5stack Atom EchoS3R
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsM5stackAtomEchoS3RClass : public DriverDeviceInfoZephyr {
 public:
  PinsM5stackAtomEchoS3RClass() {
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));

    // add other pins
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(gpio_pa), gpios), PinLogic::Output);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_1), gpios), PinLogic::InputActiveLow, 1);
  }
};

/// @ingroup audio_driver
static PinsM5stackAtomEchoS3RClass PinsM5stackAtomEchoS3R;
/// @ingroup audio_driver
static AudioBoard M5stackAtomEchoS3R{AudioDriverES8311, PinsM5stackAtomEchoS3R};

}  // namespace audio_driver