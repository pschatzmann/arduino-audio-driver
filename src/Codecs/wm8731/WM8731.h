/**
 * WM8731 Audio Codec driver
 *
 * Ported from the Atmel/Microchip ASF WM8731 driver
 * (sam/components/audio/codec/wm8731), Copyright (c) 2012-2015 Atmel
 * Corporation, redistributed under the BSD-style ASF license.
 */
#pragma once

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"

namespace audio_driver {

/// 7-bit I2C address (CSB pin low). If CSB is pulled high use 0x1B.
constexpr int WM8731_ADDR = 0x1A;

/**
 * @brief Header-only driver class for the WM8731 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class WM8731 {
 public:
  /// WM8731 register addresses
  enum class Reg : uint8_t {
    LeftLineIn = 0x00,
    RightLineIn = 0x01,
    LeftHeadphoneOut = 0x02,
    RightHeadphoneOut = 0x03,
    AnaloguePath = 0x04,
    DigitalPath = 0x05,
    PowerDown = 0x06,
    Format = 0x07,
    SamplingControl = 0x08,
    Active = 0x09,
    Reset = 0x0F,
  };

  /// Bit flags for the Left/Right Line In registers
  enum LineIn : uint16_t {
    LineInVolMask = 0x1F,
    LineInMute = 1 << 7,
    LineInBoth = 1 << 8,
  };

  /// Bit flags for the Left/Right Headphone Out registers
  enum HeadphoneOut : uint16_t {
    HeadphoneVolMask = 0x7F,
    HeadphoneZeroCross = 1 << 7,
    HeadphoneBoth = 1 << 8,
  };

  /// Bit flags for the Analogue Audio Path Control register
  enum AnaloguePath : uint16_t {
    MicBoost = 1 << 0,
    MuteMic = 1 << 1,
    InputSelectMic = 1 << 2,
    Bypass = 1 << 3,
    DacSelect = 1 << 4,
    SideTone = 1 << 5,
    SideToneAttenuationMask = 0x3 << 6,
  };

  /// Bit flags for the Digital Audio Path Control register
  enum DigitalPath : uint16_t {
    AdcHighPassDisable = 1 << 0,
    DeEmphasisDisable = 0x0 << 1,
    DeEmphasis32k = 0x1 << 1,
    DeEmphasis44_1k = 0x2 << 1,
    DeEmphasis48k = 0x3 << 1,
    DacSoftMute = 1 << 3,
    HighPassOnReset = 1 << 4,
  };

  /// Bit flags for the Power Down Control register
  enum PowerDown : uint16_t {
    LineInPowerDown = 1 << 0,
    MicPowerDown = 1 << 1,
    AdcPowerDown = 1 << 2,
    DacPowerDown = 1 << 3,
    OutputPowerDown = 1 << 4,
    OscillatorPowerDown = 1 << 5,
    ClockOutPowerDown = 1 << 6,
    PowerOff = 1 << 7,
  };

  /// Bit flags / values for the Digital Audio Interface Format register
  enum Format : uint16_t {
    FormatRightJustified = 0x0,
    FormatLeftJustified = 0x1,
    FormatI2S = 0x2,
    FormatDsp = 0x3,
    FormatMask = 0x3,
    WordLength16Bit = 0x0 << 2,
    WordLength20Bit = 0x1 << 2,
    WordLength24Bit = 0x2 << 2,
    WordLength32Bit = 0x3 << 2,
    WordLengthMask = 0x3 << 2,
    LrSwapPolarity = 1 << 4,
    LrSwap = 1 << 5,
    MasterMode = 1 << 6,
    BitClockInvert = 1 << 7,
  };

  /// Bit flags for the Sampling Control register
  enum SamplingControl : uint16_t {
    UsbMode = 1 << 0,
    BaseOversamplingRate = 1 << 1,
    SampleRateMask = 0xF << 2,
    ClkInDiv2 = 1 << 6,
    ClkOutDiv2 = 1 << 7,
  };

  /// Bit flags for the Active Control register
  enum ActiveControl : uint16_t {
    Active = 1 << 0,
  };

  WM8731() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) { i2c_addr = addr; }

  /// @brief Initialize the WM8731 codec chip
  error_t init(codec_config_t* cfg) {
    if (initialized) {
      AD_LOGW("The WM8731 has already been initialized");
      return RESULT_OK;
    }
    initialized = true;

    // reset all registers to their default values
    reset();

    // power up the needed sections, power down the rest
    updatePower(cfg->input_device, cfg->output_device);

    // analogue path: select dac output, line input, no bypass, no mic
    analogue_path = DacSelect;
    if (cfg->input_device != ADC_INPUT_NONE &&
        cfg->input_device != ADC_INPUT_LINE1 &&
        cfg->input_device != ADC_INPUT_ALL) {
      analogue_path |= InputSelectMic;
    }
    writeReg(Reg::AnaloguePath, analogue_path);

    // digital path: enable high pass filter, no de-emphasis, unmuted
    digital_path = 0;
    writeReg(Reg::DigitalPath, digital_path);

    // default headphone / line out volume (0dB)
    setVoiceVolume(70);

    // input volume (0dB)
    writeReg(Reg::LeftLineIn, 0x17 | LineInBoth);
    writeReg(Reg::RightLineIn, 0x17 | LineInBoth);

    // configure the digital audio interface and sample rate
    configI2S(CODEC_MODE_BOTH, &cfg->i2s);

    // activate the digital core
    setActive(true);
    return RESULT_OK;
  }

  /// @brief Deinitialize the WM8731 codec chip
  error_t deinit(void) {
    setActive(false);
    // power down everything (standby)
    writeReg(Reg::PowerDown, PowerOff);
    initialized = false;
    return RESULT_OK;
  }

  /// @brief Activate or standby the codec
  error_t ctrlStateActive(codec_mode_t mode, bool ctrl_state_active) {
    return ctrl_state_active ? resume() : standby();
  }

  /// @brief Configure the digital audio interface format and sample rate
  error_t configI2S(codec_mode_t mode, I2SDefinition* iface) {
    uint16_t format = 0;
    switch (iface->fmt) {
      case I2S_LEFT:
        format |= FormatLeftJustified;
        break;
      case I2S_RIGHT:
        format |= FormatRightJustified;
        break;
      case I2S_DSP:
        format |= FormatDsp;
        break;
      case I2S_NORMAL:
      default:
        format |= FormatI2S;
        break;
    }

    switch (iface->bits) {
      case BIT_LENGTH_20BITS:
        format |= WordLength20Bit;
        break;
      case BIT_LENGTH_24BITS:
        format |= WordLength24Bit;
        break;
      case BIT_LENGTH_32BITS:
        format |= WordLength32Bit;
        break;
      case BIT_LENGTH_16BITS:
      default:
        format |= WordLength16Bit;
        break;
    }

    if (iface->mode == MODE_MASTER) {
      format |= MasterMode;
    }

    writeReg(Reg::Format, format);
    writeReg(Reg::SamplingControl, getSamplingControl(iface->rate));
    return RESULT_OK;
  }

  /// @brief Mute / unmute the DAC output
  error_t setVoiceMute(bool enable) {
    if (enable) {
      digital_path |= DacSoftMute;
    } else {
      digital_path &= ~DacSoftMute;
    }
    return writeReg(Reg::DigitalPath, digital_path);
  }

  /**
   * @brief Set the headphone / line output volume
   * @param volume voice volume (0~100)
   */
  error_t setVoiceVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    voice_volume = volume;
    // register range 0x30 (-73dB) .. 0x7F (+6dB), 0..0x2F is also mute
    uint16_t reg_value = (volume == 0)
                              ? 0x00
                              : 0x30 + ((volume * (0x7F - 0x30)) / 100);
    error_t ret =
        writeReg(Reg::LeftHeadphoneOut, reg_value | HeadphoneBoth);
    ret |= writeReg(Reg::RightHeadphoneOut, reg_value | HeadphoneBoth);
    return ret;
  }

  /// @brief Get the headphone / line output volume (0~100)
  error_t getVoiceVolume(int* volume) {
    *volume = voice_volume;
    return RESULT_OK;
  }

  /// @brief Write a value to a WM8731 register
  error_t writeReg(Reg reg_addr, uint16_t value) {
    uint16_t word =
        ((static_cast<uint8_t>(reg_addr) & 0x7F) << 9) | (value & 0x1FF);
    uint8_t data[2] = {(uint8_t)((word >> 8) & 0xFF), (uint8_t)(word & 0xFF)};
    return i2c_bus_write_bytes(i2c_handle, i2c_addr, &data[0], 1, &data[1], 1);
  }

  /// @brief Reset all registers to their default values
  error_t reset() { return writeReg(Reg::Reset, 0); }

  /// @brief Activates / deactivates the digital audio interface
  error_t setActive(bool active) {
    active_control = active ? Active : 0;
    return writeReg(Reg::Active, active_control);
  }

  /// @brief Powers down all sections (standby)
  error_t standby(void) {
    return writeReg(Reg::PowerDown,
                     LineInPowerDown | MicPowerDown | AdcPowerDown |
                         DacPowerDown | OutputPowerDown);
  }

  /// @brief Restores the power configuration that was active before standby
  error_t resume(void) { return writeReg(Reg::PowerDown, power_down); }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = WM8731_ADDR;
  bool initialized = false;
  uint16_t analogue_path = 0;
  uint16_t digital_path = 0;
  uint16_t power_down = 0;
  uint16_t active_control = 0;
  int voice_volume = 70;

  /// @brief Powers up the input/output sections that are actually used
  error_t updatePower(input_device_t input_device,
                       output_device_t output_device) {
    power_down = OscillatorPowerDown | ClockOutPowerDown;
    if (input_device == ADC_INPUT_NONE) {
      power_down |= LineInPowerDown | MicPowerDown | AdcPowerDown;
    }
    if (output_device == DAC_OUTPUT_NONE) {
      power_down |= DacPowerDown | OutputPowerDown;
    }
    return writeReg(Reg::PowerDown, power_down);
  }

  /**
   * @brief Determine the SamplingControl register value for the
   * requested sample rate.
   * @note Assumes a typical MCLK of 256*fs (12.288MHz for the 48kHz
   * family, 11.2896MHz for the 44.1kHz family) in normal (non-USB) mode.
   */
  uint16_t getSamplingControl(samplerate_t rate) {
    switch (rate) {
      case RATE_8K:
        return 0x06;
      case RATE_32K:
        return 0x18;
      case RATE_44K:
        return 0x20;
      case RATE_88K:
        return 0x28;
      case RATE_96K:
        return 0x08;
      case RATE_48K:
      default:
        return 0x00;
    }
  }
};

}  // namespace audio_driver
