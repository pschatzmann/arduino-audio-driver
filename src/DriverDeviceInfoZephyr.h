
#pragma once
#ifdef __zephyr__
#include "Platforms/GPIOZephyr.h"
#include "Platforms/IDriverDeviceInfoZephyr.h"

namespace audio_driver {

/** @file */

/**
 * @brief All pins for i2s, spi, i2c and other pins
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class DriverDeviceInfoZephyr : public IDriverDeviceInfo {
 public:
  DriverDeviceInfoZephyr(const DriverDeviceInfoZephyr&) = delete;
  DriverDeviceInfoZephyr& operator=(const DriverDeviceInfoZephyr&) = delete;
  DriverDeviceInfoZephyr() = default;
  bool addI2S(InfoI2S info) {
    i2s.push_back(info);
    return true;
  }

  bool addI2S(PinFunction function, device dev_i2s, int port = 0) {
    InfoI2S info{function, dev_i2s, port};
    return addI2S(info);
  }

  /// Updates the I2S pin information using the function as key
  bool setI2S(InfoI2S pin) { return set<InfoI2S>(pin, i2s); }

  bool addSPI(InfoSPI pin) {
    if (getSPIPins(pin.function)) return false;
    spi.push_back(pin);
    return true;
  }

  bool addSPI(PinFunction function, device* dev_spi) {
    InfoSPI pin(function, dev_spi);
    return addSPI(pin);
  }

  /// Updates the SPI pin information using the function as key
  bool setSPI(InfoSPI info) { return set<InfoSPI>(info, spi); }

  bool addI2C(InfoI2C info) {
    i2c.push_back(info);
    return true;
  }

  bool addI2C(PinFunction function, device* dev_i2c, int address = -1,
              int port = 0, bool active = true) {
    InfoI2C info(function, dev_i2c, address, port, active);
    return addI2C(info);
  }
    bool addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port = -1,
              uint32_t frequency = 100000, i2c_bus_handle_t wire = DEFAULT_WIRE,
              bool active = true) {
    InfoI2C pin(function, scl, sda, port, frequency, wire, active);
    return addI2C(pin);
  }

  /// Updates the I2C pin information using the function as key
  bool setI2C(InfoI2C info) { return set<InfoI2C>(pin, info); }

  bool addPin(PinsFunction pin) {
    pins.push_back(pin);
    return true;
  }

  bool addPin(PinFunction function, GpioPin pinNo, PinLogic logic,
              int index = 0) {
    PinsFunction pin(function, pinNo, logic, index);
    return addPin(pin);
  }

  /// Updates an existing pin information using the function and index as key
  bool setPin(PinsFunction updatedPin) {
    for (PinsFunction& pin : pins) {
      if (pin.function == updatedPin.function &&
          pin.index == updatedPin.index) {
        pin = updatedPin;
        return true;
      }
    }
    return false;
  }

  /// Updates an existing pin active flag for the indicated gpio
  bool setPinActive(int gpioPin, bool active) {
    for (PinsFunction& pin : pins) {
      if (pin.pin == gpioPin) {
        pin.active = active;
        return true;
      }
    }
    return false;
  }

  /// Updates an existing pin active flag for the indicated gpio
  bool setPinActive(PinFunction func, int idx, bool active) {
    auto pin = getPin(func, idx);
    if (pin) {
      auto value = pin.value();
      value.active = active;
      // update pin
      return setPin(value);
    }
    return false;
  }

  /// Get pin information by function
  audio_driver_local::Optional<PinsFunction> getPin(PinFunction function,
                                                    int pos = 0) {
    for (PinsFunction& pin : pins) {
      if (pin.function == function && pin.index == pos) return pin;
    }
    return {};
  }

  /// Get pin information by pin ID
  audio_driver_local::Optional<PinsFunction> getPin(GpioPin pinId) {
    for (PinsFunction& pin : pins) {
      if (pin.pin == pinId) return pin;
    }
    return {};
  }

  GpioPin getPinID(PinFunction function, int pos = 0) {
    auto pin = getPin(function, pos);
    if (pin) return pin.value().pin;
    return -1;
  }

  /// Finds the I2C pin info with the help of the function
  audio_driver_local::Optional<InfoI2C> getI2CPins(
      PinFunction function) override {
    InfoI2C* pins = getPtr<InfoI2C>(function, i2c);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Finds the SPI pin info with the help of the function
  audio_driver_local::Optional<InfoSPI> getSPIPins(PinFunction function) {
    InfoSPI* pins = getPtr<InfoSPI>(function, spi);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Finds the I2S pin info with the help of the port
  audio_driver_local::Optional<InfoI2S> getI2SPins(int port) {
    for (InfoI2S& pins : i2s) {
      if (pins.port == port) return pins;
    }
    return {};
  }

  /// Finds the I2S pin info with the help of the function
  audio_driver_local::Optional<InfoI2S> getI2SPins(
      PinFunction function = PinFunction::CODEC) {
    InfoI2S* pins = getPtr<InfoI2S>(function, i2s);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Sets up all pins e.g. by calling pinMode()
  virtual bool begin() {
    AD_LOGD("DriverDeviceInfo::begin");

    // setup function pins
    setupPinMode();

    // setup spi
    bool result = true;
    for (auto& tmp : spi) {
      if (tmp.function == PinFunction::SD) {
        if (sd_active)
          result &= tmp.begin();
        else
          result &= tmp.begin();
      }
    }

    // setup i2c
    for (auto& tmp : i2c) {
      result &= tmp.begin();
    }
    return result;
  }

  void end() {
    // close spi
    for (auto& tmp : spi) {
      // close SD only when sd_active
      if (tmp.function == PinFunction::SD) {
        if (sd_active) tmp.end();
      } else {
        tmp.end();
      }
    }
    // close i2c
    for (auto& tmp : i2c) {
      AD_LOGD("DriverDeviceInfo::begin::I2C::end");
      tmp.end();
    }
  }

  /// Defines if SPI for SD should be started (by default true)
  void setSPIActiveForSD(bool active) { 
    AD_LOGD("DriverDeviceInfo::setSPIActiveForSD: %d", active);
    sd_active = active; }

  /// Check if SPI for SD should be started automatically
  bool isSPIActiveForSD() { return sd_active; }

  /// Defines if SPI for SD should be started (by default true)
  void setSDMMCActive(bool active) {
    AD_LOGD("DriverDeviceInfo::setSDMMCActive: %d", active);
    sdmmc_active = active;
    sd_active = false;
  }

  /// Check if SDMMC has been activated
  bool isSDMMCActive() { return sdmmc_active; }

  /// Returns true if some function pins have been defined
  bool hasPins() { return !pins.empty(); }

  /// returns true if pressed
  virtual bool isKeyPressed(uint8_t key) {
    auto pin_opt = getPin(PinFunction::KEY, key);
    if (!pin_opt) return false;
    auto pin = pin_opt.value();
    bool value = gpio.digitalRead(pin.pin);
    return pin.pin_logic == PinLogic::InputActiveLow ? !value : value;
  }

  API_GPIO& getGPIO() { return gpio; }

 protected:
  audio_driver_local::Vector<InfoI2S> i2s{0};
  audio_driver_local::Vector<InfoSPI> spi{0};
  audio_driver_local::Vector<InfoI2C> i2c{0};
  audio_driver_local::Vector<PinsFunction> pins{0};
  GPIOExt gpio;  // standard Arduino GPIO
  bool sd_active = false;
  bool sdmmc_active = false;

  template <typename T>
  T* getPtr(PinFunction function, audio_driver_local::Vector<T>& vect) {
    for (auto& pins : vect) {
      if (pins.function == function) return &pins;
    }
    return nullptr;
  }

  template <typename T>
  bool set(T pin, audio_driver_local::Vector<T>& vect) {
    T* pins = getPtr<T>(pin.function, vect);
    if (pins == nullptr) return false;
    *pins = pin;
    return true;
  }

  void setupPinMode() {
    gpio.begin(*this);
    AD_LOGD("DriverDeviceInfo::setupPinMode");
    // setup pins
    for (auto& tmp : pins) {
      if (tmp.pin != -1) {
        if (!hasConflict(tmp.pin)) {
          AD_LOGD("pinMode for %d", tmp.pin);
          switch (tmp.pin_logic) {
            case PinLogic::InputActiveHigh:
              gpio.pinMode(tmp.pin, INPUT);
              break;
            case PinLogic::InputActiveLow:
              gpio.pinMode(tmp.pin, INPUT_PULLUP);
              break;
            case PinLogic::Input:
              gpio.pinMode(tmp.pin, INPUT);
              break;
            case PinLogic::Output:
              gpio.pinMode(tmp.pin, OUTPUT);
              break;
            default:
              // do nothing
              break;
          }
        } else {
          AD_LOGW("Pin '%d' not set up because of conflict", tmp.pin);
          tmp.active = false;
        }
      } else {
        AD_LOGD("Pin is -1");
      }
    }
  }

  bool hasConflict(int pin) {
    return false;
  }

  bool hasSPIConflict(int pin) {
    return false;
  }

  bool hasI2CConflict(int pin) {
    return false;
  }
};

// Support for DriverDeviceInfo in Zephyr
using DriverDeviceInfo = DriverDeviceInfoZephyr;


// -- Pins
/**
 * @brief Pins need to be set up in the sketch
 * @ingroup audio_driver
 */
static DriverDeviceInfoZephyr NoPins;

}  // namespace audio_driver

#endif