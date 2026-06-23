/**
 * @file AW88298.h
 * @brief Header only C++ driver for the Awinic AW88298 smart amplifier.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/aw88298.c, Copyright 2025 TOKITA Hiroshi, Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Digital audio interface mode (AW88298_REG_I2SCTRL I2SMD field)
enum class AW88298Mode {
  I2S = 4,
  LeftJustified = 5,
  RightJustified = 6,
};

/// I2S word size codes (AW88298_REG_I2SCTRL I2SFS / I2SBCK fields)
enum class AW88298WordSize {
  Bits16,
  Bits20,
  Bits24,
  Bits32,
};

/**
 * @brief Header only C++ driver for the AW88298 smart amplifier.
 *
 * The AW88298 is controlled via a set of 16-bit (big endian) registers
 * addressed with an 8-bit register address, so the ZephyrDriver
 * writeReg16/readReg16/updateReg16 helpers are used.
 *
 * The reset pin (if present on the board) must be driven externally
 * (the Zephyr driver toggles a devicetree-defined GPIO during init when
 * available, otherwise it issues a software reset via AW88298_REG_ID).
 */
class AW88298 : public ZephyrDriverCommon {
 public:
  // ---- Register addresses ----
  enum Reg : uint8_t {
    REG_ID = 0x00,
    REG_SYSST = 0x01,
    REG_SYSINT = 0x02,
    REG_SYSINTM = 0x03,
    REG_SYSCTRL = 0x04,
    REG_SYSCTRL2 = 0x05,
    REG_I2SCTRL = 0x06,
    REG_HAGCCFG1 = 0x09,
    REG_HAGCCFG2 = 0x0A,
    REG_HAGCCFG3 = 0x0B,
    REG_HAGCCFG4 = 0x0C,
  };

  // ---- SYSCTRL bits ----
  static constexpr uint16_t SYSCTRL_PWDN = (1U << 0);
  static constexpr uint16_t SYSCTRL_AMPPD = (1U << 1);
  static constexpr uint16_t SYSCTRL_I2SEN = (1U << 6);

  // ---- SYSCTRL2 bits ----
  static constexpr uint16_t SYSCTRL2_HMUTE = (1U << 4);

  // ---- I2SCTRL fields ----
  static constexpr uint16_t I2SCTRL_I2SSR_MASK = (0x0FU << 0);
  static constexpr uint16_t I2SCTRL_I2SBCK_MASK = (0x03U << 4);
  static constexpr uint16_t I2SCTRL_I2SFS_MASK = (0x03U << 6);
  static constexpr uint16_t I2SCTRL_I2SMD_MASK = (0x07U << 8);

  // ---- HAGCCFG4 fields ----
  static constexpr uint16_t HAGCCFG4_VOL_MASK = (0xFFU << 8);

  /// Magic value written to REG_ID to trigger a software reset
  static constexpr uint16_t ID_SOFTRESET = 0x55AA;

  static constexpr int VOLUME_DB_MAX = 0;
  static constexpr int VOLUME_DB_MIN = -96;

  /// AW88298 I2C address is board dependent; 0x36 is a common default
  AW88298() { i2c_addr = 0x36; }

  /**
   * @brief Initialize the amplifier: software reset, power up
   * (clear PWDN/AMPPD), unmute, and configure the digital audio
   * interface (mode, word size, sample rate).
   *
   * @param mode I2S protocol variant
   * @param word_size I2S word size in bits (16, 20, 24, 32)
   * @param sample_rate I2S sample rate in Hz
   */
  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    return begin(AW88298Mode::I2S, bits, sample_rate);
  }

  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)fmt;
    (void)is_master;
    (void)channels;
    return begin(sample_rate, bits);
  }

  bool begin(AW88298Mode mode = AW88298Mode::I2S, uint8_t word_size = 16,
             uint32_t sample_rate = 44100) {
    bool rc = true;

    rc &= softReset();
    delayMs(50);

    // Power up: clear AMPPD and PWDN
    rc &= updateReg16(REG_SYSCTRL, SYSCTRL_AMPPD | SYSCTRL_PWDN, 0);
    // Unmute
    rc &= updateReg16(REG_SYSCTRL2, SYSCTRL2_HMUTE, 0);

    rc &= setFormat(mode, word_size, sample_rate);

    return rc;
  }

  /// Trigger a software reset via REG_ID
  bool softReset() { return updateReg16(REG_ID, 0xFFFF, ID_SOFTRESET); }

  /**
   * @brief Configure the digital audio interface: protocol mode, word
   * size and sample rate, and enable the I2S interface (I2SEN).
   * The AW88298 always operates as I2S clock slave.
   */
  bool setFormat(AW88298Mode mode, uint8_t word_size, uint32_t sample_rate) {
    uint16_t fs_code, bck_code;
    if (!wordSizeCodes(word_size, fs_code, bck_code)) return false;

    uint16_t rate_code;
    if (!sampleRateCode(sample_rate, rate_code)) return false;

    bool rc = true;
    // Enable I2S interface, clear power down
    rc &= updateReg16(REG_SYSCTRL, SYSCTRL_I2SEN | SYSCTRL_PWDN,
                       SYSCTRL_I2SEN);

    uint16_t mask = I2SCTRL_I2SMD_MASK | I2SCTRL_I2SFS_MASK |
                    I2SCTRL_I2SBCK_MASK | I2SCTRL_I2SSR_MASK;
    uint16_t value = (((uint16_t)mode << 8) & I2SCTRL_I2SMD_MASK) |
                      ((fs_code << 6) & I2SCTRL_I2SFS_MASK) |
                      ((bck_code << 4) & I2SCTRL_I2SBCK_MASK) |
                      (rate_code & I2SCTRL_I2SSR_MASK);
    rc &= updateReg16(REG_I2SCTRL, mask, value);

    return rc;
  }

  /// Enable the power amplifier output stage (clears AMPPD)
  bool startOutput() { return updateReg16(REG_SYSCTRL, SYSCTRL_AMPPD, 0); }

  /// Disable the power amplifier output stage (sets AMPPD)
  bool stopOutput() {
    return updateReg16(REG_SYSCTRL, SYSCTRL_AMPPD, 0xFFFF);
  }

  /// Mute / unmute the output (HAGCCFG4/SYSCTRL2 HMUTE bit)
  bool setMute(bool mute) override {
    return updateReg16(REG_SYSCTRL2, SYSCTRL2_HMUTE,
                        mute ? SYSCTRL2_HMUTE : 0);
  }

  /**
   * @brief Set the output volume in dB.
   * @param db volume in dB, range [-96..0]
   */
  bool setVolumeDb(int db) {
    if (db < VOLUME_DB_MIN || db > VOLUME_DB_MAX) return false;
    uint16_t volume_field = ((uint16_t)db2vol(db) << 8) & HAGCCFG4_VOL_MASK;
    return updateReg16(REG_HAGCCFG4, HAGCCFG4_VOL_MASK, volume_field);
  }

  /// Sets the output volume in % (0...100), mapped to [VOLUME_DB_MIN..VOLUME_DB_MAX] dB
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    int db = VOLUME_DB_MIN + (volume_percent * (VOLUME_DB_MAX - VOLUME_DB_MIN)) / 100;
    return setVolumeDb(db);
  }

 protected:
  /// Convert a dB attenuation value (<=0) to the AW88298 volume code
  /// (high nibble = 6dB steps, low nibble = 0.5dB steps)
  static uint8_t db2vol(int db) {
    if (db > 0) return 0;
    int hi = (-db) / 6;
    if (hi > 0xF) hi = 0xF;
    uint8_t lo = (uint8_t)(((-db) - (hi * 6)) * 2);
    return (uint8_t)((hi << 4) | (lo & 0xF));
  }

  static bool wordSizeCodes(uint8_t word_size, uint16_t& fs_code,
                             uint16_t& bck_code) {
    switch (word_size) {
      case 16:
        fs_code = (uint16_t)AW88298WordSize::Bits16;
        bck_code = (uint16_t)AW88298WordSize::Bits16;
        return true;
      case 20:
        fs_code = (uint16_t)AW88298WordSize::Bits20;
        bck_code = (uint16_t)AW88298WordSize::Bits20;
        return true;
      case 24:
        fs_code = (uint16_t)AW88298WordSize::Bits24;
        bck_code = (uint16_t)AW88298WordSize::Bits24;
        return true;
      case 32:
        fs_code = (uint16_t)AW88298WordSize::Bits32;
        bck_code = (uint16_t)AW88298WordSize::Bits32;
        return true;
      default:
        return false;
    }
  }

  static bool sampleRateCode(uint32_t sample_rate, uint16_t& code) {
    switch (sample_rate) {
      case 8000:
        code = 0x0;
        break;
      case 11025:
        code = 0x1;
        break;
      case 16000:
        code = 0x3;
        break;
      case 22050:
        code = 0x4;
        break;
      case 24000:
        code = 0x5;
        break;
      case 32000:
        code = 0x6;
        break;
      case 44100:
        code = 0x7;
        break;
      case 48000:
        code = 0x8;
        break;
      case 96000:
        code = 0x9;
        break;
      case 192000:
        code = 0xA;
        break;
      default:
        return false;
    }
    return true;
  }
};

}  // namespace audio_driver
