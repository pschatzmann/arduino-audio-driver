
#pragma once

#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "Platforms/API_SPI.h"
#include "Platforms/API_GPIO.h"
#include "Platforms/API_Delay.h"
#include "Platforms/Logger.h"
#include "Platforms/Optional.h"
#include "Platforms/Vector.h"

#if defined(__zephyr__)
#include "IDriverDeviceInfoZephyr"
#else

namespace audio_driver {

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
  GpioPin pin{};
  int index = 0;
  PinLogic pin_logic;
  bool active = true;  // false if pin conflict
};



/**
 * @brief I2S pins
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct InfoI2S {
  InfoI2S() = default;
  InfoI2S(PinFunction function, GpioPin mclk, GpioPin bck, GpioPin ws,
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

using PinsI2S = InfoI2S;

/**
 * @brief SPI pins: In Arduino we initialize the SPI, on other platform
 * we just provide the pin information
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct InfoSPI : public SPIConfig {
  InfoSPI() {
    this->clk = -1;
    this->miso = -1;
    this->mosi = -1;
    this->cs = -1;
    this->p_spi = &SPI;
  };
  InfoSPI(PinFunction function, GpioPin clk, GpioPin miso, GpioPin mosi,
          GpioPin cs, SPIClass& spi = SPI) {
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

using PinsSPI = InfoSPI;

/**
 * @brief Default SPI pins for ESP32 Lyrat, AudioDriver etc
 * CLK, MISO, MOSI, CS
 */

static InfoSPI ESP32PinsSD{PinFunction::SD, 14, 2, 15, 13, SPI};

/**
 * @brief I2C pins
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct InfoI2C : public I2CConfig {
  InfoI2C() {
    port = 0;
    address = -1;
    scl = -1;
    sda = -1;
    frequency = 100000;
    set_active = true;
    p_wire = DEFAULT_WIRE;
  };

  InfoI2C(PinFunction function, GpioPin scl, GpioPin sda, int address = -1,
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

using PinsI2C = InfoI2C;


class IDriverDeviceInfo {
 public:
   virtual audio_driver_local::Optional<InfoI2C> getI2CPins(PinFunction function) = 0;
};

} // namespace audio_driver

#endif


