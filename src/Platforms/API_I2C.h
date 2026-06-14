#pragma once
#include <stdint.h>
#include "DriverCommon.h"

// Platform-specific includes (must be at file scope, before namespace)
#if defined(ARDUINO) && !AUDIO_DRIVER_FORCE_IDF
#  include <Wire.h>
#  include <assert.h>
#  include <stdio.h>
#elif defined(ESP32_CMAKE)
#  include <assert.h>
#  include <string.h>
#  include "driver/i2c_master.h"
#elif defined(__zephyr__)
#  include <assert.h>
#  include <string.h>
#  include <zephyr/drivers/i2c.h>
#  include <zephyr/kernel.h>
#endif

namespace audio_driver {

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

// ---- Arduino I2C implementation ----
#if defined(ARDUINO) && !AUDIO_DRIVER_FORCE_IDF

inline error_t i2c_bus_create(struct I2CConfig *config) {
  AD_LOGI("i2c_bus_create");
  assert(config != nullptr);
  I2CConfig &pins = *config;
  TwoWire *p_wire = (TwoWire *)pins.p_wire;
  assert(p_wire != nullptr);

  if (pins.scl == -1 || pins.sda == -1) {
    AD_LOGI("setting up I2C w/o pins");
    p_wire->begin();
  } else {
#if defined(ESP32)
    AD_LOGI("setting up I2C scl: %d, sda: %d", pins.scl, pins.sda);
    p_wire->begin(pins.sda, pins.scl);
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
    AD_LOGI("setting up I2C scl: %d, sda: %d", pins.scl, pins.sda);
    p_wire->setSCL(pins.scl);
    p_wire->setSDA(pins.sda);
    p_wire->begin();
#else
    AD_LOGW("setting up I2C w/o pins");
    p_wire->begin();
#endif
    if (pins.frequency > 0) {
      AD_LOGI("Setting i2c clock: %u", pins.frequency);
      p_wire->setClock(pins.frequency);
    }
  }
  return RESULT_OK;
}

inline void i2c_bus_delete(i2c_bus_handle_t bus) {
  TwoWire *p_wire = (TwoWire *)bus;
  assert(p_wire != nullptr);
#if !defined(ESP8266) && FORCE_WIRE_CLOSE
  p_wire->end();
#endif
}

inline error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                                   int reglen, uint8_t *data, int datalen) {
  AD_LOGD("i2c_bus_write_bytes: addr=0x%X reglen=%d datalen=%d - reg=0x%0X ",
          addr, reglen, datalen, reg[0]);
  TwoWire *p_wire = (TwoWire *)bus;
  assert(p_wire != nullptr);
  int result = RESULT_OK;
  p_wire->beginTransmission(addr);
  if (reglen > 0) p_wire->write(reg, reglen);
  if (datalen > 0) p_wire->write(data, datalen);
  int rc = p_wire->endTransmission(I2C_END);
  if (rc != 0) {
    AD_LOGE("->p_wire->endTransmission: %d", rc);
    result = RESULT_FAIL;
  }
  return result;
}

inline error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                                  int reglen, uint8_t *outdata, int datalen) {
  AD_LOGD("i2c_bus_read_bytes: addr=0x%X reglen=%d datalen=%d - reg=0x%X", addr,
          reglen, datalen, reg[0]);
  TwoWire *p_wire = (TwoWire *)bus;
  assert(p_wire != nullptr);

  memset(outdata, 0, datalen);
  int result = RESULT_OK;

  p_wire->beginTransmission(addr);
  p_wire->write(reg, reglen);
  int rc = p_wire->endTransmission();
  if (rc != 0) {
    AD_LOGE("->p_wire->endTransmission: %d", rc);
  }

  uint8_t result_len = p_wire->requestFrom(addr, datalen, (int)I2C_END);
  if (result_len > 0) {
    result_len = p_wire->readBytes(outdata, datalen);
  } else {
    AD_LOGE("->p_wire->requestFrom %d->%d", datalen, result_len);
    result = RESULT_FAIL;
  }
  return result;
}

// ---- Espressif IDF I2C implementation ----
#elif defined(ESP32_CMAKE)

static I2CConfig *i2c_configs[5];

static inline I2CConfig *get_config(i2c_bus_handle_t hdl) {
  for (int j = 0; j < 5; j++) {
    if (i2c_configs[j]->p_wire == hdl) {
      return i2c_configs[j];
    }
  }
  return nullptr;
}

inline error_t i2c_bus_create(struct I2CConfig *config) {
  I2CConfig &pins = *config;
  i2c_port_t i2c_slave_port = (i2c_port_t)pins.port;
  i2c_configs[pins.port] = config;

  AD_LOGI("i2c_bus_create port: %d scl: %d, sda: %d address: 0x%x", pins.port,
          pins.scl, pins.sda, pins.address);

  i2c_master_bus_config_t i2c_mst_config = {};
  i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
  i2c_mst_config.i2c_port = i2c_slave_port;
  i2c_mst_config.scl_io_num = (gpio_num_t)pins.scl;
  i2c_mst_config.sda_io_num = (gpio_num_t)pins.sda;
  i2c_mst_config.glitch_ignore_cnt = 7;
  i2c_mst_config.flags.enable_internal_pullup = true;

  i2c_master_bus_handle_t bus_handle;
  if (i2c_new_master_bus(&i2c_mst_config, &bus_handle) != ESP_OK) {
    AD_LOGE("i2c_new_master_bus");
    return ESP_FAIL;
  }

  if (i2c_master_probe(bus_handle, pins.address, -1) != RESULT_OK) {
    AD_LOGE("Address check failed: scanning addresses:");
    for (int j = 0; j < 127; j++) {
      auto rc = i2c_master_probe(bus_handle, j, -1);
      AD_LOGE("- address: 0x%x -> %d", j, rc);
    }
    return ESP_FAIL;
  }

  pins.p_wire = bus_handle;
  return ESP_OK;
}

inline void i2c_bus_delete(i2c_bus_handle_t bus) {
  if (i2c_del_master_bus((i2c_master_bus_handle_t)bus) != ESP_OK) {
    AD_LOGI("i2c_bus_delete");
  }
}

inline error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                                   int reglen, uint8_t *data, int datalen) {
  AD_LOGD("i2c_bus_write_bytes address: 0x%x", addr);
  i2c_master_bus_handle_t bus_handle = (i2c_master_bus_handle_t)bus;

  I2CConfig *cfg = get_config(bus);
  assert(cfg != nullptr);
  assert(cfg->address == addr);

  i2c_device_config_t dev_cfg = {};
  dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_cfg.device_address = cfg->address;
  dev_cfg.scl_speed_hz = cfg->frequency;

  i2c_master_dev_handle_t dev_handle;
  if (!i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle) == ESP_OK) {
    AD_LOGE("i2c_new_master_bus");
    return ESP_FAIL;
  }

  int total_len = reglen + datalen;
  uint8_t total_data[total_len];
  memcpy(total_data, reg, reglen);
  memcpy(total_data + reglen, data, datalen);

  esp_err_t ret = ESP_OK;
  ret |= i2c_master_transmit(dev_handle, total_data, total_len, -1);
  if (ret == ESP_OK) {
    ret = i2c_master_bus_wait_all_done(bus_handle, -1);
    if (ret != ESP_OK) AD_LOGE("i2c_master_bus_wait_all_done");
  } else {
    AD_LOGE("i2c_master_transmit");
  }

  if (i2c_master_bus_rm_device(dev_handle) != ESP_OK) {
    AD_LOGE("i2c_master_bus_rm_device");
  }

  if (ret != ESP_OK) {
    AD_LOGE("i2c_bus_write_bytes");
  }
  return ret;
}

inline error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                                  int reglen, uint8_t *read_buffer, int read_size) {
  AD_LOGD("i2c_bus_read_bytes address: 0x%x", addr);
  i2c_master_bus_handle_t bus_handle = (i2c_master_bus_handle_t)bus;

  I2CConfig *cfg = get_config(bus);
  assert(cfg != nullptr);
  assert(cfg->address == addr);

  i2c_device_config_t dev_cfg = {};
  dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_cfg.device_address = cfg->address;
  dev_cfg.scl_speed_hz = cfg->frequency;

  i2c_master_dev_handle_t dev_handle;
  if (!i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle) == ESP_OK) {
    AD_LOGE("i2c_master_bus_add_device");
    return ESP_FAIL;
  }

  int write_size = reglen;
  uint8_t write_buffer[write_size];
  memcpy(write_buffer + 1, reg, reglen);

  esp_err_t ret = ESP_OK;
  ret = i2c_master_transmit_receive(dev_handle, write_buffer, write_size,
                                    read_buffer, read_size, -1);
  ret = i2c_master_bus_wait_all_done(bus_handle, -1);
  if (ret != ESP_OK) {
    AD_LOGE("i2c_bus_read_bytes");
  }
  if (i2c_master_bus_rm_device(dev_handle) != ESP_OK) {
    AD_LOGI("i2c_master_bus_rm_device");
  }

  return ret;
}

// ---- Zephyr I2C implementation ----
#elif defined(__zephyr__)

inline error_t i2c_bus_create(struct I2CConfig *config) {
  AD_LOGI("i2c_bus_create");
  assert(config != NULL);

  struct device *dev = (struct device *)config->p_wire;
  assert(dev != NULL);

  if (!device_is_ready(dev)) {
    AD_LOGE("I2C device not ready");
    return RESULT_FAIL;
  }

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

inline void i2c_bus_delete(i2c_bus_handle_t bus) {
  (void)bus;
}

inline error_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                                   int reglen, uint8_t *data, int datalen) {
  AD_LOGD("i2c_bus_write_bytes: addr=0x%X reglen=%d datalen=%d reg=0x%02X",
          addr, reglen, datalen, reglen > 0 ? reg[0] : 0);

  struct device *dev = (struct device *)bus;
  assert(dev != NULL);

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

inline error_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg,
                                  int reglen, uint8_t *outdata, int datalen) {
  AD_LOGD("i2c_bus_read_bytes: addr=0x%X reglen=%d datalen=%d reg=0x%02X", addr,
          reglen, datalen, reglen > 0 ? reg[0] : 0);

  struct device *dev = (struct device *)bus;
  assert(reglen == 1);
  assert(dev != NULL);

  memset(outdata, 0, datalen);

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

#endif  // platform selection

} // namespace audio_driver
