#pragma once

/*!
 * @file
 * @defgroup audio_driver Audio Driver
 * @defgroup enumerations Public enumeration types
 */

#include <stdint.h>
#include "ConfigAudioDriver.h"
#include "Platforms/Logger.h"

#if defined(ARDUINO)
#  include "Arduino.h"
#  include "Wire.h"
#  include "SPI.h"
#  define DEFAULT_WIRE &Wire
#else
#  define DEFAULT_WIRE nullptr
#  undef delay
#ifndef HIGH
#  define HIGH 0x1
#endif
#ifndef LOW
#  define LOW  0x0
#endif
#ifndef INPUT
#  define INPUT 0x0
#endif
#ifndef OUTPUT
#  define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#  define INPUT_PULLUP 0x2
#endif
#endif

#ifndef TOUCH_LIMIT
# define TOUCH_LIMIT 20
#endif

#ifndef LYRAT_MINI_RANGE
# define LYRAT_MINI_RANGE 5
#endif

#ifndef LYRAT_MINI_DELAY_MS
# define LYRAT_MINI_DELAY_MS 5
#endif

/// Fixed Definitions
#define RESULT_OK 0    /*!< error_t value indicating success (no error) */
#define RESULT_FAIL -1 /*!< Generic error_t code indicating failure */
#define ERROR_INVALID_ARG 1

#define I2C_END  true  // wether to send a stop bit at the end of the transmission

#ifdef __zephyr__
#include <zephyr/device.h>
#include <zephyr/kernel.h>
// In Zephyr, GPIO pins are defined as device tree specifications, so we use a
// pointer to the gpio_dt_spec struct instead of an integer pin number.
typedef struct gpio_dt_spec* GpioPin;
typedef struct device* i2c_bus_handle_t;
typedef struct device* spi_bus_handle_t;
#endif

#include "Platforms/AudioDriverLogger.h"

namespace audio_driver {

typedef int error_t;

#ifndef __zephyr__
// For Arduino and other platforms, we can use a simple integer pin number.
typedef int16_t GpioPin;
typedef void* i2c_bus_handle_t;
typedef void* spi_bus_handle_t;
#endif

/**
 * @enum input_device_t
 * @brief Select adc for input mic signal. If the chip only has one ADC this
 * might provide the functionality on the channel level
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum input_device_t {
  ADC_INPUT_NONE = 0x00, /*!< no input */
  ADC_INPUT_LINE1,       /*!< mic input from adc 1 */
  ADC_INPUT_LINE2,       /*!< mic input from adc 2 */
  ADC_INPUT_LINE3,       /*!< mic input from adc 3 */
  ADC_INPUT_ALL,         /*!< mic input from all adc */
  ADC_INPUT_DIFFERENCE,  /*!< mic input to adc difference channel */
} ;

/**
 * @enum output_device_t
 * @brief Select individual dac for dac output. If the device has only one DAC
 * this might provide the functionality channel level
 * @ingroup enumerations
 * @ingroup audio_driver
 */
 enum output_device_t {
  DAC_OUTPUT_NONE = 0x00, /*!< no output */
  DAC_OUTPUT_LINE1,       /*!< dac output signal to dac 1 */
  DAC_OUTPUT_LINE2,       /*!< dac output signal to dac 2 */
  DAC_OUTPUT_ALL,         /*!< dac output signal to both dacs */
} ;

/**
 * @enum i2s_master_slave_t
 * @brief Select I2S interface operating mode i.e. master or slave for audio
 * codec chip
 * @ingroup enumerations
 */
enum i2s_master_slave_t {
  MODE_SLAVE = 0x00,  /*!< set slave mode */
  MODE_MASTER = 0x01, /*!< set master mode */
} ;

/**
 * @enum samplerate_t
 * @brief Select I2S interface samples per second
 * @ingroup enumerations
 */
enum samplerate_t {
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
} ;

/**
 * @enum sample_bits_t
 * @brief Select I2S interface number of bits per sample
 * @ingroup enumerations
 */
enum sample_bits_t {
  BIT_LENGTH_MIN = -1,
  BIT_LENGTH_16BITS = 0x03,
  BIT_LENGTH_18BITS = 0x02,
  BIT_LENGTH_20BITS = 0x01,
  BIT_LENGTH_24BITS = 0x00,
  BIT_LENGTH_32BITS = 0x04,
  BIT_LENGTH_MAX,
} ;

/**
 * @enum i2s_format_t
 * @brief Select I2S interface format for audio codec chip
 * @ingroup enumerations
 */
enum i2s_format_t {
  I2S_NORMAL = 0, /*!< set normal I2S format */
  I2S_LEFT = 1,   /*!< set all left format */
  I2S_RIGHT = 2,  /*!< set all right format */
  I2S_DSP = 3,    /*!< set dsp/pcm format */
} ;

/**
 * @enum signal_t
 * @brief Usually the value is digital for i2s
 * @ingroup enumerations
 */
enum signal_t {
  SIGNAL_DIGITAL,
  SIGNAL_AMALOG,
  SIGNAL_PDM,
  SIGNAL_TDM,
} ;
/**
 * @enum es_mic_gain_t
 * @brief Microphone Gain
 * @ingroup enumerations
 */
enum es_mic_gain_t {
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
} ;

/**
 * @enum codec_mode_t
 * @brief Select media hal codec mode
 * @ingroup enumerations
 */
enum codec_mode_t {
  CODEC_MODE_MIN = -1,
  CODEC_MODE_NONE = 0x00,
  CODEC_MODE_ENCODE = 0x01,  /*!< select adc */
  CODEC_MODE_DECODE = 0x02,  /*!< select dac */
  CODEC_MODE_BOTH = 0x03,    /*!< select both adc and dac */
  CODEC_MODE_LINE_IN = 0x04, /*!< set adc channel */
  CODEC_MODE_MAX
} ;

/**
 * @enum channels_t
 * @brief Select the number of channels
 * @ingroup enumerations
 */
enum channels_t {
  CHANNELS2 = 2,
  CHANNELS4 = 4,
  CHANNELS8 = 8,
  CHANNELS16 = 16,
};

/**
 * @brief I2s interface configuration for audio codec chip
 * @ingroup audio_driver
 */
 struct I2SDefinition {
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
  /*!< signal tpye */
  signal_t signal_type;

} ;

/**
 * @brief Configure media hal for initialization of audio codec chip
 */
struct  codec_config_t{
  input_device_t input_device;   /*!< set adc channel */
  output_device_t output_device; /*!< set dac channel */
  I2SDefinition i2s;             /*!< set I2S interface configuration */
} ;

/**
 * @enum PinLogic
 * @brief input or output
 * @ingroup enumerations
 */

enum class  PinLogic {
  InputActiveHigh,
  InputActiveLow,
  InputActiveTouch,
  Input,
  Output,
  Inactive,
};

/**
 * @enum PinFunction
 * @brief Pin Functions
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum class  PinFunction {
  UNDEFINED = 0,
  HEADPHONE_DETECT,
  AUXIN_DETECT,
  PA,  // Power Amplifier
  POWER,
  LED,
  KEY,
  SD,
  CODEC,
  CODEC_ADC,
  LATCH,
  RESET,
  MCLK_SOURCE,
  EXPANDER,
};

/**
 * @enum AudioDriverKey
 * @brief Key names
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum class AudioDriverKey {
  KEY_REC = 0,
  KEY_MODE,
  KEY_PLAY,
  KEY_SET,
  KEY_VOLUME_DOWN,
  KEY_VOLUME_UP
};

} // namespace audio_driver

// automatically use namespace
using namespace audio_driver;
