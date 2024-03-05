#pragma once

#ifdef ARDUINO
#  include "Arduino.h"
#else
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


#ifdef __cplusplus
extern "C" {
#endif
void logStr(const char* msg);
#ifndef ARDUINO
void pinMode(int, int);
void digitalWrite(int, int);
void delay(unsigned long);
#endif


#ifdef __cplusplus
}
#endif
