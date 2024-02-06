#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// pinMode gives linker error if called from c for Nano BLE Sense
void arduino_pin_mode(int pin, int mode);


#ifdef __cplusplus
}
#endif
