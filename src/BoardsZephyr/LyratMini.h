#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for Lyrat Mini - use the PinsLyratMini object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyratMiniClass : public DriverDeviceInfoZephyr {
 public:
  PinsLyratMiniClass() {
    // sd pins: CLK, MISO, MOSI, CS: the SD is not working, so this is commented
    // out addSPI(ESP32PinsSD); add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));
    addI2S(PinFunction::CODEC_ADC, DEVICE_DT_GET(DT_ALIAS(i2s_2)));

    addPin(PinFunction::HEADPHONE_DETECT, GPIO_DT_SPEC_GET(DT_ALIAS(pin_headphone_detect), gpios), PinLogic::InputActiveHigh);
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(pin_pa), gpios), PinLogic::Output);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(pin_led_1), gpios), PinLogic::Output, 1);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(pin_led_2), gpios), PinLogic::Output, 2);
    //addPin(PinFunction::MCLK_SOURCE, 0, PinLogic::Inactive);
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_1), gpios), PinLogic::Input, 0);
  }


  void setRange(int value) { range = value; }

 protected:
  // analog values for rec, mute, play, set, vol-, vol+
  int analog_values[6]{2802, 2270, 1754, 1284, 827, 304};
  int range = LYRAT_MINI_RANGE;

  bool inRange(int in, int toBe) {
    return in >= (toBe - range) && in <= (toBe + range);
  }
};

/// @ingroup audio_driver
static PinsLyratMiniClass PinsLyratMini;
/// @ingroup audio_driver
static AudioBoard LyratMini{AudioDriverLyratMini, PinsLyratMini};

}  // namespace audio_driver