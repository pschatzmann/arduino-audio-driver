#if defined(ESP32_CMAKE)
#include <driver/gpio.h>
#include <esp_err.h>

namespace audio_driver {

/**
 * @class GPIO
 * @brief ESP-IDF implementation of digital GPIO abstraction.
 */
class GPIO : public API_GPIO {
public:
  GPIO() = default;

  bool begin(IDriverDeviceInfo &pins) {
    // No global init required in ESP-IDF for GPIO
    return true;
  }

  void end() {
    // Optional: reset GPIOs if needed
  }

  void pinMode(GpioPin pin, int mode) {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << pin);

    switch (mode) {
    case INPUT:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      io_conf.intr_type = GPIO_INTR_DISABLE;
      break;

    case INPUT_PULLUP:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      io_conf.intr_type = GPIO_INTR_DISABLE;
      break;

    case INPUT_PULLDOWN:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
      io_conf.intr_type = GPIO_INTR_DISABLE;
      break;

    case OUTPUT:
    default:
      io_conf.mode = GPIO_MODE_OUTPUT;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      io_conf.intr_type = GPIO_INTR_DISABLE;
      break;
    }

    gpio_config(&io_conf);
  }

  bool digitalWrite(GpioPin pin, bool value) {
    esp_err_t err = gpio_set_level((gpio_num_t)pin, value ? 1 : 0);
    return err == ESP_OK;
  }

  bool digitalRead(GpioPin pin) { return gpio_get_level((gpio_num_t)pin); }
  /// ADC via ESP-IDF requires separate channel/unit setup; not implemented here.
  int analogRead(ADCPin pin) { return -1; }
};

} // namespace audio_driver

#endif
