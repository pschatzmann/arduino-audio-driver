#include <Wire.h>
#include <assert.h>
#include <stdio.h>
#include "Utils/I2C.h"

// this method is used !
error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                              int reglen, uint8_t *data, int datalen) {
  AD_LOGD(
      "i2c_bus_write_bytes: addr=0x%X reglen=%d datalen=%d - reg=0x%0X value=0x%0X",
      addr, reglen, datalen, reg[0], data[0]);
  TwoWire *p_wire = (TwoWire *)bus;
  assert(p_wire!=nullptr);
  //assert(reglen == 1);
  //assert(datalen == 1);

  int result = RESULT_OK;
  p_wire->beginTransmission(addr);
  p_wire->write(reg, reglen);
  p_wire->write(data, datalen);
  //p_wire->write(reg[0]);
  //p_wire->write(data[0]);
 
  int rc = p_wire->endTransmission(I2C_END);
  if (rc != 0) {
    AD_LOGE("->p_wire->endTransmission: %d", rc);
    result = RESULT_FAIL;
  }
  return result;
}

// this method is used !
error_t i2c_bus_check(i2c_bus_handle_t bus, int addr) {
  AD_LOGD("i2c_bus_check: addr=0x%X",addr);
  TwoWire *p_wire = (TwoWire *)bus;
  assert(p_wire!=nullptr);
  int result = RESULT_OK;
  p_wire->beginTransmission(addr); 
  int rc = p_wire->endTransmission(I2C_END);
  if (rc != 0) {
    AD_LOGE("->p_wire->endTransmission: %d", rc);
    result = RESULT_FAIL;
  }
  return result;
}


/// This method is used
error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                             int reglen, uint8_t *outdata, int datalen) {
  AD_LOGD("i2c_bus_read_bytes: addr=%d reglen=%d datalen=%d - reg=%d", addr,
           reglen, datalen, reg[0]);
  TwoWire *p_wire = (TwoWire *)bus;
  assert(reglen == 1);
  assert(p_wire!=nullptr);

  memset(outdata,0,datalen);
  int result = RESULT_OK;

  p_wire->beginTransmission(addr);
  p_wire->write(reg, reglen);
  int rc = p_wire->endTransmission();
  if (rc != 0) {
    AD_LOGE("->p_wire->endTransmission: %d", rc);
  }

  uint8_t result_len = p_wire->requestFrom((addr), datalen, (int) true);
  if (result_len > 0) {
    result_len = p_wire->readBytes(outdata, datalen);
  } else {
    AD_LOGE("->p_wire->requestFrom %d->%d", datalen, result_len);
    result = RESULT_FAIL;
  }
  return result;
}
