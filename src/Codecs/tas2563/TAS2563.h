/**
 * @file TAS2563.h
 * @brief Header only C++ driver for the Texas Instruments TAS2563 smart
 * amplifier.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/tas2563.c / tas2563.h, Copyright 2026 Linumiz, Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// I2S sample rate selection (TDM_CFG0 SAMP_RATE field)
enum class TAS2563SampleRate {
  Rate8kHz = 0x00,
  Rate16kHz = 0x01,
  Rate24kHz = 0x02,
  Rate32kHz = 0x03,
  Rate48kHz = 0x04,
  Rate96kHz = 0x05,
  Rate192kHz = 0x06,
};

/// Digital audio interface format
enum class TAS2563Format {
  I2S,
  LeftJustified,
};

/// Power control modes (PWR_CTL PWR_MODE field)
enum class TAS2563PowerMode {
  Active = 0x00,
  Mute = 0x01,
  Shutdown = 0x02,
  LoadDiagnostics = 0x03,
};

/**
 * @brief Header only C++ driver for the TAS2563 mono Class-D smart
 * amplifier.
 *
 * Provides software reset, TDM/I2S format configuration, amplifier output
 * level (volume) and mute/shutdown control, ported from the corresponding
 * Zephyr RTOS driver.
 */
class TAS2563 : public ZephyrDriverCommon {
 public:
  // ---- Register addresses (Book 0, Page 0) ----
  enum Reg : uint8_t {
    REG_PAGE = 0x00,
    REG_SW_RESET = 0x01,
    REG_PWR_CTL = 0x02,
    REG_PB_CFG1 = 0x03,
    REG_MISC_CFG1 = 0x04,
    REG_MISC_CFG2 = 0x05,
    REG_TDM_CFG0 = 0x06,
    REG_TDM_CFG1 = 0x07,
    REG_TDM_CFG2 = 0x08,
    REG_TDM_CFG3 = 0x09,
    REG_TDM_CFG4 = 0x0A,
    REG_TDM_CFG5 = 0x0B,
    REG_TDM_CFG6 = 0x0C,
    REG_TDM_CFG7 = 0x0D,
    REG_TDM_CFG8 = 0x0E,
    REG_TDM_CFG9 = 0x0F,
    REG_TDM_CFG10 = 0x10,
    REG_TDM_DET = 0x11,
    REG_LIM_CFG0 = 0x12,
    REG_LIM_CFG1 = 0x13,
    REG_BOP_CFG0 = 0x14,
    REG_BOP_CFG1 = 0x15,
    REG_INT_MASK0 = 0x1A,
    REG_INT_MASK1 = 0x1B,
    REG_INT_MASK2 = 0x1C,
    REG_INT_MASK3 = 0x1D,
    REG_INT_LIVE0 = 0x1F,
    REG_INT_LIVE1 = 0x20,
    REG_INT_LIVE2 = 0x21,
    REG_INT_LIVE3 = 0x22,
    REG_INT_LTCH0 = 0x24,
    REG_INT_LTCH1 = 0x25,
    REG_INT_LTCH2 = 0x26,
    REG_INT_LTCH3 = 0x27,
    REG_VBAT_MSB = 0x2A,
    REG_VBAT_LSB = 0x2B,
    REG_TEMP = 0x2C,
    REG_INT_CLK_CFG = 0x30,
    REG_DIN_PD = 0x31,
    REG_MISC = 0x32,
    REG_BOOST_CFG1 = 0x33,
    REG_BOOST_CFG2 = 0x34,
    REG_BOOST_CFG3 = 0x35,
    REG_BST_ILIM_CFG0 = 0x40,
    REG_PDM_CONFIG0 = 0x41,
    REG_PDM_CONFIG3 = 0x42,
    REG_REV_ID = 0x7D,
    REG_I2C_CKSUM = 0x7E,
    REG_BOOK = 0x7F,
  };

  // ---- SW_RESET (0x01) ----
  static constexpr uint8_t SW_RESET_BIT = 1U << 0;

  // ---- PWR_CTL (0x02) ----
  static constexpr uint8_t PWR_MODE_MASK = 0x03;  // bits[1:0]
  static constexpr uint8_t VSNS_PD_MASK = 1U << 2;
  static constexpr uint8_t ISNS_PD_MASK = 1U << 3;

  // ---- PB_CFG1 (0x03) ----
  static constexpr uint8_t AMP_LEVEL_MASK = 0x3E;  // bits[5:1]
  static constexpr uint8_t AMP_LEVEL_SHIFT = 1;
  static constexpr uint8_t DC_BLOCKER_DIS = 1U << 6;

  // Amplifier output level settings (in dBV)
  static constexpr uint8_t AMP_LEVEL_8_5DBV = 0x01;
  static constexpr uint8_t AMP_LEVEL_9_0DBV = 0x02;
  static constexpr uint8_t AMP_LEVEL_10_0DBV = 0x04;
  static constexpr uint8_t AMP_LEVEL_11_0DBV = 0x06;
  static constexpr uint8_t AMP_LEVEL_12_0DBV = 0x08;
  static constexpr uint8_t AMP_LEVEL_13_0DBV = 0x0A;
  static constexpr uint8_t AMP_LEVEL_14_0DBV = 0x0C;
  static constexpr uint8_t AMP_LEVEL_15_0DBV = 0x0E;
  static constexpr uint8_t AMP_LEVEL_16_0DBV = 0x10;
  static constexpr uint8_t AMP_LEVEL_17_0DBV = 0x12;
  static constexpr uint8_t AMP_LEVEL_18_0DBV = 0x14;
  static constexpr uint8_t AMP_LEVEL_19_0DBV = 0x16;
  static constexpr uint8_t AMP_LEVEL_20_0DBV = 0x18;
  static constexpr uint8_t AMP_LEVEL_21_0DBV = 0x1A;
  static constexpr uint8_t AMP_LEVEL_22_0DBV = 0x1C;

  // ---- MISC_CFG2 (0x05) ----
  static constexpr uint8_t I2C_GLOBAL_EN = 1U << 1;

  // ---- TDM_CFG0 (0x06) ----
  static constexpr uint8_t FRAME_START = 1U << 0;
  static constexpr uint8_t SAMP_RATE_MASK = 0x0E;  // bits[3:1]
  static constexpr uint8_t SAMP_RATE_SHIFT = 1;
  static constexpr uint8_t AUTO_RATE = 1U << 4;
  static constexpr uint8_t RAMP_RATE = 1U << 5;
  static constexpr uint8_t CLASS_D_SYNC = 1U << 6;

  // ---- TDM_CFG1 (0x07) ----
  static constexpr uint8_t RX_EDGE = 1U << 0;
  static constexpr uint8_t RX_OFFSET_MASK = 0x3E;  // bits[5:1]
  static constexpr uint8_t RX_OFFSET_SHIFT = 1;
  static constexpr uint8_t RX_JUSTIFY = 1U << 6;

  // ---- TDM_CFG2 (0x08) ----
  static constexpr uint8_t RX_SLEN_MASK = 0x03;  // bits[1:0]
  static constexpr uint8_t RX_SLEN_16BITS = 0x00;
  static constexpr uint8_t RX_SLEN_24BITS = 0x01;
  static constexpr uint8_t RX_SLEN_32BITS = 0x02;

  static constexpr uint8_t RX_WLEN_MASK = 0x0C;  // bits[3:2]
  static constexpr uint8_t RX_WLEN_SHIFT = 2;
  static constexpr uint8_t RX_WLEN_16BITS = 0x00;
  static constexpr uint8_t RX_WLEN_20BITS = 0x01;
  static constexpr uint8_t RX_WLEN_24BITS = 0x02;
  static constexpr uint8_t RX_WLEN_32BITS = 0x03;

  static constexpr uint8_t RX_SCFG_MASK = 0x30;  // bits[5:4]
  static constexpr uint8_t RX_SCFG_SHIFT = 4;
  static constexpr uint8_t RX_SCFG_MONO_I2C = 0x00;
  static constexpr uint8_t RX_SCFG_MONO_LEFT = 0x01;
  static constexpr uint8_t RX_SCFG_MONO_RIGHT = 0x02;
  static constexpr uint8_t RX_SCFG_STEREO_DOWNMIX = 0x03;

  static constexpr uint8_t CFG2_CONFIG_MASK = 0x3F;  // bits[5:0]

  // ---- TDM_CFG3 (0x09) ----
  static constexpr uint8_t RX_SLOT_LEFT_MASK = 0x0F;
  static constexpr uint8_t RX_SLOT_RIGHT_MASK = 0xF0;

  // ---- TDM_CFG4 (0x0A) ----
  static constexpr uint8_t TX_EDGE = 1U << 0;
  static constexpr uint8_t TX_OFFSET_MASK = 0x0E;  // bits[3:1]
  static constexpr uint8_t TX_FILL = 1U << 4;
  static constexpr uint8_t TX_KEEP_EN = 1U << 5;

  // ---- INT_LTCH0 (0x24) ----
  static constexpr uint8_t INT_OTE = 1U << 0;
  static constexpr uint8_t INT_OI = 1U << 1;
  static constexpr uint8_t INT_TDMCKE = 1U << 2;

  // ---- BOOST_CFG1 (0x33) ----
  static constexpr uint8_t BOOST_EN = 1U << 5;
  static constexpr uint8_t BOOST_MODE_MASK = 0xC0;  // bits[7:6]
  static constexpr uint8_t BOOST_MODE_CLASS_H = 0x00;
  static constexpr uint8_t BOOST_MODE_CLASS_G = 0x01;
  static constexpr uint8_t BOOST_MODE_ALWAYS_ON = 0x02;
  static constexpr uint8_t BOOST_MODE_ALWAYS_OFF = 0x03;

  // ---- INT_CLK_CFG (0x30) ----
  static constexpr uint8_t IRQ_PIN_CFG_MASK = 0x03;
  static constexpr uint8_t IRQ_PIN_CFG_LIVE = 0x00;
  static constexpr uint8_t IRQ_PIN_CFG_LATCHED = 0x01;
  static constexpr uint8_t CLR_INT_LTCH = 1U << 2;

  // ---- MISC (0x32) ----
  static constexpr uint8_t IRQ_POL = 1U << 7;

  // ---- Defaults ----
  static constexpr uint8_t DEFAULT_PAGE = 0x00;
  static constexpr uint8_t DEFAULT_BOOK = 0x00;

  // ---- I2C Addresses (7-bit) ----
  static constexpr uint8_t I2C_ADDR_DEFAULT = 0x4C;
  static constexpr uint8_t I2C_ADDR_GLOBAL = 0x48;

  TAS2563() { i2c_addr = I2C_ADDR_DEFAULT; }

  /**
   * @brief Initialize the codec: software reset, set TDM/I2S format and
   * sample rate, set the default amplifier output level and put the
   * amplifier into shutdown mode (call start() to enable the output).
   *
   * @param rate I2S sample rate (one of TAS2563SampleRate)
   * @param word_size I2S word size in bits (16, 20, 24, 32)
   * @param format Digital audio interface format (I2S or left-justified)
   */
  /// Initializes the codec for I2S with the given sample rate (in Hz,
  /// mapped to the nearest supported TAS2563SampleRate) and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    TAS2563SampleRate rate = TAS2563SampleRate::Rate48kHz;
    if (sample_rate <= 8000)
      rate = TAS2563SampleRate::Rate8kHz;
    else if (sample_rate <= 16000)
      rate = TAS2563SampleRate::Rate16kHz;
    else if (sample_rate <= 24000)
      rate = TAS2563SampleRate::Rate24kHz;
    else if (sample_rate <= 32000)
      rate = TAS2563SampleRate::Rate32kHz;
    else if (sample_rate <= 48000)
      rate = TAS2563SampleRate::Rate48kHz;
    else if (sample_rate <= 96000)
      rate = TAS2563SampleRate::Rate96kHz;
    else
      rate = TAS2563SampleRate::Rate192kHz;
    return begin(rate, bits);
  }

  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)fmt;
    (void)is_master;
    (void)channels;
    return begin(sample_rate, bits);
  }

  bool begin(TAS2563SampleRate rate = TAS2563SampleRate::Rate48kHz,
             uint8_t word_size = 16,
             TAS2563Format format = TAS2563Format::I2S) {
    bool rc = true;

    rc &= softReset();

    uint8_t rev_id = 0;
    readReg(REG_REV_ID, rev_id);

    // shut down before re-configuring
    rc &= updateReg(REG_PWR_CTL, PWR_MODE_MASK,
                     (uint8_t)TAS2563PowerMode::Shutdown);

    rc &= setFormat(rate, word_size, format);

    // default amplifier level (16.0 dBV)
    rc &= setAmpLevel(AMP_LEVEL_16_0DBV);

    return rc;
  }

  /// Software reset (TAS2563_SW_RESET register)
  bool softReset() { return writeReg(REG_SW_RESET, SW_RESET_BIT); }

  /// Read the chip revision / product ID register
  bool getRevisionId(uint8_t& rev_id) { return readReg(REG_REV_ID, rev_id); }

  /**
   * @brief Configure the TDM/I2S digital audio interface: sample rate, word
   * size and frame format.
   */
  bool setFormat(TAS2563SampleRate rate, uint8_t word_size = 16,
                 TAS2563Format format = TAS2563Format::I2S) {
    bool rc = true;
    uint8_t tdm_cfg0 = 0, tdm_cfg1 = 0, tdm_cfg2 = 0;

    tdm_cfg0 |= (uint8_t)((uint8_t)rate << SAMP_RATE_SHIFT) & SAMP_RATE_MASK;

    switch (word_size) {
      case 16:
        tdm_cfg2 |= RX_WLEN_16BITS << RX_WLEN_SHIFT;
        tdm_cfg2 |= RX_SLEN_16BITS;
        break;
      case 24:
        tdm_cfg2 |= RX_WLEN_24BITS << RX_WLEN_SHIFT;
        tdm_cfg2 |= RX_SLEN_32BITS;
        break;
      case 32:
        tdm_cfg2 |= RX_WLEN_32BITS << RX_WLEN_SHIFT;
        tdm_cfg2 |= RX_SLEN_32BITS;
        break;
      default:
        return false;
    }

    switch (format) {
      case TAS2563Format::I2S:
        tdm_cfg0 |= FRAME_START;
        tdm_cfg1 |= (uint8_t)(1U << RX_OFFSET_SHIFT) & RX_OFFSET_MASK;
        break;
      case TAS2563Format::LeftJustified:
        tdm_cfg0 &= (uint8_t)~FRAME_START;
        tdm_cfg1 &= (uint8_t)~RX_OFFSET_MASK;
        break;
    }

    tdm_cfg2 |= (RX_SCFG_MONO_LEFT << RX_SCFG_SHIFT) & RX_SCFG_MASK;

    rc &= writeReg(REG_TDM_CFG0, tdm_cfg0);
    rc &= writeReg(REG_TDM_CFG1, tdm_cfg1);
    rc &= updateReg(REG_TDM_CFG2, CFG2_CONFIG_MASK, tdm_cfg2);

    return rc;
  }

  /// Set the amplifier output level register value directly (see AMP_LEVEL_* constants)
  bool setAmpLevel(uint8_t level) {
    return updateReg(REG_PB_CFG1, AMP_LEVEL_MASK,
                      (uint8_t)(level << AMP_LEVEL_SHIFT) & AMP_LEVEL_MASK);
  }

  /// Set output volume in percent (0..100), mapped to the amplifier level range
  bool setVolume(int volume) override {
    uint8_t amp_level;
    if (volume <= 0) {
      amp_level = AMP_LEVEL_8_5DBV;
    } else if (volume >= 100) {
      amp_level = AMP_LEVEL_22_0DBV;
    } else {
      amp_level = (uint8_t)(AMP_LEVEL_8_5DBV +
                             (volume * (AMP_LEVEL_22_0DBV - AMP_LEVEL_8_5DBV)) /
                                 100);
    }
    return setAmpLevel(amp_level);
  }

  /// Mute / unmute the amplifier output (Mute power mode vs. Active)
  bool setMute(bool mute) override {
    return updateReg(REG_PWR_CTL, PWR_MODE_MASK,
                      (uint8_t)(mute ? TAS2563PowerMode::Mute
                                     : TAS2563PowerMode::Active));
  }

  /// Start the amplifier output (PWR_MODE = Active)
  bool start() {
    return updateReg(REG_PWR_CTL, PWR_MODE_MASK,
                      (uint8_t)TAS2563PowerMode::Active);
  }

  /// Stop the amplifier output (PWR_MODE = Shutdown)
  bool stop() {
    return updateReg(REG_PWR_CTL, PWR_MODE_MASK,
                      (uint8_t)TAS2563PowerMode::Shutdown);
  }

  /// Set the active register page (only Page 0 is defined by this driver)
  bool selectPage(uint8_t page) { return writeReg(REG_PAGE, page); }

  /// Set the active register book (only Book 0 is defined by this driver)
  bool selectBook(uint8_t book) { return writeReg(REG_BOOK, book); }
};

}  // namespace audio_driver
