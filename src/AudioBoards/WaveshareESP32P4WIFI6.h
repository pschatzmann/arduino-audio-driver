#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for the Waveshare ESP32-P4-WIFI6 (ESP32-P4 + ES8311)
 *
 * Pin map from Waveshare's esp32_p4_platform BSP
 * (bsp/esp32_p4_platform.h), confirmed on real hardware: speaker playback
 * and microphone recording both work. The speaker amplifier is enabled
 * via GPIO53 (active-high). The board must be built with
 * ChipVariant=postv3 (v3.x ESP32-P4).
 *
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsWaveshareESP32P4WIFI6Class : public DriverDeviceInfo {
 public:
  PinsWaveshareESP32P4WIFI6Class() {
    // add i2c codec pins: scl, sda
    addI2C(PinFunction::CODEC, 8, 7);
    // add i2s pins: mclk, bck, ws, data_out, data_in
    addI2S(PinFunction::CODEC, 13, 12, 10, 9, 11);

    // speaker amplifier enable
    addPin(PinFunction::PA, 53, PinLogic::Output);
  }
};

/// @ingroup audio_driver
static PinsWaveshareESP32P4WIFI6Class PinsWaveshareESP32P4WIFI6;
/// @ingroup audio_driver
static AudioBoard WaveshareESP32P4WIFI6{AudioDriverES8311,
                                        PinsWaveshareESP32P4WIFI6};

}  // namespace audio_driver
