#pragma once

#ifdef __zephyr__
#  include "Platforms/GPIOZephyr.h"
#else
#  include "Platforms/GPIO.h"
#  include "Platforms/GPIOExt.h"
#endif

