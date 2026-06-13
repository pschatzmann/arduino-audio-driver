#pragma once

/*!
 * @file AudioDriverTypes.h
 * @brief Public types, enumerations, and configuration structures for the Audio
 * Driver.
 *
 * @defgroup audio_driver  Audio Driver
 * @defgroup enumerations  Public Enumeration Types
 */

// ============================================================================
// Standard Includes
// ============================================================================

#include <stdint.h>

#include "ConfigAudioDriver.h"
#include "Platforms/AudioDriverLogger.h"
#include "Platforms/Logger.h"

// ============================================================================
// Platform Abstractions
// ============================================================================

#if defined(ARDUINO)
#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
#define DEFAULT_WIRE &Wire
#else
#define DEFAULT_WIRE nullptr
#undef delay
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW 0x0
#endif
#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif
#endif

// ============================================================================
// Tuning Parameters
// ============================================================================

#ifndef TOUCH_LIMIT
#define TOUCH_LIMIT 20
#endif

#ifndef LYRAT_MINI_RANGE
#define LYRAT_MINI_RANGE 5
#endif

#ifndef LYRAT_MINI_DELAY_MS
#define LYRAT_MINI_DELAY_MS 5
#endif

// ============================================================================
// Error Codes
// ============================================================================

#define RESULT_OK 0         /*!< Indicates success (no error) */
#define RESULT_FAIL -1      /*!< Generic failure code */
#define ERROR_INVALID_ARG 1 /*!< Invalid argument supplied */

// ============================================================================
// Misc Definitions
// ============================================================================

/** Whether to send a stop bit at the end of an I2C transmission. */
#define I2C_END true

// ============================================================================
// GPIO & Bus Handle Abstractions
// ============================================================================

#ifdef __zephyr__

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

namespace audio_driver {

// In Zephyr, GPIO pins are device-tree specifications.
using GpioPin = ::gpio_dt_spec;
using i2c_bus_handle_t = ::device*;
using spi_bus_handle_t = ::device*;
static GpioPin GPIO_UNDEFINED{nullptr, 0, 0};

#ifndef GPIO_COMPARE_DEFINED
#define GPIO_COMPARE_DEFINED

static inline bool operator==(GpioPin& a, GpioPin& b) {
  return (a.port == b.port) && (a.pin == b.pin);
}

/// Support for pin compare

static inline bool operator!=(GpioPin& a, GpioPin& b) {
  return !(a == b);
}

#endif

}  // namespace audio_driver

#ifndef IS_GPIO
#define IS_GPIO(pin) (pin.port != nullptr)
#endif

#ifndef GPIO_TO_INT
#define GPIO_TO_INT(pin) pin.pin
#endif

#else  // Non-Zephyr platforms

// Non-Zephyr platforms use simple integer pin numbers.
#ifndef GPIO_UNDEFINED
#define GPIO_UNDEFINED -1
#endif

#ifndef IS_GPIO
#define IS_GPIO(pin) ((pin) != GPIO_UNDEFINED)
#endif

#ifndef GPIO_TO_INT
#define GPIO_TO_INT(pin) (pin)
#endif

#endif  // __zephyr__

// ============================================================================
// Namespace
// ============================================================================

namespace audio_driver {

using error_t = int;

#ifndef __zephyr__
using GpioPin = int16_t;
using i2c_bus_handle_t = void*;
using spi_bus_handle_t = void*;
#endif

// ----------------------------------------------------------------------------
// ADC / DAC Selection
// ----------------------------------------------------------------------------

/**
 * @enum input_device_t
 * @brief Selects the ADC input source for the microphone signal.
 *        On single-ADC chips, this may select at the channel level.
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum input_device_t {
  ADC_INPUT_NONE = 0x00, /*!< No input selected */
  ADC_INPUT_LINE1,       /*!< Mic input from ADC 1 */
  ADC_INPUT_LINE2,       /*!< Mic input from ADC 2 */
  ADC_INPUT_LINE3,       /*!< Mic input from ADC 3 */
  ADC_INPUT_ALL,         /*!< Mic input from all ADCs */
  ADC_INPUT_DIFFERENCE,  /*!< Mic input via differential channel */
};

/**
 * @enum output_device_t
 * @brief Selects the DAC output destination.
 *        On single-DAC devices, this may select at the channel level.
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum output_device_t {
  DAC_OUTPUT_NONE = 0x00, /*!< No output selected */
  DAC_OUTPUT_LINE1,       /*!< Output to DAC 1 */
  DAC_OUTPUT_LINE2,       /*!< Output to DAC 2 */
  DAC_OUTPUT_ALL,         /*!< Output to both DACs */
};

// ----------------------------------------------------------------------------
// I2S Interface Configuration
// ----------------------------------------------------------------------------

/**
 * @enum i2s_master_slave_t
 * @brief I2S operating mode for the audio codec.
 *        Note: if the microcontroller is master, the codec must be slave.
 * @ingroup enumerations
 */
enum i2s_master_slave_t {
  MODE_SLAVE = 0x00,  /*!< Codec operates as I2S slave */
  MODE_MASTER = 0x01, /*!< Codec operates as I2S master */
};

/**
 * @enum samplerate_t
 * @brief I2S sample rate in samples per second.
 * @ingroup enumerations
 */
enum samplerate_t {
  RATE_8K = 0, /*!<   8,000 samples/s */
  RATE_11K,    /*!<  11,025 samples/s */
  RATE_16K,    /*!<  16,000 samples/s */
  RATE_22K,    /*!<  22,050 samples/s */
  RATE_24K,    /*!<  24,000 samples/s */
  RATE_32K,    /*!<  32,000 samples/s */
  RATE_44K,    /*!<  44,100 samples/s */
  RATE_48K,    /*!<  48,000 samples/s */
  RATE_64K,    /*!<  64,000 samples/s */
  RATE_88K,    /*!<  88,200 samples/s */
  RATE_96K,    /*!<  96,000 samples/s */
  RATE_128K,   /*!< 128,000 samples/s */
  RATE_176K,   /*!< 176,400 samples/s */
  RATE_192K,   /*!< 192,000 samples/s */
};

/**
 * @enum sample_bits_t
 * @brief Number of bits per I2S sample.
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
};

/**
 * @enum i2s_format_t
 * @brief I2S data format for the audio codec.
 * @ingroup enumerations
 */
enum i2s_format_t {
  I2S_NORMAL = 0, /*!< Standard I2S format */
  I2S_LEFT = 1,   /*!< Left-justified format */
  I2S_RIGHT = 2,  /*!< Right-justified format */
  I2S_DSP = 3,    /*!< DSP / PCM format */
};

/**
 * @enum signal_t
 * @brief Signal type carried over the audio interface.
 * @ingroup enumerations
 */
enum signal_t {
  SIGNAL_DIGITAL, /*!< Digital (standard I2S) */
  SIGNAL_AMALOG,  /*!< Analogue */
  SIGNAL_PDM,     /*!< Pulse-density modulation */
  SIGNAL_TDM,     /*!< Time-division multiplexing */
};

/**
 * @enum channels_t
 * @brief Number of audio channels.
 * @ingroup enumerations
 */
enum channels_t {
  CHANNELS2 = 2,
  CHANNELS4 = 4,
  CHANNELS8 = 8,
  CHANNELS16 = 16,
};

// ----------------------------------------------------------------------------
// Codec Operating Mode & Gain
// ----------------------------------------------------------------------------

/**
 * @enum codec_mode_t
 * @brief Selects which codec path(s) are active.
 * @ingroup enumerations
 */
enum codec_mode_t {
  CODEC_MODE_MIN = -1,
  CODEC_MODE_NONE = 0x00,
  CODEC_MODE_ENCODE = 0x01,  /*!< ADC path only */
  CODEC_MODE_DECODE = 0x02,  /*!< DAC path only */
  CODEC_MODE_BOTH = 0x03,    /*!< ADC and DAC paths */
  CODEC_MODE_LINE_IN = 0x04, /*!< Line-in via ADC channel */
  CODEC_MODE_MAX,
};

/**
 * @enum es_mic_gain_t
 * @brief Microphone pre-amplifier gain.
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
};

// ----------------------------------------------------------------------------
// GPIO Pin Abstractions
// ----------------------------------------------------------------------------

/**
 * @enum PinLogic
 * @brief Describes the electrical role and active level of a GPIO pin.
 * @ingroup enumerations
 */
enum class PinLogic {
  InputActiveHigh,
  InputActiveLow,
  InputActiveTouch,
  Input,
  Output,
  Inactive,
};

/**
 * @enum PinFunction
 * @brief Logical function assigned to a GPIO pin.
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum class PinFunction {
  UNDEFINED = 0,
  HEADPHONE_DETECT,
  AUXIN_DETECT,
  PA, /*!< Power Amplifier enable */
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
 * @brief Logical key identifiers for hardware buttons.
 * @ingroup enumerations
 * @ingroup audio_driver
 */
enum class AudioDriverKey {
  KEY_REC = 0,
  KEY_MODE,
  KEY_PLAY,
  KEY_SET,
  KEY_VOLUME_DOWN,
  KEY_VOLUME_UP,
};

// ----------------------------------------------------------------------------
// Configuration Structures
// ----------------------------------------------------------------------------

/**
 * @brief I2S interface parameters for the audio codec.
 * @ingroup audio_driver
 */
struct I2SDefinition {
  i2s_master_slave_t mode; /*!< Master / slave operating mode */
  i2s_format_t fmt;        /*!< Data format */
  samplerate_t rate;       /*!< Sample rate */
  sample_bits_t bits;      /*!< Bits per sample */
  channels_t channels;     /*!< Number of channels */
  signal_t signal_type;    /*!< Signal type */
};

/**
 * @brief Top-level codec configuration passed to the audio driver at init.
 * @ingroup audio_driver
 */
struct codec_config_t {
  input_device_t input_device;   /*!< ADC input source */
  output_device_t output_device; /*!< DAC output destination */
  I2SDefinition i2s;             /*!< I2S interface configuration */
};

}  // namespace audio_driver

// ============================================================================
// Optional Namespace Import
// ============================================================================

#if (defined(ARDUINO) && !defined(NO_USING_NAMESPACE_AUDIO_DRIVER)) || \
    defined(USING_NAMESPACE_AUDIO_DRIVER)
using namespace audio_driver;
#endif
