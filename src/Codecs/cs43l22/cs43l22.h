/**
  ******************************************************************************
  * @file    cs43l22.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the cs43l22.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
//#include "../Common/audio.h"
#include <stdint.h>
#include "stdbool.h"
#include "stddef.h"
#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "Codecs/CodecConstants.h"

/** @addtogroup Component
  * @{
  */

/** @addtogroup CS43L22
  * @{
  */

/** @defgroup CS43L22_Exported_Constants
  * @{
  */

/******************************************************************************/
/***************************  Codec User defines ******************************/
/******************************************************************************/
/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K          ((uint32_t)192000)
#define AUDIO_FREQUENCY_96K           ((uint32_t)96000)
#define AUDIO_FREQUENCY_48K           ((uint32_t)48000)
#define AUDIO_FREQUENCY_44K           ((uint32_t)44100)
#define AUDIO_FREQUENCY_32K           ((uint32_t)32000)
#define AUDIO_FREQUENCY_22K           ((uint32_t)22050)
#define AUDIO_FREQUENCY_16K           ((uint32_t)16000)
#define AUDIO_FREQUENCY_11K           ((uint32_t)11025)
#define AUDIO_FREQUENCY_8K            ((uint32_t)8000)

/** CS43L22 Registers  ***/
#define   CS43L22_REG_ID                  0x01
#define   CS43L22_REG_POWER_CTL1          0x02
#define   CS43L22_REG_POWER_CTL2          0x04
#define   CS43L22_REG_CLOCKING_CTL        0x05
#define   CS43L22_REG_INTERFACE_CTL1      0x06
#define   CS43L22_REG_INTERFACE_CTL2      0x07
#define   CS43L22_REG_PASSTHR_A_SELECT    0x08
#define   CS43L22_REG_PASSTHR_B_SELECT    0x09
#define   CS43L22_REG_ANALOG_ZC_SR_SETT   0x0A
#define   CS43L22_REG_PASSTHR_GANG_CTL    0x0C
#define   CS43L22_REG_PLAYBACK_CTL1       0x0D
#define   CS43L22_REG_MISC_CTL            0x0E
#define   CS43L22_REG_PLAYBACK_CTL2       0x0F
#define   CS43L22_REG_PASSTHR_A_VOL       0x14
#define   CS43L22_REG_PASSTHR_B_VOL       0x15
#define   CS43L22_REG_PCMA_VOL            0x1A
#define   CS43L22_REG_PCMB_VOL            0x1B
#define   CS43L22_REG_BEEP_FREQ_ON_TIME   0x1C
#define   CS43L22_REG_BEEP_VOL_OFF_TIME   0x1D
#define   CS43L22_REG_BEEP_TONE_CFG       0x1E
#define   CS43L22_REG_TONE_CTL            0x1F
#define   CS43L22_REG_MASTER_A_VOL        0x20
#define   CS43L22_REG_MASTER_B_VOL        0x21
#define   CS43L22_REG_HEADPHONE_A_VOL     0x22
#define   CS43L22_REG_HEADPHONE_B_VOL     0x23
#define   CS43L22_REG_SPEAKER_A_VOL       0x24
#define   CS43L22_REG_SPEAKER_B_VOL       0x25
#define   CS43L22_REG_CH_MIXER_SWAP       0x26
#define   CS43L22_REG_LIMIT_CTL1          0x27
#define   CS43L22_REG_LIMIT_CTL2          0x28
#define   CS43L22_REG_LIMIT_ATTACK_RATE   0x29
#define   CS43L22_REG_OVF_CLK_STATUS      0x2E
#define   CS43L22_REG_BATT_COMPENSATION   0x2F
#define   CS43L22_REG_VP_BATTERY_LEVEL    0x30
#define   CS43L22_REG_SPEAKER_STATUS      0x31
#define   CS43L22_REG_TEMPMONITOR_CTL     0x32
#define   CS43L22_REG_THERMAL_FOLDBACK    0x33
#define   CS43L22_REG_CHARGE_PUMP_FREQ    0x34

#ifndef CODEC_STANDARD
#define CODEC_STANDARD                0x04
#endif

#ifndef I2S_STANDARD
#define I2S_STANDARD                  I2S_STANDARD_PHILIPS
#endif

/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
/**
  * @brief  CS43L22 ID
  */
#define  CS43L22_ID            0xE0
#define  CS43L22_ID_MASK       0xF8
/**
  * @brief Chip ID Register: Chip I.D. and Revision Register
  *  Read only register
  *  Default value: 0x01
  *  [7:3] CHIPID[4:0]: I.D. code for the CS43L22.
  *        Default value: 11100b
  *  [2:0] REVID[2:0]: CS43L22 revision level.
  *        Default value:
  *        000 - Rev A0
  *        001 - Rev A1
  *        010 - Rev B0
  *        011 - Rev B1
  */
#define CS43L22_CHIPID_ADDR    0x01

#define CS43L22_VOLUME_CONVERT(Volume) \
  (((Volume) > 100) ? 255 : ((uint8_t)(((Volume) * 255) / 100)))

namespace audio_driver {

/**
 * @brief Header-only driver class for the CS43L22 codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class CS43L22 {
 public:
  CS43L22() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(uint16_t addr) { device_addr = addr; }

  /// Initializes the audio codec and the control interface.
  /// @param outputDevice can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  ///                      OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO
  /// @param volume Initial volume level (from 0 (Mute) to 100 (Max))
  uint32_t init(uint16_t outputDevice, uint8_t volume, uint32_t audioFreq) {
    uint32_t counter = 0;

    /* Keep Codec powered OFF */
    counter += writeReg(CS43L22_REG_POWER_CTL1, 0x01);

    /*Save Output device for mute ON/OFF procedure*/
    switch (outputDevice) {
      case OUTPUT_DEVICE_SPEAKER:
        output_dev = 0xFA;
        break;

      case OUTPUT_DEVICE_HEADPHONE:
        output_dev = 0xAF;
        break;

      case OUTPUT_DEVICE_BOTH:
        output_dev = 0xAA;
        break;

      case OUTPUT_DEVICE_AUTO:
        output_dev = 0x05;
        break;

      default:
        output_dev = 0x05;
        break;
    }

    counter += writeReg(CS43L22_REG_POWER_CTL2, output_dev);

    /* Clock configuration: Auto detection */
    counter += writeReg(CS43L22_REG_CLOCKING_CTL, 0x81);

    /* Set the Slave Mode and the audio Standard */
    counter += writeReg(CS43L22_REG_INTERFACE_CTL1, CODEC_STANDARD);

    /* Set the Master volume */
    counter += setVolume(volume);

    /* If the Speaker is enabled, set the Mono mode and volume attenuation level
     */
    if (outputDevice != OUTPUT_DEVICE_HEADPHONE) {
      /* Set the Speaker Mono mode */
      counter += writeReg(CS43L22_REG_PLAYBACK_CTL2, 0x06);

      /* Set the Speaker attenuation level */
      counter += writeReg(CS43L22_REG_SPEAKER_A_VOL, 0x00);
      counter += writeReg(CS43L22_REG_SPEAKER_B_VOL, 0x00);
    }

    /* Additional configuration for the CODEC. These configurations are done to
    reduce the time needed for the Codec to power off. If these configurations are
    removed, then a long delay should be added between powering off the Codec and
    switching off the I2S peripheral MCLK clock (which is the operating clock for
    Codec). If this delay is not inserted, then the codec will not shut down
    properly and it results in high noise after shut down. */

    /* Disable the analog soft ramp */
    counter += writeReg(CS43L22_REG_ANALOG_ZC_SR_SETT, 0x00);
    /* Disable the digital soft ramp */
    counter += writeReg(CS43L22_REG_MISC_CTL, 0x04);
    /* Disable the limiter attack level */
    counter += writeReg(CS43L22_REG_LIMIT_CTL1, 0x00);
    /* Adjust Bass and Treble levels */
    counter += writeReg(CS43L22_REG_TONE_CTL, 0x0F);
    /* Adjust PCM volume level */
    counter += writeReg(CS43L22_REG_PCMA_VOL, 0x0A);
    counter += writeReg(CS43L22_REG_PCMB_VOL, 0x0A);

    /* Return communication control value */
    return counter;
  }

  /// Deinitializes the audio codec.
  void deinit() {}

  /// Get the CS43L22 ID.
  uint32_t readID() {
    uint8_t value = 0;
    return (uint32_t)(value & CS43L22_ID_MASK);
  }

  /// Start the audio Codec play feature.
  /// @note For this codec no Play options are required.
  uint32_t play(uint16_t* pBuffer, uint16_t size) {
    uint32_t counter = 0;

    if (is_stop == 1) {
      /* Enable the digital soft ramp */
      counter += writeReg(CS43L22_REG_MISC_CTL, 0x06);

      /* Enable Output device */
      counter += setMute(AUDIO_MUTE_OFF);

      /* Power on the Codec */
      counter += writeReg(CS43L22_REG_POWER_CTL1, 0x9E);
      is_stop = 0;
    }

    /* Return communication control value */
    return counter;
  }

  /// Pauses playing on the audio codec.
  uint32_t pause() {
    uint32_t counter = 0;

    /* Pause the audio file playing */
    /* Mute the output first */
    counter += setMute(AUDIO_MUTE_ON);

    /* Put the Codec in Power save mode */
    counter += writeReg(CS43L22_REG_POWER_CTL1, 0x01);

    return counter;
  }

  /// Resumes playing on the audio codec.
  uint32_t resume() {
    uint32_t counter = 0;
    /* Resumes the audio file playing */
    /* Unmute the output first */
    counter += setMute(AUDIO_MUTE_OFF);

    delayMs(10);

    counter += writeReg(CS43L22_REG_POWER_CTL2, output_dev);

    /* Exit the Power save mode */
    counter += writeReg(CS43L22_REG_POWER_CTL1, 0x9E);

    return counter;
  }

  /// Stops audio Codec playing. It powers down the codec.
  /// @param cmd selects the power down mode (e.g. AUDIO_MUTE_ON).
  uint32_t stop(uint32_t cmd) {
    uint32_t counter = 0;

    /* Mute the output first */
    counter += setMute(AUDIO_MUTE_ON);

    /* Disable the digital soft ramp */
    counter += writeReg(CS43L22_REG_MISC_CTL, 0x04);

    /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
    counter += writeReg(CS43L22_REG_POWER_CTL1, 0x9F);

    is_stop = 1;
    return counter;
  }

  /// Sets higher or lower the codec volume level.
  /// @param volume a byte value from 0 to 100 (which is mapped to 0 - 255,
  ///                refer to codec registers description for more details).
  uint32_t setVolume(uint8_t volume) {
    uint32_t counter = 0;
    uint8_t convertedvol = CS43L22_VOLUME_CONVERT(volume);

    if (convertedvol > 0xE6) {
      /* Set the Master volume */
      counter += writeReg(CS43L22_REG_MASTER_A_VOL, convertedvol - 0xE7);
      counter += writeReg(CS43L22_REG_MASTER_B_VOL, convertedvol - 0xE7);
    } else {
      /* Set the Master volume */
      counter += writeReg(CS43L22_REG_MASTER_A_VOL, convertedvol + 0x19);
      counter += writeReg(CS43L22_REG_MASTER_B_VOL, convertedvol + 0x19);
    }

    return counter;
  }

  /// Sets new frequency.
  uint32_t setFrequency(uint32_t audioFreq) { return 0; }

  /// Enables or disables the mute feature on the audio codec.
  /// @param cmd AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable
  ///            the mute mode.
  uint32_t setMute(uint32_t cmd) {
    uint32_t counter = 0;

    /* Set the Mute mode */
    if (cmd == AUDIO_MUTE_ON) {
      counter += writeReg(CS43L22_REG_POWER_CTL2, 0xFF);
      counter += writeReg(CS43L22_REG_HEADPHONE_A_VOL, 0x01);
      counter += writeReg(CS43L22_REG_HEADPHONE_B_VOL, 0x01);
    } else /* AUDIO_MUTE_OFF Disable the Mute */
    {
      counter += writeReg(CS43L22_REG_HEADPHONE_A_VOL, 0x00);
      counter += writeReg(CS43L22_REG_HEADPHONE_B_VOL, 0x00);
      counter += writeReg(CS43L22_REG_POWER_CTL2, output_dev);
    }
    return counter;
  }

  /// Switch dynamically (while audio file is played) the output target
  /// (speaker or headphone).
  /// @param output specifies the audio output target: OUTPUT_DEVICE_SPEAKER,
  ///        OUTPUT_DEVICE_HEADPHONE, OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO
  uint32_t setOutputMode(uint8_t output) {
    uint32_t counter = 0;

    switch (output) {
      case OUTPUT_DEVICE_SPEAKER:
        counter += writeReg(CS43L22_REG_POWER_CTL2,
                             0xFA); /* SPK always ON & HP always OFF */
        output_dev = 0xFA;
        break;

      case OUTPUT_DEVICE_HEADPHONE:
        counter += writeReg(CS43L22_REG_POWER_CTL2,
                             0xAF); /* SPK always OFF & HP always ON */
        output_dev = 0xAF;
        break;

      case OUTPUT_DEVICE_BOTH:
        counter += writeReg(CS43L22_REG_POWER_CTL2,
                             0xAA); /* SPK always ON & HP always ON */
        output_dev = 0xAA;
        break;

      case OUTPUT_DEVICE_AUTO:
        counter += writeReg(CS43L22_REG_POWER_CTL2,
                             0x05); /* Detect the HP or the SPK automatically */
        output_dev = 0x05;
        break;

      default:
        counter += writeReg(CS43L22_REG_POWER_CTL2,
                             0x05); /* Detect the HP or the SPK automatically */
        output_dev = 0x05;
        break;
    }
    return counter;
  }

  /// Resets cs43l22 registers.
  uint32_t reset() { return 0; }
  uint8_t writeReg(uint8_t reg, uint8_t value) {
    i2c_bus_write_bytes(i2c_handle, device_addr, &reg, 1, &value, 1);
    return 0;
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  uint16_t device_addr = 0;
  uint8_t is_stop = 1;
  volatile uint8_t output_dev = 0;
};

} // namespace audio_driver
