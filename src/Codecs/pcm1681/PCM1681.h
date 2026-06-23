/**
 * @file PCM1681.h
 * @brief Header only C++ driver for the Texas Instruments PCM1681 8-channel
 * audio DAC.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/pcm1681.c / pcm1681.h, Copyright (c) 2025 Basalte bv,
 * Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Digital audio interface format (PCM1681_FMTx_REG field)
enum class PCM1681Format {
  RightJustified24 = 0x00,
  RightJustified16 = 0x03,
  I2S_16_24 = 0x04,
  LeftJustified16_24 = 0x05,
  I2S_TDM_24 = 0x06,
  LeftJustifiedTDM24 = 0x07,
  I2S_DSP_24 = 0x08,
  LeftJustifiedDSP24 = 0x09,
};

/// De-emphasis sample rate selection (PCM1681_DMFx_REG field)
enum class PCM1681DeemphasisRate {
  Rate44100 = 0x00,
  Rate48000 = 0x01,
  Rate32000 = 0x02,
};

/// Digital filter roll-off (PCM1681_FLT_REG)
enum class PCM1681FilterRollOff {
  Sharp = 0x00,
  Slow = 0x01,
};

/// Output attenuation scale (PCM1681_DAMS_REG)
enum class PCM1681AttenuationScale {
  Fine = 0x00,  ///< 0.5dB steps
  Wide = 0x01,  ///< 1.0dB steps
};

/**
 * @brief Header only C++ driver for the PCM1681 8-channel audio DAC.
 *
 * Maintains a local shadow of the PCM1681 register map (matching the
 * factory default values) and writes it out via I2C. Provides per-channel
 * volume (attenuation) and mute control plus DAI format configuration,
 * ported from the corresponding Zephyr RTOS driver.
 */
class PCM1681 : public ZephyrDriverCommon {
 public:
  static constexpr int N_CHANNELS = 8;
  static constexpr int N_REGISTERS = 20;

  // ---- Register addresses ----
  enum Reg : uint8_t {
    REG_0 = 0x00,   ///< Factory use only - not accessible
    AT1x_REG = 0x01,
    AT2x_REG = 0x02,
    AT3x_REG = 0x03,
    AT4x_REG = 0x04,
    AT5x_REG = 0x05,
    AT6x_REG = 0x06,
    MUTx_REG = 0x07,
    DACx_REG = 0x08,
    FMTx_REG = 0x09,  ///< also FLT_REG (shares same register)
    SRST_REG = 0x0A,  ///< also ZREV/DREV/DMFx/DMC
    REVx_REG = 0x0B,
    FLTx_REG = 0x0C,  ///< also OVER_REG
    DAMS_REG = 0x0D,  ///< also AZROx
    REG_14 = 0x0E,    ///< read only - not writeable
    REG_15 = 0x0F,    ///< factory use only - not accessible
    AT7x_REG = 0x10,
    AT8x_REG = 0x11,
    MUT_OR_REG = 0x12,
    DAC_OR_REG = 0x13,
  };

  PCM1681() { i2c_addr = 0x4C; }

  /**
   * @brief Initialize the codec: reset the local register shadow to the
   * factory defaults, configure the DAI format and write everything to the
   * device.
   *
   * @param format Digital audio interface format
   */
  /// Initializes the codec for I2S (sample rate and word size are
  /// determined by the external I2S clock/format, not configurable here)
  bool begin(uint32_t sample_rate, uint8_t bits) {
    (void)sample_rate;
    (void)bits;
    return begin(PCM1681Format::I2S_16_24);
  }

  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)fmt;
    (void)is_master;
    (void)channels;
    return begin(sample_rate, bits);
  }

  bool begin(PCM1681Format format = PCM1681Format::I2S_16_24) {
    static const uint8_t default_reg_map[N_REGISTERS] = {
        0x00, /* register 0x00. Factory use only */
        0xFF, /* register 0x01 */
        0xFF, /* register 0x02 */
        0xFF, /* register 0x03 */
        0xFF, /* register 0x04 */
        0xFF, /* register 0x05 */
        0xFF, /* register 0x06 */
        0x00, /* register 0x07 */
        0x00, /* register 0x08 */
        0x05, /* register 0x09 */
        0x00, /* register 0x0A */
        0xFF, /* register 0x0B */
        0x0F, /* register 0x0C */
        0x00, /* register 0x0D */
        0x00, /* register 0x0E. Read only */
        0x00, /* register 0x0F. Factory use only */
        0xFF, /* register 0x10 */
        0xFF, /* register 0x11 */
        0x00, /* register 0x12 */
        0x00, /* register 0x13 */
    };

    for (int i = 0; i < N_REGISTERS; i++) reg_map[i] = default_reg_map[i];

    bool rc = setFormat(format, false);
    rc &= applyProperties();
    return rc;
  }

  /**
   * @brief Set the volume (attenuation) of a single output channel.
   *
   * @param channel channel number, 1..8
   * @param volume volume in percent, 0..100
   * @param apply write the change to the device immediately
   */
  bool setVolume(uint8_t channel, uint8_t volume, bool apply = true) {
    if (channel == 0 || channel > N_CHANNELS) return false;
    if (volume > 100) volume = 100;

    PCM1681AttenuationScale dams =
        (reg_map[DAMS_REG] & 0x80) ? PCM1681AttenuationScale::Wide
                                    : PCM1681AttenuationScale::Fine;

    uint8_t attenuation =
        (dams == PCM1681AttenuationScale::Wide)
            ? (uint8_t)(101 * volume / 100 + 154 + 0.5)
            : (uint8_t)(127 * volume / 100 + 128 + 0.5);

    uint8_t reg = attenuationReg(channel);
    reg_map[reg] = attenuation;

    return apply ? writeReg(reg, attenuation) : true;
  }

  /// Set the volume (attenuation) for all 8 channels (0..100)
  bool setVolume(uint8_t volume) {
    bool rc = true;
    for (uint8_t ch = 1; ch <= N_CHANNELS; ch++) {
      rc &= setVolume(ch, volume, false);
    }
    return rc & applyProperties();
  }

  /**
   * @brief Mute or unmute a single output channel.
   *
   * @param channel channel number, 1..8
   * @param mute true to mute
   * @param apply write the change to the device immediately
   */
  bool setMute(uint8_t channel, bool mute, bool apply = true) {
    if (channel == 0 || channel > N_CHANNELS) return false;

    uint8_t reg;
    uint8_t pos;
    if (channel < 7) {
      reg = MUTx_REG;
      pos = channel - 1;
    } else {
      reg = MUT_OR_REG;
      pos = channel - 7;
    }

    updateBit(reg, pos, mute);
    return apply ? writeReg(reg, reg_map[reg]) : true;
  }

  /// Mute or unmute all 8 channels
  bool setMute(bool mute) override {
    bool rc = true;
    for (uint8_t ch = 1; ch <= N_CHANNELS; ch++) {
      rc &= setMute(ch, mute, false);
    }
    return rc & applyProperties();
  }

  /**
   * @brief Enable or disable the DAC output of a single channel
   * (used for start_output / stop_output).
   *
   * @param channel channel number, 1..8
   * @param enable true to enable the DAC output
   * @param apply write the change to the device immediately
   */
  bool setDacEnabled(uint8_t channel, bool enable, bool apply = true) {
    if (channel == 0 || channel > N_CHANNELS) return false;

    uint8_t reg;
    uint8_t pos;
    if (channel < 7) {
      reg = DACx_REG;
      pos = channel - 1;
    } else {
      reg = DAC_OR_REG;
      pos = channel - 7;
    }

    /* DAC_ENABLED = 0, DAC_DISABLED = 1 -> invert "enable" */
    updateBit(reg, pos, !enable);
    return apply ? writeReg(reg, reg_map[reg]) : true;
  }

  /// Enable or disable the DAC output of all channels
  bool setDacEnabled(bool enable) {
    bool rc = true;
    for (uint8_t ch = 1; ch <= N_CHANNELS; ch++) {
      rc &= setDacEnabled(ch, enable, false);
    }
    return rc & applyProperties();
  }

  /**
   * @brief Configure the digital audio interface format.
   *
   * @param format Digital audio interface format
   * @param apply write the change to the device immediately
   */
  bool setFormat(PCM1681Format format, bool apply = true) {
    reg_map[FMTx_REG] =
        (uint8_t)((reg_map[FMTx_REG] & ~0x0F) | ((uint8_t)format & 0x0F));
    return apply ? writeReg(FMTx_REG, reg_map[FMTx_REG]) : true;
  }

  /// Configure the digital de-emphasis filter sample rate
  bool setDeemphasisRate(PCM1681DeemphasisRate rate, bool apply = true) {
    reg_map[SRST_REG] = (uint8_t)((reg_map[SRST_REG] & ~0x18) |
                                   (((uint8_t)rate << 3) & 0x18));
    return apply ? writeReg(SRST_REG, reg_map[SRST_REG]) : true;
  }

  /// Enable or disable the digital de-emphasis filter
  bool setDeemphasisEnabled(bool enable, bool apply = true) {
    reg_map[SRST_REG] = (uint8_t)((reg_map[SRST_REG] & ~0x01) | (enable ? 1 : 0));
    return apply ? writeReg(SRST_REG, reg_map[SRST_REG]) : true;
  }

  /// Configure the digital filter roll-off characteristic
  bool setFilterRollOff(PCM1681FilterRollOff roll_off, bool apply = true) {
    reg_map[FMTx_REG] = (uint8_t)((reg_map[FMTx_REG] & ~0x20) |
                                   (((uint8_t)roll_off << 5) & 0x20));
    return apply ? writeReg(FMTx_REG, reg_map[FMTx_REG]) : true;
  }

  /// Configure the attenuation scale (fine 0.5dB / wide 1.0dB steps)
  bool setAttenuationScale(PCM1681AttenuationScale scale, bool apply = true) {
    reg_map[DAMS_REG] = (uint8_t)((reg_map[DAMS_REG] & ~0x80) |
                                   (((uint8_t)scale << 7) & 0x80));
    return apply ? writeReg(DAMS_REG, reg_map[DAMS_REG]) : true;
  }

  /// Write the full local register shadow to the device (skips registers
  /// that are not writeable: REG_0, REG_14 and REG_15)
  bool applyProperties() {
    bool rc = true;
    for (uint8_t reg = 0; reg < N_REGISTERS; reg++) {
      if (!isWriteable(reg)) continue;
      rc &= writeReg(reg, reg_map[reg]);
    }
    return rc;
  }

 protected:
  uint8_t reg_map[N_REGISTERS] = {0};

  static bool isAccessible(uint8_t reg) { return reg != REG_0 && reg != REG_15; }

  static bool isWriteable(uint8_t reg) {
    return isAccessible(reg) && reg != REG_14;
  }

  static uint8_t attenuationReg(uint8_t channel) {
    switch (channel) {
      case 7:
        return AT7x_REG;
      case 8:
        return AT8x_REG;
      default:
        return channel;  // AT1x_REG .. AT6x_REG == 1..6
    }
  }

  void updateBit(uint8_t reg, uint8_t pos, bool value) {
    if (value) {
      reg_map[reg] |= (uint8_t)(1U << pos);
    } else {
      reg_map[reg] &= (uint8_t)~(1U << pos);
    }
  }
};

}  // namespace audio_driver
