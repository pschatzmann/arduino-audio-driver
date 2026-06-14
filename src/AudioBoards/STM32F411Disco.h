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

#if defined(IS_ZEPHYR)
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2c_1)));
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, DEVICE_DT_GET(DT_ALIAS(i2s_1)));

        // add other pins
    addPin(PinFunction::KEY, GPIO_DT_SPEC_GET(DT_ALIAS(key1), gpios), PinLogic::Output);      // user button
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios), PinLogic::Output, 0);  // green
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios), PinLogic::Output, 1);   // red
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios), PinLogic::Output, 2);  // orange
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led4), gpios), PinLogic::Output, 3);  // red
    addPin(PinFunction::LED, GPIO_DT_SPEC_GET(DT_ALIAS(led5), gpios), PinLogic::Output, 4);  // blue
    addPin(PinFunction::PA, GPIO_DT_SPEC_GET(DT_ALIAS(pa), gpios), PinLogic::Output);  // reset pin (active high)

#else

    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, PB6, PB9);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, PC7, PC10, PA4, PC3, PC12);

    // add other pins
    addPin(PinFunction::KEY, PA0, PinLogic::Output);      // user button
    addPin(PinFunction::LED, PD12, PinLogic::Output, 0);  // green
    addPin(PinFunction::LED, PD5, PinLogic::Output, 1);   // red
    addPin(PinFunction::LED, PD13, PinLogic::Output, 2);  // orange
    addPin(PinFunction::LED, PD14, PinLogic::Output, 3);  // red
    addPin(PinFunction::LED, PD15, PinLogic::Output, 4);  // blue
    addPin(PinFunction::PA, PD4, PinLogic::Output);  // reset pin (active high)
    // addPin(PinFunction::CODEC_ADC, PC3, PinLogic::Input); // Microphone
#endif
  }
};

/// @ingroup audio_driver
static PinsSTM32F411DiscoClass PinsSTM32F411Disco;

/// @ingroup audio_driver
static AudioBoard STM32F411Disco{AudioDriverCS43l22, PinsSTM32F411Disco};

}  // namespace audio_driver
