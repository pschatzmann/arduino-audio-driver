#pragma once

#ifdef ARDUINO
#  include "Arduino.h"
#else

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
void pinMode(int, int);
void digitalWrite(int, int);
void delay(uint32_t);
#endif

#ifdef __cplusplus
extern "C" {
#endif



// pinMode gives linker error if called from c for Nano BLE Sense
void arduino_pin_mode(int pin, int mode);


#ifdef __cplusplus
}
#endif
