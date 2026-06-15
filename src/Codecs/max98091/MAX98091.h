/**
 * @file MAX98091.h
 * @brief Header only C++ driver for the Maxim MAX98091 audio codec.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/max98091.c / max98091.h,
 * Copyright (c) 2025 Silicon Signals Pvt. Ltd., Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Digital audio interface protocol (slave mode)
enum class MAX98091Protocol {
  I2S,
  LeftJustified,
  RightJustified,
};

/// Logical audio channels
enum class MAX98091Channel {
  FrontLeft,
  FrontRight,
  HeadphoneLeft,
  HeadphoneRight,
  All,
};

/**
 * @brief Header only C++ driver for the MAX98091 audio codec.
 *
 * Provides a soft reset, basic playback configuration (protocol, sample
 * rate, word size, system clock) and output volume/mute control, ported
 * from the corresponding Zephyr RTOS driver.
 */
class MAX98091 : public ZephyrDriverCommon {
 public:
  // ---- Register addresses ----
  enum Reg : uint8_t {
    REG_SOFTWARE_RESET = 0x00,
    REG_DEVICE_STATUS = 0x01,
    REG_JACK_STATUS = 0x02,
    REG_INTERRUPT_S = 0x03,
    REG_MASTER_CLOCK = 0x04,
    REG_QUICK_SAMPLE_RATE = 0x05,
    REG_DAI_INTERFACE = 0x06,
    REG_DAC_PATH = 0x07,
    REG_MIC_DIRECT_TO_ADC = 0x08,
    REG_LINE_TO_ADC = 0x09,
    REG_ANALOG_MIC_LOOP = 0x0A,
    REG_ANALOG_LINE_LOOP = 0x0B,
    REG_RESERVED = 0x0C,
    REG_LINE_INPUT_CONFIG = 0x0D,
    REG_LINE_INPUT_LEVEL = 0x0E,
    REG_INPUT_MODE = 0x0F,
    REG_MIC1_INPUT_LEVEL = 0x10,
    REG_MIC2_INPUT_LEVEL = 0x11,
    REG_MIC_BIAS_VOLTAGE = 0x12,
    REG_DIGITAL_MIC_ENABLE = 0x13,
    REG_DIGITAL_MIC_CONFIG = 0x14,
    REG_LEFT_ADC_MIXER = 0x15,
    REG_RIGHT_ADC_MIXER = 0x16,
    REG_LEFT_ADC_LEVEL = 0x17,
    REG_RIGHT_ADC_LEVEL = 0x18,
    REG_ADC_BIQUAD_LEVEL = 0x19,
    REG_ADC_SIDETONE = 0x1A,
    REG_SYSTEM_CLOCK = 0x1B,
    REG_CLOCK_MODE = 0x1C,
    REG_CLOCK_RATIO_NI_MSB = 0x1D,
    REG_CLOCK_RATIO_NI_LSB = 0x1E,
    REG_CLOCK_RATIO_MI_MSB = 0x1F,
    REG_CLOCK_RATIO_MI_LSB = 0x20,
    REG_MASTER_MODE = 0x21,
    REG_INTERFACE_FORMAT = 0x22,
    REG_TDM_CONTROL = 0x23,
    REG_TDM_FORMAT = 0x24,
    REG_IO_CONFIGURATION = 0x25,
    REG_FILTER_CONFIG = 0x26,
    REG_DAI_PLAYBACK_LEVEL = 0x27,
    REG_DAI_PLAYBACK_LEVEL_EQ = 0x28,
    REG_LEFT_HP_MIXER = 0x29,
    REG_RIGHT_HP_MIXER = 0x2A,
    REG_HP_CONTROL = 0x2B,
    REG_LEFT_HP_VOLUME = 0x2C,
    REG_RIGHT_HP_VOLUME = 0x2D,
    REG_LEFT_SPK_MIXER = 0x2E,
    REG_RIGHT_SPK_MIXER = 0x2F,
    REG_SPK_CONTROL = 0x30,
    REG_LEFT_SPK_VOLUME = 0x31,
    REG_RIGHT_SPK_VOLUME = 0x32,
    REG_DRC_TIMING = 0x33,
    REG_DRC_COMPRESSOR = 0x34,
    REG_DRC_EXPANDER = 0x35,
    REG_DRC_GAIN = 0x36,
    REG_RCV_LOUTL_MIXER = 0x37,
    REG_RCV_LOUTL_CONTROL = 0x38,
    REG_RCV_LOUTL_VOLUME = 0x39,
    REG_LOUTR_MIXER = 0x3A,
    REG_LOUTR_CONTROL = 0x3B,
    REG_LOUTR_VOLUME = 0x3C,
    REG_JACK_DETECT = 0x3D,
    REG_INPUT_ENABLE = 0x3E,
    REG_OUTPUT_ENABLE = 0x3F,
    REG_LEVEL_CONTROL = 0x40,
    REG_DSP_FILTER_ENABLE = 0x41,
    REG_BIAS_CONTROL = 0x42,
    REG_DAC_CONTROL = 0x43,
    REG_ADC_CONTROL = 0x44,
    REG_DEVICE_SHUTDOWN = 0x45,
    REG_EQUALIZER_BASE = 0x46,
    REG_RECORD_BIQUAD_BASE = 0xAF,
    REG_DMIC3_VOLUME = 0xBE,
    REG_DMIC4_VOLUME = 0xBF,
    REG_DMIC34_BQ_PREATTEN = 0xC0,
    REG_RECORD_TDM_SLOT = 0xC1,
    REG_SAMPLE_RATE = 0xC2,
    REG_DMIC34_BIQUAD_BASE = 0xC3,
    REG_REVISION_ID = 0xFF,
  };

  // ---- M98091_REG_SOFTWARE_RESET ----
  static constexpr uint8_t SWRESET_MASK = (1 << 7);

  // ---- M98091_REG_QUICK_SAMPLE_RATE ----
  static constexpr uint8_t SR_96K_MASK = (1 << 5);
  static constexpr uint8_t SR_32K_MASK = (1 << 4);
  static constexpr uint8_t SR_48K_MASK = (1 << 3);
  static constexpr uint8_t SR_44K1_MASK = (1 << 2);
  static constexpr uint8_t SR_16K_MASK = (1 << 1);
  static constexpr uint8_t SR_8K_MASK = (1 << 0);

  // ---- M98091_REG_DAI_INTERFACE ----
  static constexpr uint8_t RJ_M_MASK = (1 << 5);
  static constexpr uint8_t RJ_S_MASK = (1 << 4);
  static constexpr uint8_t LJ_M_MASK = (1 << 3);
  static constexpr uint8_t LJ_S_MASK = (1 << 2);
  static constexpr uint8_t I2S_M_MASK = (1 << 1);
  static constexpr uint8_t I2S_S_MASK = (1 << 0);

  // ---- M98091_REG_SYSTEM_CLOCK (PSCLK) ----
  static constexpr uint8_t PSCLK_DISABLED = (0 << 4);
  static constexpr uint8_t PSCLK_DIV1 = (1 << 4);
  static constexpr uint8_t PSCLK_DIV2 = (2 << 4);
  static constexpr uint8_t PSCLK_DIV4 = (3 << 4);

  // ---- M98091_REG_MASTER_MODE ----
  static constexpr uint8_t MAS_MASK = (1 << 7);

  // ---- M98091_REG_INTERFACE_FORMAT ----
  static constexpr uint8_t RJ_MASK = (1 << 5);
  static constexpr uint8_t WCI_MASK = (1 << 4);
  static constexpr uint8_t BCI_MASK = (1 << 3);
  static constexpr uint8_t DLY_MASK = (1 << 2);
  static constexpr uint8_t WS_MASK = (3 << 0);
  static constexpr uint8_t WS_16B = (0 << 0);

  // ---- M98091_REG_IO_CONFIGURATION ----
  static constexpr uint8_t LTEN_MASK = (1 << 5);
  static constexpr uint8_t LBEN_MASK = (1 << 4);
  static constexpr uint8_t DMONO_MASK = (1 << 3);
  static constexpr uint8_t HIZOFF_MASK = (1 << 2);
  static constexpr uint8_t SDOEN_MASK = (1 << 1);
  static constexpr uint8_t SDIEN_MASK = (1 << 0);

  // ---- M98091_REG_LEFT/RIGHT_HP_MIXER ----
  static constexpr uint8_t MIXHPL_MIC2_MASK = (1 << 5);
  static constexpr uint8_t MIXHPL_MIC1_MASK = (1 << 4);
  static constexpr uint8_t MIXHPL_LINEB_MASK = (1 << 3);
  static constexpr uint8_t MIXHPL_LINEA_MASK = (1 << 2);
  static constexpr uint8_t MIXHPL_DACR_MASK = (1 << 1);
  static constexpr uint8_t MIXHPL_DACL_MASK = (1 << 0);
  static constexpr uint8_t MIXHPL_MASK = (63 << 0);

  static constexpr uint8_t MIXHPR_MIC2_MASK = (1 << 5);
  static constexpr uint8_t MIXHPR_MIC1_MASK = (1 << 4);
  static constexpr uint8_t MIXHPR_LINEB_MASK = (1 << 3);
  static constexpr uint8_t MIXHPR_LINEA_MASK = (1 << 2);
  static constexpr uint8_t MIXHPR_DACR_MASK = (1 << 1);
  static constexpr uint8_t MIXHPR_DACL_MASK = (1 << 0);
  static constexpr uint8_t MIXHPR_MASK = (63 << 0);

  // ---- M98091_REG_HP_CONTROL ----
  static constexpr uint8_t MIXHPRSEL_MASK = (1 << 5);
  static constexpr uint8_t MIXHPLSEL_MASK = (1 << 4);
  static constexpr uint8_t MIXHPRG_MASK = (3 << 2);
  static constexpr uint8_t MIXHPLG_MASK = (3 << 0);

  // ---- M98091_REG_LEFT/RIGHT_HP_VOLUME ----
  static constexpr uint8_t HPLM_MASK = (1 << 7);
  static constexpr uint8_t HPVOLL_MASK = (31 << 0);

  // ---- M98091_REG_LEFT_SPK_MIXER ----
  static constexpr uint8_t MIXSPL_MIC2_MASK = (1 << 5);
  static constexpr uint8_t MIXSPL_MIC1_MASK = (1 << 4);
  static constexpr uint8_t MIXSPL_LINEB_MASK = (1 << 3);
  static constexpr uint8_t MIXSPL_LINEA_MASK = (1 << 2);
  static constexpr uint8_t MIXSPL_DACR_MASK = (1 << 1);
  static constexpr uint8_t MIXSPL_DACL_MASK = (1 << 0);
  static constexpr uint8_t MIXSPL_MASK = (63 << 0);

  // ---- M98091_REG_RIGHT_SPK_MIXER ----
  static constexpr uint8_t SPK_SLAVE_MASK = (1 << 6);
  static constexpr uint8_t MIXSPR_MIC2_MASK = (1 << 5);
  static constexpr uint8_t MIXSPR_MIC1_MASK = (1 << 4);
  static constexpr uint8_t MIXSPR_LINEB_MASK = (1 << 3);
  static constexpr uint8_t MIXSPR_LINEA_MASK = (1 << 2);
  static constexpr uint8_t MIXSPR_DACR_MASK = (1 << 1);
  static constexpr uint8_t MIXSPR_DACL_MASK = (1 << 0);
  static constexpr uint8_t MIXSPR_MASK = (63 << 0);

  // ---- M98091_REG_SPK_CONTROL ----
  static constexpr uint8_t MIXSPRG_MASK = (3 << 2);
  static constexpr uint8_t MIXSPLG_MASK = (3 << 0);

  // ---- M98091_REG_LEFT/RIGHT_SPK_VOLUME ----
  static constexpr uint8_t SPLM_MASK = (1 << 7);
  static constexpr uint8_t SPVOLL_MASK = (63 << 0);

  // ---- M98091_REG_OUTPUT_ENABLE ----
  static constexpr uint8_t HPREN_MASK = (1 << 7);
  static constexpr uint8_t HPLEN_MASK = (1 << 6);
  static constexpr uint8_t SPREN_MASK = (1 << 5);
  static constexpr uint8_t SPLEN_MASK = (1 << 4);
  static constexpr uint8_t RCVLEN_MASK = (1 << 3);
  static constexpr uint8_t RCVREN_MASK = (1 << 2);
  static constexpr uint8_t DAREN_MASK = (1 << 1);
  static constexpr uint8_t DALEN_MASK = (1 << 0);

  // ---- M98091_REG_DEVICE_SHUTDOWN ----
  static constexpr uint8_t SHDNN_MASK = (1 << 7);

  static constexpr uint8_t DEFAULT_VOLUME = 0x2A;
  static constexpr uint8_t REVA = 0x50;

  MAX98091() { i2c_addr = 0x10; }

  /**
   * @brief Initialize the codec: soft reset, configure the system clock,
   * digital audio interface protocol, sample rate and word size, and (if
   * playback is true) configure the default output (speaker/headphone)
   * path.
   *
   * @param protocol Digital audio interface protocol (slave mode)
   * @param sample_rate I2S sample rate in Hz
   *        (8000, 16000, 32000, 44100, 48000 or 96000)
   * @param mclk_freq MCLK frequency in Hz (10MHz .. 60MHz)
   * @param word_size I2S word size in bits (only 16 bits is supported)
   * @param channels number of channels (1 = mono, 2 = stereo)
   * @param playback enable & configure the output (speaker/headphone) path
   */
  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    return begin(MAX98091Protocol::I2S, sample_rate, 12000000, bits);
  }

  /// Initializes the codec for I2S with the given sample rate, bits per
  /// sample, enabling the playback path if mode includes CODEC_MODE_DECODE,
  /// using the given I2S data format and number of channels. This chip only
  /// supports slave mode, so is_master is ignored.
  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)is_master;
    MAX98091Protocol protocol = MAX98091Protocol::I2S;
    switch (fmt) {
      case I2S_LEFT:
        protocol = MAX98091Protocol::LeftJustified;
        break;
      case I2S_RIGHT:
        protocol = MAX98091Protocol::RightJustified;
        break;
      default:
        protocol = MAX98091Protocol::I2S;
        break;
    }
    return begin(protocol, sample_rate, 12000000, bits, channels,
                  mode & CODEC_MODE_DECODE);
  }

  bool begin(MAX98091Protocol protocol = MAX98091Protocol::I2S,
             uint32_t sample_rate = 44100, uint32_t mclk_freq = 12000000,
             uint8_t word_size = 16, uint8_t channels = 2,
             bool playback = true) {
    bool rc = true;

    rc &= softReset();

    /* Put the audio codec into shutdown mode */
    rc &= writeReg(REG_DEVICE_SHUTDOWN, 0x00);

    rc &= writeReg(REG_DAC_CONTROL, 0x00);

    rc &= writeReg(REG_TDM_CONTROL, 0x00);

    /* Set DLY = 1 to conform to the I2S standard.
     * DLY is only effective when TDM = 0 */
    rc &= writeReg(REG_INTERFACE_FORMAT, DLY_MASK);

    /* Configure system clock */
    rc &= setSystemClock(mclk_freq);

    rc &= setProtocol(protocol);
    rc &= setAudioFormat(sample_rate, word_size, channels);

    if (playback) rc &= configureOutput();

    /* Bring the audio codec out of shutdown mode */
    rc &= writeReg(REG_DEVICE_SHUTDOWN, SHDNN_MASK);

    return rc;
  }

  /// Soft reset of the codec
  bool softReset() {
    bool rc = writeReg(REG_SOFTWARE_RESET, 0x01);
    delayMs(20);
    return rc;
  }

  /// Configure the digital audio interface protocol (slave mode)
  bool setProtocol(MAX98091Protocol protocol) {
    uint8_t fmt_reg = 0;
    switch (protocol) {
      case MAX98091Protocol::I2S:
        fmt_reg = I2S_S_MASK;
        break;
      case MAX98091Protocol::LeftJustified:
        fmt_reg = LJ_S_MASK;
        break;
      case MAX98091Protocol::RightJustified:
        fmt_reg = RJ_S_MASK;
        break;
      default:
        return false;
    }
    return writeReg(REG_DAI_INTERFACE, fmt_reg);
  }

  /**
   * @brief Configure the sample rate, channel count and word size of the
   * digital audio interface.
   *
   * @param sample_rate sample rate in Hz
   *        (8000, 16000, 32000, 44100, 48000 or 96000)
   * @param word_size word size in bits (only 16 bits is supported)
   * @param channels number of channels (1 = mono, 2 = stereo)
   */
  bool setAudioFormat(uint32_t sample_rate, uint8_t word_size = 16,
                      uint8_t channels = 2) {
    bool rc = true;
    uint8_t sample_rate_mask;

    switch (sample_rate) {
      case 8000:
        sample_rate_mask = SR_8K_MASK;
        break;
      case 16000:
        sample_rate_mask = SR_16K_MASK;
        break;
      case 32000:
        sample_rate_mask = SR_32K_MASK;
        break;
      case 44100:
        sample_rate_mask = SR_44K1_MASK;
        break;
      case 48000:
        sample_rate_mask = SR_48K_MASK;
        break;
      case 96000:
        sample_rate_mask = SR_96K_MASK;
        break;
      default:
        return false;
    }
    rc &= writeReg(REG_QUICK_SAMPLE_RATE, sample_rate_mask);

    uint8_t mono = (channels == 1) ? 1 : 0;
    rc &= updateReg(REG_IO_CONFIGURATION, DMONO_MASK, mono);

    // only 16 bit word size is supported by the codec's quick config
    (void)word_size;
    rc &= updateReg(REG_INTERFACE_FORMAT, WS_MASK, WS_16B);

    return rc;
  }

  /**
   * @brief Configure the master clock pre-scaler (PSCLK) based on the
   * MCLK frequency, and ensure the codec remains in slave mode.
   *
   * @param mclk_freq MCLK frequency in Hz, in the range 10MHz .. 60MHz
   */
  bool setSystemClock(uint32_t mclk_freq) {
    uint8_t psclk;
    if (mclk_freq >= 10000000 && mclk_freq <= 20000000) {
      psclk = PSCLK_DIV1;
    } else if (mclk_freq > 20000000 && mclk_freq <= 40000000) {
      psclk = PSCLK_DIV2;
    } else if (mclk_freq > 40000000 && mclk_freq <= 60000000) {
      psclk = PSCLK_DIV4;
    } else {
      return false;
    }

    bool rc = writeReg(REG_SYSTEM_CLOCK, psclk);
    rc &= updateReg(REG_MASTER_MODE, MAS_MASK, 0);
    return rc;
  }

  /// Set the output volume for the given channel(s).
  /// Range 0..31 for headphone channels, 0..63 for speaker channels.
  bool setOutputVolume(uint8_t volume, MAX98091Channel channel = MAX98091Channel::All) {
    return setVolumeOrMute(channel, volume, true);
  }

  /// Mute / unmute the output for the given channel(s)
  bool setOutputMute(bool mute, MAX98091Channel channel = MAX98091Channel::All) {
    return setVolumeOrMute(channel, mute ? 1 : 0, false);
  }

  /// Sets the output volume in % (0...100) for all channels
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((volume_percent * 31) / 100);
    return setOutputVolume(vol, MAX98091Channel::All);
  }

  /// Mutes/unmutes all outputs
  bool setMute(bool mute) override { return setOutputMute(mute, MAX98091Channel::All); }

  /**
   * @brief Configure the default playback path: route the DAC to the
   * speaker mixers, select DAC-only source for the headphone mixer,
   * enable headphone/speaker/DAC outputs and set the default volume.
   */
  bool configureOutput() {
    bool rc = true;

    rc &= updateReg(REG_IO_CONFIGURATION, SDIEN_MASK, SDIEN_MASK);

    rc &= writeReg(REG_LEFT_SPK_MIXER, MIXSPL_DACL_MASK);
    rc &= writeReg(REG_RIGHT_SPK_MIXER, MIXSPR_DACR_MASK);

    /* select DAC only source */
    rc &= writeReg(REG_HP_CONTROL, 0x00);

    /* HPREN, HPLEN, SPREN, SPLEN, DAREN, DALEN */
    rc &= writeReg(REG_OUTPUT_ENABLE, 0xf3);

    rc &= setOutputVolume(DEFAULT_VOLUME, MAX98091Channel::All);
    rc &= setOutputMute(false, MAX98091Channel::All);

    return rc;
  }

  /// Read the device revision ID register (REG_REVISION_ID).
  /// Valid MAX98091 devices report a value in the range REVA..REVA+0x0F.
  bool readRevisionId(uint8_t& id) { return readReg(REG_REVISION_ID, id); }

 protected:
  bool setVolumeOrMute(MAX98091Channel channel, uint8_t value, bool is_volume) {
    uint8_t hp_mask = is_volume ? HPVOLL_MASK : HPLM_MASK;
    uint8_t spk_mask = is_volume ? SPVOLL_MASK : SPLM_MASK;

    switch (channel) {
      case MAX98091Channel::FrontLeft:
        return updateReg(REG_LEFT_SPK_VOLUME, spk_mask, value);
      case MAX98091Channel::FrontRight:
        return updateReg(REG_RIGHT_SPK_VOLUME, spk_mask, value);
      case MAX98091Channel::HeadphoneLeft:
        return updateReg(REG_LEFT_HP_VOLUME, hp_mask, value);
      case MAX98091Channel::HeadphoneRight:
        return updateReg(REG_RIGHT_HP_VOLUME, hp_mask, value);
      case MAX98091Channel::All: {
        bool rc = true;
        rc &= updateReg(REG_LEFT_SPK_VOLUME, spk_mask, value);
        rc &= updateReg(REG_RIGHT_SPK_VOLUME, spk_mask, value);
        rc &= updateReg(REG_LEFT_HP_VOLUME, hp_mask, value);
        rc &= updateReg(REG_RIGHT_HP_VOLUME, hp_mask, value);
        return rc;
      }
      default:
        return false;
    }
  }
};

}  // namespace audio_driver
