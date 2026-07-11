#pragma once
// All ESP32 boards using gpios as ints are automatically included in AudioBoard.h
#if defined(ESP32)
#include "AudioKitAC101.h"
#include "AudioKitEs8388v1.h"
#include "AudioKitEs8388v2.h"
#include "Lyrat42.h"
#include "Lyrat43.h"
#include "LyratMini.h"
#include "M5stackAtomEchoS3R.h"
#include "ESP32S3AISmartSpeaker.h"
#include "ESP32S3HosyondDisplay.h"
#endif

// STM32 boards use STM32-specific pin name macros (e.g. PA0, PB6) that are
// only defined by the STM32 core, so only pull them in on that platform.
#if defined(ARDUINO_ARCH_STM32) || defined(STM32)
#include "STM32F411Disco.h"
#include "STM32F723Disco.h"
#endif
