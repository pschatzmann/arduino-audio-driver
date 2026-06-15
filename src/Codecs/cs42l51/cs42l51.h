/**
 ******************************************************************************
 * @file    cs42l51.h
 * @brief   Header only C++ port of the CS42L51 BSP audio codec driver.
 * @attention
 *
 * Copyright (c) 2017-2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#pragma once

#include <stdint.h>

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_Delay.h"
#include "Platforms/API_I2C.h"

namespace audio_driver {

/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
enum CS42L51Reg : uint8_t {
  CS42L51_CHIP_ID = 0x01,
  CS42L51_PWR_CTRL_1 = 0x02,
  CS42L51_MIC_PWR_SPEED_CTRL = 0x03,
  CS42L51_INTERFACE_CTRL = 0x04,
  CS42L51_MIC_CTRL = 0x05,
  CS42L51_ADC_CTRL = 0x06,
  CS42L51_ADCX_INPUT_SELECT = 0x07,
  CS42L51_DAC_OUTPUT_CTRL = 0x08,
  CS42L51_DAC_CTRL = 0x09,
  CS42L51_ALCA_PGAA_CTRL = 0x0A,
  CS42L51_ALCB_PGAB_CTRL = 0x0B,
  CS42L51_ADCA_ATTENUATOR = 0x0C,
  CS42L51_ADCB_ATTENUATOR = 0x0D,
  CS42L51_ADCA_MIXER_VOL_CTRL = 0x0E,
  CS42L51_ADCB_MIXER_VOL_CTRL = 0x0F,
  CS42L51_PCMA_MIXER_VOL_CTRL = 0x10,
  CS42L51_PCMB_MIXER_VOL_CTRL = 0x11,
  CS42L51_BEEP_FREQ_AND_TIMING_CFG = 0x12,
  CS42L51_BEEP_OFF_TIME_AND_VOL = 0x13,
  CS42L51_BEEP_CFG_AND_TONE_CFG = 0x14,
  CS42L51_TONE_CTRL = 0x15,
  CS42L51_AOUTA_VOL_CTRL = 0x16,
  CS42L51_AOUTB_VOL_CTRL = 0x17,
  CS42L51_ADC_PCM_CHANNEL_MIXER = 0x18,
  CS42L51_LIMITER_THR_SZC_DISABLE = 0x19,
  CS42L51_LIMITER_RELEASE_RATE_REG = 0x1A,
  CS42L51_LIMITER_ATTACK_RATE_REG = 0x1B,
  CS42L51_ALC_ENABLE_AND_ATTACK_RATE = 0x1C,
  CS42L51_ALC_RELEASE_RATE = 0x1D,
  CS42L51_ALC_THR = 0x1E,
  CS42L51_NOISE_GATE_CFG_AND_MISC = 0x1F,
  CS42L51_STATUS = 0x20,
  CS42L51_CHARGE_PUMP_FREQ = 0x21,
};

/******************************************************************************/
/***************************  Codec User types  *******************************/
/******************************************************************************/

/// Component status
enum CS42L51Status : int32_t {
  CS42L51_OK = 0,
  CS42L51_ERROR = -1,
};

/// Audio input devices: can be combined with |
enum CS42L51InputDevice : uint32_t {
  CS42L51_IN_NONE = 0x0000,
  CS42L51_IN_MIC1 = 0x0100,
  CS42L51_IN_MIC2 = 0x0200,
  CS42L51_IN_LINE1 = 0x0400,
  CS42L51_IN_LINE2 = 0x0800,
  CS42L51_IN_LINE3 = 0x1000,
};

/// Audio output devices: can be combined with |
enum CS42L51OutputDevice : uint32_t {
  CS42L51_OUT_NONE = 0x0000,
  CS42L51_OUT_HEADPHONE = 0x0001,
};

/// Audio frequencies
enum CS42L51Frequency : uint32_t {
  CS42L51_FREQUENCY_192K = 192000,
  CS42L51_FREQUENCY_176K = 176400,
  CS42L51_FREQUENCY_96K = 96000,
  CS42L51_FREQUENCY_88K = 88200,
  CS42L51_FREQUENCY_48K = 48000,
  CS42L51_FREQUENCY_44K = 44100,
  CS42L51_FREQUENCY_32K = 32000,
  CS42L51_FREQUENCY_22K = 22050,
  CS42L51_FREQUENCY_16K = 16000,
  CS42L51_FREQUENCY_11K = 11025,
  CS42L51_FREQUENCY_8K = 8000,
};

/// Audio resolutions (only applicable for CS42L51_PROTOCOL_R_JUSTIFIED)
enum CS42L51Resolution : uint8_t {
  CS42L51_RESOLUTION_16B = 0x05,
  CS42L51_RESOLUTION_18B = 0x04,
  CS42L51_RESOLUTION_20B = 0x03,
  CS42L51_RESOLUTION_24B = 0x02,
};

/// Codec stop / power down options
enum CS42L51PowerDownMode : uint8_t {
  CS42L51_PDWN_HW = 0x00,
  CS42L51_PDWN_SW = 0x01,
};

/// Volume input/output selection
enum CS42L51VolumeSelect : uint8_t {
  CS42L51_VOLUME_INPUT = 0,
  CS42L51_VOLUME_OUTPUT = 1,
};

/// Mute commands
enum CS42L51Mute : uint8_t {
  CS42L51_MUTE_OFF = 0,
  CS42L51_MUTE_ON = 1,
};

/// Audio protocols
enum CS42L51Protocol : uint8_t {
  CS42L51_PROTOCOL_L_JUSTIFIED = 0,
  CS42L51_PROTOCOL_I2S = 1,
  CS42L51_PROTOCOL_R_JUSTIFIED = 2,
};

/// CS42L51 chip identification
static constexpr uint8_t CS42L51_ID = 0xD8;
static constexpr uint8_t CS42L51_ID_MASK = 0xF8;

/**
 * @brief Header-only driver class for the CS42L51 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class CS42L51 {
 public:
  CS42L51() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(uint16_t addr) { device_addr = addr; }

  /// Initializes the audio codec and the control interface.
  /// @param inputDevice combination of CS42L51_IN_MIC1, CS42L51_IN_MIC2,
  ///        CS42L51_IN_LINE1, CS42L51_IN_LINE2, CS42L51_IN_LINE3 or
  ///        CS42L51_IN_NONE
  /// @param outputDevice CS42L51_OUT_HEADPHONE or CS42L51_OUT_NONE
  /// @param volume Initial volume level (from 0 (Mute) to 100 (Max))
  /// @param audioFreq Audio frequency (not used)
  uint32_t init(uint32_t inputDevice, uint32_t outputDevice, uint8_t volume,
                uint32_t audioFreq) {
    uint32_t ret = 0;
    uint8_t tmp;

    if (is_initialized == 0) {
      /* Set the device in standby mode */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp |= 0x01U;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);

      /* Set all power down bits to 1 */
      tmp = 0x7FU;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);
      tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
      tmp |= 0x0EU;
      ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);

      is_initialized = 1;
    } else {
      /* Set all power down bits to 1 except PDN to mute ADCs and DACs*/
      tmp = 0x7EU;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);
      tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
      tmp |= 0x0EU;
      ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);

      /* Disable zero cross and soft ramp */
      tmp = readReg(CS42L51_DAC_CTRL);
      tmp &= 0xFCU;
      ret += writeReg(CS42L51_DAC_CTRL, tmp);

      /* Power control : Enter standby (PDN = 1) */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp |= 0x01U;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);
    }

    /* Mic Power and Speed Control : Auto detect on, Speed mode SSM, tri state
     * off, MCLK divide by 2 off */
    tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
    tmp = ((tmp & 0x0EU) | 0xA0U);
    ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);

    /* Interface control : Loopback off, Slave, I2S (SDIN and SOUT), Digital
     * mix off, Mic mix off */
    tmp = 0x0CU;
    ret += writeReg(CS42L51_INTERFACE_CTRL, tmp);

    /* Mic control : ADC single volume off, ADCB boost off, ADCA boost off,
     * MicBias on AIN3B/MICIN2 pin, MicBias level 0.8xVA, MICB boost 32db,
     * MICA boost 32dB */
    tmp = 0x03U;
    ret += writeReg(CS42L51_MIC_CTRL, tmp);

    /* ADC control : ADCB HPF on, ADCB HPF freeze off, ADCA HPF on, ADCA HPF
     * freeze off, Soft ramp B on, Zero cross B on, Soft ramp A on, Zero cross
     * A on */
    tmp = 0xAFU;
    ret += writeReg(CS42L51_ADC_CTRL, tmp);

    /* DAC output control : HP Gain to 1, Single volume control off, PCM
     * invert signals polarity off, DAC channels mute on */
    tmp = 0xC3U;
    ret += writeReg(CS42L51_DAC_OUTPUT_CTRL, tmp);

    /* DAC control : Signal processing to DAC, Freeze off, De-emphasis off,
     * Analog output auto mute off, DAC soft ramp */
    tmp = 0x42U;
    ret += writeReg(CS42L51_DAC_CTRL, tmp);

    /* ALCA and PGAA Control : ALCA soft ramp disable on, ALCA zero cross
     * disable on, PGA A Gain +8dB */
    tmp = 0xD0U;
    ret += writeReg(CS42L51_ALCA_PGAA_CTRL, tmp);

    /* ALCB and PGAB Control : ALCB soft ramp disable on, ALCB zero cross
     * disable on, PGA B Gain +8dB */
    ret += writeReg(CS42L51_ALCB_PGAB_CTRL, tmp);

    if ((inputDevice & CS42L51_IN_LINE1) == CS42L51_IN_LINE1) {
      /* ADCA/ADCB Attenuator : -10dB */
      tmp = 0xF6U;
    } else {
      /* ADCA/ADCB Attenuator : 0dB */
      tmp = 0x00U;
    }
    /* ADCA Attenuator */
    ret += writeReg(CS42L51_ADCA_ATTENUATOR, tmp);
    /* ADCB Attenuator */
    ret += writeReg(CS42L51_ADCB_ATTENUATOR, tmp);

    /* ADCA mixer volume control : ADCA mixer channel mute on, ADCA mixer
     * volume 0dB */
    tmp = 0x80U;
    ret += writeReg(CS42L51_ADCA_MIXER_VOL_CTRL, tmp);
    /* ADCB mixer volume control : ADCB mixer channel mute on, ADCB mixer
     * volume 0dB */
    ret += writeReg(CS42L51_ADCB_MIXER_VOL_CTRL, tmp);

    /* PCMA mixer volume control : PCMA mixer channel mute off, PCMA mixer
     * volume 0dB */
    tmp = 0x00U;
    ret += writeReg(CS42L51_PCMA_MIXER_VOL_CTRL, tmp);
    /* PCMB mixer volume control : PCMB mixer channel mute off, PCMB mixer
     * volume 0dB */
    ret += writeReg(CS42L51_PCMB_MIXER_VOL_CTRL, tmp);
    /* PCM channel mixer : AOUTA Left, AOUTB Right */
    ret += writeReg(CS42L51_ADC_PCM_CHANNEL_MIXER, tmp);

    if ((outputDevice & CS42L51_OUT_HEADPHONE) == CS42L51_OUT_HEADPHONE) {
      tmp = volumeConvert(volume);
      /* AOUTA volume control : AOUTA volume */
      ret += writeReg(CS42L51_AOUTA_VOL_CTRL, tmp);
      /* AOUTB volume control : AOUTB volume */
      ret += writeReg(CS42L51_AOUTB_VOL_CTRL, tmp);
    }

    /* ALC enable and attack rate : ALCB and ALCA enable, fastest attack */
    tmp = 0x40U;
    ret += writeReg(CS42L51_ALC_ENABLE_AND_ATTACK_RATE, tmp);

    /* Store current devices */
    current_devices = (outputDevice | inputDevice);

    return ret;
  }

  /// Deinitializes the audio codec.
  void deinit() { is_initialized = 0; }

  /// Get the CS42L51 ID.
  uint32_t readID() {
    uint8_t value = readReg(CS42L51_CHIP_ID);
    return static_cast<uint32_t>(value & CS42L51_ID_MASK);
  }

  /// Start the audio Codec play feature.
  uint32_t play() {
    uint32_t ret = 0;
    uint8_t tmp;

    if ((current_devices & CS42L51_OUT_HEADPHONE) == CS42L51_OUT_HEADPHONE) {
      /* Unmute the output first */
      ret += setMute(CS42L51_MUTE_OFF);

      /* DAC control : Signal processing to DAC, Freeze off, De-emphasis off,
       * Analog output auto mute off, DAC soft ramp */
      tmp = 0x42U;
      ret += writeReg(CS42L51_DAC_CTRL, tmp);

      /* Power control 1 : PDN_DACA, PDN_DACB disable. */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp &= 0x9FU;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);
    }

    if ((current_devices & CS42L51_IN_LINE1) == CS42L51_IN_LINE1) {
      /* ADC Input Select, Invert and Mute : AIN1B to PGAB, AIN1A to PGAA,
       * ADCB invert off, ADCA invert off, ADCB mute off, ADCA mute off */
      tmp = 0x00U;
      ret += writeReg(CS42L51_ADCX_INPUT_SELECT, tmp);

      /* Power control 1 : PDN_PGAA, PDN_PGAA, PDN_ADCB, PDN_ADCA disable. */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp &= 0xF9U;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);

      /* Mic Power and Speed Control : PDN_MICA, PDN_MICB, PDN_MIC_BIAS
       * disable. */
      tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
      tmp &= 0xFFU;
      ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);
    }

    if ((current_devices & CS42L51_IN_MIC1) == CS42L51_IN_MIC1) {
      /* ADC Input Select, Invert and Mute : AIN1B to PGAB, AIN3A to PreAmp to
       * PGAA, ADCB invert off, ADCA invert off, ADCB mute on, ADCA mute off */
      tmp = 0x32U;
      ret += writeReg(CS42L51_ADCX_INPUT_SELECT, tmp);

      /* Power control 1 : PDN_PGAA, PDN_ADCA disable. */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp &= 0xF5U;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);

      /* Mic Power and Speed Control : PDN_MICA, PDN_MIC_BIAS disable. */
      tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
      tmp &= 0xF9U;
      ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);
    }

    if ((current_devices & CS42L51_IN_MIC2) == CS42L51_IN_MIC2) {
      /* Power control 1 : PDN_PGAB, PDN_ADCB disable. */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp &= 0xEBU;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);

      /* Mic Power and Speed Control : PDN_MICB, PDN_MIC_BIAS disable. */
      tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
      tmp &= 0xF5U;
      ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);
    }

    /* Power control : Exit standby (PDN = 0) */
    tmp = readReg(CS42L51_PWR_CTRL_1);
    tmp &= 0xFEU;
    ret += writeReg(CS42L51_PWR_CTRL_1, tmp);

    return ret;
  }

  /// Pauses playing on the audio codec.
  uint32_t pause() { return setMute(CS42L51_MUTE_ON); }

  /// Resumes playing on the audio codec.
  uint32_t resume() { return setMute(CS42L51_MUTE_OFF); }

  /// Stops audio Codec playing. It powers down the codec.
  /// @param cmd CS42L51_PDWN_SW (mute only) or CS42L51_PDWN_HW (full power
  ///        down)
  uint32_t stop(uint32_t cmd) {
    uint32_t ret = 0;
    uint8_t tmp;

    /* Mute the output first */
    ret += setMute(CS42L51_MUTE_ON);

    if (cmd == CS42L51_PDWN_SW) {
      /* Only output mute required*/
    } else /* CS42L51_PDWN_HW */
    {
      /* Set all power down bits to 1 except PDN to mute ADCs and DACs*/
      tmp = 0x7EU;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);
      tmp = readReg(CS42L51_MIC_PWR_SPEED_CTRL);
      tmp |= 0x0EU;
      ret += writeReg(CS42L51_MIC_PWR_SPEED_CTRL, tmp);

      /* Disable zero cross and soft ramp */
      tmp = readReg(CS42L51_DAC_CTRL);
      tmp &= 0xFCU;
      ret += writeReg(CS42L51_DAC_CTRL, tmp);

      /* Power control : Enter standby (PDN = 1) */
      tmp = readReg(CS42L51_PWR_CTRL_1);
      tmp |= 0x01U;
      ret += writeReg(CS42L51_PWR_CTRL_1, tmp);
    }

    return ret;
  }

  /// Sets higher or lower the output codec volume level.
  /// @param volume a value from 0 to 100 (refer to codec registers
  ///        description for more details).
  uint32_t setVolume(uint8_t volume) {
    uint32_t ret = 0;
    uint8_t convertedvol = volumeConvert(volume);

    /* AOUTA volume control : AOUTA volume */
    ret += writeReg(CS42L51_AOUTA_VOL_CTRL, convertedvol);
    /* AOUTB volume control : AOUTB volume */
    ret += writeReg(CS42L51_AOUTB_VOL_CTRL, convertedvol);

    return ret;
  }

  /// Get the output codec volume level.
  uint8_t getVolume() {
    uint8_t tmp = readReg(CS42L51_AOUTA_VOL_CTRL);
    return volumeInvert(tmp);
  }

  /// Sets new frequency.
  uint32_t setFrequency(uint32_t audioFreq) { return 0; }

  /// Enables or disables the mute feature on the audio codec.
  /// @param cmd CS42L51_MUTE_ON to enable the mute or CS42L51_MUTE_OFF to
  ///        disable the mute mode.
  uint32_t setMute(uint32_t cmd) {
    uint32_t ret = 0;
    uint8_t tmp = readReg(CS42L51_DAC_OUTPUT_CTRL);

    /* Set the Mute mode */
    if (cmd == CS42L51_MUTE_ON) {
      tmp |= 0x03U;
    } else /* CS42L51_MUTE_OFF Disable the Mute */
    {
      tmp &= 0xFCU;
    }

    ret += writeReg(CS42L51_DAC_OUTPUT_CTRL, tmp);

    return ret;
  }

  /// Switch dynamically (while audio file is played) the output target.
  /// @note Only CS42L51_OUT_HEADPHONE output is supported by this codec.
  uint32_t setOutputMode(uint32_t output) {
    if (output != CS42L51_OUT_HEADPHONE) return CS42L51_ERROR;
    return CS42L51_OK;
  }

  /// Resets cs42l51 registers.
  uint32_t reset() { return 0; }

  uint8_t readReg(uint8_t reg) {
    uint8_t value = 0;
    i2c_bus_read_bytes(i2c_handle, device_addr, &reg, 1, &value, 1);
    return value;
  }

  uint8_t writeReg(uint8_t reg, uint8_t value) {
    i2c_bus_write_bytes(i2c_handle, device_addr, &reg, 1, &value, 1);
    return 0;
  }

  /// Converts a volume in the range 0-100 to the codec's register value
  inline constexpr uint8_t volumeConvert(uint8_t volume) {
    return volume >= 100 ? static_cast<uint8_t>(0)
                         : static_cast<uint8_t>(volume * 2 + 56);
  }

  /// Converts a codec register value back to a volume in the range 0-100
  inline constexpr uint8_t volumeInvert(uint8_t reg) {
    return reg == 0 ? static_cast<uint8_t>(100)
                    : static_cast<uint8_t>((reg - 56) / 2);
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  uint16_t device_addr = 0;
  uint8_t is_initialized = 0;
  uint32_t current_devices = CS42L51_OUT_NONE;
};

}  // namespace audio_driver
