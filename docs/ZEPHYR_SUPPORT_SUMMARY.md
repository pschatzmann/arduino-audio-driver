## Summary of Changes: `main` → `zephyr`

**71 commits** | **118 source files changed** (+20,401 / -15,018 lines, excluding generated docs)

---

### 1. Header-Only Library Conversion

The entire library has been converted from a compiled library (`.c` / `.cpp` source files) to a **header-only C++ library**. All codec implementations that were previously in `.c`/`.cpp` files under `src/Driver/` have been rewritten as inline C++ classes in `.h` files under `src/Codecs/`.

**Deleted** (36 files): All `.c`/`.cpp` source files under `src/Driver/` — `ac101.c`, `es8388.c`, `wm8960.cpp`, `wm8994.c`, etc., plus platform `.cpp` files (`API_I2C_Arduino.cpp`, `API_I2C_EspressifIDF.cpp`, `API_SPI_Arduino.cpp`, `LoggerSTD.cpp`, etc.).

**CMakeLists.txt** now declares the library as `INTERFACE` (no `SRC_DIRS`/`add_library` with sources), and the ESP-IDF component registration only sets `INCLUDE_DIRS`.

---

### 2. Zephyr RTOS Support

The core addition of this branch — full **Zephyr RTOS** platform support alongside Arduino and ESP-IDF:

- **GPIO abstraction**: `GpioPin` is now `gpio_dt_spec` on Zephyr (device-tree based) vs `int16_t` on Arduino/IDF. New files: `src/Platforms/GPIO_Zephyr.h`, `src/Platforms/GPIO_Arduino.h`, `src/Platforms/GPIO_EspressifIDF.h`. The dispatcher `src/Platforms/GPIO.h` selects the right implementation via `#ifdef`.
- **I2C/SPI**: `src/Platforms/API_I2C.h` now contains inline implementations for all three platforms (Arduino, ESP-IDF, Zephyr) in a single header, replacing the separate `.cpp` files. Same pattern for SPI.
- **Device info**: New `src/DriverDeviceInfoZephyr.h` provides a Zephyr-specific `DriverDeviceInfoZephyr` class using Zephyr's `device*` handles instead of integer pin numbers.
- **Board definitions**: Each board header (e.g., `src/AudioBoards/AudioKitEs8388v1.h`) now has `#if defined(IS_ZEPHYR)` / `#else` blocks to support device-tree-based pin configuration.
- **CMake**: Adds `__zephyr__` compile definition and links `zephyr_interface` when `ZEPHYR_PLATFORM` is set.
- **Delay abstraction**: New `src/Platforms/API_Delay.h` — `delay()` renamed to `delayMs()` for cross-platform compatibility.

---

### 3. Major Renames and Restructuring

| Old Name | New Name |
|---|---|
| `src/Driver.h` | `src/AudioDriver.h` |
| `src/DriverConstants.h` | `src/Codecs/CodecConstants.h` |
| `src/DriverPins.h` | `src/DriverDeviceInfo.h` |
| `src/Platforms/IDriverPins.h` | `src/Platforms/IDriverDeviceInfo.h` |
| `DriverPins` class | `DriverDeviceInfo` class |
| `PinsI2S`, `PinsSPI`, `PinsI2C` | `InfoI2S`, `InfoSPI`, `InfoI2C` (with backward aliases) |
| `PinsFunction` | `InfoGPIO` |
| `src/Driver/*` (codec dir) | `src/Codecs/*` (uppercase filenames) |

Board definitions moved from inline in `AudioBoard.h` to individual files under `src/AudioBoards/`, with an aggregator `src/AudioBoards/AudioBoards.h`. Boards are only auto-included on non-Zephyr platforms.

---

### 4. New Codec Drivers (14 added)

These codecs are **new** (not present on `main` in any form):

| Codec | File |
|---|---|
| AW88298 | `src/Codecs/aw88298/AW88298.h` |
| CS42L51 | `src/Codecs/cs42l51/CS42L51.h` |
| DA7212 | `src/Codecs/da7212/DA7212.h` |
| MAX98091 | `src/Codecs/max98091/MAX98091.h` |
| PCM1681 | `src/Codecs/pcm1681/PCM1681.h` |
| SF32LB | `src/Codecs/sf32lb/SF32LB.h` |
| SGTL5000 | `src/Codecs/sgtl5000/SGTL5000.h` |
| TAS2563 | `src/Codecs/tas2563/TAS2563.h` |
| TAS6422DAC | `src/Codecs/tas6422dac/TAS6422DAC.h` |
| TLV320AIC3110 | `src/Codecs/tlv320aic3110/TLV320AIC3110.h` |
| TLV320DAC310x | `src/Codecs/tlv320dac310x/TLV320DAC310x.h` |
| WM8731 | `src/Codecs/wm8731/WM8731.h` |
| WM8904 | `src/Codecs/wm8904/WM8904.h` |
| WM8962 | `src/Codecs/wm8962/WM8962.h` |

---

### 5. Converted Codecs (C/C++ → Header-Only C++)

All existing codecs were rewritten from C (with separate `.c`/`.h` pairs and `typedef` structs) to idiomatic C++ header-only classes. Codec file names are now **UpperCase** (e.g., `es8388.c` → `ES8388.h`). The `PCBCUPID_` prefix was removed from NAU8325.

---

### 6. Platform Abstraction Cleanup

- **`DriverCommon.h`**: Now contains all platform detection, type aliases (`GpioPin`, `ADCPin`, `i2c_bus_handle_t`), enum definitions (converted from `typedef enum` to `enum`/`enum class`), and Zephyr-specific includes. Platform constants (`HIGH`, `LOW`, `INPUT`, `OUTPUT`) are defined for non-Arduino builds.
- **Macros**: `IS_GPIO(pin)`, `GPIO_TO_INT(pin)`, `GPIO_UNDEFINED` are now platform-aware macros.
- **`etc.h`**: Deleted (was a grab-bag of platform compat definitions now in `DriverCommon.h`).

---

### 7. Build & Config

- **Version**: `0.2.1` → `0.3.0`
- **Project name**: `audio_driver` → `arduino-audio-driver`
- **`AUDIO_DRIVER_USE_NEW_API`**: New define in `src/ConfigAudioDriver.h` marking the new class names are in use.
- **README**: Updated to reflect multi-platform scope (Arduino + Zephyr + IDF), header-only nature, removed inline driver/board listing (moved to Wiki).

---

### 8. Board Definitions

Board definitions are now modular per-file under `src/AudioBoards/` with dual-path pin setup (`IS_ZEPHYR` for device-tree specs, else integer pins). New board files: `Lyrat42.h`, `Lyrat43.h`, `LyratMini.h` (with ADC key support), `M5stackAtomEchoS3R.h`. The `ESP32S3AISmartSpeaker.h` and `STM32F411Disco.h` were moved into this directory.
