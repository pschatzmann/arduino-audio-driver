#pragma once
#include <stdint.h>

#include "API_GPIO.h"

/**
 * @file GPIO.h
 * @brief Simple GPIO abstraction for digital input/output.
 */

namespace audio_driver {

/**
 * @class GPIO
 * @brief Abstraction for digital GPIO pin operations.
 */
class GPIOZephyr : public API_GPIO {
 public:
  /**
   * @brief Construct a new GPIO object.
   * @param pin The pin number or handle.
   */
  GPIOZephyr() = default;
  bool begin(IDriverDeviceInfo& pins) { return true; }
  void end() {}
  void pinMode(GpioPin pin, int mode) {  }
  bool digitalWrite(GpioPin pin, bool value) {
    
    return true;
  }
  bool digitalRead(GpioPin pin) { return false; }
};

using GPIO = GPIOZephyr;

}  // namespace audio_driver