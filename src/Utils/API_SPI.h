#pragma once
#include <stdint.h>

#include "DriverCommon.h"
#ifdef __cplusplus

typedef int16_t GpioPin;

struct SPIConfig {
  spi_bus_handle_t p_spi;
  GpioPin clk;
  GpioPin miso;
  GpioPin mosi;
  GpioPin cs;
};

extern "C" {
#endif

error_t spi_bus_create(struct SPIConfig *pin);

void spi_bus_delete(spi_bus_handle_t bus);

#ifdef __cplusplus
}
#endif
