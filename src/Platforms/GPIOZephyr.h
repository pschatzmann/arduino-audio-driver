#pragma once
#include <stdint.h>
#include "API_GPIO.h"
#include <zephyr/drivers/gpio.h>
#include "IDriverDeviceInfo.h"


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
  GPIOZephyr() = default;

  bool begin(IDriverDeviceInfo& pins) { return true; }

  void end() {}

  void pinMode(GpioPin pin, int mode) {
    if (pin == nullptr || !gpio_is_ready_dt(pin)) {
      AD_LOGE("GPIO pin not ready");
      return;
    }

    gpio_flags_t flags;
    switch (mode) {
      case OUTPUT:
        flags = GPIO_OUTPUT;
        break;
      case INPUT:
        flags = GPIO_INPUT;
        break;
      case INPUT_PULLUP:
        flags = GPIO_INPUT | GPIO_PULL_UP;
        break;
      // case INPUT_PULLDOWN:
      //   flags = GPIO_INPUT | GPIO_PULL_DOWN;
      //   break;
      default:
        flags = GPIO_OUTPUT;
        break;
    }

    int rc = gpio_pin_configure_dt(pin, flags);
    if (rc != 0) {
      AD_LOGE("Failed to configure GPIO pin: %d", rc);
    }
  }

  bool digitalWrite(GpioPin pin, bool value) {
    if (pin == nullptr || !gpio_is_ready_dt(pin)) {
      AD_LOGE("GPIO pin not ready");
      return false;
    }

    int rc = gpio_pin_set_dt(pin, value ? 1 : 0);
    if (rc != 0) {
      AD_LOGE("Failed to write GPIO pin: %d", rc);
      return false;
    }
    return true;
  }

  bool digitalRead(GpioPin pin) {
    if (pin == nullptr || !gpio_is_ready_dt(pin)) {
      AD_LOGE("GPIO pin not ready");
      return false;
    }

    int rc = gpio_pin_get_dt(pin);
    if (rc < 0) {
      AD_LOGE("Failed to read GPIO pin: %d", rc);
      return false;
    }
    return rc != 0;
  }
};

using GPIO = GPIOZephyr;

}  // namespace audio_driver