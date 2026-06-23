/**
 * @file TAS6422DAC.h
 * @brief Header only C++ driver for the Texas Instruments TAS6422 2-channel
 * digital input Class-D audio amplifier.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/tas6422dac.c / tas6422dac.h, Copyright (c) 2023 Centralp,
 * Apache-2.0).
 */
#pragma once

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Output channel selection
enum class TAS6422Channel {
  Channel1,
  Channel2,
  All,
};

/// Channel state (CH_STATE_CTRL register)
enum class TAS6422ChannelState {
  Play = 0,
  Hiz = 1,
  Mute = 2,
  DcLoad = 3,
};

/// Serial audio port input format (SAP_CTRL register)
enum class TAS6422InputFormat {
  Format24BitRight = 0,
  Format20BitRight = 1,
  Format18BitRight = 2,
  Format16BitRight = 3,
  I2S = 4,
  Left = 5,
  Dsp = 6,
};

/**
 * @brief Header only C++ driver for the TAS6422 2-channel Class-D amplifier.
 *
 * Provides a soft reset, I2S/sample-rate configuration, per-channel
 * mute/play state control and output volume control, ported from the
 * corresponding Zephyr RTOS driver.
 */
class TAS6422DAC : public ZephyrDriverCommon {
 public:
  // ---- Register addresses ----
  enum Reg : uint8_t {
    MODE_CTRL_ADDR = 0x00,
    MISC_CTRL_1_ADDR = 0x01,
    MISC_CTRL_2_ADDR = 0x02,
    SAP_CTRL_ADDR = 0x03,
    CH_STATE_CTRL_ADDR = 0x04,
    CH1_VOLUME_CTRL_ADDR = 0x05,
    CH2_VOLUME_CTRL_ADDR = 0x06,
    DC_LDG_CTRL_1_ADDR = 0x09,
    DC_LDG_CTRL_2_ADDR = 0x0A,
    DC_LDG_REPORT_1_ADDR = 0x0C,
    DC_LDG_REPORT_3_ADDR = 0x0E,
    CH_FAULTS_ADDR = 0x10,
    GLOBAL_FAULTS_1_ADDR = 0x11,
    GLOBAL_FAULTS_2_ADDR = 0x12,
    WARNINGS_ADDR = 0x13,
    PIN_CTRL_ADDR = 0x14,
    MISC_CTRL_3_ADDR = 0x21,
    ILIMIT_STATUS_ADDR = 0x25,
    MISC_CTRL_4_ADDR = 0x26,
    MISC_CTRL_5_ADDR = 0x28,
  };

  // ---- Mode Control Register bits ----
  static constexpr uint8_t MODE_CTRL_RESET = (1u << 7);
  static constexpr uint8_t MODE_CTRL_PBTL_CH12 = (1u << 4);
  static constexpr uint8_t MODE_CTRL_CH1_LO_MODE = (1u << 3);
  static constexpr uint8_t MODE_CTRL_CH2_LO_MODE = (1u << 2);

  // ---- Misc Control 1 Register bits ----
  static constexpr uint8_t MISC_CTRL_1_HPF_BYPASS = (1u << 7);
  static constexpr uint8_t MISC_CTRL_1_OTW_CONTROL_MASK = (0x3u << 5);
  static constexpr uint8_t MISC_CTRL_1_OC_CONTROL_MASK = (1u << 4);
  static constexpr uint8_t MISC_CTRL_1_VOLUME_RATE_MASK = (0x3u << 2);
  static constexpr uint8_t MISC_CTRL_1_GAIN_MASK = 0x3u;

  // ---- Misc Control 2 Register bits ----
  static constexpr uint8_t MISC_CTRL_2_PWM_FREQUENCY_MASK = (0x7u << 4);
  static constexpr uint8_t MISC_CTRL_2_PWM_FREQUENCY_8_FS = 0;
  static constexpr uint8_t MISC_CTRL_2_PWM_FREQUENCY_10_FS = 1;
  static constexpr uint8_t MISC_CTRL_2_PWM_FREQUENCY_38_FS = 5;
  static constexpr uint8_t MISC_CTRL_2_PWM_FREQUENCY_44_FS = 6;
  static constexpr uint8_t MISC_CTRL_2_PWM_FREQUENCY_48_FS = 7;
  static constexpr uint8_t MISC_CTRL_2_SDM_OSR = (1u << 2);
  static constexpr uint8_t MISC_CTRL_2_OUTPUT_PHASE_MASK = 0x3u;

  // ---- Serial Audio-Port Control Register bits ----
  static constexpr uint8_t SAP_CTRL_INPUT_SAMPLING_RATE_MASK = (0x3u << 6);
  static constexpr uint8_t SAP_CTRL_INPUT_SAMPLING_RATE_44_1_KHZ = 0;
  static constexpr uint8_t SAP_CTRL_INPUT_SAMPLING_RATE_48_KHZ = 1;
  static constexpr uint8_t SAP_CTRL_INPUT_SAMPLING_RATE_96_KHZ = 2;
  static constexpr uint8_t SAP_CTRL_TDM_SLOT_SELECT = (1u << 5);
  static constexpr uint8_t SAP_CTRL_TDM_SLOT_SIZE = (1u << 4);
  static constexpr uint8_t SAP_CTRL_TDM_SLOT_SELECT_2 = (1u << 3);
  static constexpr uint8_t SAP_CTRL_INPUT_FORMAT_MASK = 0x7u;

  // ---- Channel State Control Register bits ----
  static constexpr uint8_t CH_STATE_CTRL_CH1_STATE_CTRL_MASK = (0x3u << 6);
  static constexpr uint8_t CH_STATE_CTRL_CH2_STATE_CTRL_MASK = (0x3u << 4);

  // ---- Channel Volume Control Register ----
  static constexpr uint8_t CH_VOLUME_CTRL_VOLUME_MASK = 0xFFu;

  // ---- Output volume limits (in 0.5dB steps, signed) ----
  static constexpr int OUTPUT_VOLUME_MAX = (24 * 2);
  static constexpr int OUTPUT_VOLUME_MIN = (-100 * 2);

  TAS6422DAC() { i2c_addr = 0x6C; }

  /**
   * @brief Initialize the codec: soft reset, configure the I2S input
   * format/sample rate and apply default output settings (overcurrent
   * level, PWM frequency). The channels are muted (Hiz state is not used)
   * until start() is called.
   *
   * Note: TAS6422 also has an optional hardware MUTE pin; if your board
   * wires it, drive it externally (active = muted) in addition to the
   * register based mute.
   *
   * @param sample_rate I2S sample rate in Hz (44100, 48000 or 96000)
   */
  /// Initializes the codec for I2S with the given sample rate (bits per
  /// sample is not configurable on this chip)
  bool begin(uint32_t sample_rate, uint8_t bits) {
    (void)bits;
    return begin(sample_rate);
  }

  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)fmt;
    (void)is_master;
    (void)channels;
    return begin(sample_rate, bits);
  }

  bool begin(uint32_t sample_rate = 44100) {
    bool rc = true;
    rc &= softReset();
    rc &= configureDai(sample_rate);
    rc &= configureOutput();
    rc &= setChannelState(TAS6422Channel::All, TAS6422ChannelState::Mute);
    return rc;
  }

  /// Trigger a soft reset (sets the RESET bit in MODE_CTRL)
  bool softReset() {
    uint8_t val = 0;
    if (!readReg(MODE_CTRL_ADDR, val)) return false;
    val |= MODE_CTRL_RESET;
    return writeReg(MODE_CTRL_ADDR, val);
  }

  /**
   * @brief Configure the serial audio port: I2S input format and input
   * sampling rate.
   *
   * @param sample_rate 44100, 48000 or 96000 Hz
   */
  bool configureDai(uint32_t sample_rate) {
    uint8_t val = 0;
    if (!readReg(SAP_CTRL_ADDR, val)) return false;

    val &= (uint8_t)~SAP_CTRL_INPUT_FORMAT_MASK;
    val |= (uint8_t)TAS6422InputFormat::I2S;

    val &= (uint8_t)~SAP_CTRL_INPUT_SAMPLING_RATE_MASK;
    switch (sample_rate) {
      case 44100:
        val |= (SAP_CTRL_INPUT_SAMPLING_RATE_44_1_KHZ << 6);
        break;
      case 48000:
        val |= (SAP_CTRL_INPUT_SAMPLING_RATE_48_KHZ << 6);
        break;
      case 96000:
        val |= (SAP_CTRL_INPUT_SAMPLING_RATE_96_KHZ << 6);
        break;
      default:
        return false;
    }

    return writeReg(SAP_CTRL_ADDR, val);
  }

  /**
   * @brief Apply default output settings: overcurrent level = 1, PWM
   * frequency = 10 fs (reduced from default to avoid component
   * overtemperature).
   *
   * This chip only drives speaker outputs (no headphone path), so it is
   * only configured for DAC_OUTPUT_LINE2 (speaker) and DAC_OUTPUT_ALL. For
   * DAC_OUTPUT_NONE / DAC_OUTPUT_LINE1 (headphone, not supported) all
   * channels are kept muted.
   */
  bool configureOutput() {
    bool rc = true;
    uint8_t val = 0;

    if (output_device == DAC_OUTPUT_NONE || output_device == DAC_OUTPUT_LINE1) {
      return setChannelState(TAS6422Channel::All, TAS6422ChannelState::Mute);
    }

    rc &= readReg(MISC_CTRL_1_ADDR, val);
    val &= (uint8_t)~MISC_CTRL_1_OC_CONTROL_MASK;
    rc &= writeReg(MISC_CTRL_1_ADDR, val);

    rc &= readReg(MISC_CTRL_2_ADDR, val);
    val &= (uint8_t)~MISC_CTRL_2_PWM_FREQUENCY_MASK;
    val |= (uint8_t)(MISC_CTRL_2_PWM_FREQUENCY_10_FS << 4);
    rc &= writeReg(MISC_CTRL_2_ADDR, val);

    return rc;
  }

  /// Set the channel state (Play, Hiz, Mute or DcLoad) for the given channel(s)
  bool setChannelState(TAS6422Channel channel, TAS6422ChannelState state) {
    uint8_t val = 0;
    if (!readReg(CH_STATE_CTRL_ADDR, val)) return false;

    switch (channel) {
      case TAS6422Channel::Channel1:
        val &= (uint8_t)~CH_STATE_CTRL_CH1_STATE_CTRL_MASK;
        val |= (uint8_t)((uint8_t)state << 6);
        break;
      case TAS6422Channel::Channel2:
        val &= (uint8_t)~CH_STATE_CTRL_CH2_STATE_CTRL_MASK;
        val |= (uint8_t)((uint8_t)state << 4);
        break;
      case TAS6422Channel::All:
        val &= (uint8_t)~(CH_STATE_CTRL_CH1_STATE_CTRL_MASK |
                           CH_STATE_CTRL_CH2_STATE_CTRL_MASK);
        val |= (uint8_t)((uint8_t)state << 6) | (uint8_t)((uint8_t)state << 4);
        break;
      default:
        return false;
    }

    return writeReg(CH_STATE_CTRL_ADDR, val);
  }

  /// Unmute (set to Play state) the given channel(s)
  bool start(TAS6422Channel channel = TAS6422Channel::All) {
    return setChannelState(channel, TAS6422ChannelState::Play);
  }

  /// Mute the given channel(s)
  bool stop(TAS6422Channel channel = TAS6422Channel::All) {
    return setChannelState(channel, TAS6422ChannelState::Mute);
  }

  /// Mute / unmute the given channel(s)
  bool setMute(bool mute, TAS6422Channel channel = TAS6422Channel::All) {
    return setChannelState(channel, mute ? TAS6422ChannelState::Mute
                                          : TAS6422ChannelState::Play);
  }

  /// Stores the output device selection for use by configureOutput()
  bool setDevices(input_device_t input_device, output_device_t output_device) override {
    (void)input_device;
    this->output_device = output_device;
    return true;
  }

  /**
   * @brief Set the output volume in 0.5dB steps.
   *
   * @param volume signed volume in 0.5dB steps, range
   * OUTPUT_VOLUME_MIN (-200, i.e. -100dB) .. OUTPUT_VOLUME_MAX (48, i.e.
   * +24dB)
   */
  bool setOutputVolume(int volume, TAS6422Channel channel = TAS6422Channel::All) {
    if (volume > OUTPUT_VOLUME_MAX || volume < OUTPUT_VOLUME_MIN) return false;

    uint8_t vol_val = (uint8_t)(volume + 0xCF);

    switch (channel) {
      case TAS6422Channel::Channel1:
        return writeReg(CH1_VOLUME_CTRL_ADDR, vol_val);
      case TAS6422Channel::Channel2:
        return writeReg(CH2_VOLUME_CTRL_ADDR, vol_val);
      case TAS6422Channel::All: {
        bool rc = true;
        rc &= writeReg(CH1_VOLUME_CTRL_ADDR, vol_val);
        rc &= writeReg(CH2_VOLUME_CTRL_ADDR, vol_val);
        return rc;
      }
      default:
        return false;
    }
  }

  /// Sets the output volume in % (0...100) for all channels, mapped to
  /// [OUTPUT_VOLUME_MIN..OUTPUT_VOLUME_MAX] (0.5dB steps)
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    int vol = OUTPUT_VOLUME_MIN +
              (volume_percent * (OUTPUT_VOLUME_MAX - OUTPUT_VOLUME_MIN)) / 100;
    return setOutputVolume(vol, TAS6422Channel::All);
  }

  /// Read and clear the global/channel fault registers (writing 1 to
  /// MISC_CTRL_3 CLEAR_FAULT)
  bool clearFaults() {
    uint8_t val = 0;
    if (!readReg(MISC_CTRL_3_ADDR, val)) return false;
    val |= (1u << 7);  // MISC_CTRL_3_CLEAR_FAULT
    return writeReg(MISC_CTRL_3_ADDR, val);
  }

  /// Read the channel faults register (over-current / DC detection)
  bool getChannelFaults(uint8_t& value) { return readReg(CH_FAULTS_ADDR, value); }

  /// Read the global faults 1 register (clock / supply voltage faults)
  bool getGlobalFaults1(uint8_t& value) { return readReg(GLOBAL_FAULTS_1_ADDR, value); }

  /// Read the global faults 2 register (thermal shutdown faults)
  bool getGlobalFaults2(uint8_t& value) { return readReg(GLOBAL_FAULTS_2_ADDR, value); }

  /// Read the warnings register (POR / over-temperature warnings)
  bool getWarnings(uint8_t& value) { return readReg(WARNINGS_ADDR, value); }

 protected:
  /// Output device selection set via setDevices(), used by configureOutput()
  output_device_t output_device = DAC_OUTPUT_ALL;
};

}  // namespace audio_driver
