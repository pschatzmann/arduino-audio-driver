
#pragma once
#include "Common.h"
#include "SPI.h"
#include "Utils/Optional.h"
#include "Utils/Vector.h"
#include "Wire.h"

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
  PA, // Power Amplifier
  POWER,
  LED,
  KEY,
  SD,
  CODEC,
  CODEC_ADC,
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
  int port; // port number
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
          uint32_t frequency = 100000, TwoWire &wire = Wire) {
    this->function = function;
    this->scl = scl;
    this->sda = sda;
    this->port = port;
    this->frequency = frequency;
    this->p_wire = &wire;
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
      // if no pins are defined, just call begin
      if (!pinsAvailable()) {
        AD_LOGI("setting up I2C w/o pins");
        p_wire->begin();
      } else {
        // begin with defined pins, if supported
#if defined(ESP32) || defined(ARDUINO_ARCH_STM32)
        AD_LOGI("setting up I2C scl: %d, sda: %d", scl, sda);
        p_wire->begin(sda, scl);
#elif defined(ARDUINO_ARCH_RP2040)
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
    }
    return true;
  }
  void end() { p_wire->end(); }
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
};

/**
 * @brief All pins for i2s, spi, i2c and other pins
 * @ingroup audio_driver
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class DriverPins {
public:
  void addI2S(PinsI2S pin) { i2s.push_back(pin); }
  void addI2S(PinFunction function, GpioPin mclk, GpioPin bck, GpioPin ws,
              GpioPin data_out, GpioPin data_in = -1, int port = 0) {
    PinsI2S pin{function, mclk, bck, ws, data_out, data_in, port};
    addI2S(pin);
  }
  void addSPI(PinsSPI pin) { spi.push_back(pin); }
  void addSPI(PinFunction function, GpioPin clk, GpioPin miso, GpioPin mosi,
              GpioPin cs, SPIClass &spi = SPI) {
    PinsSPI pin(function, clk, miso, mosi, cs, spi);
    addSPI(pin);
  }

  void addI2C(PinsI2C pin) { i2c.push_back(pin); }
  void addI2C(PinFunction function, GpioPin scl, GpioPin sda, int port = -1,
              uint32_t frequency = 100000, TwoWire &wire = Wire) {
    PinsI2C pin(function, scl, sda, port, frequency);
    addI2C(pin);
  }

  void addPin(PinsFunction pin) { pins.push_back(pin); }

  void addPin(PinFunction function, GpioPin pinNo, PinLogic logic,
              int index = 0) {
    PinsFunction pin(function, pinNo, logic, index);
    addPin(pin);
  }
  /// Get pin information by function
  Optional<PinsFunction> getPin(PinFunction function, int pos = 0) {
    for (PinsFunction &pin : pins) {
      if (pin.function == function && pin.index == pos)
        return pin;
    }
    return {};
  }

  /// Get pin information by pin ID
  Optional<PinsFunction> getPin(GpioPin pinId) {
    for (PinsFunction &pin : pins) {
      if (pin.pin == pinId)
        return pin;
    }
    return {};
  }

  GpioPin getPinID(PinFunction function, int pos = 0) {
    auto pin = getPin(function, pos);
    if (pin)
      return pin.value().pin;
    return -1;
  }

  Optional<PinsI2C> getI2CPins(PinFunction function) {
    for (PinsI2C &pin : i2c) {
      if (pin.function == function)
        return pin;
    }
    return {};
  }

  Optional<PinsSPI> getSPIPins(PinFunction function) {
    for (PinsSPI &pins : spi) {
      if (pins.function == function)
        return pins;
    }
    return {};
  }

  /// Finds the I2S pins with the help of the port
  Optional<PinsI2S> getI2SPins(int port) {
    for (PinsI2S &pins : i2s) {
      if (pins.port == port)
        return pins;
    }
    return {};
  }

  /// Finds the I2S pins with the help of the function
  Optional<PinsI2S> getI2SPins(PinFunction function = PinFunction::CODEC) {
    for (PinsI2S &pins : i2s) {
      if (pins.function == function)
        return pins;
    }
    return {};
  }

  virtual bool begin() {
    // setup spi
    bool result = true;
    for (auto &tmp : spi) {
      if (tmp.function == PinFunction::SD && sd_active)
        result &= tmp.begin();
      else
        result &= tmp.begin();
    }
    // setup i2c
    for (auto &tmp : i2c) {
      result &= tmp.begin();
    }
    // setup pins
    for (auto &tmp : pins) {
      if (tmp.pin != -1) {
        if (!hasConflict(tmp.pin)) {
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
        }
      }
    }
    return result;
  }

  void end() {
    // setup spi
    for (auto &tmp : spi) {
      tmp.end();
    }
    // setup i2c
    for (auto &tmp : i2c) {
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
  Vector<PinsI2S> i2s{0};
  Vector<PinsSPI> spi{0};
  Vector<PinsI2C> i2c{0};
  Vector<PinsFunction> pins{0};
  bool sd_active = true;

  bool hasConflict(int pin) {
    if (sd_active) {
      auto sd = getSPIPins(PinFunction::SD);
      if (!sd)
        return false;
      PinsSPI spi = sd.value();
      return spi.cs == pin || spi.clk == pin || spi.miso == pin ||
             spi.mosi == pin;
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
    addI2C(PinFunction::CODEC, 23, 18, 0x20);
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
    addI2C(PinFunction::CODEC, 23, 18, 0x20);
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
    addI2C(PinFunction::CODEC, 23, 18, 0x20);
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
    addI2C(PinFunction::CODEC, 32, 33, 0x20);
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
    addI2C(PinFunction::CODEC, 23, 18, 0x20);
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
    addI2C(PinFunction::CODEC, 32, 22, 0x20);
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

#if defined(ARDUINO_ARCH_STM32)

/**
 * @brief Pins for alt AC101 AudioDriver - use the PinsAudioKitAC101 object!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinsSTM32F411DiscoClass : public DriverPins {
public:
  PinsSTM32F411DiscoClass() {
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(PinFunction::CODEC, PB6, PB9, 0x25);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(PinFunction::CODEC, PC7, PC10, PA4, PC3, PC12);

    // add other pins
    addPin(PinFunction::KEY, PA0, PinLogic::Output);     // user button
    addPin(PinFunction::LED, PD12, PinLogic::Output, 0); // green
    addPin(PinFunction::LED, PD5, PinLogic::Output, 1);  // red
    addPin(PinFunction::LED, PD13, PinLogic::Output, 2); // orange
    addPin(PinFunction::LED, PD14, PinLogic::Output, 3); // red
    addPin(PinFunction::LED, PD15, PinLogic::Output, 4); // blue
    addPin(PinFunction::PA, PD4, PinLogic::Output, ); // reset pin (active high)
    addPin(PinFunction::CODEC_ADC, PC3, PinLogic::Input); // Microphone
  }
} PinsSTM32F411Disco;

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

} // namespace audio_driver