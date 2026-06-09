// I2C Driver for Zephyr RTOS
#if defined(__zephyr__)

#include <assert.h>
#include <string.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>

#include "Platforms/API_I2C.h"

namespace audio_driver {


// In Zephyr, the I2CConfig should hold a pointer to the i2c device:
//   struct device *p_i2c_dev;  (instead of TwoWire *p_wire)
// and the bus handle is also struct device *.

error_t i2c_bus_create(struct I2CConfig* config) {
  AD_LOGI("i2c_bus_create");
  assert(config != NULL);

  struct device* dev = (struct device*)config->p_wire;
  assert(dev != NULL);

  if (!device_is_ready(dev)) {
    AD_LOGE("I2C device not ready");
    return RESULT_FAIL;
  }

  // Configure I2C speed if specified
  if (config->frequency > 0) {
    uint32_t i2c_cfg = I2C_MODE_CONTROLLER;

    if (config->frequency <= 100000) {
      i2c_cfg |= I2C_SPEED_SET(I2C_SPEED_STANDARD);
    } else if (config->frequency <= 400000) {
      i2c_cfg |= I2C_SPEED_SET(I2C_SPEED_FAST);
    } else if (config->frequency <= 1000000) {
      i2c_cfg |= I2C_SPEED_SET(I2C_SPEED_FAST_PLUS);
    } else {
      i2c_cfg |= I2C_SPEED_SET(I2C_SPEED_HIGH);
    }

    int rc = i2c_configure(dev, i2c_cfg);
    if (rc != 0) {
      AD_LOGE("Failed to configure I2C: %d", rc);
      return RESULT_FAIL;
    }
    AD_LOGI("Setting I2C clock: %u Hz", config->frequency);
  }

  return RESULT_OK;
}

void i2c_bus_delete(i2c_bus_handle_t bus) {
  // In Zephyr, devices are statically allocated — nothing to free.
  // If you held a mutex or semaphore, release it here.
  (void)bus;
}

error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t* reg,
                            int reglen, uint8_t* data, int datalen) {
  AD_LOGD("i2c_bus_write_bytes: addr=0x%X reglen=%d datalen=%d reg=0x%02X",
          addr, reglen, datalen, reglen > 0 ? reg[0] : 0);

  struct device* dev = (struct device*)bus;
  assert(dev != NULL);

  // Build a gather-write: [reg bytes] + [data bytes] in a single transaction
  struct i2c_msg msgs[2];
  uint8_t num_msgs = 0;

  if (reglen > 0) {
    msgs[num_msgs].buf = reg;
    msgs[num_msgs].len = reglen;
    msgs[num_msgs].flags = I2C_MSG_WRITE;
    num_msgs++;
  }

  if (datalen > 0) {
    msgs[num_msgs].buf = data;
    msgs[num_msgs].len = datalen;
    msgs[num_msgs].flags = I2C_MSG_WRITE;
    num_msgs++;
  }

  // Mark the last message with STOP
  if (num_msgs > 0) {
    msgs[num_msgs - 1].flags |= I2C_MSG_STOP;
  }

  int rc = i2c_transfer(dev, msgs, num_msgs, (uint16_t)addr);
  if (rc != 0) {
    AD_LOGE("i2c_transfer (write) failed: %d", rc);
    return RESULT_FAIL;
  }
  return RESULT_OK;
}

error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t* reg,
                           int reglen, uint8_t* outdata, int datalen) {
  AD_LOGD("i2c_bus_read_bytes: addr=0x%X reglen=%d datalen=%d reg=0x%02X", addr,
          reglen, datalen, reglen > 0 ? reg[0] : 0);

  struct device* dev = (struct device*)bus;
  assert(reglen == 1);
  assert(dev != NULL);

  memset(outdata, 0, datalen);

  // Write register address, then repeated-start read
  struct i2c_msg msgs[2] = {
      {
          .buf = reg,
          .len = (uint32_t)reglen,
          .flags = I2C_MSG_WRITE,
      },
      {
          .buf = outdata,
          .len = (uint32_t)datalen,
          .flags = I2C_MSG_READ | I2C_MSG_STOP,
      },
  };

  int rc = i2c_transfer(dev, msgs, 2, (uint16_t)addr);
  if (rc != 0) {
    AD_LOGE("i2c_transfer (read) failed: %d", rc);
    return RESULT_FAIL;
  }
  return RESULT_OK;
}

} // namespace audio_driver

#endif  // CONFIG_ZEPHYR
