/**
 * SGTL5000 Audio Codec driver
 *
 * Ported from the PJRC Teensy Audio Library control_sgtl5000.cpp/.h
 * (https://github.com/PaulStoffregen/Audio), Copyright (c) 2014 Paul
 * Stoffregen, MIT licensed.
 */
#pragma once

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_Delay.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"

namespace audio_driver {

/// 7-bit I2C address with CTRL_ADR0_CS pin low (normal configuration)
constexpr int SGTL5000_ADDR = 0x0A;
/// 7-bit I2C address with CTRL_ADR0_CS pin high
constexpr int SGTL5000_ADDR_CS_HIGH = 0x2A;

/**
 * @brief Header-only driver class for the SGTL5000 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SGTL5000 {
 public:
  /// SGTL5000 register addresses
  enum class Reg : uint16_t {
    ChipId = 0x0000,
    DigPower = 0x0002,
    ClkCtrl = 0x0004,
    I2SCtrl = 0x0006,
    SssCtrl = 0x000A,
    AdcDacCtrl = 0x000E,
    DacVol = 0x0010,
    PadStrength = 0x0014,
    AnaAdcCtrl = 0x0020,
    AnaHpCtrl = 0x0022,
    AnaCtrl = 0x0024,
    LinregCtrl = 0x0026,
    RefCtrl = 0x0028,
    MicCtrl = 0x002A,
    LineOutCtrl = 0x002C,
    LineOutVol = 0x002E,
    AnaPower = 0x0030,
    PllCtrl = 0x0032,
    ClkTopCtrl = 0x0034,
    AnaStatus = 0x0036,
    ShortCtrl = 0x003C,
    DapControl = 0x0100,
  };

  /// Bit flags for the ANA_CTRL register (0x0024)
  enum AnaCtrlBits : uint16_t {
    MuteLineOut = 1 << 8,
    SelectHeadphoneLineIn = 1 << 6,
    EnableZeroCrossHp = 1 << 5,
    MuteHeadphone = 1 << 4,
    SelectAdcLineIn = 1 << 2,
    EnableZeroCrossAdc = 1 << 1,
    MuteAdc = 1 << 0,
  };

  /// I2S_MODE field of the I2S_CTRL register (0x0006)
  enum I2SMode : uint16_t {
    I2SModeI2SOrLeftJustified = 0x0,
    I2SModeRightJustified = 0x1,
    I2SModePcm = 0x2,
  };

  SGTL5000() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) { i2c_addr = addr; }

  /// @brief Initialize the SGTL5000 codec chip
  error_t init(codec_config_t* cfg) {
    if (initialized) {
      AD_LOGW("The SGTL5000 has already been initialized");
      return RESULT_OK;
    }
    initialized = true;

    // VDDD is externally driven (typical for the common breakout boards)
    writeReg(Reg::AnaPower, 0x4060);
    // VDDA & VDDIO both over 3.1V
    writeReg(Reg::LinregCtrl, 0x006C);
    // VAG=1.575V, normal ramp, +12.5% bias current
    writeReg(Reg::RefCtrl, 0x01F2);
    // LO_VAGCNTRL=1.65V, OUT_CURRENT=0.54mA
    writeReg(Reg::LineOutCtrl, 0x0F22);
    // allow up to 125mA on the short detectors
    writeReg(Reg::ShortCtrl, 0x4446);
    // enable zero cross detectors
    ana_ctrl = 0x0137;
    writeReg(Reg::AnaCtrl, ana_ctrl);

    // power up lineout, headphone, adc, dac
    writeReg(Reg::AnaPower, 0x40FF);
    // power up all digital blocks (I2S in/out, DAC, ADC, DAP)
    writeReg(Reg::DigPower, 0x0073);
    delayMs(400);

    // default line out level ~1.3Vp-p
    writeReg(Reg::LineOutVol, 0x1D1D);

    // route I2S_IN -> DAC and ADC -> I2S_OUT
    writeReg(Reg::SssCtrl, 0x0010);
    // unmute the DAC
    writeReg(Reg::AdcDacCtrl, 0x0000);
    // digital gain 0dB
    writeReg(Reg::DacVol, 0x3C3C);

    // configure the digital audio interface and sample rate
    configI2S(CODEC_MODE_BOTH, &cfg->i2s);

    // set the default headphone/lineout volume and unmute
    setVoiceVolume(70);

    return RESULT_OK;
  }

  /// @brief Deinitialize the SGTL5000 codec chip
  error_t deinit(void) {
    setVoiceMute(true);
    // power down all analogue and digital blocks
    writeReg(Reg::AnaPower, 0x0000);
    writeReg(Reg::DigPower, 0x0000);
    initialized = false;
    return RESULT_OK;
  }

  /// @brief Activate or mute the codec
  error_t ctrlStateActive(codec_mode_t mode, bool ctrl_state_active) {
    return setVoiceMute(!ctrl_state_active);
  }

  /// @brief Configure the digital audio interface format and sample rate
  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    uint16_t i2s_ctrl = 0;

    switch (iface->fmt) {
      case I2S_LEFT:
        i2s_ctrl |= (I2SModeI2SOrLeftJustified << 2) | (1 << 1);  // LRALIGN=1
        break;
      case I2S_RIGHT:
        i2s_ctrl |= (I2SModeRightJustified << 2);
        break;
      case I2S_DSP:
        i2s_ctrl |= (I2SModePcm << 2);
        break;
      case I2S_NORMAL:
      default:
        i2s_ctrl |= (I2SModeI2SOrLeftJustified << 2);  // LRALIGN=0
        break;
    }

    switch (iface->bits) {
      case BIT_LENGTH_20BITS:
        i2s_ctrl |= (0x2 << 4);
        break;
      case BIT_LENGTH_24BITS:
        i2s_ctrl |= (0x1 << 4);
        break;
      case BIT_LENGTH_32BITS:
        i2s_ctrl |= (0x0 << 4);
        break;
      case BIT_LENGTH_16BITS:
      default:
        i2s_ctrl |= (0x3 << 4);
        break;
    }

    if (iface->mode == MODE_MASTER) {
      i2s_ctrl |= (1 << 7);
    }

    writeReg(Reg::I2SCtrl, i2s_ctrl);
    writeReg(Reg::ClkCtrl, getClkCtrl(iface->rate));
    return RESULT_OK;
  }

  /// @brief Mute / unmute the headphone and line outputs
  error_t setVoiceMute(bool enable) {
    if (enable) {
      ana_ctrl |= (MuteHeadphone | MuteLineOut);
    } else {
      ana_ctrl &= ~(MuteHeadphone | MuteLineOut);
    }
    return writeReg(Reg::AnaCtrl, ana_ctrl);
  }

  /**
   * @brief Set the headphone / line output volume
   * @param volume voice volume (0~100)
   */
  error_t setVoiceVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    voice_volume = volume;

    if (volume == 0) {
      return setVoiceMute(true);
    }

    // 0x00 = +12dB ... 0x7F = -51.5dB, 0.5dB steps
    uint16_t reg_value = 0x7F - ((volume * 0x7F) / 100);
    error_t ret = writeReg(Reg::AnaHpCtrl, reg_value | (reg_value << 8));
    ret |= setVoiceMute(false);
    return ret;
  }

  /// @brief Get the headphone / line output volume (0~100)
  error_t getVoiceVolume(int* volume) {
    *volume = voice_volume;
    return RESULT_OK;
  }

  /// @brief Read a 16 bit value from a SGTL5000 register
  uint16_t readReg(Reg reg) {
    uint16_t reg_addr = static_cast<uint16_t>(reg);
    uint8_t addr_bytes[2] = {(uint8_t)(reg_addr >> 8),
                             (uint8_t)(reg_addr & 0xFF)};
    uint8_t data[2] = {0, 0};
    i2c_bus_read_bytes(i2c_handle, i2c_addr, addr_bytes, 2, data, 2);
    return ((uint16_t)data[0] << 8) | data[1];
  }

  /// @brief Write a 16 bit value to a SGTL5000 register
  error_t writeReg(Reg reg, uint16_t value) {
    uint16_t reg_addr = static_cast<uint16_t>(reg);
    uint8_t addr_bytes[2] = {(uint8_t)(reg_addr >> 8),
                             (uint8_t)(reg_addr & 0xFF)};
    uint8_t data[2] = {(uint8_t)(value >> 8), (uint8_t)(value & 0xFF)};
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, addr_bytes, 2, data, 2);
  }

  /**
   * @brief Determine the CLK_CTRL register value for the requested sample
   * rate.
   * @note Assumes the codec is an I2S slave clocked with MCLK = 256*fs.
   */
  uint16_t getClkCtrl(samplerate_t rate) {
    uint16_t sys_fs;
    switch (rate) {
      case RATE_32K:
        sys_fs = 0x0;
        break;
      case RATE_44K:
        sys_fs = 0x1;
        break;
      case RATE_96K:
        sys_fs = 0x3;
        break;
      case RATE_48K:
      default:
        sys_fs = 0x2;
        break;
    }
    // RATE_MODE=0 (SYS_FS specifies the rate), MCLK_FREQ=0 (256*Fs)
    return (sys_fs << 2);
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = SGTL5000_ADDR;
  bool initialized = false;
  uint16_t ana_ctrl = 0;
  int voice_volume = 70;
};

}  // namespace audio_driver
