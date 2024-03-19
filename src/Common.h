#pragma once
#include <stdint.h>

#include "Utils/Logger.h"
/*!
 * @file
 * @defgroup audio_driver Audio Driver
 * @defgroup enumerations Public enumeration types
 */

// To increase the max volume e.g. for ai_thinker (ES8388) 2957 or A202 -> set
// to 1 or 2 0 AUX volume is LINE level 1 you can control the AUX volume with
// setVolume()
#ifndef AI_THINKER_ES8388_VOLUME_HACK
#define AI_THINKER_ES8388_VOLUME_HACK 1
#endif

// Default volume at startup
#ifndef DRIVER_DEFAULT_VOLUME
#define DRIVER_DEFAULT_VOLUME 70
#endif

// Define the default gain for the microphone amp (see values from
// es_mic_gain_t) Alternativly you can call es8388_set_mic_gain(es_mic_gain_t
// gain) if you prefer to use value from an comprehensive enum
#ifndef ES8388_DEFAULT_INPUT_GAIN
#define ES8388_DEFAULT_INPUT_GAIN 25
#endif

/// Fixed Definitions
#define RESULT_OK 0    /*!< error_t value indicating success (no error) */
#define RESULT_FAIL -1 /*!< Generic error_t code indicating failure */
#define ERROR_INVALID_ARG 1
#define I2C_END true

#ifdef __cplusplus
namespace audio_driver {
#endif

typedef int error_t;
typedef void *i2c_bus_handle_t;
typedef void *i2c_cmd_handle_t;

/**
 * @enum input_device_t
 * @brief Select adc channel for input mic signal
 * @ingroup enumerations
 * @ingroup audio_driver
 */
typedef enum {
  ADC_INPUT_NONE = 0x00, /*!< no input */
  ADC_INPUT_LINE1,       /*!< mic input to adc channel 1 */
  ADC_INPUT_LINE2,       /*!< mic input to adc channel 2 */
  ADC_INPUT_LINE3,       /*!< mic input to adc channel 3 */
  ADC_INPUT_ALL,         /*!< mic input to both channels of adc */
  ADC_INPUT_DIFFERENCE,  /*!< mic input to adc difference channel */
} input_device_t;

/**
 * @enum output_device_t
 * @brief Select channel for dac output
 * @ingroup enumerations
 * @ingroup audio_driver
 */
typedef enum {
  DAC_OUTPUT_NONE = 0x00, /*!< no output */
  DAC_OUTPUT_LINE1,       /*!< dac output signal to channel 1 */
  DAC_OUTPUT_LINE2,       /*!< dac output signal to channel 2 */
  DAC_OUTPUT_ALL,         /*!< dac output signal to both channels */
} output_device_t;

/**
 * @enum i2s_master_slave_t
 * @brief Select I2S interface operating mode i.e. master or slave for audio
 * codec chip
 * @ingroup enumerations
 */
typedef enum {
  MODE_SLAVE = 0x00,  /*!< set slave mode */
  MODE_MASTER = 0x01, /*!< set master mode */
} i2s_master_slave_t;

/**
 * @enum samplerate_t
 * @brief Select I2S interface samples per second
 * @ingroup enumerations
 */
typedef enum {
  RATE_8K = 0, /*!< set to  8k samples per second */
  RATE_11K,    /*!< set to 11.025k samples per second */
  RATE_16K,    /*!< set to 16k samples in per second */
  RATE_22K,    /*!< set to 22.050k samples per second */
  RATE_24K,    /*!< set to 24k samples in per second */
  RATE_32K,    /*!< set to 32k samples in per second */
  RATE_44K,    /*!< set to 44.1k samples per second */
  RATE_48K,    /*!< set to 48k samples per second */
  RATE_64K,    /*!< set to 64k samples per second */
  RATE_88K,    /*!< set to 88.2k samples per second */
  RATE_96K,    /*!< set to 96k samples per second */
  RATE_128K,   /*!< set to 128K samples per second */
  RATE_176K,   /*!< set to 176.4K samples per second */
  RATE_192K,   /*!< set to 192k samples per second */
} samplerate_t;

/**
 * @enum sample_bits_t
 * @brief Select I2S interface number of bits per sample
 * @ingroup enumerations
 */
typedef enum {
  BIT_LENGTH_MIN = -1,
  BIT_LENGTH_16BITS = 0x03,
  BIT_LENGTH_18BITS = 0x02,
  BIT_LENGTH_20BITS = 0x01,
  BIT_LENGTH_24BITS = 0x00,
  BIT_LENGTH_32BITS = 0x04,
  BIT_LENGTH_MAX,
} sample_bits_t;

/**
 * @enum i2s_format_t
 * @brief Select I2S interface format for audio codec chip
 * @ingroup enumerations
 */
typedef enum {
  I2S_NORMAL = 0, /*!< set normal I2S format */
  I2S_LEFT = 1,   /*!< set all left format */
  I2S_RIGHT = 2,  /*!< set all right format */
  I2S_DSP = 3,    /*!< set dsp/pcm format */
  TDM = 4,        /*!< set tdm format */
} i2s_format_t;

/**
 * @enum es_mic_gain_t
 * @brief Microphone Gain
 * @ingroup enumerations
 */
typedef enum {
  MIC_GAIN_MIN = -1,
  MIC_GAIN_0DB = 0,
  MIC_GAIN_3DB = 3,
  MIC_GAIN_6DB = 6,
  MIC_GAIN_9DB = 9,
  MIC_GAIN_12DB = 12,
  MIC_GAIN_15DB = 15,
  MIC_GAIN_18DB = 18,
  MIC_GAIN_21DB = 21,
  MIC_GAIN_24DB = 24,
  MIC_GAIN_MAX,
} es_mic_gain_t;

/**
 * @enum codec_mode_t
 * @brief Select media hal codec mode
 * @ingroup enumerations
 */
typedef enum {
  CODEC_MODE_MIN = -1,
  CODEC_MODE_NONE = 0x00,
  CODEC_MODE_ENCODE = 0x01,  /*!< select adc */
  CODEC_MODE_DECODE = 0x02,  /*!< select dac */
  CODEC_MODE_BOTH = 0x03,    /*!< select both adc and dac */
  CODEC_MODE_LINE_IN = 0x04, /*!< set adc channel */
  CODEC_MODE_MAX
} codec_mode_t;

typedef enum {
  CHANNELS2 = 2,
  CHANNELS8 = 8,
  CHANNELS16 = 16,
} channels_t;

/**
 * @brief I2s interface configuration for audio codec chip
 * @ingroup audio_driver
 */
typedef struct {
  /*!< Audio codec chip mode: if the microcontroller is master the codec must be
   * slave! */
  i2s_master_slave_t mode;
  /*!< I2S interface format */
  i2s_format_t fmt;
  /*!< I2S sample rate in samples per second */
  samplerate_t rate;
  /*!< i2s number of bits per sample */
  sample_bits_t bits;
  /*!< i2s number of channels */
  channels_t channels;
} I2SDefinition;

/**
 * @brief Configure media hal for initialization of audio codec chip
 */
typedef struct {
  input_device_t input_device;   /*!< set adc channel */
  output_device_t output_device; /*!< set dac channel */
  I2SDefinition i2s;             /*!< set I2S interface configuration */
} codec_config_t;

#ifdef __cplusplus
}
// automatically use namespace
using namespace audio_driver;
#endif
