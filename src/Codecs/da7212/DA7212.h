/**
 * @file DA7212.h
 * @brief Header only C++ driver for the Dialog Semiconductor DA7212 audio codec.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/da7212.c / da7212.h, Copyright 2025 NXP, Apache-2.0).
 *
 * @note The PLL is not configured by this driver: it keeps the PLL disabled
 * and uses MCLK directly as the system clock (matching the Zephyr driver's
 * default, devicetree independent, configuration path).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Digital audio interface protocol (DAI_CTRL FORMAT field)
enum class DA7212Protocol {
  I2S = 0x00,
  LeftJustified = 0x01,
  RightJustified = 0x02,
  DSPMode = 0x03,  ///< used for both PCM-A and PCM-B
};

/// Sample rate (SR register)
enum class DA7212SampleRate {
  Rate8kHz = (1U << 0),
  Rate11025Hz = (2U << 0),
  Rate12kHz = (3U << 0),
  Rate16kHz = (5U << 0),
  Rate22050Hz = (6U << 0),
  Rate24kHz = (7U << 0),
  Rate32kHz = (9U << 0),
  Rate44100Hz = (10U << 0),
  Rate48kHz = (11U << 0),
  Rate88200Hz = (14U << 0),
  Rate96kHz = (15U << 0),
};

/// Logical audio channels
enum class DA7212Channel {
  FrontLeft,
  FrontRight,
  HeadphoneLeft,
  HeadphoneRight,
  All,
};

/**
 * @brief Header only C++ driver for the DA7212 audio codec.
 *
 * Provides soft reset, basic playback/capture configuration (I2S protocol,
 * sample rate, word length), input routing and output/input volume & mute
 * control, ported from the corresponding Zephyr RTOS driver.
 */
class DA7212 : public ZephyrDriverCommon {
 public:
  // ---- Register addresses ----
  enum Reg : uint8_t {
    REG_DIG_ROUTING_DAI = 0x21,
    REG_SR = 0x22,
    REG_REFERENCES = 0x23,
    REG_PLL_FRAC_TOP = 0x24,
    REG_PLL_FRAC_BOT = 0x25,
    REG_PLL_INTEGER = 0x26,
    REG_PLL_CTRL = 0x27,
    REG_DAI_CLK_MODE = 0x28,
    REG_DAI_CTRL = 0x29,
    REG_DIG_ROUTING_DAC = 0x2A,
    REG_CIF_CTRL = 0x1D,
    REG_MIXIN_L_SELECT = 0x32,
    REG_MIXIN_R_SELECT = 0x33,
    REG_MIXIN_L_GAIN = 0x34,
    REG_ADC_L_GAIN = 0x36,
    REG_ADC_R_GAIN = 0x37,
    REG_DAC_L_GAIN = 0x45,
    REG_DAC_R_GAIN = 0x46,
    REG_CP_CTRL = 0x47,
    REG_HP_L_GAIN = 0x48,
    REG_HP_R_GAIN = 0x49,
    REG_LINE_GAIN = 0x4A,
    REG_MIXOUT_L_SELECT = 0x4B,
    REG_MIXOUT_R_SELECT = 0x4C,
    REG_AUX_L_CTRL = 0x60,
    REG_AUX_R_CTRL = 0x61,
    REG_MICBIAS_CTRL = 0x62,
    REG_MIC_1_CTRL = 0x63,
    REG_MIC_2_CTRL = 0x64,
    REG_MIXIN_L_CTRL = 0x65,
    REG_MIXIN_R_CTRL = 0x66,
    REG_ADC_L_CTRL = 0x67,
    REG_ADC_R_CTRL = 0x68,
    REG_DAC_L_CTRL = 0x69,
    REG_DAC_R_CTRL = 0x6A,
    REG_HP_L_CTRL = 0x6B,
    REG_HP_R_CTRL = 0x6C,
    REG_LINE_CTRL = 0x6D,
    REG_MIXOUT_L_CTRL = 0x6E,
    REG_MIXOUT_R_CTRL = 0x6F,
    REG_GAIN_RAMP_CTRL = 0x92,
    REG_MIC_CONFIG = 0x93,
    REG_PC_COUNT = 0x94,
    REG_CP_VOL_THRESHOLD1 = 0x95,
    REG_CP_DELAY = 0x96,
    REG_CP_DETECTOR = 0x97,
  };

  // ---- Bit masks ----
  static constexpr uint8_t CIF_CTRL_SOFT_RESET = (1U << 7);

  static constexpr uint8_t DAI_CLK_EN_MASK = (1U << 7);
  static constexpr uint8_t DAI_BCLKS_PER_WCLK_MASK = 0x03;
  static constexpr uint8_t DAI_BCLKS_PER_WCLK_BCLK32 = 0;
  static constexpr uint8_t DAI_BCLKS_PER_WCLK_BCLK64 = 1;
  static constexpr uint8_t DAI_BCLKS_PER_WCLK_BCLK128 = 2;
  static constexpr uint8_t DAI_BCLKS_PER_WCLK_BCLK256 = 3;

  static constexpr uint8_t DAI_EN_MASK = (1U << 7);
  static constexpr uint8_t DAI_OE_MASK = (1U << 6);
  static constexpr uint8_t DAI_FORMAT_MASK = 0x03;
  static constexpr uint8_t DAI_WORD_LENGTH_MASK = 0x0C;
  static constexpr uint8_t DAI_WORD_LENGTH_16B = (0U << 2);
  static constexpr uint8_t DAI_WORD_LENGTH_20B = (1U << 2);
  static constexpr uint8_t DAI_WORD_LENGTH_24B = (2U << 2);
  static constexpr uint8_t DAI_WORD_LENGTH_32B = (3U << 2);

  // REG_DIG_ROUTING_DAI
  static constexpr uint8_t DIG_ROUTING_DAI_R_SRC_ADC_RIGHT = (1U << 4);
  static constexpr uint8_t DIG_ROUTING_DAI_L_SRC_ADC_LEFT = (0U << 0);

  // REG_DIG_ROUTING_DAC
  static constexpr uint8_t DIG_ROUTING_DAC_R_RSC_DAC_R = (3U << 4);
  static constexpr uint8_t DIG_ROUTING_DAC_L_RSC_DAC_L = (2U << 0);
  static constexpr uint8_t DIG_ROUTING_DAC_R_RSC_ADC_R_OUTPUT = (1U << 4);
  static constexpr uint8_t DIG_ROUTING_DAC_L_RSC_ADC_L_OUTPUT = (0U << 0);

  static constexpr uint8_t REFERENCES_BIAS_EN_MASK = (1U << 3);
  static constexpr uint8_t PLL_FBDIV_INTEGER_RESET_VALUE = 0x20;

  // REG_HP_x_GAIN / REG_ADC_x_GAIN
  static constexpr uint8_t HP_GAIN_MASK = 0x3F;
  static constexpr uint8_t ADC_GAIN_MASK = 0x7F;
  static constexpr uint8_t HP_DEFAULT_GAIN = 0x2F;
  static constexpr uint8_t DAC_DEFAULT_GAIN = 0x67;

  // REG_HP_x_CTRL / REG_ADC_x_CTRL / REG_LINE_CTRL: mute/unmute
  static constexpr uint8_t MUTE_MASK = (3U << 6);
  static constexpr uint8_t UNMUTE_MASK = (1U << 7);  ///< *_EN bit kept set

  // REG_ADC_x_CTRL / REG_DAC_x_CTRL
  static constexpr uint8_t EN_MASK = (1U << 7);
  static constexpr uint8_t RAMP_EN_MASK = (1U << 5);

  // REG_HP_x_CTRL
  static constexpr uint8_t AMP_EN_MASK = (1U << 7);
  static constexpr uint8_t AMP_RAMP_EN_MASK = (1U << 5);
  static constexpr uint8_t AMP_ZC_EN_MASK = (1U << 4);
  static constexpr uint8_t AMP_OE_MASK = (1U << 3);

  // REG_MIXOUT_x_CTRL
  static constexpr uint8_t MIXOUT_AMP_EN_MASK = (1U << 7);
  static constexpr uint8_t MIXOUT_AMP_SOFT_MIX_EN_MASK = (1U << 4);
  static constexpr uint8_t MIXOUT_AMP_MIX_EN_MASK = (1U << 3);

  // REG_MIXOUT_x_SELECT
  static constexpr uint8_t MIXOUT_L_SELECT_DAC_L_MASK = (1U << 3);
  static constexpr uint8_t MIXOUT_R_SELECT_DAC_R_MASK = (1U << 3);

  // REG_MIXIN_x_SELECT
  static constexpr uint8_t MIXIN_L_SELECT_AUX_L_SEL_MASK = (1U << 0);
  static constexpr uint8_t MIXIN_R_SELECT_AUX_R_SEL_MASK = (1U << 0);

  // REG_MIXIN_x_CTRL
  static constexpr uint8_t MIXIN_AMP_EN_MASK = (1U << 7);
  static constexpr uint8_t MIXIN_AMP_MIX_EN_MASK = (1U << 3);

  // REG_AUX_x_CTRL
  static constexpr uint8_t AUX_AMP_EN_MASK = (1U << 7);
  static constexpr uint8_t AUX_AMP_RAMP_EN_MASK = (1U << 5);
  static constexpr uint8_t AUX_AMP_ZC_EN_MASK = (1U << 4);
  static constexpr uint8_t AUX_L_CTRL_AMP_ZC_SEL_INPUT_AUX_L_IF = (1U << 2);

  // REG_MIC_x_CTRL
  static constexpr uint8_t MIC_CTRL_AMP_IN_SEL_MIC_P = (1U << 2);

  // REG_CP_CTRL
  static constexpr uint8_t CP_CTRL_EN_MASK = (1U << 7);
  static constexpr uint8_t CP_CTRL_SMALL_SWITCH_FREQ_EN_MASK = (1U << 6);
  static constexpr uint8_t CP_CTRL_MCHANGE_OUTPUT = (3U << 4);
  static constexpr uint8_t CP_CTRL_MOD_CPVDD_1 = (3U << 2);
  static constexpr uint8_t CP_CTRL_ANALOG_VLL_LV_BOOSTS_CP = (1U << 0);

  // REG_GAIN_RAMP_CTRL
  static constexpr uint8_t GAIN_RAMP_CTRL_RATE_NR_MUL_16 = (2U << 0);

  // REG_PC_COUNT
  static constexpr uint8_t PC_COUNT_RESYNC_MASK = (1U << 1);

  // REG_CP_DELAY
  static constexpr uint8_t CP_DELAY_ON_OFF_LIMITER_AUT = (2U << 6);
  static constexpr uint8_t CP_DELAY_TAU_DELAY_4MS = (2U << 3);
  static constexpr uint8_t CP_DELAY_FCONTROL_0HZ_OR_1MHZ = (5U << 0);

  DA7212() { i2c_addr = 0x1A; }

  /**
   * @brief Initialize the codec: soft reset, default routing, PLL disabled
   * (MCLK is used directly as the system clock), DAI / protocol / sample
   * rate / word length configuration and optional playback/capture path
   * setup.
   *
   * @param protocol Digital audio interface protocol
   * @param sample_rate I2S sample rate in Hz
   * @param word_size I2S word size in bits (16, 20, 24, 32)
   * @param master true if the codec should drive BCLK/WCLK (master mode)
   * @param playback Enable & configure the output (headphone) path
   * @param capture Enable & configure the input path
   */
  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    return begin(DA7212Protocol::I2S, sample_rate, bits);
  }

  /// Initializes the codec for I2S with the given sample rate, bits per
  /// sample, enabling the playback and/or capture path depending on mode,
  /// using the given I2S data format and master/slave mode. The channels
  /// parameter is not configurable on this chip and is ignored.
  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)channels;
    DA7212Protocol protocol = DA7212Protocol::I2S;
    switch (fmt) {
      case I2S_LEFT:
        protocol = DA7212Protocol::LeftJustified;
        break;
      case I2S_RIGHT:
        protocol = DA7212Protocol::RightJustified;
        break;
      case I2S_DSP:
        protocol = DA7212Protocol::DSPMode;
        break;
      default:
        protocol = DA7212Protocol::I2S;
        break;
    }
    return begin(protocol, sample_rate, bits, is_master,
                  mode & CODEC_MODE_DECODE, mode & CODEC_MODE_ENCODE);
  }

  bool begin(DA7212Protocol protocol = DA7212Protocol::I2S,
             uint32_t sample_rate = 44100, uint8_t word_size = 16,
             bool master = false, bool playback = true,
             bool capture = true) {
    bool rc = true;

    rc &= softReset();

    /* DAI right/left output stream comes from ADC right/left */
    rc &= writeReg(REG_DIG_ROUTING_DAI,
                    DIG_ROUTING_DAI_R_SRC_ADC_RIGHT |
                        DIG_ROUTING_DAI_L_SRC_ADC_LEFT);

    /* Default sample rate 16kHz, overwritten below */
    rc &= writeReg(REG_SR, (uint8_t)DA7212SampleRate::Rate16kHz);

    /* Enable voltage reference and bias */
    rc &= writeReg(REG_REFERENCES, REFERENCES_BIAS_EN_MASK);

    /* Keep PLL disabled, use MCLK as system clock */
    rc &= writeReg(REG_PLL_FRAC_TOP, 0);
    rc &= writeReg(REG_PLL_FRAC_BOT, 0);
    rc &= writeReg(REG_PLL_INTEGER, PLL_FBDIV_INTEGER_RESET_VALUE);
    rc &= writeReg(REG_PLL_CTRL, 0x00);

    /* Default clock mode: BCLK number per WCLK = 64 */
    rc &= writeReg(REG_DAI_CLK_MODE, DAI_BCLKS_PER_WCLK_BCLK64);

    /* Enable DAI, default word length 16 bits, I2S format, output enabled */
    rc &= writeReg(REG_DAI_CTRL, DAI_EN_MASK | DAI_OE_MASK |
                                      DAI_WORD_LENGTH_16B |
                                      (uint8_t)DA7212Protocol::I2S);

    /* Route DAC to MixOut by default */
    rc &= writeReg(REG_DIG_ROUTING_DAC,
                    DIG_ROUTING_DAC_R_RSC_DAC_R | DIG_ROUTING_DAC_L_RSC_DAC_L);

    rc &= setClockMode(master, word_size);
    rc &= routeDacInput(playback);
    rc &= setProtocol(protocol);
    rc &= setAudioFormat(sample_rate, word_size);

    if (playback) rc &= configureOutput();
    if (capture) rc &= configureInput();

    return rc;
  }

  /// Soft reset of the codec (CIF_CTRL soft reset bit)
  bool softReset() { return writeReg(REG_CIF_CTRL, CIF_CTRL_SOFT_RESET); }

  /**
   * @brief Configure master/slave clock mode.
   * @param master true => codec drives BCLK/WCLK (DAI_CLK_EN=1)
   * @param word_size BCLK count per WCLK period is derived from this
   */
  bool setClockMode(bool master, uint8_t word_size) {
    if (!master) {
      return updateReg(REG_DAI_CLK_MODE, DAI_CLK_EN_MASK, 0);
    }

    uint8_t val;
    switch (word_size) {
      case 16:
        val = DAI_BCLKS_PER_WCLK_BCLK32;
        break;
      case 32:
        val = DAI_BCLKS_PER_WCLK_BCLK64;
        break;
      case 64:
        val = DAI_BCLKS_PER_WCLK_BCLK128;
        break;
      case 128:
        val = DAI_BCLKS_PER_WCLK_BCLK256;
        break;
      default:
        val = DAI_BCLKS_PER_WCLK_BCLK32;
        break;
    }
    bool rc = updateReg(REG_DAI_CLK_MODE, DAI_CLK_EN_MASK, DAI_CLK_EN_MASK);
    rc &= updateReg(REG_DAI_CLK_MODE, DAI_BCLKS_PER_WCLK_MASK, val);
    return rc;
  }

  /// Route the DAI input to the DACs (playback) or bypass ADC->DAC
  bool routeDacInput(bool playback) {
    if (playback) {
      return writeReg(REG_DIG_ROUTING_DAC, DIG_ROUTING_DAC_R_RSC_DAC_R |
                                                DIG_ROUTING_DAC_L_RSC_DAC_L);
    }
    return writeReg(REG_DIG_ROUTING_DAC, DIG_ROUTING_DAC_R_RSC_ADC_R_OUTPUT |
                                              DIG_ROUTING_DAC_L_RSC_ADC_L_OUTPUT);
  }

  /// Configure the digital audio interface protocol (I2S, LJ, RJ, DSP)
  bool setProtocol(DA7212Protocol protocol) {
    return updateReg(REG_DAI_CTRL, DAI_FORMAT_MASK, (uint8_t)protocol);
  }

  /// Configure sample rate (SR register) and word length (DAI_CTRL)
  bool setAudioFormat(uint32_t sample_rate, uint8_t word_size = 16) {
    bool rc = true;
    DA7212SampleRate sr;

    switch (sample_rate) {
      case 8000:
        sr = DA7212SampleRate::Rate8kHz;
        break;
      case 11025:
        sr = DA7212SampleRate::Rate11025Hz;
        break;
      case 12000:
        sr = DA7212SampleRate::Rate12kHz;
        break;
      case 16000:
        sr = DA7212SampleRate::Rate16kHz;
        break;
      case 22050:
        sr = DA7212SampleRate::Rate22050Hz;
        break;
      case 24000:
        sr = DA7212SampleRate::Rate24kHz;
        break;
      case 32000:
        sr = DA7212SampleRate::Rate32kHz;
        break;
      case 44100:
        sr = DA7212SampleRate::Rate44100Hz;
        break;
      case 48000:
        sr = DA7212SampleRate::Rate48kHz;
        break;
      case 88200:
        sr = DA7212SampleRate::Rate88200Hz;
        break;
      case 96000:
        sr = DA7212SampleRate::Rate96kHz;
        break;
      default:
        return false;
    }
    rc &= writeReg(REG_SR, (uint8_t)sr);

    uint8_t wl;
    switch (word_size) {
      case 16:
        wl = DAI_WORD_LENGTH_16B;
        break;
      case 20:
        wl = DAI_WORD_LENGTH_20B;
        break;
      case 24:
        wl = DAI_WORD_LENGTH_24B;
        break;
      case 32:
        wl = DAI_WORD_LENGTH_32B;
        break;
      default:
        return false;
    }
    rc &= updateReg(REG_DAI_CTRL, DAI_WORD_LENGTH_MASK, wl);
    return rc;
  }

  /// Set the headphone output volume (0..63, HP_x_GAIN) for the given channel(s)
  bool setOutputVolume(uint8_t volume, DA7212Channel channel = DA7212Channel::All) {
    if (volume > HP_GAIN_MASK) volume = HP_GAIN_MASK;
    return updateOutGain(channel, volume);
  }

  /// Mute / unmute the headphone (and line) output for the given channel(s)
  bool setOutputMute(bool mute, DA7212Channel channel = DA7212Channel::All) {
    uint8_t val = mute ? MUTE_MASK : UNMUTE_MASK;
    switch (channel) {
      case DA7212Channel::FrontLeft:
      case DA7212Channel::HeadphoneLeft:
        return updateReg(REG_HP_L_CTRL, MUTE_MASK, val);
      case DA7212Channel::FrontRight:
      case DA7212Channel::HeadphoneRight:
        return updateReg(REG_HP_R_CTRL, MUTE_MASK, val);
      case DA7212Channel::All: {
        bool rc = true;
        rc &= updateReg(REG_HP_L_CTRL, MUTE_MASK, val);
        rc &= updateReg(REG_HP_R_CTRL, MUTE_MASK, val);
        rc &= updateReg(REG_LINE_CTRL, MUTE_MASK, val);
        return rc;
      }
      default:
        return false;
    }
  }

  /// Sets the output volume in % (0...100) for all channels
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((volume_percent * HP_GAIN_MASK) / 100);
    return setOutputVolume(vol, DA7212Channel::All);
  }

  /// Mutes/unmutes all outputs
  bool setMute(bool mute) override { return setOutputMute(mute, DA7212Channel::All); }

  /// Stores the output device selection for use by configureOutput()
  bool setDevices(input_device_t input_device, output_device_t output_device) override {
    (void)input_device;
    this->output_device = output_device;
    return true;
  }

  /// Activates/deactivates the playback and/or capture path depending on
  /// codec_mode_t by muting/unmuting the output and input independently
  bool setActive(codec_mode_t mode) override {
    bool rc = setOutputMute(!(mode & CODEC_MODE_DECODE), DA7212Channel::All);
    rc &= setInputMute(!(mode & CODEC_MODE_ENCODE), DA7212Channel::All);
    return rc;
  }

  /// Set the ADC input volume (0..127, ADC_x_GAIN) for the given channel(s)
  bool setInputVolume(uint8_t volume, DA7212Channel channel = DA7212Channel::All) {
    if (volume > ADC_GAIN_MASK) volume = ADC_GAIN_MASK;
    switch (channel) {
      case DA7212Channel::FrontLeft:
        return writeReg(REG_ADC_L_GAIN, volume);
      case DA7212Channel::FrontRight:
        return writeReg(REG_ADC_R_GAIN, volume);
      case DA7212Channel::All: {
        bool rc = true;
        rc &= writeReg(REG_ADC_L_GAIN, volume);
        rc &= writeReg(REG_ADC_R_GAIN, volume);
        return rc;
      }
      default:
        return false;
    }
  }

  /// Sets the input volume in % (0...100) for all channels
  bool setInputVolume(int volume) override {
    input_volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((input_volume_percent * ADC_GAIN_MASK) / 100);
    return setInputVolume(vol, DA7212Channel::All);
  }


  /// Returns true: the input volume is supported
  bool isInputVolumeSupported() override { return true; }

  /// Mute / unmute the ADC input for the given channel(s)
  bool setInputMute(bool mute, DA7212Channel channel = DA7212Channel::All) {
    uint8_t val = mute ? MUTE_MASK : UNMUTE_MASK;
    if (channel == DA7212Channel::FrontRight) {
      return updateReg(REG_ADC_R_CTRL, MUTE_MASK, val);
    }
    if (channel == DA7212Channel::All) {
      bool rc = true;
      rc &= updateReg(REG_ADC_L_CTRL, MUTE_MASK, val);
      rc &= updateReg(REG_ADC_R_CTRL, MUTE_MASK, val);
      return rc;
    }
    return updateReg(REG_ADC_L_CTRL, MUTE_MASK, val);
  }

  /// Route the AUX input(s) to the input mixer(s) for the given channel(s)
  bool routeInput(DA7212Channel channel = DA7212Channel::All) {
    bool rc = true;
    if (channel == DA7212Channel::FrontLeft || channel == DA7212Channel::All)
      rc &= writeReg(REG_MIXIN_L_SELECT, MIXIN_L_SELECT_AUX_L_SEL_MASK);
    if (channel == DA7212Channel::FrontRight || channel == DA7212Channel::All)
      rc &= writeReg(REG_MIXIN_R_SELECT, MIXIN_R_SELECT_AUX_R_SEL_MASK);
    return rc;
  }

  /// Route the DACs to the output mixers (MixOut)
  bool routeOutput() {
    bool rc = true;
    rc &= writeReg(REG_MIXOUT_L_SELECT, MIXOUT_L_SELECT_DAC_L_MASK);
    rc &= writeReg(REG_MIXOUT_R_SELECT, MIXOUT_R_SELECT_DAC_R_MASK);
    return rc;
  }

  /**
   * @brief Configure the output (playback) path: charge pump, DAC -> MixOut
   * routing, DACs, headphone amps, MixOut amps, default DAC/HP gain and
   * unmute.
   */
  bool configureOutput() {
    bool rc = true;

    if (output_device == DAC_OUTPUT_NONE) {
      /* keep DACs/headphone amps disabled and muted */
      rc &= setOutputMute(true, DA7212Channel::All);
      return rc;
    }

    /* Power charge pump: 0xFD */
    rc &= writeReg(REG_CP_CTRL, CP_CTRL_EN_MASK |
                                     CP_CTRL_SMALL_SWITCH_FREQ_EN_MASK |
                                     CP_CTRL_MCHANGE_OUTPUT |
                                     CP_CTRL_MOD_CPVDD_1 |
                                     CP_CTRL_ANALOG_VLL_LV_BOOSTS_CP);

    rc &= routeOutput();

    /* Enable DACs with ramp */
    rc &= writeReg(REG_DAC_L_CTRL, EN_MASK | RAMP_EN_MASK);
    rc &= writeReg(REG_DAC_R_CTRL, EN_MASK | RAMP_EN_MASK);

    /* Enable HP amps, ZC and OE */
    rc &= writeReg(REG_HP_L_CTRL, AMP_EN_MASK | AMP_RAMP_EN_MASK |
                                       AMP_ZC_EN_MASK | AMP_OE_MASK);
    rc &= writeReg(REG_HP_R_CTRL, AMP_EN_MASK | AMP_RAMP_EN_MASK |
                                       AMP_ZC_EN_MASK | AMP_OE_MASK);

    /* Enable MixOut amplifiers and mixing into HP */
    rc &= writeReg(REG_MIXOUT_L_CTRL, MIXOUT_AMP_EN_MASK |
                                           MIXOUT_AMP_SOFT_MIX_EN_MASK |
                                           MIXOUT_AMP_MIX_EN_MASK);
    rc &= writeReg(REG_MIXOUT_R_CTRL, MIXOUT_AMP_EN_MASK |
                                           MIXOUT_AMP_SOFT_MIX_EN_MASK |
                                           MIXOUT_AMP_MIX_EN_MASK);

    /* DAC gain */
    rc &= writeReg(REG_DAC_L_GAIN, DAC_DEFAULT_GAIN);
    rc &= writeReg(REG_DAC_R_GAIN, DAC_DEFAULT_GAIN);

    /* Default HP volume and unmute */
    rc &= setOutputVolume(HP_DEFAULT_GAIN, DA7212Channel::All);
    rc &= setOutputMute(false, DA7212Channel::All);

    return rc;
  }

  /**
   * @brief Configure the input (capture) path: AUX -> MixIn routing, charge
   * pump, AUX/MIC/MixIn amps, ADCs, gain ramp & charge pump delay, default
   * ADC volume and unmute.
   */
  bool configureInput() {
    bool rc = true;

    rc &= routeInput(DA7212Channel::All);

    /* Charge pump control: 0xFD */
    rc &= writeReg(REG_CP_CTRL, CP_CTRL_EN_MASK |
                                     CP_CTRL_SMALL_SWITCH_FREQ_EN_MASK |
                                     CP_CTRL_MCHANGE_OUTPUT |
                                     CP_CTRL_MOD_CPVDD_1 |
                                     CP_CTRL_ANALOG_VLL_LV_BOOSTS_CP);

    /* AUX_L_CTRL: 0xB4 */
    rc &= writeReg(REG_AUX_L_CTRL, AUX_AMP_EN_MASK | AUX_AMP_RAMP_EN_MASK |
                                        AUX_AMP_ZC_EN_MASK |
                                        AUX_L_CTRL_AMP_ZC_SEL_INPUT_AUX_L_IF);

    /* AUX_R_CTRL: 0xB0 */
    rc &= writeReg(REG_AUX_R_CTRL,
                    AUX_AMP_EN_MASK | AUX_AMP_RAMP_EN_MASK | AUX_AMP_ZC_EN_MASK);

    /* MIC_x_CTRL: 0x04 */
    rc &= writeReg(REG_MIC_1_CTRL, MIC_CTRL_AMP_IN_SEL_MIC_P);
    rc &= writeReg(REG_MIC_2_CTRL, MIC_CTRL_AMP_IN_SEL_MIC_P);

    /* MIXIN_L/R_CTRL: 0x88 */
    rc &= writeReg(REG_MIXIN_L_CTRL, MIXIN_AMP_EN_MASK | MIXIN_AMP_MIX_EN_MASK);
    rc &= writeReg(REG_MIXIN_R_CTRL, MIXIN_AMP_EN_MASK | MIXIN_AMP_MIX_EN_MASK);

    /* ADC_L/R_CTRL: 0xA0 */
    rc &= writeReg(REG_ADC_L_CTRL, EN_MASK | RAMP_EN_MASK);
    rc &= writeReg(REG_ADC_R_CTRL, EN_MASK | RAMP_EN_MASK);

    /* GAIN_RAMP_CTRL: 0x02 */
    rc &= writeReg(REG_GAIN_RAMP_CTRL, GAIN_RAMP_CTRL_RATE_NR_MUL_16);

    /* PC_COUNT: 0x02 */
    rc &= writeReg(REG_PC_COUNT, PC_COUNT_RESYNC_MASK);

    /* CP_DELAY: 0x95 */
    rc &= writeReg(REG_CP_DELAY, CP_DELAY_ON_OFF_LIMITER_AUT |
                                      CP_DELAY_TAU_DELAY_4MS |
                                      CP_DELAY_FCONTROL_0HZ_OR_1MHZ);

    /* Default ADC volume and unmute */
    rc &= setInputVolume(HP_DEFAULT_GAIN, DA7212Channel::All);
    rc &= setInputMute(false, DA7212Channel::All);

    return rc;
  }

 protected:
  /// Output device selection set via setDevices(), used by configureOutput()
  output_device_t output_device = DAC_OUTPUT_ALL;

  bool updateOutGain(DA7212Channel channel, uint8_t volume) {
    switch (channel) {
      case DA7212Channel::FrontLeft:
      case DA7212Channel::HeadphoneLeft:
        return writeReg(REG_HP_L_GAIN, volume);
      case DA7212Channel::FrontRight:
      case DA7212Channel::HeadphoneRight:
        return writeReg(REG_HP_R_GAIN, volume);
      case DA7212Channel::All: {
        bool rc = true;
        rc &= writeReg(REG_HP_L_GAIN, volume);
        rc &= writeReg(REG_HP_R_GAIN, volume);
        return rc;
      }
      default:
        return false;
    }
  }
};

}  // namespace audio_driver
