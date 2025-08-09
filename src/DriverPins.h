
#pragma once
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "Platforms/API_SPI.h"
#include "Platforms/Logger.h"
#include "Platforms/Optional.h"
#include "Platforms/Vector.h"

#ifdef ARDUINO
#include "Wire.h"
#define DEFAULT_WIRE &Wire
#else
#define DEFAULT_WIRE nullptr
#endif

#ifndef TOUCH_LIMIT
#define TOUCH_LIMIT 20
#endif

#ifndef LYRAT_MINI_RANGE
#define LYRAT_MINI_RANGE 5
#endif

#ifndef LYRAT_MINI_DELAY_MS
#define LYRAT_MINI_DELAY_MS 5
#endif

namespace audio_driver {

/** @file */

/**
 * @enum PinLogic
 * @brief input or output
 * @ingroup enumerations
 */

enum class PinLogic {
  InputActiveHigh,
  InputActiveLow,
  InputActiveTouch,
  Input,
  Output,
  Inactive,
};

/**
 * @enum PinFunction
 * @brief Pin Functions
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum class PinFunction {
  UNDEFINED = 0,
  HEADPHONE_DETECT,
  AUXIN_DETECT,
  PA,  // Power Amplifier
  POWER,
  LED,
  KEY,
  SD,
  CODEC,
  CODEC_ADC,
  LATCH,
  RESET,
  MCLK_SOURCE,
};

/**
 * @enum AudioDriverKey
 * @brief Key names
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum AudioDriverKey {
  KEY_REC = 0,
  KEY_MODE,
  KEY_PLAY,
  KEY_SET,
  KEY_VOLUME_DOWN,
  KEY_VOLUME_UP
};

/**
 * @brief I2S pins
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinsI2S {
  PinsI2S() = default;
  PinsI2S(PinFunction function, GpioPin mclk, GpioPin bck, GpioPin ws,
          GpioPin data_out, GpioPin data_in = -1, int port = 0) {
    this->function = function;
    this->mclk = mclk;
    this->bck = bck;
    this->ws = ws;
    this->data_out = data_out;
    this->data_in = data_in;
    this->port = port;
  }
  PinFunction function;
  GpioPin mclk;
  GpioPin bck;
  GpioPin ws;
  GpioPin data_out;
  GpioPin data_in;
  int port;  // port number
};

/**
 * @brief SPI pins: In Arduino we initialize the SPI, on other platform
 * we just provide the pin information
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinsSPI : public SPIConfig {
  PinsSPI() {
    this->clk = -1;
    this->miso = -1;
    this->mosi = -1;
    this->cs = -1;
    this->p_spi = &SPI;
  };
  PinsSPI(PinFunction function, GpioPin clk, GpioPin miso, GpioPin mosi,
          GpioPin cs, SPIClass &spi = SPI) {
    this->function = function;
    this->clk = clk;
    this->miso = miso;
    this->mosi = mosi;
    this->cs = cs;
    this->p_spi = &spi;
  }

  PinFunction function;
  bool set_active = true;
  bool pinsAvailable() { return clk != -1 && miso != -1 && mosi != -1; }
  operator bool() { return pinsAvailable(); }
  bool begin() {
    if (set_active) {
      AD_LOGD("PinsSPI::begin for %d", (int)function);
      spi_bus_create(this);
    } else {
      AD_LOGI("SPI, not active, MOSI, MISO, SCLK, SSEL not modified");
    }
    return true;
  }
  void end() {
    AD_LOGD("PinsSPI::end");
    spi_bus_delete(p_spi);
  }
};

/**
 * @brief Default SPI pins for ESP32 Lyrat, AudioDriver etc
 * CLK, MISO, MOSI, CS
 */

static PinsSPI ESP32PinsSD{PinFunction::SD, 14, 2, 15, 13, SPI};

/**
 * @brief I2C pins
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinsI2C : public I2CConfig {
  PinsI2C() {
    port = 0;
    address = -1;
    scl = -1;
    sda = -1;
    frequency = 100000;
    set_active = true;
    p_wire = DEFAULT_WIRE;
  };

  PinsI2C(PinFunction function, GpioPin scl, GpioPin sda, int address = -1,
          uint32_t frequency = 100000, i2c_bus_handle_t wire = DEFAULT_WIRE,
          bool active = true) {
    this->function = function;
    this->scl = scl;
    this->sda = sda;
    this->port = 0;
    this->frequency = frequency;
    this->p_wire = wire;
    this->set_active = active;
    this->address = address;
  }

  PinFunction function;
  bool set_active = true;
  bool pinsAvailable() { return scl != -1 && sda != -1 && frequency != 0; }
  operator bool() { return pinsAvailable(); }

  bool begin() {    
    if (set_active) {
      AD_LOGD("PinsI2C::begin for function %d on port %d", (int)function, port);
      return i2c_bus_create(this) == RESULT_OK;
    }
    return true;
  }
  void end() {
    if (set_active) i2c_bus_delete(p_wire);
  }
};

/**
 * @brief Pins for LED, Buttons, AMP etc
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinsFunction {
  PinsFunction() = default;
  PinsFunction(PinFunction function, GpioPin pin, PinLogic logic,
               int index = 0) {
    this->function = function;
    this->pin = pin;
    this->index = index;
    this->pin_logic = logic;
  }
  PinFunction function;
  int pin = -1;
  int index = 0;
  PinLogic pin_logic;
  bool active = true;  // false if pin conflict
};

/**
 * @brief All pins for i2s, spi, i2c and other pins
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class DriverPins {
 public:
  DriverPins(const DriverPins &) = delete;
  DriverPins &operator=(const DriverPins &) = delete;
  DriverPins() = default;

  bool addI2S(PinsI2S pin) {
    if (getI2SPins(pin.function)) return false;
    i2s.push_back(pin);
    return true;
  }

  bool addI2S(PinFunction function, GpioPin mclk, GpioPin bck, GpioPin ws,
              GpioPin data_out, GpioPin data_in = -1, int port = 0) {
    PinsI2S pin{function, mclk, bck, ws, data_out, data_in, port};
    return addI2S(pin);
  }

  /// Updates the I2S pin information using the function as key
  bool setI2S(PinsI2S pin) { return set<PinsI2S>(pin, i2s); }

  bool addSPI(PinsSPI pin) {
    if (getSPIPins(pin.function)) return false;
    spi.push_back(pin);
    return true;
  }

  bool addSPI(PinFunction function, GpioPin clk, GpioPin miso, GpioPin mosi,
              GpioPin cs, SPIClass &spi) {
    PinsSPI pin(function, clk, miso, mosi, cs, spi);
    return addSPI(pin);
  }

  /// Updates the SPI pin information using the function as key
  bool setSPI(PinsSPI pin) { return set<PinsSPI>(pin, spi); }

  bool addI2C(PinsI2C pin) {
    if (getI2CPins(pin.function)) return false;
    i2c.push_back(pin);
    return true;
  }

#ifdef ARDUINO
  /// Just define your initialzed wire object
  bool addI2C(PinFunction function, TwoWire &wire, bool setActive = false) {
    PinsI2C pin(function, -1, -1, -1, -1, &wire, setActive);
    return addI2C(pin);
  }

  bool addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port,
              uint32_t frequency, TwoWire &wire,
              bool active = true) {
    PinsI2C pin(function, scl, sda, port, frequency, &wire, active);
    return addI2C(pin);
  }
#endif

  bool addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port = -1,
              uint32_t frequency = 100000, i2c_bus_handle_t wire=DEFAULT_WIRE, bool active = true) {
    PinsI2C pin(function, scl, sda, port, frequency, wire, active);
    return addI2C(pin);
  }

  /// Updates the I2C pin information using the function as key
  bool setI2C(PinsI2C pin) { return set<PinsI2C>(pin, i2c); }

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
  bool setPin(PinsFunction updatedPin){
    for (PinsFunction &pin : pins) {
      if (pin.function == updatedPin.function && pin.index == updatedPin.index){
        pin = updatedPin;
        return true;
      }
    }
    return false;
  }

  /// Updates an existing pin active flag for the indicated gpio
  bool setPinActive(int gpioPin, bool active) {
    for (PinsFunction &pin : pins) {
      if (pin.pin == gpioPin){
        pin.active = active;
        return true;
      }
    }
    return false;
  }

  /// Updates an existing pin active flag for the indicated gpio
  bool setPinActive(PinFunction func, int idx, bool active) {
    auto pin = getPin(func, idx);
    if (pin){
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
    for (PinsFunction &pin : pins) {
      if (pin.function == function && pin.index == pos) return pin;
    }
    return {};
  }

  /// Get pin information by pin ID
  audio_driver_local::Optional<PinsFunction> getPin(GpioPin pinId) {
    for (PinsFunction &pin : pins) {
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
  audio_driver_local::Optional<PinsI2C> getI2CPins(PinFunction function) {
    PinsI2C *pins = getPtr<PinsI2C>(function, i2c);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Finds the SPI pin info with the help of the function
  audio_driver_local::Optional<PinsSPI> getSPIPins(PinFunction function) {
    PinsSPI *pins = getPtr<PinsSPI>(function, spi);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Finds the I2S pin info with the help of the port
  audio_driver_local::Optional<PinsI2S> getI2SPins(int port) {
    for (PinsI2S &pins : i2s) {
      if (pins.port == port) return pins;
    }
    return {};
  }

  /// Finds the I2S pin info with the help of the function
  audio_driver_local::Optional<PinsI2S> getI2SPins(
      PinFunction function = PinFunction::CODEC) {
    PinsI2S *pins = getPtr<PinsI2S>(function, i2s);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Sets up all pins e.g. by calling pinMode()
  virtual bool begin() {
    AD_LOGD("DriverPins::begin");

    // setup function pins
    setupPinMode();

    // setup spi
    bool result = true;
    for (auto &tmp : spi) {
      if (tmp.function == PinFunction::SD) {
        if (sd_active)
          result &= tmp.begin();
        else
          result &= tmp.begin();
      }
    }

    // setup i2c
    for (auto &tmp : i2c) {
      result &= tmp.begin();
    }
    return result;
  }

  void end() {
    // close spi
    for (auto &tmp : spi) {
      // close SD only when sd_active
      if (tmp.function == PinFunction::SD) {
        if (sd_active) tmp.end();
      } else {
        tmp.end();
      }
    }
    // close i2c
    for (auto &tmp : i2c) {
      AD_LOGD("DriverPins::begin::I2C::end");
      tmp.end();
    }
  }

  /// Defines if SPI for SD should be started (by default true)
  void setSPIActiveForSD(bool active) { sd_active = active; }

  /// Check if SPI for SD should be started automatically
  bool isSPIActiveForSD() { return sd_active; }

  /// Returns true if some function pins have been defined
  bool hasPins() { return !pins.empty(); }

  /// returns true if pressed
  virtual bool isKeyPressed(uint8_t key) {
    auto pin_opt = getPin(PinFunction::KEY, key);
    if (!pin_opt) return false;
    auto pin = pin_opt.value();
    bool value = digitalRead(pin.pin);
    return pin.pin_logic == PinLogic::InputActiveLow ? !value : value;
  }

 protected:
  audio_driver_local::Vector<PinsI2S> i2s{0};
  audio_driver_local::Vector<PinsSPI> spi{0};
  audio_driver_local::Vector<PinsI2C> i2c{0};
  audio_driver_local::Vector<PinsFunction> pins{0};
  bool sd_active = true;

  template <typename T>
  T *getPtr(PinFunction function, audio_driver_local::Vector<T> &vect) {
    for (auto &pins : vect) {
      if (pins.function == function) return &pins;
    }
    return nullptr;
  }

  template <typename T>
  bool set(T pin, audio_driver_local::Vector<T> &vect) {
    T *pins = getPtr<T>(pin.function, vect);
    if (pins == nullptr) return false;
    *pins = pin;
    return true;
  }

  void setupPinMode() {
    AD_LOGD("DriverPins::setupPinMode");
    // setup pins
    for (auto &tmp : pins) {
      if (tmp.pin != -1) {
        if (!hasConflict(tmp.pin)) {
          AD_LOGD("pinMode for %d", tmp.pin);
          switch (tmp.pin_logic) {
            case PinLogic::InputActiveHigh:
              pinMode(tmp.pin, INPUT);
              break;
            case PinLogic::InputActiveLow:
              pinMode(tmp.pin, INPUT_PULLUP);
              break;
            case PinLogic::Input:
              pinMode(tmp.pin, INPUT);
              break;
            case PinLogic::Output:
              pinMode(tmp.pin, OUTPUT);
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
      PinsSPI spi = sd.value();
      return spi.cs == pin || spi.clk == pin || spi.miso == pin ||
             spi.mosi == pin;
    }
    return false;
  }

  bool hasI2CConflict(int pin) {
    for (auto &i2c_entry : i2c) {
      if (i2c_entry.scl == pin || i2c_entry.sda == pin) {
        return true;
      }
    }
    return false;
  }
};

/**
 * @brief Support for Touch
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class DriverTouchClass : public DriverPins {
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
    // sd pins
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
    // sd pins
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
class PinsLyratMiniClass : public DriverPins {
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
    delay(LYRAT_MINI_DELAY_MS);
    int value1 = analogRead(39);
    bool result1 = inRange(value, analog_values[key]);
    result = result && result1;
    AD_LOGD("values: %d,%d for key: %d -> %s", value, value1, (int)key, result ? "true":"false");
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
class PinsAudioKitEs8388v1Class : public DriverPins {
 public:
  PinsAudioKitEs8388v1Class() {
    // sd pins
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
class PinsAudioKitEs8388v2Class : public DriverPins {
 public:
  PinsAudioKitEs8388v2Class() {
    // sd pins
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
class PinsAudioKitAC101Class : public DriverPins {
 public:
  PinsAudioKitAC101Class() {
    // sd pins
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

#if defined(ARDUINO_GENERIC_F411VETX)

/**
 * @brief Pins for alt AC101 AudioDriver - use the PinsAudioKitAC101 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsSTM32F411DiscoClass : public DriverPins {
 public:
  PinsSTM32F411DiscoClass() {
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
  }
};

static PinsSTM32F411DiscoClass PinsSTM32F411Disco;

#endif

// -- Pins
/**
 * @brief Pins need to be set up in the sketch
 * @ingroup audio_driver
 */
static DriverPins NoPins;
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



}  // namespace audio_driver