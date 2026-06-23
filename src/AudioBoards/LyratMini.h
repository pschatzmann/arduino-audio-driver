#pragma once
#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins for Lyrat Mini - use the PinsLyratMini object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyratMiniClass : public DriverDeviceInfo {
 public:
  PinsLyratMiniClass() {
#if defined(IS_ZEPHYR)
    // sd pins: CLK, MISO, MOSI, CS: the SD is not working, so this is commented
    // out addSPI(ESP32PinsSD); add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));
    addI2S(PinFunction::CODEC_ADC, DEVICE_DT_GET(DT_ALIAS(i2s_2)));

    addPin(PinFunction::HEADPHONE_DETECT, GPIO_DT_SPEC_GET(DT_ALIAS(headphone_detect), gpios), PinLogic::InputActiveHigh);
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(pa), gpios), PinLogic::Output);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios), PinLogic::Output, 1);
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios), PinLogic::Output, 2);
    //addPin(PinFunction::MCLK_SOURCE, 0, PinLogic::Inactive);
    // adc pin must be defuubed for readubg key values

#else
    // sd pins: CLK, MISO, MOSI, CS: the SD is not working, so this is commented
    // out addSPI(ESP32PinsSD); add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 23, 18);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 5, 25, 26, 35, 0);
    addI2S(PinFunction::CODEC_ADC, 0, 32, 33, -1, 36, 1);

    addPin(PinFunction::HEADPHONE_DETECT, 19, PinLogic::InputActiveHigh);
    addPin(PinFunction::PA, 21, PinLogic::Output);
    addPin(PinFunction::LED, 22, PinLogic::Output, 1);
    addPin(PinFunction::LED, 27, PinLogic::Output, 2);
    addPin(PinFunction::MCLK_SOURCE, 0, PinLogic::Inactive);
    // adc pin
    addPin(PinFunction::KEY, 39, PinLogic::Input, 0);
#endif
  }


  void setRange(int value) { range = value; }


#if defined(IS_ZEPHYR)

  /// In Zephyr we we must use the ADC to read the key values, so we implement the logic here
  bool isKeyPressed(uint8_t key) override {
    adc_dt_spec adc = DEVICE_DT_GET(DT_ALIAS(adc_pins));
    int analog_value = gpio.analogRead(key_adc);
    return inRange(analog_value, analog_values[key]);
  }

#else

  // Key is key index 0-5 for rec, mute, play, set, vol-, vol+
  bool isKeyPressed(uint8_t key) override {
    int pin = getPinID(PinFunction::KEY);
    if (pin == GPIO_UNDEFINED) {
      AD_LOGE("PinFunction::KEY not defined");
      return false;
    }
    int analog_value = gpio.analogRead(pin);
    return inRange(analog_value, analog_values[key]);
  }

#endif
 
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
