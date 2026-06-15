#pragma once

#include <assert.h>
#include <stdint.h>

#include "DriverCommon.h"
#include "Platforms/API_Delay.h"
#include "Platforms/API_I2C.h"

namespace audio_driver {

/**
 * @brief Common parent class for codec drivers that have been ported from
 * the Zephyr RTOS audio driver implementations (drivers/audio).
 *
 * It provides the basic I2C register access helpers (8 bit and 16 bit
 * register values addressed via an 8 bit register address) that are used by
 * most of these drivers. Chip specific classes can add additional register
 * access methods (e.g. for paged/book based register maps) by using the
 * protected wire and i2c_addr members directly.
 */
class ZephyrDriverCommon {
 public:

  /// Defines the I2C address of the codec
  void setAddress(uint8_t addr) { i2c_addr = addr; }

  /// Provides the actual I2C address of the codec
  uint8_t address() { return i2c_addr; }

  /// Defines the I2C communication object
  void setWire(i2c_bus_handle_t w) { wire = w; }

  /// Provides the actual I2C communication object
  i2c_bus_handle_t getWire() { return wire; }

  /// Initializes the codec 
  virtual bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
                      i2s_format_t fmt, bool is_master, uint8_t channels) = 0;

  /// Defines the output volume in % (0...100). Chip specific subclasses map
  /// this to their native volume range.
  virtual bool setVolume(int volume) {
    volume_percent = volume;
    return true;
  }

  /// Provides the output volume in % (0...100) that was set with setVolume()
  virtual int getVolume() { return volume_percent; }

  /// Mutes/unmutes all outputs. Chip specific subclasses map this to their
  /// native mute functionality.
  virtual bool setMute(bool mute) {
    (void)mute;
    return false;
  }

  /// Activates/deactivates the playback and/or capture path at runtime
  /// (without reconfiguring the codec), based on codec_mode_t
  /// (CODEC_MODE_DECODE: playback active, CODEC_MODE_ENCODE: capture
  /// active). By default this just mutes/unmutes all outputs depending on
  /// CODEC_MODE_DECODE; chip specific subclasses that support muting the
  /// input path independently override this to also mute/unmute the
  /// capture path depending on CODEC_MODE_ENCODE.
  virtual bool setActive(codec_mode_t mode) {
    return setMute(!(mode & CODEC_MODE_DECODE));
  }

  /// Defines the input volume in % (0...100). Chip specific subclasses map
  /// this to their native input volume range.
  virtual bool setInputVolume(int volume) {
    (void)volume;
    return false;
  }

  /// Provides the input volume in % (0...100) that was set with
  /// setInputVolume()
  virtual int getInputVolume() { return input_volume_percent; }

  /// Returns true if the driver supports setting the input volume, false otherwise. By default we return false, but some drivers (e.g. WM8962) override this to return true.
  virtual bool isInputVolumeSupported() { return false;}

  /// Selects the ADC input source / DAC output destination. By default this
  /// is a no-op; chip specific subclasses that support input/output routing
  /// override this to configure the corresponding registers.
  virtual bool setDevices(input_device_t input_device, output_device_t output_device){ return true;}

 protected:
  i2c_bus_handle_t wire = nullptr;
  uint8_t i2c_addr = 0;
  /// Last volume (in %) provided to setVolume()
  int volume_percent = 100;
  /// Last input volume (in %) provided to setInputVolume()
  int input_volume_percent = 100;

  /// Writes a single byte to an 8 bit register address
  bool writeReg(uint8_t reg, uint8_t value) {
    assert(wire != nullptr);
    return i2c_bus_write_bytes(wire, i2c_addr, &reg, 1, &value, 1) == 0;
  }

  /// Reads a single byte from an 8 bit register address
  bool readReg(uint8_t reg, uint8_t& value) {
    assert(wire != nullptr);
    return i2c_bus_read_bytes(wire, i2c_addr, &reg, 1, &value, 1) == 0;
  }

  /// Read-Modify-Write of a single byte register
  bool updateReg(uint8_t reg, uint8_t mask, uint8_t value) {
    uint8_t old = 0;
    if (!readReg(reg, old)) return false;
    uint8_t updated = (old & ~mask) | (value & mask);
    if (updated == old) return true;
    return writeReg(reg, updated);
  }

  /// Writes a 16 bit (big endian) value to an 8 bit register address
  bool writeReg16(uint8_t reg, uint16_t value) {
    assert(wire != nullptr);
    uint8_t data[2] = {(uint8_t)(value >> 8), (uint8_t)(value & 0xFF)};
    return i2c_bus_write_bytes(wire, i2c_addr, &reg, 1, data, 2) == 0;
  }

  /// Reads a 16 bit (big endian) value from an 8 bit register address
  bool readReg16(uint8_t reg, uint16_t& value) {
    assert(wire != nullptr);
    uint8_t data[2] = {0, 0};
    if (i2c_bus_read_bytes(wire, i2c_addr, &reg, 1, data, 2) != 0)
      return false;
    value = ((uint16_t)data[0] << 8) | data[1];
    return true;
  }

  /// Read-Modify-Write of a 16 bit (big endian) register
  bool updateReg16(uint8_t reg, uint16_t mask, uint16_t value) {
    uint16_t old = 0;
    if (!readReg16(reg, old)) return false;
    uint16_t updated = (old & ~mask) | (value & mask);
    if (updated == old) return true;
    return writeReg16(reg, updated);
  }

};

}  // namespace audio_driver
