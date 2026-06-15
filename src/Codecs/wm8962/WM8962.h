/**
 * @file WM8962.h
 * @brief Header only C++ driver for the Wolfson/Cirrus WM8962 audio codec.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/wm8962.c / wm8962.h, Copyright 2025 NXP, Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Digital audio interface protocol
enum class WM8962Protocol {
  RightJustified = 0x0,
  LeftJustified = 0x1,
  I2S = 0x2,
  PCMB = 0x3 | 0x10,
  PCMA = 0x4 - 1,
};

/// Input PGA source (used for routeInput)
enum class WM8962InputPgaSource {
  Input1 = 8,
  Input2 = 4,
  Input3 = 2,
  Input4 = 1,
};

/// Logical audio channels
enum class WM8962Channel {
  FrontLeft,
  FrontRight,
  HeadphoneLeft,
  HeadphoneRight,
  All,
};

/// Write sequencer sequence ids
enum class WM8962Sequence : uint16_t {
  DACToHeadphonePowerUp = 0x80,
  AnalogueInputPowerUp = 0x92,
  ChipPowerDown = 0x9B,
  SpeakerSleep = 0xE4,
  SpeakerWake = 0xE8,
};

/**
 * @brief Header only C++ driver for the WM8962 audio codec.
 *
 * The WM8962 register map extends beyond a single byte, therefore both
 * the register address and the register value are transferred as 16 bit
 * big endian quantities (4 bytes per write, 2+2 bytes per read).
 *
 * Provides a soft reset, basic playback/capture configuration, input/output
 * routing and output/input volume & mute control, ported from the
 * corresponding Zephyr RTOS driver.
 */
class WM8962 : public ZephyrDriverCommon {
 public:
  // ---- Register addresses (16 bit) ----
  enum Reg : uint16_t {
    REG_LINVOL = 0x00,
    REG_RINVOL = 0x01,
    REG_LOUT1 = 0x02,
    REG_ROUT1 = 0x03,
    REG_CLOCK1 = 0x04,
    REG_DACCTL1 = 0x05,
    REG_DACCTL2 = 0x06,
    REG_IFACE0 = 0x07,
    REG_CLOCK2 = 0x08,
    REG_IFACE1 = 0x09,
    REG_LDAC = 0x0A,
    REG_RDAC = 0x0B,
    REG_IFACE2 = 0x0E,
    REG_RESET = 0x0F,
    REG_3D = 0x10,
    REG_ALC1 = 0x11,
    REG_ALC2 = 0x12,
    REG_ALC3 = 0x13,
    REG_NOISEG = 0x14,
    REG_LADC = 0x15,
    REG_RADC = 0x16,
    REG_ADDCTL1 = 0x17,
    REG_ADDCTL2 = 0x18,
    REG_POWER1 = 0x19,
    REG_POWER2 = 0x1A,
    REG_ADDCTL3 = 0x1B,
    REG_APOP1 = 0x1C,
    REG_APOP2 = 0x1D,
    REG_INPUT_MIXER_1 = 0x1F,
    REG_LINPATH = 0x20,
    REG_RINPATH = 0x21,
    REG_INPUTMIX = 0x22,
    REG_LEFT_INPUT_PGA = 0x25,
    REG_RIGHT_INPUT_PGA = 0x26,
    REG_MONOMIX2 = 0x27,
    REG_LOUT2 = 0x28,
    REG_ROUT2 = 0x29,
    REG_TEMP = 0x2F,
    REG_ADDCTL4 = 0x30,
    REG_CLASSD1 = 0x31,
    REG_CLASSD3 = 0x33,
    REG_CLK4 = 0x38,
    REG_DC_SERVO_0 = 0x3C,
    REG_DC_SERVO_1 = 0x3D,
    REG_ANALOG_HP_0 = 0x45,
    REG_CHARGE_PUMP_1 = 0x48,
    REG_WRITE_SEQ_CTRL_1 = 0x57,
    REG_WRITE_SEQ_CTRL_2 = 0x5A,
    REG_WRITE_SEQ_CTRL_3 = 0x5D,
    REG_LEFT_HEADPHONE_MIXER = 0x64,
    REG_RIGHT_HEADPHONE_MIXER = 0x65,
    REG_LEFT_HEADPHONE_MIXER_VOLUME = 0x66,
    REG_RIGHT_HEADPHONE_MIXER_VOLUME = 0x67,
    REG_LEFT_SPEAKER_MIXER = 0x69,
    REG_RIGHT_SPEAKER_MIXER = 0x6A,
    REG_LEFT_SPEAKER_MIXER_VOLUME = 0x6B,
    REG_RIGHT_SPEAKER_MIXER_VOLUME = 0x6C,
    REG_PLL2 = 0x81,
    REG_FLL_CTRL_1 = 0x9B,
    REG_FLL_CTRL_2 = 0x9C,
    REG_FLL_CTRL_3 = 0x9D,
    REG_FLL_CTRL_6 = 0xA0,
    REG_FLL_CTRL_7 = 0xA1,
    REG_FLL_CTRL_8 = 0xA2,
    REG_INT_STATUS_2 = 0x231,
  };

  static constexpr uint16_t WSEQ_ENA = 0x20;
  static constexpr uint16_t WSEQ_DONE_EINT_MASK = 0x80;

  static constexpr uint16_t L_CH_MUTE_MASK = 2;
  static constexpr uint16_t R_CH_MUTE_MASK = 1;

  static constexpr uint16_t CLOCK2_BCLK_DIV_MASK = 0x0F;

  static constexpr uint16_t IFACE0_FORMAT_MASK = 0x13;
  static constexpr uint16_t IFACE0_WL_MASK = 0x0C;
  static constexpr uint16_t IFACE0_WL_SHIFT = 0x02;
  static constexpr uint16_t IFACE0_WL_16BITS = 0x00;
  static constexpr uint16_t IFACE0_WL_20BITS = 0x01;
  static constexpr uint16_t IFACE0_WL_24BITS = 0x02;
  static constexpr uint16_t IFACE0_WL_32BITS = 0x03;

  // ---- Output register bit masks (REG_LOUT1/ROUT1/LOUT2/ROUT2) ----
  static constexpr uint16_t REGMASK_OUT_VU = 0b100000000;
  static constexpr uint16_t REGMASK_OUT_ZC = 0b010000000;
  static constexpr uint16_t REGMASK_OUT_VOL = 0b001111111;

  // ---- Input register bit masks (REG_LINVOL/RINVOL) ----
  static constexpr uint16_t REGMASK_IN_VU = 0b100000000;
  static constexpr uint16_t REGMASK_IN_MUTE = 0b010000000;
  static constexpr uint16_t REGMASK_IN_ZC = 0b001000000;
  static constexpr uint16_t REGMASK_IN_VOLUME = 0b000111111;

  // ---- Default volumes ----
  static constexpr uint16_t ADC_MAX_VOLUME_VALUE = 0xFF;
  static constexpr uint16_t DAC_MAX_VOLUME_VALUE = 0xFF;
  static constexpr uint16_t HEADPHONE_MAX_VOLUME_VALUE = 0x7F;
  static constexpr uint16_t HEADPHONE_MIN_VOLUME_VALUE = 0x2F;
  static constexpr uint16_t LINEIN_MAX_VOLUME_VALUE = 0x3F;
  static constexpr uint16_t SPEAKER_MAX_VOLUME_VALUE = 0x7F;
  static constexpr uint16_t SPEAKER_MIN_VOLUME_VALUE = 0x2F;

  static constexpr uint16_t ADC_DEFAULT_VOLUME_VALUE = 0x1C0;
  static constexpr uint16_t DAC_DEFAULT_VOLUME_VALUE = 0x1C0;
  static constexpr uint16_t HEADPHONE_DEFAULT_VOLUME_VALUE = 0x179;
  static constexpr uint16_t LINEIN_DEFAULT_VOLUME_VALUE = 0x12D;
  static constexpr uint16_t SPEAKER_DEFAULT_VOLUME_VALUE = 0x179;

  WM8962() { i2c_addr = 0x1A; }  // 0x34 >> 1

  /**
   * @brief Initialize the codec: soft reset, power up the analog/digital
   * blocks, run the built-in write sequences, configure the digital audio
   * interface protocol, sample rate / word size and (optionally) enable the
   * playback and/or capture path.
   *
   * @param protocol Digital audio interface protocol
   * @param sample_rate I2S sample rate in Hz (e.g. 44100, 48000)
   * @param word_size I2S word size in bits (16, 20, 24, 32)
   * @param mclk_to_fs_ratio MCLK / sample-rate ratio (e.g. 256 for
   *        sysclk = 256 * fs); used to select the SYSCLK divider
   * @param playback Enable & configure the output (headphone/speaker) path
   * @param capture Enable & configure the input path
   */
  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    return begin(WM8962Protocol::I2S, sample_rate, bits);
  }

  /// Initializes the codec for I2S with the given sample rate, bits per
  /// sample, enabling the playback and/or capture path depending on mode and
  /// using the given I2S data format. The is_master and channels parameters
  /// are not configurable on this chip and are ignored.
  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)is_master;
    (void)channels;
    WM8962Protocol protocol = WM8962Protocol::I2S;
    switch (fmt) {
      case I2S_LEFT:
        protocol = WM8962Protocol::LeftJustified;
        break;
      case I2S_RIGHT:
        protocol = WM8962Protocol::RightJustified;
        break;
      case I2S_DSP:
        protocol = WM8962Protocol::PCMA;
        break;
      default:
        protocol = WM8962Protocol::I2S;
        break;
    }
    return begin(protocol, sample_rate, bits, 256, mode & CODEC_MODE_DECODE,
                  mode & CODEC_MODE_ENCODE);
  }

  bool begin(WM8962Protocol protocol = WM8962Protocol::I2S,
             uint32_t sample_rate = 44100, uint8_t word_size = 16,
             uint32_t mclk_to_fs_ratio = 256, bool playback = true,
             bool capture = true) {
    bool rc = true;

    rc &= softReset();

    /* disable internal osc/FLL2/FLL3/FLL */
    rc &= writeRegWide(REG_PLL2, 0);
    rc &= updateRegWide(REG_FLL_CTRL_1, 1, 0);
    rc &= writeRegWide(REG_CLOCK2, 0x9E4);
    rc &= writeRegWide(REG_POWER1, 0x1FE);
    rc &= writeRegWide(REG_POWER2, 0x1E0);

    /* enable system clock */
    rc &= updateRegWide(REG_CLOCK2, 0x20, 0x20);

    runSequence(WM8962Sequence::DACToHeadphonePowerUp);
    runSequence(WM8962Sequence::AnalogueInputPowerUp);
    runSequence(WM8962Sequence::SpeakerWake);

    rc &= setProtocol(protocol);

    /* ADC volume, 0dB */
    rc &= writeRegWide(REG_LADC, ADC_DEFAULT_VOLUME_VALUE);
    rc &= writeRegWide(REG_RADC, ADC_DEFAULT_VOLUME_VALUE);
    /* Digital DAC volume, -15.5dB */
    rc &= writeRegWide(REG_LDAC, DAC_DEFAULT_VOLUME_VALUE);
    rc &= writeRegWide(REG_RDAC, DAC_DEFAULT_VOLUME_VALUE);
    /* speaker volume 6dB */
    rc &= writeRegWide(REG_LOUT2, SPEAKER_DEFAULT_VOLUME_VALUE);
    rc &= writeRegWide(REG_ROUT2, SPEAKER_DEFAULT_VOLUME_VALUE);
    /* input PGA volume */
    rc &= writeRegWide(REG_LINVOL, LINEIN_DEFAULT_VOLUME_VALUE);
    rc &= writeRegWide(REG_RINVOL, LINEIN_DEFAULT_VOLUME_VALUE);
    /* Headphone volume */
    rc &= writeRegWide(REG_LOUT1, HEADPHONE_DEFAULT_VOLUME_VALUE);
    rc &= writeRegWide(REG_ROUT1, HEADPHONE_DEFAULT_VOLUME_VALUE);

    rc &= setAudioFormat(sample_rate, word_size, mclk_to_fs_ratio);

    if (playback) rc &= configureOutput();
    if (capture) rc &= configureInput();

    return rc;
  }

  /// Soft reset of the codec (writes 0x6243 to REG_RESET)
  bool softReset() { return writeRegWide(REG_RESET, 0x6243); }

  /// Configure the digital audio interface protocol (I2S, LJ, RJ, PCMA/B)
  bool setProtocol(WM8962Protocol protocol) {
    return updateRegWide(REG_IFACE0, IFACE0_FORMAT_MASK, (uint16_t)protocol);
  }

  /**
   * @brief Configure word size, sysclk/sample-rate divider (REG_ADDCTL3) and
   * bit-clock divider (REG_CLK4 = sysclk/fs ratio).
   *
   * @param sample_rate sample rate in Hz
   * @param word_size word size in bits (16, 20, 24, 32)
   * @param mclk_to_fs_ratio ratio between sysclk and the sample rate
   *        (e.g. 256, 512, ...) used to compute REG_CLK4
   */
  bool setAudioFormat(uint32_t sample_rate, uint8_t word_size = 16,
                       uint32_t mclk_to_fs_ratio = 256) {
    bool rc = true;
    uint16_t wl_val;

    switch (word_size) {
      case 16:
        wl_val = IFACE0_WL_16BITS;
        break;
      case 20:
        wl_val = IFACE0_WL_20BITS;
        break;
      case 24:
        wl_val = IFACE0_WL_24BITS;
        break;
      case 32:
        wl_val = IFACE0_WL_32BITS;
        break;
      default:
        return false;
    }
    rc &= updateRegWide(REG_IFACE0, IFACE0_WL_MASK,
                         (uint16_t)(wl_val << IFACE0_WL_SHIFT));

    uint16_t addctl3_val;
    switch (sample_rate) {
      case 8000:
        addctl3_val = 0x15;
        break;
      case 11025:
        addctl3_val = 0x04;
        break;
      case 12000:
        addctl3_val = 0x14;
        break;
      case 16000:
        addctl3_val = 0x13;
        break;
      case 22050:
        addctl3_val = 0x02;
        break;
      case 24000:
        addctl3_val = 0x12;
        break;
      case 32000:
        addctl3_val = 0x11;
        break;
      case 44100:
        addctl3_val = 0x00;
        break;
      case 48000:
        addctl3_val = 0x10;
        break;
      case 88200:
        addctl3_val = 0x06;
        break;
      case 96000:
        addctl3_val = 0x16;
        break;
      default:
        return false;
    }
    rc &= writeRegWide(REG_ADDCTL3, addctl3_val);

    uint16_t clk4_val;
    switch (mclk_to_fs_ratio) {
      case 64:
        clk4_val = 0x00;
        break;
      case 128:
        clk4_val = 0x02;
        break;
      case 192:
        clk4_val = 0x04;
        break;
      case 256:
        clk4_val = 0x06;
        break;
      case 384:
        clk4_val = 0x08;
        break;
      case 512:
        clk4_val = 0x0A;
        break;
      case 768:
        clk4_val = 0x0C;
        break;
      case 1024:
        clk4_val = 0x0E;
        break;
      case 1536:
        clk4_val = 0x12;
        break;
      case 3072:
        clk4_val = 0x14;
        break;
      case 6144:
        clk4_val = 0x16;
        break;
      default:
        return false;
    }
    rc &= writeRegWide(REG_CLK4, clk4_val);

    return rc;
  }

  /// Set the output volume (0..127) for the given channel(s)
  bool setOutputVolume(uint8_t volume, WM8962Channel channel = WM8962Channel::All) {
    if (volume > REGMASK_OUT_VOL) volume = REGMASK_OUT_VOL;
    const uint16_t val = (uint16_t)((1U << 8U) | (volume & REGMASK_OUT_VOL));
    const uint16_t mask = REGMASK_OUT_VU | REGMASK_OUT_ZC | REGMASK_OUT_VOL;
    return updateOutput(channel, val, mask);
  }

  /// Mute / unmute the output for the given channel(s)
  bool setOutputMute(bool mute, WM8962Channel channel = WM8962Channel::All) {
    bool rc = true;
    switch (channel) {
      case WM8962Channel::FrontLeft:
        rc &= updateRegWide(REG_CLASSD1, L_CH_MUTE_MASK, mute ? 2 : 0);
        break;
      case WM8962Channel::FrontRight:
        rc &= updateRegWide(REG_CLASSD1, R_CH_MUTE_MASK, mute ? 1 : 0);
        break;
      case WM8962Channel::HeadphoneLeft:
        rc &= updateRegWide(REG_POWER2, L_CH_MUTE_MASK, mute ? 2 : 0);
        break;
      case WM8962Channel::HeadphoneRight:
        rc &= updateRegWide(REG_POWER2, R_CH_MUTE_MASK, mute ? 1 : 0);
        break;
      case WM8962Channel::All: {
        uint16_t val = mute ? 3 : 0;
        rc &= updateRegWide(REG_CLASSD1, L_CH_MUTE_MASK | R_CH_MUTE_MASK, val);
        rc &= updateRegWide(REG_POWER2, L_CH_MUTE_MASK | R_CH_MUTE_MASK, val);
        break;
      }
      default:
        return false;
    }
    return rc;
  }

  /// Sets the output volume in % (0...100) for all channels
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((volume_percent * REGMASK_OUT_VOL) / 100);
    return setOutputVolume(vol, WM8962Channel::All);
  }

  /// Mutes/unmutes all outputs
  bool setMute(bool mute) override { return setOutputMute(mute, WM8962Channel::All); }

  /// Activates/deactivates the playback and/or capture path depending on
  /// codec_mode_t by muting/unmuting the output and input independently
  bool setActive(codec_mode_t mode) override {
    bool rc = setOutputMute(!(mode & CODEC_MODE_DECODE), WM8962Channel::All);
    rc &= setInputMute(!(mode & CODEC_MODE_ENCODE), WM8962Channel::All);
    return rc;
  }

  /// Set the input volume for the given channel(s) (6 bit value)
  bool setInputVolume(uint8_t volume, WM8962Channel channel = WM8962Channel::All) {
    if (volume > REGMASK_IN_VOLUME) volume = REGMASK_IN_VOLUME;
    const uint16_t val = (uint16_t)(REGMASK_IN_VU | (volume & REGMASK_IN_VOLUME));
    const uint16_t mask = (uint16_t)(REGMASK_IN_VU | REGMASK_IN_VOLUME);
    return updateInput(channel, val, mask);
  }

  /// Sets the input volume in % (0...100) for all channels
  bool setInputVolume(int volume) override {
    input_volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    uint8_t vol = (uint8_t)((input_volume_percent * REGMASK_IN_VOLUME) / 100);
    return setInputVolume(vol, WM8962Channel::All);
  }


  /// Returns true: the input volume is supported
  bool isInputVolumeSupported() override { return true; }

  /// Mute / unmute the input for the given channel(s)
  bool setInputMute(bool mute, WM8962Channel channel = WM8962Channel::All) {
    const uint16_t val =
        (uint16_t)(REGMASK_IN_VU | ((mute ? 1U : 0U) << 7U));
    const uint16_t mask = (uint16_t)(REGMASK_IN_VU | REGMASK_IN_MUTE);
    return updateInput(channel, val, mask);
  }

  /**
   * @brief Select the input PGA source for the given channel
   * (FrontLeft / FrontRight only).
   */
  bool routeInput(WM8962InputPgaSource source, WM8962Channel channel) {
    uint16_t reg;
    switch (channel) {
      case WM8962Channel::FrontLeft:
        reg = REG_LEFT_INPUT_PGA;
        break;
      case WM8962Channel::FrontRight:
        reg = REG_RIGHT_INPUT_PGA;
        break;
      default:
        return false;
    }
    return writeRegWide(reg, (uint16_t)source);
  }

  /**
   * @brief Select the output mixer source register for the given channel
   * (the raw mixer-enable value is written as-is).
   */
  bool routeOutput(uint16_t mixer_value, WM8962Channel channel) {
    uint16_t reg;
    switch (channel) {
      case WM8962Channel::HeadphoneLeft:
        reg = REG_LEFT_HEADPHONE_MIXER;
        break;
      case WM8962Channel::HeadphoneRight:
        reg = REG_RIGHT_HEADPHONE_MIXER;
        break;
      case WM8962Channel::FrontLeft:
        reg = REG_LEFT_SPEAKER_MIXER;
        break;
      case WM8962Channel::FrontRight:
        reg = REG_RIGHT_SPEAKER_MIXER;
        break;
      default:
        return false;
    }
    return writeRegWide(reg, mixer_value);
  }

  /// Apply pending volume changes (set VU bit for output and input)
  bool applyProperties() {
    bool rc = true;
    rc &= updateRegWide(REG_LOUT1, REGMASK_OUT_VU, REGMASK_OUT_VU);
    rc &= updateRegWide(REG_LINVOL, REGMASK_IN_VU, REGMASK_IN_VU);
    return rc;
  }

  /// Configure the default output path: default volume, unmuted
  bool configureOutput() {
    bool rc = true;
    rc &= setOutputVolume((uint8_t)HEADPHONE_DEFAULT_VOLUME_VALUE,
                           WM8962Channel::All);
    rc &= setOutputMute(false, WM8962Channel::All);
    rc &= applyProperties();
    return rc;
  }

  /// Configure the default input path: route input1/input3, default volume, unmuted
  bool configureInput() {
    bool rc = true;
    rc &= routeInput(WM8962InputPgaSource::Input1, WM8962Channel::FrontLeft);
    rc &= routeInput(WM8962InputPgaSource::Input3, WM8962Channel::FrontRight);

    /* Input MIXER source: route input PGA to both input mixers */
    rc &= writeRegWide(REG_INPUTMIX, ((1U & 7U) << 3U) | (1U & 7U));
    /* Input MIXER enable */
    rc &= writeRegWide(REG_INPUT_MIXER_1, 3);

    rc &= setInputVolume((uint8_t)LINEIN_DEFAULT_VOLUME_VALUE,
                          WM8962Channel::All);
    rc &= setInputMute(false, WM8962Channel::All);
    return rc;
  }

  /**
   * @brief Trigger one of the codec's built-in write sequences and wait
   * (up to the documented timeout) until it has completed.
   */
  bool runSequence(WM8962Sequence id) {
    uint32_t delay_remaining_ms;
    switch (id) {
      case WM8962Sequence::DACToHeadphonePowerUp:
        delay_remaining_ms = 93;
        break;
      case WM8962Sequence::AnalogueInputPowerUp:
        delay_remaining_ms = 75;
        break;
      case WM8962Sequence::ChipPowerDown:
        delay_remaining_ms = 32;
        break;
      case WM8962Sequence::SpeakerSleep:
      case WM8962Sequence::SpeakerWake:
      default:
        delay_remaining_ms = 2;
        break;
    }

    bool rc = true;
    rc &= writeRegWide(REG_WRITE_SEQ_CTRL_1, WSEQ_ENA);
    rc &= writeRegWide(REG_WRITE_SEQ_CTRL_2, (uint16_t)id);

    uint16_t status = 1;
    while (delay_remaining_ms > 0) {
      if (!readRegWide(REG_WRITE_SEQ_CTRL_3, status)) break;
      if ((status & 1U) == 0U) break;
      delayMs(1);
      delay_remaining_ms--;
    }
    return rc && (status & 1U) == 0U;
  }

 protected:
  /// Writes a 16 bit big endian register address followed by a 16 bit big
  /// endian register value (4 bytes total)
  bool writeRegWide(uint16_t reg, uint16_t value) {
    assert(wire != nullptr);
    uint8_t data[4] = {
        (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF),
        (uint8_t)(value >> 8), (uint8_t)(value & 0xFF),
    };
    return i2c_bus_write_bytes(wire, i2c_addr, data, 2, data + 2, 2) == 0;
  }

  /// Reads a 16 bit big endian register value addressed via a 16 bit big
  /// endian register address
  bool readRegWide(uint16_t reg, uint16_t& value) {
    assert(wire != nullptr);
    uint8_t reg_bytes[2] = {(uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF)};
    uint8_t data[2] = {0, 0};
    if (i2c_bus_read_bytes(wire, i2c_addr, reg_bytes, 2, data, 2) != 0)
      return false;
    value = ((uint16_t)data[0] << 8) | data[1];
    return true;
  }

  /// Read-Modify-Write of a 16 bit register addressed via a 16 bit address
  bool updateRegWide(uint16_t reg, uint16_t mask, uint16_t value) {
    uint16_t old = 0;
    if (!readRegWide(reg, old)) return false;
    uint16_t updated = (old & ~mask) | (value & mask);
    if (updated == old) return true;
    return writeRegWide(reg, updated);
  }

  bool updateOutput(WM8962Channel channel, uint16_t val, uint16_t mask) {
    bool rc = true;
    switch (channel) {
      case WM8962Channel::FrontLeft:
        return updateRegWide(REG_LOUT2, mask, val);
      case WM8962Channel::FrontRight:
        return updateRegWide(REG_ROUT2, mask, val);
      case WM8962Channel::HeadphoneLeft:
        return updateRegWide(REG_LOUT1, mask, val);
      case WM8962Channel::HeadphoneRight:
        return updateRegWide(REG_ROUT1, mask, val);
      case WM8962Channel::All:
        rc &= updateRegWide(REG_LOUT1, mask, val);
        rc &= updateRegWide(REG_ROUT1, mask, val);
        rc &= updateRegWide(REG_LOUT2, mask, val);
        rc &= updateRegWide(REG_ROUT2, mask, val);
        return rc;
      default:
        return false;
    }
  }

  bool updateInput(WM8962Channel channel, uint16_t val, uint16_t mask) {
    bool rc = true;
    switch (channel) {
      case WM8962Channel::FrontLeft:
        return updateRegWide(REG_LINVOL, mask, val);
      case WM8962Channel::FrontRight:
        return updateRegWide(REG_RINVOL, mask, val);
      case WM8962Channel::All:
        rc &= updateRegWide(REG_LINVOL, mask, val);
        rc &= updateRegWide(REG_RINVOL, mask, val);
        return rc;
      default:
        return false;
    }
  }
};

}  // namespace audio_driver
