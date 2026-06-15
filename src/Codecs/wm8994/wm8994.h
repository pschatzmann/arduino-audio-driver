/**
 ******************************************************************************
 * @file    wm8994.h
 * @author  MCD Application Team
 * @version V2.0.0
 * @date    24-June-2015
 * @brief   This file contains all the functions prototypes for the
 *          wm8994.c driver.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WM8994_H
#define __WM8994_H

/* Includes ------------------------------------------------------------------*/
// #include "../Common/audio.h"
#include <stddef.h>
#include <stdint.h>

#include "Codecs/CodecConstants.h"
#include "DriverCommon.h"
#include "Platforms/API_Delay.h"
#include "Platforms/API_I2C.h"
#include "stdbool.h"

/** @addtogroup Component
 * @{
 */

/** @addtogroup WM8994
 * @{
 */

/** @defgroup WM8994_Exported_Constants
 * @{
 */

/******************************************************************************/
/***************************  Codec User defines ******************************/
/******************************************************************************/
/* Codec output DEVICE */
#ifndef OUTPUT_DEVICE_SPEAKER
#define OUTPUT_DEVICE_SPEAKER ((uint16_t)0x0001)
#define OUTPUT_DEVICE_HEADPHONE ((uint16_t)0x0002)
#define OUTPUT_DEVICE_BOTH ((uint16_t)0x0003)
#define OUTPUT_DEVICE_AUTO ((uint16_t)0x0004)
#endif
#define INPUT_DEVICE_DIGITAL_MICROPHONE_1 ((uint16_t)0x0100)
#define INPUT_DEVICE_DIGITAL_MICROPHONE_2 ((uint16_t)0x0200)
#define INPUT_DEVICE_INPUT_LINE_1 ((uint16_t)0x0300)
#define INPUT_DEVICE_INPUT_LINE_2 ((uint16_t)0x0400)

/* Volume Levels values */
#define DEFAULT_VOLMIN 0x00
#define DEFAULT_VOLMAX 0xFF
#define DEFAULT_VOLSTEP 0x04

#define AUDIO_PAUSE 0
#define AUDIO_RESUME 1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW 1
#define CODEC_PDWN_SW 2

/* MUTE commands */
#define AUDIO_MUTE_ON 1
#define AUDIO_MUTE_OFF 0

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K ((uint32_t)192000)
#define AUDIO_FREQUENCY_96K ((uint32_t)96000)
#define AUDIO_FREQUENCY_48K ((uint32_t)48000)
#define AUDIO_FREQUENCY_44K ((uint32_t)44100)
#define AUDIO_FREQUENCY_32K ((uint32_t)32000)
#define AUDIO_FREQUENCY_22K ((uint32_t)22050)
#define AUDIO_FREQUENCY_16K ((uint32_t)16000)
#define AUDIO_FREQUENCY_11K ((uint32_t)11025)
#define AUDIO_FREQUENCY_8K ((uint32_t)8000)

#define WM8994_VOLUME_CONVERT(Volume) \
  (((Volume) > 100) ? 100 : ((uint8_t)(((Volume) * 63) / 100)))
#define VOLUME_IN_CONVERT(Volume) \
  (((Volume) >= 100) ? 239 : ((uint8_t)(((Volume) * 240) / 100)))

/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
/**
 * @brief  WM8994 ID
 */
#define WM8994_ID 0x8994

/**
 * @brief Device ID Register: Reading from this register will indicate device
 *                            family ID 8994h
 */
#define WM8994_CHIPID_ADDR 0x00

/** @brief Default I2C address of the WM8994 codec */
#define WM8994_ADDR 0x1A

/* Uncomment this line to enable verifying data sent to codec after each write
   operation (for debug purpose) */
#if !defined(VERIFY_WRITTENDATA)
/* #define VERIFY_WRITTENDATA */
#endif /* VERIFY_WRITTENDATA */

/**
 * @}
 */

/** @defgroup WM8994_Exported_Functions
 * @{
 */

namespace audio_driver {

/**
 * @brief Header-only driver class for the WM8994 audio codec chip
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class WM8994 {
 public:
  WM8994() = default;

  /// Defines the I2C bus instance to be used
  void setWire(i2c_bus_handle_t handle) { i2c_handle = handle; }

  /// Defines the I2C device address
  void setAddress(int addr) {
    if (addr > 0) i2c_addr = addr;
  }

  /**
   * @brief Initializes the audio codec and the control interface.
   * @param OutputInputDevice: can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
   *  OUTPUT_DEVICE_BOTH, OUTPUT_DEVICE_AUTO, INPUT_DEVICE_DIGITAL_MICROPHONE_1,
   *  INPUT_DEVICE_DIGITAL_MICROPHONE_2, INPUT_DEVICE_INPUT_LINE_1 or
   *  INPUT_DEVICE_INPUT_LINE_2.
   * @param Volume: Initial volume level (from 0 (Mute) to 100 (Max))
   * @param AudioFreq: Audio Frequency
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t init(uint16_t OutputInputDevice, uint8_t Volume,
                uint32_t AudioFreq) {
    uint32_t counter = 0;
    uint16_t output_device = OutputInputDevice & 0xFF;
    uint16_t input_device = OutputInputDevice & 0xFF00;
    uint16_t power_mgnt_reg_1 = 0;

    /* wm8994 Errata Work-Arounds */
    counter += writeReg16(0x102, 0x0003);
    counter += writeReg16(0x817, 0x0000);
    counter += writeReg16(0x102, 0x0000);

    /* Enable VMID soft start (fast), Start-up Bias Current Enabled */
    counter += writeReg16(0x39, 0x006C);

    /* Enable bias generator, Enable VMID */
    counter += writeReg16(0x01, 0x0003);

    /* Add Delay */
    delayMs(50);

    /* Path Configurations for output */
    if (output_device > 0) {
      outputEnabled = 1;
      switch (output_device) {
        case OUTPUT_DEVICE_SPEAKER:
          /* Enable DAC1 (Left), Enable DAC1 (Right),
          Disable DAC2 (Left), Disable DAC2 (Right)*/
          counter += writeReg16(0x05, 0x0C0C);

          /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
          counter += writeReg16(0x601, 0x0000);

          /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
          counter += writeReg16(0x602, 0x0000);

          /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
          counter += writeReg16(0x604, 0x0002);

          /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
          counter += writeReg16(0x605, 0x0002);
          break;

        case OUTPUT_DEVICE_HEADPHONE:
          /* Disable DAC1 (Left), Disable DAC1 (Right),
          Enable DAC2 (Left), Enable DAC2 (Right)*/
          counter += writeReg16(0x05, 0x0303);

          /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
          counter += writeReg16(0x601, 0x0001);

          /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
          counter += writeReg16(0x602, 0x0001);

          /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
          counter += writeReg16(0x604, 0x0000);

          /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
          counter += writeReg16(0x605, 0x0000);
          break;

        case OUTPUT_DEVICE_BOTH:
          /* Enable DAC1 (Left), Enable DAC1 (Right),
          also Enable DAC2 (Left), Enable DAC2 (Right)*/
          counter += writeReg16(0x05, 0x0303 | 0x0C0C);

          /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
          counter += writeReg16(0x601, 0x0001);

          /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
          counter += writeReg16(0x602, 0x0001);

          /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
          counter += writeReg16(0x604, 0x0002);

          /* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
          counter += writeReg16(0x605, 0x0002);
          break;

        case OUTPUT_DEVICE_AUTO:
        default:
          /* Disable DAC1 (Left), Disable DAC1 (Right),
          Enable DAC2 (Left), Enable DAC2 (Right)*/
          counter += writeReg16(0x05, 0x0303);

          /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
          counter += writeReg16(0x601, 0x0001);

          /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
          counter += writeReg16(0x602, 0x0001);

          /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
          counter += writeReg16(0x604, 0x0000);

          /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
          counter += writeReg16(0x605, 0x0000);
          break;
      }
    } else {
      outputEnabled = 0;
    }

    /* Path Configurations for input */
    if (input_device > 0) {
      inputEnabled = 1;
      switch (input_device) {
        case INPUT_DEVICE_DIGITAL_MICROPHONE_2:
          /* Enable AIF1ADC2 (Left), Enable AIF1ADC2 (Right)
           * Enable DMICDAT2 (Left), Enable DMICDAT2 (Right)
           * Enable Left ADC, Enable Right ADC */
          counter += writeReg16(0x04, 0x0C30);

          /* Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1 */
          counter += writeReg16(0x450, 0x00DB);

          /* Disable IN1L, IN1R, IN2L, IN2R, Enable Thermal sensor & shutdown */
          counter += writeReg16(0x02, 0x6000);

          /* Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path */
          counter += writeReg16(0x608, 0x0002);

          /* Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path */
          counter += writeReg16(0x609, 0x0002);

          /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC2 signal detect */
          counter += writeReg16(0x700, 0x000E);
          break;

        case INPUT_DEVICE_INPUT_LINE_1:
          /* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
           * Enable Left ADC, Enable Right ADC */
          counter += writeReg16(0x04, 0x0303);

          /* Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
          counter += writeReg16(0x440, 0x00DB);

          /* Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown */
          counter += writeReg16(0x02, 0x6350);

          /* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
          counter += writeReg16(0x606, 0x0002);

          /* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
          counter += writeReg16(0x607, 0x0002);

          /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */
          counter += writeReg16(0x700, 0x000D);
          break;

        case INPUT_DEVICE_DIGITAL_MICROPHONE_1:
        case INPUT_DEVICE_INPUT_LINE_2:
        default:
          /* Actually, no other input devices supported */
          counter++;
          break;
      }
    } else {
      inputEnabled = 0;
    }

    /*  Clock Configurations */
    switch (AudioFreq) {
      case AUDIO_FREQUENCY_8K:
        /* AIF1 Sample Rate = 8 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0003);
        break;

      case AUDIO_FREQUENCY_16K:
        /* AIF1 Sample Rate = 16 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0033);
        break;

      case AUDIO_FREQUENCY_48K:
        /* AIF1 Sample Rate = 48 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0083);
        break;

      case AUDIO_FREQUENCY_96K:
        /* AIF1 Sample Rate = 96 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x00A3);
        break;

      case AUDIO_FREQUENCY_11K:
        /* AIF1 Sample Rate = 11.025 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0013);
        break;

      case AUDIO_FREQUENCY_22K:
        /* AIF1 Sample Rate = 22.050 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0043);
        break;

      case AUDIO_FREQUENCY_44K:
        /* AIF1 Sample Rate = 44.1 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0073);
        break;

      default:
        /* AIF1 Sample Rate = 48 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0083);
        break;
    }
    /* AIF1 Word Length = 16-bits, AIF1 Format = I2S (Default Register Value) */
    counter += writeReg16(0x300, 0x4010);

    /* slave mode */
    counter += writeReg16(0x302, 0x0000);

    /* Enable the DSP processing clock for AIF1, Enable the core clock */
    counter += writeReg16(0x208, 0x000A);

    /* Enable AIF1 Clock, AIF1 Clock Source = MCLK1 pin */
    counter += writeReg16(0x200, 0x0001);

    if (output_device > 0) /* Audio output selected */
    {
      /* Analog Output Configuration */

      /* Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL */
      counter += writeReg16(0x03, 0x0300);

      /* Left Speaker Mixer Volume = 0dB */
      counter += writeReg16(0x22, 0x0000);

      /* Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB ((0x23, 0x0100) = class AB)*/
      counter += writeReg16(0x23, 0x0000);

      /* Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path,
      Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path */
      counter += writeReg16(0x36, 0x0300);

      /* Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR */
      counter += writeReg16(0x01, 0x3003);

      /* Headphone/Speaker Enable */

      /* Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0 */
      counter += writeReg16(0x51, 0x0005);

      /* Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages */
      /* idem for Speaker */
      power_mgnt_reg_1 |= 0x0303 | 0x3003;
      counter += writeReg16(0x01, power_mgnt_reg_1);

      /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages */
      counter += writeReg16(0x60, 0x0022);

      /* Enable Charge Pump */
      counter += writeReg16(0x4C, 0x9F25);

      /* Add Delay */
      delayMs(15);

      /* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
      counter += writeReg16(0x2D, 0x0001);

      /* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
      counter += writeReg16(0x2E, 0x0001);

      /* Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR) */
      /* idem for SPKOUTL and SPKOUTR */
      counter += writeReg16(0x03, 0x0030 | 0x0300);

      /* Enable DC Servo and trigger start-up mode on left and right channels */
      counter += writeReg16(0x54, 0x0033);

      /* Add Delay */
      delayMs(250);

      /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps */
      counter += writeReg16(0x60, 0x00EE);

      /* Unmutes */

      /* Unmute DAC 1 (Left) */
      counter += writeReg16(0x610, 0x00C0);

      /* Unmute DAC 1 (Right) */
      counter += writeReg16(0x611, 0x00C0);

      /* Unmute the AIF1 Timeslot 0 DAC path */
      counter += writeReg16(0x420, 0x0000);

      /* Unmute DAC 2 (Left) */
      counter += writeReg16(0x612, 0x00C0);

      /* Unmute DAC 2 (Right) */
      counter += writeReg16(0x613, 0x00C0);

      /* Unmute the AIF1 Timeslot 1 DAC2 path */
      counter += writeReg16(0x422, 0x0000);

      /* Volume Control */
      setVolume(Volume);
    }

    if (input_device > 0) /* Audio input selected */
    {
      if ((input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_1) ||
          (input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_2)) {
        /* Enable Microphone bias 1 generator, Enable VMID */
        power_mgnt_reg_1 |= 0x0013;
        counter += writeReg16(0x01, power_mgnt_reg_1);

        /* ADC oversample enable */
        counter += writeReg16(0x620, 0x0002);

        /* AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
        counter += writeReg16(0x411, 0x3800);
      } else if ((input_device == INPUT_DEVICE_INPUT_LINE_1) ||
                 (input_device == INPUT_DEVICE_INPUT_LINE_2)) {
        /* Enable normal bias generator, Enable VMID */
        power_mgnt_reg_1 |= 0x0003;
        counter += writeReg16(0x01, power_mgnt_reg_1);

        /* Disable mute on IN1L, IN1L Volume = +0dB */
        counter += writeReg16(0x18, 0x000B);

        /* Disable mute on IN1R, IN1R Volume = +0dB */
        counter += writeReg16(0x1A, 0x000B);

        /* Disable mute on IN1L_TO_MIXINL, Gain = +0dB */
        counter += writeReg16(0x29, 0x0025);

        /* Disable mute on IN1R_TO_MIXINL, Gain = +0dB */
        counter += writeReg16(0x2A, 0x0025);

        /* IN1LN_TO_IN1L, IN1LP_TO_VMID, IN1RN_TO_IN1R, IN1RP_TO_VMID */
        counter += writeReg16(0x28, 0x0011);

        /* AIF ADC1 HPF enable, HPF cut = hifi mode fc=4Hz at fs=48kHz */
        counter += writeReg16(0x410, 0x1800);
      }
      /* Volume Control */
      setVolume(Volume);
    }
    /* Return communication control value */
    return counter;
  }

  /// Deinitializes the audio codec.
  void deinit() {
    /* Deinitialize Audio Codec interface */
  }

  /// Get the WM8994 ID.
  uint32_t readId() { return ((uint32_t)readReg16(WM8994_CHIPID_ADDR)); }

  /**
   * @brief Start the audio Codec play feature.
   * @note For this codec no Play options are required.
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t play(uint16_t *pBuffer, uint16_t Size) {
    uint32_t counter = 0;

    /* Resumes the audio file playing */
    /* Unmute the output first */
    counter += setMute(AUDIO_MUTE_OFF);

    return counter;
  }

  /// Pauses playing on the audio codec.
  uint32_t pause() {
    uint32_t counter = 0;

    /* Pause the audio file playing */
    /* Mute the output first */
    counter += setMute(AUDIO_MUTE_ON);

    /* Put the Codec in Power save mode */
    counter += writeReg16(0x02, 0x01);

    return counter;
  }

  /// Resumes playing on the audio codec.
  uint32_t resume() {
    uint32_t counter = 0;

    /* Resumes the audio file playing */
    /* Unmute the output first */
    counter += setMute(AUDIO_MUTE_OFF);

    return counter;
  }

  /**
   * @brief Stops audio Codec playing. It powers down the codec.
   * @param CodecPdwnMode: selects the power down mode.
   *          - CODEC_PDWN_SW: only mutes the audio codec. When resuming from this
   *                           mode the codec keeps the previous initialization
   *                           (no need to re-Initialize the codec registers).
   *          - CODEC_PDWN_HW: Physically power down the codec. When resuming from
   *                           this mode, the codec is set to default configuration
   *                           (user should re-Initialize the codec in order to
   *                            play again the audio stream).
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t stop(uint32_t CodecPdwnMode) {
    uint32_t counter = 0;

    if (outputEnabled != 0) {
      /* Mute the output first */
      counter += setMute(AUDIO_MUTE_ON);

      if (CodecPdwnMode == CODEC_PDWN_SW) {
        /* Only output mute required*/
      } else /* CODEC_PDWN_HW */
      {
        /* Mute the AIF1 Timeslot 0 DAC1 path */
        counter += writeReg16(0x420, 0x0200);

        /* Mute the AIF1 Timeslot 1 DAC2 path */
        counter += writeReg16(0x422, 0x0200);

        /* Disable DAC1L_TO_HPOUT1L */
        counter += writeReg16(0x2D, 0x0000);

        /* Disable DAC1R_TO_HPOUT1R */
        counter += writeReg16(0x2E, 0x0000);

        /* Disable DAC1 and DAC2 */
        counter += writeReg16(0x05, 0x0000);

        /* Reset Codec by writing in 0x0000 address register */
        counter += writeReg16(0x0000, 0x0000);

        outputEnabled = 0;
      }
    }
    return counter;
  }

  /**
   * @brief Sets higher or lower the codec volume level.
   * @param Volume: a byte value from 0 to 255 (refer to codec registers
   *         description for more details).
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t setVolume(uint8_t Volume) {
    uint32_t counter = 0;
    uint8_t convertedvol = WM8994_VOLUME_CONVERT(Volume);

    /* Output volume */
    if (outputEnabled != 0) {
      if (convertedvol > 0x3E) {
        /* Unmute audio codec */
        counter += setMute(AUDIO_MUTE_OFF);

        /* Left Headphone Volume */
        counter += writeReg16(0x1C, 0x3F | 0x140);

        /* Right Headphone Volume */
        counter += writeReg16(0x1D, 0x3F | 0x140);

        /* Left Speaker Volume */
        counter += writeReg16(0x26, 0x3F | 0x140);

        /* Right Speaker Volume */
        counter += writeReg16(0x27, 0x3F | 0x140);
      } else if (Volume == 0) {
        /* Mute audio codec */
        counter += setMute(AUDIO_MUTE_ON);
      } else {
        /* Unmute audio codec */
        counter += setMute(AUDIO_MUTE_OFF);

        /* Left Headphone Volume */
        counter += writeReg16(0x1C, convertedvol | 0x140);

        /* Right Headphone Volume */
        counter += writeReg16(0x1D, convertedvol | 0x140);

        /* Left Speaker Volume */
        counter += writeReg16(0x26, convertedvol | 0x140);

        /* Right Speaker Volume */
        counter += writeReg16(0x27, convertedvol | 0x140);
      }
    }

    /* Input volume */
    if (inputEnabled != 0) {
      convertedvol = VOLUME_IN_CONVERT(Volume);

      /* Left AIF1 ADC1 volume */
      counter += writeReg16(0x400, convertedvol | 0x100);

      /* Right AIF1 ADC1 volume */
      counter += writeReg16(0x401, convertedvol | 0x100);

      /* Left AIF1 ADC2 volume */
      counter += writeReg16(0x404, convertedvol | 0x100);

      /* Right AIF1 ADC2 volume */
      counter += writeReg16(0x405, convertedvol | 0x100);
    }
    return counter;
  }

  /**
   * @brief Enables or disables the mute feature on the audio codec.
   * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
   *             mute mode.
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t setMute(uint32_t Cmd) {
    uint32_t counter = 0;

    if (outputEnabled != 0) {
      /* Set the Mute mode */
      if (Cmd == AUDIO_MUTE_ON) {
        /* Soft Mute the AIF1 Timeslot 0 DAC1 path L&R */
        counter += writeReg16(0x420, 0x0200);

        /* Soft Mute the AIF1 Timeslot 1 DAC2 path L&R */
        counter += writeReg16(0x422, 0x0200);
      } else /* AUDIO_MUTE_OFF Disable the Mute */
      {
        /* Unmute the AIF1 Timeslot 0 DAC1 path L&R */
        counter += writeReg16(0x420, 0x0000);

        /* Unmute the AIF1 Timeslot 1 DAC2 path L&R */
        counter += writeReg16(0x422, 0x0000);
      }
    }
    return counter;
  }

  /**
   * @brief Switch dynamically (while audio file is played) the output target
   *         (speaker or headphone).
   * @param Output: specifies the audio output target: OUTPUT_DEVICE_SPEAKER,
   *         OUTPUT_DEVICE_HEADPHONE, OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t setOutputMode(uint8_t Output) {
    uint32_t counter = 0;

    switch (Output) {
      case OUTPUT_DEVICE_SPEAKER:
        /* Enable DAC1 (Left), Enable DAC1 (Right),
        Disable DAC2 (Left), Disable DAC2 (Right)*/
        counter += writeReg16(0x05, 0x0C0C);

        /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
        counter += writeReg16(0x601, 0x0000);

        /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
        counter += writeReg16(0x602, 0x0000);

        /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
        counter += writeReg16(0x604, 0x0002);

        /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
        counter += writeReg16(0x605, 0x0002);
        break;

      case OUTPUT_DEVICE_HEADPHONE:
        /* Disable DAC1 (Left), Disable DAC1 (Right),
        Enable DAC2 (Left), Enable DAC2 (Right)*/
        counter += writeReg16(0x05, 0x0303);

        /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
        counter += writeReg16(0x601, 0x0001);

        /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
        counter += writeReg16(0x602, 0x0001);

        /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
        counter += writeReg16(0x604, 0x0000);

        /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
        counter += writeReg16(0x605, 0x0000);
        break;

      case OUTPUT_DEVICE_BOTH:
        /* Enable DAC1 (Left), Enable DAC1 (Right),
        also Enable DAC2 (Left), Enable DAC2 (Right)*/
        counter += writeReg16(0x05, 0x0303 | 0x0C0C);

        /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
        counter += writeReg16(0x601, 0x0001);

        /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
        counter += writeReg16(0x602, 0x0001);

        /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
        counter += writeReg16(0x604, 0x0002);

        /* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
        counter += writeReg16(0x605, 0x0002);
        break;

      default:
        /* Disable DAC1 (Left), Disable DAC1 (Right),
        Enable DAC2 (Left), Enable DAC2 (Right)*/
        counter += writeReg16(0x05, 0x0303);

        /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
        counter += writeReg16(0x601, 0x0001);

        /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
        counter += writeReg16(0x602, 0x0001);

        /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
        counter += writeReg16(0x604, 0x0000);

        /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
        counter += writeReg16(0x605, 0x0000);
        break;
    }
    return counter;
  }

  /**
   * @brief Sets new frequency.
   * @param AudioFreq: Audio frequency used to play the audio stream.
   * @retval 0 if correct communication, else wrong communication
   */
  uint32_t setFrequency(uint32_t AudioFreq) {
    uint32_t counter = 0;

    /*  Clock Configurations */
    switch (AudioFreq) {
      case AUDIO_FREQUENCY_8K:
        /* AIF1 Sample Rate = 8 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0003);
        break;

      case AUDIO_FREQUENCY_16K:
        /* AIF1 Sample Rate = 16 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0033);
        break;

      case AUDIO_FREQUENCY_48K:
        /* AIF1 Sample Rate = 48 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0083);
        break;

      case AUDIO_FREQUENCY_96K:
        /* AIF1 Sample Rate = 96 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x00A3);
        break;

      case AUDIO_FREQUENCY_11K:
        /* AIF1 Sample Rate = 11.025 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0013);
        break;

      case AUDIO_FREQUENCY_22K:
        /* AIF1 Sample Rate = 22.050 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0043);
        break;

      case AUDIO_FREQUENCY_44K:
        /* AIF1 Sample Rate = 44.1 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0073);
        break;

      default:
        /* AIF1 Sample Rate = 48 (KHz), ratio=256 */
        counter += writeReg16(0x210, 0x0083);
        break;
    }
    return counter;
  }

  /// Resets wm8994 registers.
  uint32_t reset() {
    uint32_t counter = 0;

    /* Reset Codec by writing in 0x0000 address register */
    counter = writeReg16(0x0000, 0x0000);
    outputEnabled = 0;
    inputEnabled = 0;

    return counter;
  }
  /// Writes a 16-bit register value over I2C
  void writeReg(uint16_t reg, uint16_t value) {
    i2c_bus_write_bytes(i2c_handle, i2c_addr, (uint8_t *)&reg,
                        sizeof(uint16_t), (uint8_t *)&value, sizeof(value));
  }

  /// Reads back a register value over I2C
  uint16_t readReg16(uint16_t reg) {
    uint16_t value = 0;
    i2c_bus_read_bytes(i2c_handle, i2c_addr, (uint8_t *)&reg, sizeof(uint16_t),
                       (uint8_t *)&value, sizeof(value));
    return value;
  }

  /**
   * @brief  Writes a single data value to a codec register.
   * @param  Reg: Register address
   * @param  Value: Data to be written
   * @retval 0 on success, non-zero on verify failure
   */
  uint8_t writeReg16(uint16_t Reg, uint16_t Value) {
    uint32_t result = 0;

    writeReg(Reg, Value);

#ifdef VERIFY_WRITTENDATA
    /* Verify that the data has been correctly written */
    result = (readReg16(Reg) == Value) ? 0 : 1;
#endif /* VERIFY_WRITTENDATA */

    return result;
  }

 protected:
  i2c_bus_handle_t i2c_handle = nullptr;
  int i2c_addr = WM8994_ADDR;
  uint32_t outputEnabled = 0;
  uint32_t inputEnabled = 0;
};

}  // namespace audio_driver

#endif /* __WM8994_H */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
