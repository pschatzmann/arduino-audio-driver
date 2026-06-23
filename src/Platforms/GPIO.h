#pragma once

#ifdef __zephyr__
#  include "Platforms/GPIO_Zephyr.h"
#elif defined(ESP32_CMAKE)
#  include "Platforms/GPIO_EspressifIDF.h"
#  include "Platforms/GPIOExt.h"
#elif defined(ARDUINO)
#  include "Platforms/GPIO_Arduino.h"
#  include "Platforms/GPIOExt.h"
#endif
