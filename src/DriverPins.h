
#pragma once
#include "Wire.h"
#include "SPI.h"
#include "DriverCommon.h"
#include "Utils/Optional.h"
#include "Utils/Vector.h"

namespace audio_driver {

/** @file */

using GpioPin = int16_t;

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
 * @brief SPI pins
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinsSPI {
  PinsSPI() = default;
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
  SPIClass *p_spi = &SPI;
  GpioPin clk = -1;
  GpioPin miso = -1;
  GpioPin mosi = -1;
  GpioPin cs = -1;
  bool set_active = true;
  bool pinsAvailable() { return clk != -1 && miso != -1 && mosi != -1; }
  operator bool() { return pinsAvailable(); }
  bool begin() {
    if (set_active) {
      AD_LOGD("PinsSPI::begin for %d", function);
      // setup chip select
      if (cs != -1) {
        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);
      }
      // if no pins are defined, just call begin
      if (!pinsAvailable()) {
        AD_LOGI("setting up SPI w/o pins");
        p_spi->begin();
      } else {
// begin spi and set up pins if supported
#if defined(ARDUINO_ARCH_STM32)
        AD_LOGI("setting up SPI miso:%d,mosi:%d, clk:%d, cs:%d", miso, mosi,
                clk, cs);
        p_spi->setMISO(miso);
        p_spi->setMOSI(mosi);
        p_spi->setSCLK(clk);
        p_spi->setSSEL(cs);
        p_spi->begin();
#elif defined(ESP32)
        AD_LOGI("setting up SPI miso:%d,mosi:%d, clk:%d, cs:%d", miso, mosi,
                clk, cs);
        p_spi->begin(clk, miso, mosi, cs);
#elif defined(ARDUINO_ARCH_AVR)
        AD_LOGW("setting up SPI w/o pins");
        p_spi->begin();
#endif
      }
    } else {
      AD_LOGI("SPI, not active, MOSI, MISO, SCLK, SSEL not modified");
    }
    return true;
  }
  void end() { p_spi->end(); }
};

/**
 * @brief Default SPI pins for ESP32 Lyrat, AudioDriver etc
 */
PinsSPI ESP32PinsSD{PinFunction::SD, 14, 2, 15, 13, SPI};

/**
 * @brief I2C pins
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinsI2C {
  PinsI2C() = default;
  PinsI2C(PinFunction function, GpioPin scl, GpioPin sda, int port = -1,
          uint32_t frequency = 100000, TwoWire &wire = Wire, bool active=true) {
    this->function = function;
    this->scl = scl;
    this->sda = sda;
    this->port = port;
    this->frequency = frequency;
    this->p_wire = &wire;
    this->set_active = active;
  }

  PinFunction function;
  uint32_t frequency = 100000;
  int port = -1;
  GpioPin scl = -1;
  GpioPin sda = -1;
  bool set_active = true;
  TwoWire *p_wire;
  bool pinsAvailable() { return scl != -1 && sda != -1 && frequency != 0; }
  operator bool() { return pinsAvailable(); }

  bool begin() {
    if (set_active) {
      AD_LOGD("PinsI2C::begin for %d", function);
      // if no pins are defined, just call begin
      if (!pinsAvailable()) {
        AD_LOGI("setting up I2C w/o pins");
        p_wire->begin();
      } else {
        // begin with defined pins, if supported
#if defined(ESP32)
        AD_LOGI("setting up I2C scl: %d, sda: %d", scl, sda);
        p_wire->begin(sda, scl);
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
        AD_LOGI("setting up I2C scl: %d, sda: %d", scl, sda);
        p_wire->setSCL(scl);
        p_wire->setSDA(sda);
        p_wire->begin();
#else
        AD_LOGW("setting up I2C w/o pins");
        p_wire->begin();
#endif
      }
      AD_LOGI("Setting i2c clock: %u", frequency);
      p_wire->setClock(frequency);
    } else {
      AD_LOGI("I2C, not activated, SDA, SCL, i2c clock not modified");
    }
    return true;
  }
  void end() {
#if !defined(ESP8266) && FORCE_WIRE_CLOSE
    if (set_active) p_wire->end();
#endif
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
              GpioPin cs, SPIClass &spi = SPI) {
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
  
  bool addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port = -1,
              uint32_t frequency = 100000, TwoWire &wire = Wire, bool active = true) {
    PinsI2C pin(function, scl, sda, port, frequency, wire, active);
    return addI2C(pin);
  }

  /// Just define your initialzed wire object
  bool addI2C(PinFunction function, TwoWire &wire, bool setActive = false) {
    PinsI2C pin(function, -1, -1, -1, -1, wire, setActive);
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
  /// Get pin information by function
  audio_driver_local::Optional<PinsFunction> getPin(PinFunction function, int pos = 0) {
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
  audio_driver_local::Optional<PinsI2S> getI2SPins(PinFunction function = PinFunction::CODEC) {
    PinsI2S *pins = getPtr<PinsI2S>(function, i2s);
    if (pins == nullptr) return {};
    return *pins;
  }

  /// Sets up all pins e.g. by calling pinMode()
  virtual bool begin() {
    AD_LOGD("DriverPins::begin");

    // setup function pins
    AD_LOGD("DriverPins::begin::setupPinMode");
    setupPinMode();

    // setup spi
    AD_LOGD("DriverPins::begin::SPI");
    bool result = true;
    for (auto &tmp : spi) {
      AD_LOGD("DriverPins::begin::SPI::begin");
      if (tmp.function == PinFunction::SD) {
        if (sd_active)
          result &= tmp.begin();
        else
          result &= tmp.begin();
      }
    }

    // setup i2c
    AD_LOGD("DriverPins::begin::I2C");
    for (auto &tmp : i2c) {
      AD_LOGD("DriverPins::begin::I2C port:%d", tmp.port);
      result &= tmp.begin();
    }
    return result;
  }

  void end() {
    // setup spi
    for (auto &tmp : spi) {
      AD_LOGD("DriverPins::begin::SPI::end");
      tmp.end();
    }
    // setup i2c
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
    // setup pins
    for (auto &tmp : pins) {
      if (tmp.pin != -1) {
        if (!hasConflict(tmp.pin)) {
          AD_LOGD("pinMode %d", tmp.pin);
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
      AD_LOGD("Pin %d set", tmp.pin);
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
 * @brief Pins for Lyrat 4.3 - use the PinsLyrat43 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsLyrat43Class : public DriverPins {
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
class PinsLyrat42Class : public DriverPins {
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
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, 23, 18);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, 0, 5, 25, 26, 35, 0);
    addI2S(PinFunction::CODEC_ADC, 0, 32, 33, -1, 36, 1);

    // add other pins
    addPin(PinFunction::KEY, 5, PinLogic::InputActiveLow, 1);
    addPin(PinFunction::KEY, 4, PinLogic::InputActiveLow, 2);
    addPin(PinFunction::KEY, 2, PinLogic::InputActiveLow, 3);
    addPin(PinFunction::KEY, 3, PinLogic::InputActiveLow, 4);
    addPin(PinFunction::KEY, 1, PinLogic::InputActiveLow, 5);
    //    addPin(PinFunction::KEY, 0, 6};
    addPin(PinFunction::HEADPHONE_DETECT, 19, PinLogic::InputActiveLow);
    addPin(PinFunction::PA, 21, PinLogic::Output);
    addPin(PinFunction::LED, 22, PinLogic::Output, 1);
    addPin(PinFunction::LED, 27, PinLogic::Output, 2);
    addPin(PinFunction::MCLK_SOURCE, 0, PinLogic::Output);
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