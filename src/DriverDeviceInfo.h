
#pragma once
#ifdef __zephyr__
#include "DriverDeviceInfoZephyr.h"
#else
#include "Platforms/API_Delay.h"
#include "Platforms/GPIO.h"
#include "Platforms/IDriverDeviceInfo.h"

namespace audio_driver {

/** @file */

/**
 * @brief All pins for i2s, spi, i2c and other pins
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class DriverDeviceInfo : public IDriverDeviceInfo {
 public:
  DriverDeviceInfo(const DriverDeviceInfo&) = delete;
  DriverDeviceInfo& operator=(const DriverDeviceInfo&) = delete;
  DriverDeviceInfo() = default;

  bool addI2S(InfoI2S pin) {
    if (getI2SPins(pin.function)) return false;
    i2s.push_back(pin);
    return true;
  }

  bool addI2S(PinFunction function, GpioPin mclk, GpioPin bck, GpioPin ws,
              GpioPin data_out, GpioPin data_in = -1, int port = 0) {
    InfoI2S pin{function, mclk, bck, ws, data_out, data_in, port};
    return addI2S(pin);
  }

  /// Updates the I2S pin information using the function as key
  bool setI2S(InfoI2S pin) { return set<InfoI2S>(pin, i2s); }

  bool addSPI(InfoSPI pin) {
    if (getSPIPins(pin.function)) return false;
    spi.push_back(pin);
    return true;
  }

  bool addSPI(PinFunction function, GpioPin clk, GpioPin miso, GpioPin mosi,
              GpioPin cs, SPIClass& spi) {
    InfoSPI pin(function, clk, miso, mosi, cs, spi);
    return addSPI(pin);
  }

  /// Updates the SPI pin information using the function as key
  bool setSPI(InfoSPI pin) { return set<InfoSPI>(pin, spi); }

  bool addI2C(InfoI2C pin) {
    if (getI2CPins(pin.function)) return false;
    i2c.push_back(pin);
    return true;
  }

#ifdef ARDUINO
  /// Just define your initialzed wire object
  bool addI2C(PinFunction function, TwoWire& wire, bool setActive = false) {
    PinsI2C pin(function, -1, -1, -1, -1, &wire, setActive);
    return addI2C(pin);
  }

  bool addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port,
              uint32_t frequency, TwoWire& wire, bool active = true) {
    PinsI2C pin(function, scl, sda, port, frequency, &wire, active);
    return addI2C(pin);
  }
#endif

  bool addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port = -1,
              uint32_t frequency = 100000, i2c_bus_handle_t wire = DEFAULT_WIRE,
              bool active = true) {
    InfoI2C pin(function, scl, sda, port, frequency, wire, active);
    return addI2C(pin);
  }

  /// Updates the I2C pin information using the function as key
  bool setI2C(InfoI2C pin) { return set<InfoI2C>(pin, i2c); }

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
    return hasSPIConflict(pin) || hasI2CConflict(pin);
  }

  bool hasSPIConflict(int pin) {
    if (sd_active) {
      auto sd = getSPIPins(PinFunction::SD);
      if (!sd) return false;
      InfoSPI spi = sd.value();
      return spi.cs == pin || spi.clk == pin || spi.miso == pin ||
             spi.mosi == pin;
    }
    return false;
  }

  bool hasI2CConflict(int pin) {
    for (auto& i2c_entry : i2c) {
      if (i2c_entry.scl == pin || i2c_entry.sda == pin) {
        return true;
      }
    }
    return false;
  }
};

using DriverPins = DriverDeviceInfo;

/**
 * @brief Support for Touch
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class DriverTouchClass : public DriverDeviceInfo {
  bool isKeyPressed(uint8_t key) override {
    bool result = false;
#if defined(ESP32) && defined(ARDUINO)
#if SOC_TOUCH_SENSOR_SUPPORTED
    auto pin_opt = getPin(PinFunction::KEY, key);
    if (!pin_opt) return false;
    auto pin = pin_opt.value();
    int value = touchRead(pin.pin);
    result = value <= touch_limit;
    if (result) {
      // retry to confirm reading
      value = touchRead(pin.pin);
      result = value <= touch_limit;
    }
#endif
#endif
    return result;
  }
  void setTouchLimit(int limit) { touch_limit = limit; }

 protected:
  int touch_limit = TOUCH_LIMIT;
};

/**
 * @brief Pins for Lyrat 4.3 - use the PinsLyrat43 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyrat43Class : public DriverTouchClass {
 public:
  PinsLyrat43Class() {
    // sd pins 14, 2, 15, 13
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 23, 18);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 5, 25, 26, 35);

    // add other pins
    addPin(PinFunction::KEY, 36, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 39, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 33, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 32, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, 27, PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, 12, PinLogic::InputActiveLow);
    addPin(PinFunction::HEADPHONE_DETECT, 19, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
    addPin(PinFunction::LED, 22, PinLogic::Output);
  }
};

/**
 * @brief Pins for Lyrat 4.2 - use the PinsLyrat42 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyrat42Class : public DriverTouchClass {
 public:
  PinsLyrat42Class() {
    // sd pins 14, 2, 15, 13
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 23, 18);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 5, 25, 26, 35);

    // add other pins
    addPin(PinFunction::KEY, 36, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 39, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 33, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 32, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, 27, PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, 12, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
    addPin(PinFunction::LED, 22, PinLogic::Output, 1);
    addPin(PinFunction::LED, 19, PinLogic::Output, 2);
  }
};

/**
 * @brief Pins for Lyrat Mini - use the PinsLyratMini object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyratMiniClass : public DriverDeviceInfo {
 public:
  PinsLyratMiniClass() {
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
    addPin(PinFunction::KEY, 39, PinLogic::Input, 0);
  }

#ifdef ARDUINO
  /// When the button is released we might get some missreadings: so we read
  /// twice to guarantee a stable result
  bool isKeyPressed(uint8_t key) override {
    if (key > 5) return false;
    int value = analogRead(39);
    bool result = inRange(value, analog_values[key]);
    delayMs(LYRAT_MINI_DELAY_MS);
    int value1 = analogRead(39);
    bool result1 = inRange(value, analog_values[key]);
    result = result && result1;
    AD_LOGD("values: %d,%d for key: %d -> %s", value, value1, (int)key,
            result ? "true" : "false");
    return result;
  }
#endif

  void setRange(int value) { range = value; }

 protected:
  // analog values for rec, mute, play, set, vol-, vol+
  int analog_values[6]{2802, 2270, 1754, 1284, 827, 304};
  int range = LYRAT_MINI_RANGE;

  bool inRange(int in, int toBe) {
    return in >= (toBe - range) && in <= (toBe + range);
  }
};

/**
 * @brief Pins for Es8388 AudioDriver - use the PinsAudioKitEs8388v1 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsAudioKitEs8388v1Class : public DriverDeviceInfo {
 public:
  PinsAudioKitEs8388v1Class() {
    // sd pins 14, 2, 15, 13
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 32, 33);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 27, 25, 26, 35);

    // add other pins
    addPin(PinFunction::KEY, 36, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 19, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 23, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 18, PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, 5, PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, 12, PinLogic::InputActiveLow);
    addPin(PinFunction::HEADPHONE_DETECT, 39, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
    addPin(PinFunction::LED, 22, PinLogic::Output);
  }
};

/**
 * @brief Pins for alt Es8388 AudioDriver - use the PinsAudioKitEs8388v2 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsAudioKitEs8388v2Class : public DriverDeviceInfo {
 public:
  PinsAudioKitEs8388v2Class() {
    // sd pins 14, 2, 15, 13
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 23, 18);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 5, 25, 26, 35);

    // add other pins
    addPin(PinFunction::KEY, 36, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 19, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 23, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 18, PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, 5, PinLogic::InputActiveLow, 6);
    addPin(PinFunction::AUXIN_DETECT, 12, PinLogic::InputActiveLow);
    addPin(PinFunction::HEADPHONE_DETECT, 39, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
    addPin(PinFunction::LED, 22, PinLogic::Output);
  }
};

/**
 * @brief Pins for alt AC101 AudioDriver - use the PinsAudioKitAC101 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsAudioKitAC101Class : public DriverDeviceInfo {
 public:
  PinsAudioKitAC101Class() {
    // sd pins 14, 2, 15, 13
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 32, 33);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 27, 26, 25, 35);

    // add other pins
    addPin(PinFunction::KEY, 36, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 13, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 19, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 23, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 18, PinLogic::InputActiveLow, 5);
    addPin(PinFunction::KEY, 5, PinLogic::InputActiveLow, 6);
    addPin(PinFunction::LED, 22, PinLogic::Output, 0);
    addPin(PinFunction::LED, 19, PinLogic::Output, 1);
    addPin(PinFunction::HEADPHONE_DETECT, 5, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
  }
};

/**
 * @brief Pins for M5stack Atom EchoS3R
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsM5stackAtomEchoS3RClass : public DriverDeviceInfo {
 public:
  PinsM5stackAtomEchoS3RClass() {
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 0, 45, 0x18);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 11, 17, 3, 48, 4);

    // add other pins
    addPin(PinFunction::PA, 18, PinLogic::Output);
    addPin(PinFunction::KEY, 41, PinLogic::InputActiveLow, 1);
  }
};


// -- Pins
/**
 * @brief Pins need to be set up in the sketch
 * @ingroup audio_driver
 */
static DriverDeviceInfo NoPins;
/// @ingroup audio_driver
static PinsLyrat43Class PinsLyrat43;
/// @ingroup audio_driver
static PinsLyrat42Class PinsLyrat42;
/// @ingroup audio_driver
static PinsLyratMiniClass PinsLyratMini;
/// @ingroup audio_driver
static PinsAudioKitEs8388v1Class PinsAudioKitEs8388v1;
/// @ingroup audio_driver
static PinsAudioKitEs8388v2Class PinsAudioKitEs8388v2;
/// @ingroup audio_driver
static PinsAudioKitAC101Class PinsAudioKitAC101;
/// @ingroup audio_driver
static PinsM5stackAtomEchoS3RClass PinsM5stackAtomEchoS3R;
}  // namespace audio_driver

#endif
