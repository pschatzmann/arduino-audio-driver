/**
 * @file WM8904.h
 * @brief Header only C++ driver for the Wolfson/Cirrus WM8904 audio codec.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/wm8904.c / wm8904.h, Copyright 2024 NXP, Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Audio data transfer protocol
enum class WM8904Protocol {
  RightJustified = 0x0,
  LeftJustified = 0x1,
  I2S = 0x2,
  PCMA = 0x3,
  PCMB = 0x3 | (1 << 4),
};

/// SYSCLK / fs ratio
enum class WM8904FsRatio {
  Ratio64X = 0x0,
  Ratio128X = 0x1,
  Ratio192X = 0x2,
  Ratio256X = 0x3,
  Ratio384X = 0x4,
  Ratio512X = 0x5,
  Ratio768X = 0x6,
  Ratio1024X = 0x7,
  Ratio1408X = 0x8,
  Ratio1536X = 0x9,
};

/// Sample rate (WM8904_REG_CLK_RATES_1 SAMPLE_RATE field)
enum class WM8904SampleRate {
  Rate8kHz = 0x0,
  Rate12kHz = 0x1,  ///< also 11.025kHz
  Rate16kHz = 0x2,
  Rate24kHz = 0x3,  ///< also 22.05kHz
  Rate32kHz = 0x4,
  Rate48kHz = 0x5,  ///< also 44.1kHz
};

/// Logical audio channels
enum class WM8904Channel {
  FrontLeft,
  FrontRight,
  HeadphoneLeft,
  HeadphoneRight,
  All,
};

/**
 * @brief Header only C++ driver for the WM8904 audio codec.
 *
 * Provides a soft reset, basic playback/capture configuration, input
 * routing and output/input volume & mute control, ported from the
 * corresponding Zephyr RTOS driver.
 */
class WM8904 : public ZephyrDriverCommon {
 public:
  // ---- Register addresses ----
  enum Reg : uint8_t {
    REG_RESET = 0x00,
    REG_ANALOG_ADC_0 = 0x0A,
    REG_POWER_MGMT_0 = 0x0C,
    REG_POWER_MGMT_2 = 0x0E,
    REG_POWER_MGMT_3 = 0x0F,
    REG_POWER_MGMT_6 = 0x12,
    REG_CLK_RATES_0 = 0x14,
    REG_CLK_RATES_1 = 0x15,
    REG_CLK_RATES_2 = 0x16,
    REG_AUDIO_IF_0 = 0x18,
    REG_AUDIO_IF_1 = 0x19,
    REG_AUDIO_IF_2 = 0x1A,
    REG_AUDIO_IF_3 = 0x1B,
    REG_DAC_DIGITAL_VOLUME_LEFT = 0x1E,
    REG_DAC_DIGITAL_VOLUME_RIGHT = 0x1F,
    REG_DAC_DIG_1 = 0x21,
    REG_ADC_DIGITAL_VOLUME_LEFT = 0x24,
    REG_ADC_DIGITAL_VOLUME_RIGHT = 0x25,
    REG_DAC_DIG_0 = 0x27,
    REG_ANALOG_LEFT_IN_0 = 0x2C,
    REG_ANALOG_RIGHT_IN_0 = 0x2D,
    REG_ANALOG_LEFT_IN_1 = 0x2E,
    REG_ANALOG_RIGHT_IN_1 = 0x2F,
    REG_ANALOG_OUT1_LEFT = 0x39,
    REG_ANALOG_OUT1_RIGHT = 0x3A,
    REG_ANALOG_OUT2_LEFT = 0x3B,
    REG_ANALOG_OUT2_RIGHT = 0x3C,
    REG_ANALOG_OUT12_ZC = 0x3D,
    REG_DC_SERVO_0 = 0x43,
    REG_ANALOG_HP_0 = 0x5A,
    REG_CHRG_PUMP_0 = 0x62,
    REG_CLS_W_0 = 0x68,
    REG_WRT_SEQUENCER_0 = 0x6C,
    REG_WRT_SEQUENCER_3 = 0x6F,
    REG_WRT_SEQUENCER_4 = 0x70,
    REG_FLL_CONTROL_1 = 0x74,
    REG_FLL_CONTROL_2 = 0x75,
    REG_FLL_CONTROL_3 = 0x76,
    REG_FLL_CONTROL_4 = 0x77,
    REG_FLL_CONTROL_5 = 0x78,
    REG_GPIO_CONTROL_1 = 0x79,
    REG_GPIO_CONTROL_2 = 0x7A,
    REG_GPIO_CONTROL_3 = 0x7B,
    REG_GPIO_CONTROL_4 = 0x7C,
    REG_FLL_NCO_TEST_0 = 0xF7,
    REG_FLL_NCO_TEST_1 = 0xF8,
  };

  // ---- Volume defaults / limits ----
  static constexpr uint16_t OUTPUT_VOLUME_MIN = 0b000000;
  static constexpr uint16_t OUTPUT_VOLUME_MAX = 0b111111;
  static constexpr uint16_t OUTPUT_VOLUME_DEFAULT = 0b101101;
  static constexpr uint16_t INPUT_VOLUME_MIN = 0b00000;
  static constexpr uint16_t INPUT_VOLUME_MAX = 0b11111;
  static constexpr uint16_t INPUT_VOLUME_DEFAULT = 0b00101;

  // ---- Output register bit masks (REG_ANALOG_OUT1/2_LEFT/RIGHT) ----
  static constexpr uint16_t REGMASK_OUT_MUTE = 0b100000000;
  static constexpr uint16_t REGMASK_OUT_VU = 0b010000000;
  static constexpr uint16_t REGMASK_OUT_ZC = 0b001000000;
  static constexpr uint16_t REGMASK_OUT_VOL = 0b000111111;

  // ---- Input register bit masks (REG_ANALOG_LEFT/RIGHT_IN_0) ----
  static constexpr uint8_t REGMASK_IN_MUTE = 0b10000000;
  static constexpr uint8_t REGMASK_IN_VOLUME = 0b00011111;

  // ---- Input select masks (REG_ANALOG_LEFT/RIGHT_IN_1) ----
  static constexpr uint8_t REGMASK_INSEL_CMENA = 0b01000000;
  static constexpr uint8_t REGMASK_INSEL_IP_SEL_N = 0b00110000;
  static constexpr uint8_t REGMASK_INSEL_IP_SEL_P = 0b00001100;
  static constexpr uint8_t REGMASK_INSEL_MODE = 0b00000011;

  WM8904() { i2c_addr = 0x1A; }

  /**
   * @brief Initialize the codec: soft reset, power up the analog/digital
   * blocks, configure the digital audio interface protocol, sample rate /
   * word size and (optionally) enable the playback and/or capture path.
   *
   * @param protocol Digital audio interface protocol
   * @param sample_rate I2S sample rate in Hz (e.g. 44100, 48000)
   * @param word_size I2S word size in bits (16, 20, 24, 32)
   * @param playback Enable & configure the output (headphone/line) path
   * @param capture Enable & configure the input path
   */
  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    return begin(WM8904Protocol::I2S, sample_rate, bits);
  }

  /// Initializes the codec for I2S with the given sample rate, bits per
  /// sample, enabling the playback and/or capture path depending on mode and
  /// using the given I2S data format. The is_master and channels parameters
  /// are not configurable on this chip and are ignored.
  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)is_master;
    (void)channels;
    WM8904Protocol protocol = WM8904Protocol::I2S;
    switch (fmt) {
      case I2S_LEFT:
        protocol = WM8904Protocol::LeftJustified;
        break;
      case I2S_RIGHT:
        protocol = WM8904Protocol::RightJustified;
        break;
      default:
        protocol = WM8904Protocol::I2S;
        break;
    }
    return begin(protocol, sample_rate, bits, mode & CODEC_MODE_DECODE,
                  mode & CODEC_MODE_ENCODE);
  }

  bool begin(WM8904Protocol protocol = WM8904Protocol::I2S,
             uint32_t sample_rate = 44100, uint8_t word_size = 16,
             bool playback = true, bool capture = true) {
    bool rc = true;

    rc &= softReset();

    /* MCLK_INV=0, SYSCLK_SRC=0, TOCLK_RATE=0, OPCLK_ENA=1,
     * CLK_SYS_ENA=1, CLK_DSP_ENA=1, TOCLK_ENA=1 */
    rc &= writeReg16(REG_CLK_RATES_2, 0x000F);

    /* WSEQ_ENA=1, WSEQ_WRITE_INDEX=0_0000 */
    rc &= writeReg16(REG_WRT_SEQUENCER_0, 0x0100);

    /* WSEQ_ABORT=0, WSEQ_START=1, WSEQ_START_INDEX=00_0000 */
    rc &= writeReg16(REG_WRT_SEQUENCER_3, 0x0100);

    // wait until the write sequencer has finished
    uint16_t value = 1;
    int retries = 1000;
    while ((value & 1U) != 0U && retries-- > 0) {
      if (!readReg16(REG_WRT_SEQUENCER_4, value)) break;
    }

    /* TOCLK_RATE_DIV16=0, TOCLK_RATE_x4=1, SR_MODE=0, MCLK_DIV=1 */
    rc &= writeReg16(REG_CLK_RATES_0, 0xA45F);

    /* INL_ENA=1, INR_ENA=1 */
    rc &= writeReg16(REG_POWER_MGMT_0, 0x0003);

    /* HPL_PGA_ENA=1, HPR_PGA_ENA=1 */
    rc &= writeReg16(REG_POWER_MGMT_2, 0x0003);

    /* DACL_ENA=1, DACR_ENA=1, ADCL_ENA=1, ADCR_ENA=1 */
    rc &= writeReg16(REG_POWER_MGMT_6, 0x000F);

    /* ADC_OSR128=1 */
    rc &= writeReg16(REG_ANALOG_ADC_0, 0x0001);

    /* AIFADCR_SRC=1, AIFDACR_SRC=1 */
    rc &= writeReg16(REG_AUDIO_IF_0, 0x0050);

    /* DAC_OSR128=1 */
    rc &= writeReg16(REG_DAC_DIG_1, 0x0040);

    /* Enable DC servos for headphone out */
    rc &= writeReg16(REG_DC_SERVO_0, 0x0003);

    /* HPL_RMV_SHORT=1, HPL_ENA_OUTP=1, HPL_ENA_DLY=1, HPL_ENA=1,
     * HPR_RMV_SHORT=1, HPR_ENA_OUTP=1, HPR_ENA_DLY=1, HPR_ENA=1 */
    rc &= writeReg16(REG_ANALOG_HP_0, 0x00FF);

    /* CP_DYN_PWR=1 */
    rc &= writeReg16(REG_CLS_W_0, 0x0001);

    /* CP_ENA=1 */
    rc &= writeReg16(REG_CHRG_PUMP_0, 0x0001);

    rc &= setProtocol(protocol);
    rc &= setAudioFormat(sample_rate, word_size);

    if (playback) rc &= configureOutput();
    if (capture) rc &= configureInput();

    return rc;
  }

  /// Soft reset of the codec (writes 0x00 to REG_RESET)
  bool softReset() { return writeReg16(REG_RESET, 0x00); }

  /// Configure the digital audio interface protocol (I2S, LJ, RJ, PCMA/B)
  bool setProtocol(WM8904Protocol protocol) {
    return updateReg16(REG_AUDIO_IF_1, (0x0003U | (1U << 4U)),
                        (uint16_t)protocol);
  }

  /**
   * @brief Configure the sample rate and word size of the digital audio
   * interface. The SYSCLK / fs ratio is determined from the current
   * REG_CLK_RATES_0 setting (assumes MCLK_DIV = 1, i.e. SYSCLK = MCLK / 2,
   * matching the divider written in begin()).
   *
   * @param sample_rate sample rate in Hz
   * @param word_size word size in bits (16, 20, 24, 32)
   */
  bool setAudioFormat(uint32_t sample_rate, uint8_t word_size = 16) {
    bool rc = true;
    WM8904SampleRate wm_sample_rate;

    switch (sample_rate) {
      case 8000:
        wm_sample_rate = WM8904SampleRate::Rate8kHz;
        break;
      case 11025:
      case 12000:
        wm_sample_rate = WM8904SampleRate::Rate12kHz;
        break;
      case 16000:
        wm_sample_rate = WM8904SampleRate::Rate16kHz;
        break;
      case 22050:
      case 24000:
        wm_sample_rate = WM8904SampleRate::Rate24kHz;
        break;
      case 32000:
        wm_sample_rate = WM8904SampleRate::Rate32kHz;
        break;
      case 44100:
      case 48000:
        wm_sample_rate = WM8904SampleRate::Rate48kHz;
        break;
      default:
        return false;
    }

    /* Disable SYSCLK */
    rc &= writeReg16(REG_CLK_RATES_2, 0x00);

    /* Set Clock ratio and sample rate (fs ratio fixed to 256X, matches
     * MCLK_DIV=1 in REG_CLK_RATES_0 written during begin()) */
    rc &= writeReg16(REG_CLK_RATES_1, ((uint16_t)WM8904FsRatio::Ratio256X
                                        << 10U) |
                                           (uint16_t)wm_sample_rate);

    uint16_t word_size_bits = 0;
    switch (word_size) {
      case 16:
        word_size_bits = 0;
        break;
      case 20:
        word_size_bits = 1;
        break;
      case 24:
        word_size_bits = 2;
        break;
      case 32:
        word_size_bits = 3;
        break;
      default:
        word_size_bits = 0;
        break;
    }
    /* Set bit resolution */
    rc &= updateReg16(REG_AUDIO_IF_1, 0x000CU, (word_size_bits << 2U));

    /* Enable SYSCLK */
    rc &= writeReg16(REG_CLK_RATES_2, 0x1007);

    return rc;
  }

  /// Set the output volume (0..63) for the given channel(s)
  bool setOutputVolume(uint8_t volume, WM8904Channel channel = WM8904Channel::All) {
    if (volume > OUTPUT_VOLUME_MAX) volume = OUTPUT_VOLUME_MAX;
    const uint16_t val = (uint16_t)((1U << 6U) | (volume & REGMASK_OUT_VOL));
    const uint16_t mask = REGMASK_OUT_VU | REGMASK_OUT_ZC | REGMASK_OUT_VOL;
    return updateOutput(channel, val, mask);
  }

  /// Mute / unmute the output for the given channel(s)
  bool setOutputMute(bool mute, WM8904Channel channel = WM8904Channel::All) {
    const uint16_t val = (uint16_t)((mute ? 1U : 0U) << 8U);
    const uint16_t mask = REGMASK_OUT_MUTE;
    return updateOutput(channel, val, mask);
  }

  /// Sets the output volume in % (0...100) for all channels
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((volume_percent * OUTPUT_VOLUME_MAX) / 100);
    return setOutputVolume(vol, WM8904Channel::All);
  }

  /// Mutes/unmutes all outputs
  bool setMute(bool mute) override { return setOutputMute(mute, WM8904Channel::All); }

  /// Activates/deactivates the playback and/or capture path depending on
  /// codec_mode_t by muting/unmuting the output and input independently
  bool setActive(codec_mode_t mode) override {
    bool rc = setOutputMute(!(mode & CODEC_MODE_DECODE), WM8904Channel::All);
    rc &= setInputMute(!(mode & CODEC_MODE_ENCODE), WM8904Channel::All);
    return rc;
  }

  /// Set the input volume (0..31) for the given channel(s)
  bool setInputVolume(uint8_t volume, WM8904Channel channel = WM8904Channel::All) {
    if (volume > INPUT_VOLUME_MAX) volume = INPUT_VOLUME_MAX;
    const uint8_t val = (uint8_t)(volume & REGMASK_IN_VOLUME);
    const uint8_t mask = REGMASK_IN_VOLUME;
    return updateInput(channel, val, mask);
  }

  /// Sets the input volume in % (0...100) for all channels
  bool setInputVolume(int volume) override {
    input_volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((input_volume_percent * INPUT_VOLUME_MAX) / 100);
    return setInputVolume(vol, WM8904Channel::All);
  }


  /// Returns true: the input volume is supported
  bool isInputVolumeSupported() override { return true; }

  /// Mute / unmute the input for the given channel(s)
  bool setInputMute(bool mute, WM8904Channel channel = WM8904Channel::All) {
    const uint8_t val = (uint8_t)((mute ? 1U : 0U) << 7U);
    const uint8_t mask = REGMASK_IN_MUTE;
    return updateInput(channel, val, mask);
  }

  /**
   * @brief Route an analog input (1..3) to the given channel's PGA.
   * Only FrontLeft, FrontRight and All are valid channels.
   *
   * @param input input number 1..3
   */
  bool routeInput(uint32_t input, WM8904Channel channel = WM8904Channel::All) {
    if (input < 1 || input > 3) return false;

    uint8_t val = (uint8_t)(((input - 1) & 0x03) << 4U) |
                  (uint8_t)(((input - 1) & 0x03) << 2U);
    uint8_t mask = REGMASK_INSEL_CMENA | REGMASK_INSEL_IP_SEL_P |
                   REGMASK_INSEL_IP_SEL_N | REGMASK_INSEL_MODE;

    bool rc = true;
    if (channel == WM8904Channel::FrontLeft || channel == WM8904Channel::All)
      rc &= updateReg(REG_ANALOG_LEFT_IN_1, mask, val);
    if (channel == WM8904Channel::FrontRight || channel == WM8904Channel::All)
      rc &= updateReg(REG_ANALOG_RIGHT_IN_1, mask, val);
    return rc;
  }

  /// Apply the pending volume changes (set VU bit for both output channel pairs)
  bool applyProperties() {
    bool rc = true;
    rc &= updateReg16(REG_ANALOG_OUT1_LEFT, REGMASK_OUT_MUTE,
                       (uint16_t)(1U << 7U));
    rc &= updateReg16(REG_ANALOG_OUT2_LEFT, REGMASK_OUT_MUTE,
                       (uint16_t)(1U << 7U));
    return rc;
  }

  /// Configure the default output (line out) path: default volume, unmuted
  bool configureOutput() {
    bool rc = true;
    rc &= setOutputVolume((uint8_t)OUTPUT_VOLUME_DEFAULT, WM8904Channel::All);
    rc &= setOutputMute(false, WM8904Channel::All);
    rc &= applyProperties();
    return rc;
  }

  /// Configure the default input path: route input 2, default volume, unmuted
  bool configureInput() {
    bool rc = true;
    rc &= routeInput(2, WM8904Channel::FrontLeft);
    rc &= routeInput(2, WM8904Channel::FrontRight);
    rc &= setInputVolume((uint8_t)INPUT_VOLUME_DEFAULT, WM8904Channel::All);
    rc &= setInputMute(false, WM8904Channel::All);
    return rc;
  }

 protected:
  bool updateOutput(WM8904Channel channel, uint16_t val, uint16_t mask) {
    bool rc = true;
    switch (channel) {
      case WM8904Channel::FrontLeft:
        return updateReg16(REG_ANALOG_OUT2_LEFT, mask, val);
      case WM8904Channel::FrontRight:
        return updateReg16(REG_ANALOG_OUT2_RIGHT, mask, val);
      case WM8904Channel::HeadphoneLeft:
        return updateReg16(REG_ANALOG_OUT1_LEFT, mask, val);
      case WM8904Channel::HeadphoneRight:
        return updateReg16(REG_ANALOG_OUT1_RIGHT, mask, val);
      case WM8904Channel::All:
        rc &= updateReg16(REG_ANALOG_OUT1_LEFT, mask, val);
        rc &= updateReg16(REG_ANALOG_OUT1_RIGHT, mask, val);
        rc &= updateReg16(REG_ANALOG_OUT2_LEFT, mask, val);
        rc &= updateReg16(REG_ANALOG_OUT2_RIGHT, mask, val);
        return rc;
      default:
        return false;
    }
  }

  bool updateInput(WM8904Channel channel, uint8_t val, uint8_t mask) {
    bool rc = true;
    switch (channel) {
      case WM8904Channel::FrontLeft:
        return updateReg(REG_ANALOG_LEFT_IN_0, mask, val);
      case WM8904Channel::FrontRight:
        return updateReg(REG_ANALOG_RIGHT_IN_0, mask, val);
      case WM8904Channel::All:
        rc &= updateReg(REG_ANALOG_LEFT_IN_0, mask, val);
        rc &= updateReg(REG_ANALOG_RIGHT_IN_0, mask, val);
        return rc;
      default:
        return false;
    }
  }
};

}  // namespace audio_driver
