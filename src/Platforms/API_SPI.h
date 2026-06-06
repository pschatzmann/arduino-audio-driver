#pragma once
#include <stdint.h>

#include "DriverCommon.h"

#ifndef ARDUINO
// prevent compile errors
struct SPIClass {
  void *ref = nullptr;
} static SPI;
#endif

struct SPIConfig {
  spi_bus_handle_t p_spi;
  GpioPin clk;
  GpioPin miso;
  GpioPin mosi;
  GpioPin cs;
};


error_t spi_bus_create(struct SPIConfig *pin);

void spi_bus_delete(spi_bus_handle_t bus);

