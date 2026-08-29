#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for the GMIC HA-TOYMD audio module (ESP32-S3 + ES8311)
 *
 * A bare production audio module: one codec, one microphone, one speaker,
 * no screen and no display bus. The vendor does not publish a pin map; these
 * values were recovered by reading the GPIO matrix routing registers off the
 * running chip over its built-in USB-JTAG port, then confirmed by bringing the
 * codec up from scratch.
 *
 * Hardware: ESP32-S3 (QFN56) - 4 MB flash - 2 MB quad PSRAM - ES8311 codec at
 * 0x18 - Wi-Fi 2.4 GHz only - native USB serial/JTAG (VID 303A / PID 1001).
 *
 * @author Fanyao Zeng
 * @copyright GPLv3
 */
class PinsGmicHaToyMdClass : public DriverDeviceInfo {
 public:
  PinsGmicHaToyMdClass() {
    // add i2c codec pins: scl, sda
    addI2C(PinFunction::CODEC, 18, 17);
    // add i2s pins: mclk, bck, ws, data_out, data_in
    addI2S(PinFunction::CODEC, 16, 9, 45, 8, 10);

    // speaker amplifier enable
    addPin(PinFunction::PA, 48, PinLogic::Output);
    // three front buttons (observed, not vendor-confirmed)
    addPin(PinFunction::KEY, 12, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 14, PinLogic::InputActiveLow, 3);
  }
};

/// @ingroup audio_driver
static PinsGmicHaToyMdClass PinsGmicHaToyMd;
/// @ingroup audio_driver
static AudioBoard GmicHaToyMd{AudioDriverES8311, PinsGmicHaToyMd};

}  // namespace audio_driver
