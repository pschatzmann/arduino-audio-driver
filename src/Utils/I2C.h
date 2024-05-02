#pragma once
#include <stdint.h>
#include "DriverCommon.h"


#ifdef __cplusplus
extern "C" {
#endif


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
error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg, int regLen, uint8_t *data, int datalen);

/**
 * @brief Requests ACK from I2C device
 * 
 * @param bus        I2C bus handle
 * @param addr       The address of the device
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
error_t i2c_bus_check(i2c_bus_handle_t bus, int addr);


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
error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg, int reglen, uint8_t *outdata, int datalen);


#ifdef __cplusplus
}
#endif

