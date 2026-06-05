#pragma once
#include <stdint.h>
#include "DriverCommon.h"


struct I2CConfig {
  i2c_bus_handle_t p_wire;
  uint32_t frequency;
  int port;
  int address;
// Zephyr manages the pins in the devicetree!
#if !defined(__zephyr__)
  GpioPin scl;
  GpioPin sda;
#endif
};

#ifndef ARDUINO
struct TwoWire {};
#endif

#ifdef __cplusplus
extern "C" {
#endif

error_t i2c_bus_create(struct I2CConfig *pin);

void i2c_bus_delete(i2c_bus_handle_t bus);

/**
 * @brief Write bytes to I2C bus
 *
 * @param bus        I2C bus handle
 * @param addr       The address of the device
 * @param reg        The register of the device
 * @param regLen     The length of register
 * @param data       The data pointer
 * @param datalen    The length of data
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                            int regLen, uint8_t *data, int datalen);

/**
 * @brief Read bytes to I2C bus
 *
 * @param bus        I2C bus handle
 * @param addr       The address of the device
 * @param reg        The register of the device
 * @param regLen     The length of register
 * @param outdata    The outdata pointer
 * @param datalen        The length of outdata
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                           int reglen, uint8_t *outdata, int datalen);

#ifdef __cplusplus
}
#endif
