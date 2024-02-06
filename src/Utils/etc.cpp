#include "Arduino.h"
#include "etc.h"

void arduino_pin_mode(int pin, int mode){
    pinMode(pin, mode);
}
