#pragma once
/**
 * @file STM32F411Disco.h
 * @brief Device definition for STM32F411 Discovery board
 */

#include "AudioBoard.h"

namespace audio_driver {

/**
 * @brief Pins the STM32F411Disco board using the CS43L22 codec
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsSTM32F411DiscoClass : public DriverDeviceInfo {
 public:
  PinsSTM32F411DiscoClass() {
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));

    // add other pins
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key_1, gpios), PinLogic::Output);      // user button
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led_green), gpios), PinLogic::Output, 0);  // green
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led_red), gpios), PinLogic::Output, 1);   // red
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led_orange), gpios), PinLogic::Output, 2);  // orange
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led_red), gpios), PinLogic::Output, 3);  // red
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led_blue), gpios), PinLogic::Output, 4);  // blue
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(gpio_pa), gpios), PinLogic::Output);  // reset pin (active high)
  }
};

/// @ingroup audio_driver
static PinsSTM32F411DiscoClass PinsSTM32F411Disco;

/// @ingroup audio_driver
static AudioBoard STM32F411Disco{AudioDriverCS43l22, PinsSTM32F411Disco};

}