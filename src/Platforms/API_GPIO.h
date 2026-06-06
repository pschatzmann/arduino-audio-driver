#pragma once
#include "DriverCommon.h"

namespace audio_driver {

class IDriverDeviceInfo;

class API_GPIO {
 public:
  virtual bool begin(IDriverDeviceInfo& pins) = 0;
  virtual void end() = 0;
  virtual void pinMode(GpioPin pin, int mode) = 0;
  virtual bool digitalWrite(GpioPin pin, bool value) = 0;
  virtual bool digitalRead(GpioPin pin) = 0;
};

}