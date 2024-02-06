
#pragma once
#include "Common.h"
#include "SPI.h"
#include "Utils/Optional.h"
#include "Utils/Vector.h"
#include "Wire.h"

namespace audio_driver {

/** @file */

using Pin = int16_t;

/*!
 * @enum PinFunctionEnum
 * @brief Pin Functions
 * @ingroup enumerations
 */
enum PinFunctionEnum {
  HEADPHONE_DETECT,
  AUXIN_DETECT,
  PA, // Power Amplifier
  LED,
  KEY,
  SD,
  CODEC,
  CODEC_ADC,
  MCLK_SOURCE,
};

/**
 * @brief I2S pins
 */
struct PinsI2S {
  PinsI2S() = default;
  PinsI2S(PinFunctionEnum function, Pin mclk, Pin bck, Pin ws, Pin data_out,
          Pin data_in = -1, int port = 0) {
    this->function = function;
    this->mclk = mclk;
    this->bck = bck;
    this->ws = ws;
    this->data_out = data_out;
    this->data_in = data_in;
    this->port = port;
  }
  PinFunctionEnum function;
  Pin mclk;
  Pin bck;
  Pin ws;
  Pin data_out;
  Pin data_in;
  int port; // port number
};

/**
 * @brief SPI pins
 */
struct PinsSPI {
  PinsSPI() = default;
  PinsSPI(PinFunctionEnum function, Pin clk, Pin miso, Pin mosi, Pin cs,
          SPIClass &spi = SPI) {
    this->function = function;
    this->clk = clk;
    this->miso = miso;
    this->mosi = mosi;
    this->cs = cs;
    this->p_spi = &spi;
  }

  PinFunctionEnum function;
  SPIClass *p_spi = &SPI;
  Pin clk = -1;
  Pin miso = -1;
  Pin mosi = -1;
  Pin cs = -1;
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
        AUDIODRIVER_LOGI("setting up SPI w/o pins");
        p_spi->begin();
      } else {
// begin spi and set up pins if supported
#if defined(ARDUINO_ARCH_STM32)
        LAUDIODRIVER_LOGIOGI("setting up SPI miso:%d,mosi:%d, clk:%d, cs:%d", miso, mosi, clk,
             cs);
        p_spi->setMISO(miso);
        p_spi->setMOSI(mosi);
        p_spi->setSCLK(clk);
        p_spi->setSSEL(cs);
        p_spi->begin();
#elif defined(ESP32)
        AUDIODRIVER_LOGI("setting up SPI miso:%d,mosi:%d, clk:%d, cs:%d", miso, mosi, clk,
             cs);
        p_spi->begin(clk, miso, mosi, cs);
#elif defined(ARDUINO_ARCH_AVR)
        LOGW("setting up SPI w/o pins");
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
PinsSPI ESP32PinsSD{SD, 14, 2, 15, 13, SPI};

/**
 * @brief I2C pins
 */
struct PinsI2C {
  PinsI2C() = default;
  PinsI2C(PinFunctionEnum function, Pin scl, Pin sda, int port = -1,
          uint32_t frequency = 100000, TwoWire &wire = Wire) {
    this->function = function;
    this->scl = scl;
    this->sda = sda;
    this->port = port;
    this->frequency = frequency;
    this->p_wire = &wire;
  }

  PinFunctionEnum function;
  uint32_t frequency = 100000;
  int port = -1;
  Pin scl = -1;
  Pin sda = -1;
  bool set_active = true;
  TwoWire *p_wire;
  bool pinsAvailable() { return scl != -1 && sda != -1 && frequency != 0; }
  operator bool() { return pinsAvailable(); }

  bool begin() {
    if (set_active) {
      // if no pins are defined, just call begin
      if (!pinsAvailable()) {
        AUDIODRIVER_LOGI("setting up I2C w/o pins");
        p_wire->begin();
      } else {
        // begin with defined pins, if supported
#if defined(ESP32) || defined(ARDUINO_ARCH_STM32)
        AUDIODRIVER_LOGI("setting up I2C scl: %d, sda: %d", scl, sda);
        p_wire->begin(sda, scl);
#elif defined(ARDUINO_ARCH_RP2040)
        AUDIODRIVER_LOGI("setting up I2C scl: %d, sda: %d", scl, sda);
        p_wire->setSCL(scl);
        p_wire->setSDA(sda);
        p_wire->begin();
#else
        LOGW("setting up I2C w/o pins");
        p_wire->begin();
      }
#endif
      }
      LOGI("Setting i2c clock: %u", frequency);
      p_wire->setClock(frequency);
    }
    return true;
  }
  void end() { p_wire->end(); }
};

/**
 * @brief Pins for LED, Buttons, AMP etc
 */
struct PinsFunction {
  PinsFunction() = default;
  PinsFunction(PinFunctionEnum function, Pin pin, int index = 0) {
    this->function = function;
    this->pin = pin;
    this->index = index;
  }
  PinFunctionEnum function;
  int pin = -1;
  int index = 0;
};

/**
 * @brief All pins for i2s, spi, i2c and other pins
 * @ingroup audio_driver
 */
class DriverPins {
public:
  void addI2S(PinsI2S pin) { i2s.push_back(pin); }
  void addI2S(PinFunctionEnum function, Pin mclk, Pin bck, Pin ws, Pin data_out,
              Pin data_in = -1, int port = 0) {
    PinsI2S pin{function, mclk, bck, ws, data_out, data_in, port};
    addI2S(pin);
  }
  void addSPI(PinsSPI pin) { spi.push_back(pin); }
  void addSPI(PinFunctionEnum function, Pin clk, Pin miso, Pin mosi, Pin cs,
              SPIClass &spi = SPI) {
    PinsSPI pin(function, clk, miso, mosi, cs, spi);
    addSPI(pin);
  }

  void addI2C(PinsI2C pin) { i2c.push_back(pin); }
  void addI2C(PinFunctionEnum function, Pin scl, Pin sda, int port = -1,
          uint32_t frequency = 100000, TwoWire &wire = Wire) {
    PinsI2C pin(function, scl, sda, port, frequency);
    addI2C(pin);
  }

  void addPin(PinsFunction pin) { pins.push_back(pin); }

  void addPin(PinFunctionEnum function, Pin pinNo, int index = 0) {
    PinsFunction pin(function, pinNo, index);
    addPin(pin);
  }

  Pin getPin(PinFunctionEnum function) {
    for (PinsFunction &pin : pins) {
      if (pin.function == function)
        return pin.pin;
    }
    return -1;
  }

  Pin getPin(PinFunctionEnum function, int pos) {
    for (PinsFunction &pin : pins) {
      if (pin.function == function && pin.index == pos)
        return pin.pin;
    }
    return -1;
  }

  Optional<PinsI2C> getI2CPins(PinFunctionEnum function) {
    for (PinsI2C &pin : i2c) {
      if (pin.function == function)
        return pin;
    }
    return {};
  }

  Optional<PinsSPI> getSPIPins(PinFunctionEnum function) {
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
  Optional<PinsI2S> getI2SPins(PinFunctionEnum function = CODEC) {
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
      result &= tmp.begin();
    }
    // setup i2c
    for (auto &tmp : i2c) {
      result &= tmp.begin();
    }
    // setup pins
    for (auto &tmp : pins) {
      if (tmp.pin != -1) {

        switch (tmp.function) {
        case HEADPHONE_DETECT:
          pinMode(tmp.pin, INPUT_PULLUP);
          break;
        case AUXIN_DETECT:
          pinMode(tmp.pin, INPUT_PULLUP);
          break;
        case KEY:
          pinMode(tmp.pin, INPUT);
          break;
        case LED:
          pinMode(tmp.pin, OUTPUT);
          break;
        case PA:
          pinMode(tmp.pin, OUTPUT);
          digitalWrite(tmp.pin, HIGH);
          break;
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

protected:
  Vector<PinsI2S> i2s{0};
  Vector<PinsSPI> spi{0};
  Vector<PinsI2C> i2c{0};
  Vector<PinsFunction> pins{0};
};

/**
 * @brief Pins for Lyrat 4.3 - use the PinsLyrat43 object!
 */
class PinsLyrat43Class : public DriverPins {
public:
  PinsLyrat43Class() {
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, 23, 18, 0x20);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, 0, 5, 25, 26, 35);

    // add other pins
    addPin(KEY, 36, 1);
    addPin(KEY, 39, 2);
    addPin(KEY, 33, 3);
    addPin(KEY, 32, 4);
    addPin(KEY, 13, 5);
    addPin(KEY, 27, 6);
    addPin(AUXIN_DETECT, 12);
    addPin(HEADPHONE_DETECT, 19);
    addPin(PA, 21);
    addPin(LED, 22);
  }
};

/**
 * @brief Pins for Lyrat 4.2 - use the PinsLyrat42 object!
 */
class PinsLyrat42Class : public DriverPins {
public:
  PinsLyrat42Class() {
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, 23, 18, 0x20);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, 0, 5, 25, 26, 35);

    // add other pins
    addPin(KEY, 36, 1);
    addPin(KEY, 39, 2);
    addPin(KEY, 33, 3);
    addPin(KEY, 32, 4);
    addPin(KEY, 13, 5);
    addPin(KEY, 27, 6);
    addPin(AUXIN_DETECT, 12);
    addPin(PA, 21);
    addPin(LED, 22, 1);
    addPin(LED, 19, 2);
  }
};

/**
 * @brief Pins for Lyrat Mini - use the PinsLyratMini object!
 */
class PinsLyratMiniClass : public DriverPins {
public:
  PinsLyratMiniClass() {
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, 23, 18, 0x20);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, 0, 5, 25, 26, 35, 0);
    addI2S(CODEC_ADC, 0, 32, 33, -1, 36, 1);

    // add other pins
    addPin(KEY, 5, 1);
    addPin(KEY, 4, 2);
    addPin(KEY, 2, 3);
    addPin(KEY, 3, 4);
    addPin(KEY, 1, 5);
    //    addPin(KEY, 0, 6};
    addPin(HEADPHONE_DETECT, 19);
    addPin(PA, 21);
    addPin(LED, 22, 1);
    addPin(LED, 27, 2);
    addPin(MCLK_SOURCE, 0);
  }
};

/**
 * @brief Pins for Es8388 AudioDriver - use the PinsAudioKitEs8388v1 object!
 */
class PinsAudioKitEs8388v1Class : public DriverPins {
public:
  PinsAudioKitEs8388v1Class() {
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, 32, 33, 0x20);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, 0, 27, 25, 26, 35);

    // add other pins
    addPin(KEY, 36, 1);
    addPin(KEY, 13, 2);
    addPin(KEY, 19, 3);
    addPin(KEY, 23, 4);
    addPin(KEY, 18, 5);
    addPin(KEY, 5, 6);
    addPin(AUXIN_DETECT, 12);
    addPin(HEADPHONE_DETECT, 39);
    addPin(PA, 21);
    addPin(LED, 22);
  }
};

/**
 * @brief Pins for alt Es8388 AudioDriver - use the PinsAudioKitEs8388v2 object!
 */
class PinsAudioKitEs8388v2Class : public DriverPins {
public:
  PinsAudioKitEs8388v2Class() {
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, 23, 18, 0x20);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, 0, 5, 25, 26, 35);

    // add other pins
    addPin(KEY, 36, 1);
    addPin(KEY, 13, 2);
    addPin(KEY, 19, 3);
    addPin(KEY, 23, 4);
    addPin(KEY, 18, 5);
    addPin(KEY, 5, 6);
    addPin(AUXIN_DETECT, 12);
    addPin(HEADPHONE_DETECT, 39);
    addPin(PA, 21);
    addPin(LED, 22);
  }
};

/**
 * @brief Pins for alt AC101 AudioDriver - use the PinsAudioKitAC101 object!
 */
class PinsAudioKitAC101Class : public DriverPins {
public:
  PinsAudioKitAC101Class() {
    // sd pins
    addSPI(ESP32PinsSD);
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, 32, 22, 0x20);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, 0, 27, 26, 25, 35);

    // add other pins
    addPin(KEY, 36, 1);
    addPin(KEY, 13, 2);
    addPin(KEY, 19, 3);
    addPin(KEY, 23, 4);
    addPin(KEY, 18, 5);
    addPin(KEY, 5, 6);
    addPin(LED, 22);
    addPin(LED, 19);
    addPin(HEADPHONE_DETECT, 5);
    addPin(PA, 21);
  }
};

#if defined(ARDUINO_ARCH_STM32)

/**
 * @brief Pins for alt AC101 AudioDriver - use the PinsAudioKitAC101 object!
 */
class PinsSTM32F411DiscoClass : public DriverPins {
public:
  PinsSTM32F411DiscoClass() {
    // add i2c codec pins: scl, sda, port, frequency
    addI2C(CODEC, PB6, PB9, 0x25);
    // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
    addI2S(CODEC, PC7, PC10, PA4, PC3, PC12);

    // add other pins
    addPin(KEY, PA0);       // user button
    addPin(LED, PD12, 0);   // green
    addPin(LED, PD5, 1);    // red
    addPin(LED, PD13, 2);   // orange
    addPin(LED, PD14, 3);   // red
    addPin(LED, PD15, 4);   // blue
    addPin(PA, PD4);        // reset pin (active high)
    addPin(CODEC_ADC, PC3); // Microphone
  }
} PinsSTM32F411Disco;

#endif

} // namespace audio_driver