#pragma once

#ifdef ARDUINO
#  include "Arduino.h"
namespace audio_driver {
inline void delayMs(unsigned long ms) { delay(ms); }
} // namespace audio_driver
#elif defined(__zephyr__)
#  include <zephyr/kernel.h>
namespace audio_driver {
inline void delayMs(unsigned long ms) { k_msleep(ms); }
} // namespace audio_driver
#elif defined(ESP32)
namespace audio_driver {
inline void delayMs(unsigned long ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }
} // namespace audio_driver
#else
namespace audio_driver {
inline void delayMs(unsigned long ms) {}
} // namespace audio_driver
#endif
