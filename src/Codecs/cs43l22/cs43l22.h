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

/** CS43l22 Registers  ***/
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

/* Uncomment this line to enable verifying data sent to codec after each write
   operation (for debug purpose) */
#if !defined(VERIFY_WRITTENDATA)
/* #define VERIFY_WRITTENDATA */
#endif /* VERIFY_WRITTENDATA */

namespace audio_driver {

/** @defgroup CS43L22_Exported_Functions
  * @{
  */

static uint8_t Is_cs43l22_Stop = 1;
static i2c_bus_handle_t cs43l22_i2c_handle = NULL;
static volatile uint8_t OutputDev = 0;

static uint8_t CODEC_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);

/*------------------------------------------------------------------------------
                           Audio Codec functions 
------------------------------------------------------------------------------*/
/* High Layer codec functions */
inline uint32_t cs43l22_Init(uint16_t DeviceAddr, uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq, i2c_bus_handle_t cs43l22_i2c_handle);
inline void     cs43l22_DeInit(void);
inline uint32_t cs43l22_ReadID(uint16_t DeviceAddr);
inline uint32_t cs43l22_Play(uint16_t DeviceAddr, uint16_t* pBuffer, uint16_t Size);
inline uint32_t cs43l22_Pause(uint16_t DeviceAddr);
inline uint32_t cs43l22_Resume(uint16_t DeviceAddr);
inline uint32_t cs43l22_Stop(uint16_t DeviceAddr, uint32_t Cmd);
inline uint32_t cs43l22_SetVolume(uint16_t DeviceAddr, uint8_t Volume);
inline uint32_t cs43l22_SetFrequency(uint16_t DeviceAddr, uint32_t AudioFreq);
inline uint32_t cs43l22_SetMute(uint16_t DeviceAddr, uint32_t Cmd);
inline uint32_t cs43l22_SetOutputMode(uint16_t DeviceAddr, uint8_t Output);
inline uint32_t cs43l22_Reset(uint16_t DeviceAddr);

/* AUDIO IO functions */
inline void      AUDIO_IO_Init(void);
inline void      AUDIO_IO_DeInit(void);
inline void      AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t          AUDIO_IO_Read(uint8_t Addr, uint8_t Reg);

/* Audio driver structure */
//extern AUDIO_DrvTypeDef   cs43l22_drv;

/*------------------------------------------------------------------------------
                           Implementations
------------------------------------------------------------------------------*/

static uint8_t CODEC_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value) {
  uint32_t result = 0;

  AUDIO_IO_Write(Addr, Reg, Value);

#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */
  result = (AUDIO_IO_Read(Addr, Reg) == Value) ? 0 : 1;
#endif /* VERIFY_WRITTENDATA */

  return result;
}

/**
 * @brief Initializes the audio codec and the control interface.
 * @param DeviceAddr: Device address on communication Bus.
 * @param OutputDevice: can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
 *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO .
 * @param Volume: Initial volume level (from 0 (Mute) to 100 (Max))
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_Init(uint16_t DeviceAddr, uint16_t OutputDevice,
                      uint8_t Volume, uint32_t AudioFreq,
                      i2c_bus_handle_t handle) {
  uint32_t counter = 0;
  cs43l22_i2c_handle = handle;

  /* Initialize the Control interface of the Audio Codec */
  AUDIO_IO_Init();

  /* Keep Codec powered OFF */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL1, 0x01);

  /*Save Output device for mute ON/OFF procedure*/
  switch (OutputDevice) {
    case OUTPUT_DEVICE_SPEAKER:
      OutputDev = 0xFA;
      break;

    case OUTPUT_DEVICE_HEADPHONE:
      OutputDev = 0xAF;
      break;

    case OUTPUT_DEVICE_BOTH:
      OutputDev = 0xAA;
      break;

    case OUTPUT_DEVICE_AUTO:
      OutputDev = 0x05;
      break;

    default:
      OutputDev = 0x05;
      break;
  }

  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2, OutputDev);

  /* Clock configuration: Auto detection */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_CLOCKING_CTL, 0x81);

  /* Set the Slave Mode and the audio Standard */
  counter +=
      CODEC_IO_Write(DeviceAddr, CS43L22_REG_INTERFACE_CTL1, CODEC_STANDARD);

  /* Set the Master volume */
  counter += cs43l22_SetVolume(DeviceAddr, Volume);

  /* If the Speaker is enabled, set the Mono mode and volume attenuation level
   */
  if (OutputDevice != OUTPUT_DEVICE_HEADPHONE) {
    /* Set the Speaker Mono mode */
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_PLAYBACK_CTL2, 0x06);

    /* Set the Speaker attenuation level */
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_SPEAKER_A_VOL, 0x00);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_SPEAKER_B_VOL, 0x00);
  }

  /* Additional configuration for the CODEC. These configurations are done to
  reduce the time needed for the Codec to power off. If these configurations are
  removed, then a long delay should be added between powering off the Codec and
  switching off the I2S peripheral MCLK clock (which is the operating clock for
  Codec). If this delay is not inserted, then the codec will not shut down
  properly and it results in high noise after shut down. */

  /* Disable the analog soft ramp */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_ANALOG_ZC_SR_SETT, 0x00);
  /* Disable the digital soft ramp */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MISC_CTL, 0x04);
  /* Disable the limiter attack level */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_LIMIT_CTL1, 0x00);
  /* Adjust Bass and Treble levels */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_TONE_CTL, 0x0F);
  /* Adjust PCM volume level */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_PCMA_VOL, 0x0A);
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_PCMB_VOL, 0x0A);

  /* Return communication control value */
  return counter;
}

/**
 * @brief  Deinitializes the audio codec.
 * @param  None
 * @retval  None
 */
inline void cs43l22_DeInit(void) {
  /* Deinitialize Audio Codec interface */
  AUDIO_IO_DeInit();
}

/**
 * @brief  Get the CS43L22 ID.
 * @param DeviceAddr: Device address on communication Bus.
 * @retval The CS43L22 ID
 */
inline uint32_t cs43l22_ReadID(uint16_t DeviceAddr) {
  uint8_t Value;
  /* Initialize the Control interface of the Audio Codec */
  AUDIO_IO_Init();

  Value = AUDIO_IO_Read(DeviceAddr, CS43L22_CHIPID_ADDR);
  Value = (Value & CS43L22_ID_MASK);

  return ((uint32_t)Value);
}

/**
 * @brief Start the audio Codec play feature.
 * @note For this codec no Play options are required.
 * @param DeviceAddr: Device address on communication Bus.
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_Play(uint16_t DeviceAddr, uint16_t* pBuffer, uint16_t Size) {
  uint32_t counter = 0;

  if (Is_cs43l22_Stop == 1) {
    /* Enable the digital soft ramp */
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MISC_CTL, 0x06);

    /* Enable Output device */
    counter += cs43l22_SetMute(DeviceAddr, AUDIO_MUTE_OFF);

    /* Power on the Codec */
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL1, 0x9E);
    Is_cs43l22_Stop = 0;
  }

  /* Return communication control value */
  return counter;
}

/**
 * @brief Pauses playing on the audio codec.
 * @param DeviceAddr: Device address on communication Bus.
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_Pause(uint16_t DeviceAddr) {
  uint32_t counter = 0;

  /* Pause the audio file playing */
  /* Mute the output first */
  counter += cs43l22_SetMute(DeviceAddr, AUDIO_MUTE_ON);

  /* Put the Codec in Power save mode */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL1, 0x01);

  return counter;
}

/**
 * @brief Resumes playing on the audio codec.
 * @param DeviceAddr: Device address on communication Bus.
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_Resume(uint16_t DeviceAddr) {
  uint32_t counter = 0;
  volatile uint32_t index = 0x00;
  /* Resumes the audio file playing */
  /* Unmute the output first */
  counter += cs43l22_SetMute(DeviceAddr, AUDIO_MUTE_OFF);

  delayMs(10);

  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2, OutputDev);

  /* Exit the Power save mode */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL1, 0x9E);

  return counter;
}

/**
 * @brief Stops audio Codec playing. It powers down the codec.
 * @param DeviceAddr: Device address on communication Bus.
 * @param CodecPdwnMode: selects the  power down mode.
 *          - CODEC_PDWN_HW: Physically power down the codec. When resuming from
 * this mode, the codec is set to default configuration (user should
 * re-Initialize the codec in order to play again the audio stream).
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_Stop(uint16_t DeviceAddr, uint32_t CodecPdwnMode) {
  uint32_t counter = 0;

  /* Mute the output first */
  counter += cs43l22_SetMute(DeviceAddr, AUDIO_MUTE_ON);

  /* Disable the digital soft ramp */
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MISC_CTL, 0x04);

  /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
  counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL1, 0x9F);

  Is_cs43l22_Stop = 1;
  return counter;
}

/**
 * @brief Sets higher or lower the codec volume level.
 * @param DeviceAddr: Device address on communication Bus.
 * @param Volume: a byte value from 0 to 100 ( which is mapped to 0 - 255 ,
 *                 refer to codec registers description for more details).
 *
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_SetVolume(uint16_t DeviceAddr, uint8_t Volume) {
  uint32_t counter = 0;
  uint8_t convertedvol = CS43L22_VOLUME_CONVERT(Volume);

  if (convertedvol > 0xE6) {
    /* Set the Master volume */
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MASTER_A_VOL,
                              convertedvol - 0xE7);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MASTER_B_VOL,
                              convertedvol - 0xE7);
  } else {
    /* Set the Master volume */
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MASTER_A_VOL,
                              convertedvol + 0x19);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_MASTER_B_VOL,
                              convertedvol + 0x19);
  }

  return counter;
}

/**
 * @brief Sets new frequency.
 * @param DeviceAddr: Device address on communication Bus.
 * @param AudioFreq: Audio frequency used to play the audio stream.
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_SetFrequency(uint16_t DeviceAddr, uint32_t AudioFreq) {
  return 0;
}

/**
 * @brief Enables or disables the mute feature on the audio codec.
 * @param DeviceAddr: Device address on communication Bus.
 * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
 *             mute mode.
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_SetMute(uint16_t DeviceAddr, uint32_t Cmd) {
  uint32_t counter = 0;

  /* Set the Mute mode */
  if (Cmd == AUDIO_MUTE_ON) {
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2, 0xFF);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_HEADPHONE_A_VOL, 0x01);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_HEADPHONE_B_VOL, 0x01);
  } else /* AUDIO_MUTE_OFF Disable the Mute */
  {
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_HEADPHONE_A_VOL, 0x00);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_HEADPHONE_B_VOL, 0x00);
    counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2, OutputDev);
  }
  return counter;
}

/**
 * @brief Switch dynamically (while audio file is played) the output target
 *         (speaker or headphone).
 * @note This function modifies a global variable of the audio codec driver:
 * OutputDev.
 * @param DeviceAddr: Device address on communication Bus.
 * @param Output: specifies the audio output target: OUTPUT_DEVICE_SPEAKER,
 *         OUTPUT_DEVICE_HEADPHONE, OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_SetOutputMode(uint16_t DeviceAddr, uint8_t Output) {
  uint32_t counter = 0;

  switch (Output) {
    case OUTPUT_DEVICE_SPEAKER:
      counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2,
                                0xFA); /* SPK always ON & HP always OFF */
      OutputDev = 0xFA;
      break;

    case OUTPUT_DEVICE_HEADPHONE:
      counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2,
                                0xAF); /* SPK always OFF & HP always ON */
      OutputDev = 0xAF;
      break;

    case OUTPUT_DEVICE_BOTH:
      counter += CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2,
                                0xAA); /* SPK always ON & HP always ON */
      OutputDev = 0xAA;
      break;

    case OUTPUT_DEVICE_AUTO:
      counter +=
          CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2,
                         0x05); /* Detect the HP or the SPK automatically */
      OutputDev = 0x05;
      break;

    default:
      counter +=
          CODEC_IO_Write(DeviceAddr, CS43L22_REG_POWER_CTL2,
                         0x05); /* Detect the HP or the SPK automatically */
      OutputDev = 0x05;
      break;
  }
  return counter;
}

/**
 * @brief Resets cs43l22 registers.
 * @param DeviceAddr: Device address on communication Bus.
 * @retval 0 if correct communication, else wrong communication
 */
inline uint32_t cs43l22_Reset(uint16_t DeviceAddr) { return 0; }

/**
 * @brief  Writes/Read a single data.
 * @param  Addr: I2C address
 * @param  Reg: Reg address
 * @param  Value: Data to be written
 * @retval None
 */
inline void AUDIO_IO_Init(void) {}
inline void AUDIO_IO_DeInit(void) {}
inline void AUDIO_IO_Write(uint8_t addr, uint8_t reg, uint8_t value) {
  i2c_bus_write_bytes(cs43l22_i2c_handle, addr, &reg, 1, &value, 1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

} // namespace audio_driver
