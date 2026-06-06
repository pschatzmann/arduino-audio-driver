
#include "API_Delay.h"

#ifdef ARDUINO
#include "Arduino.h"
void delayMs(unsigned long ms) { delay(ms); }
#elif defined(__zephyr__)
#include <zephyr/kernel.h>
void delayMs(unsigned long ms) { k_msleep(ms); }
#elif defined(ESP32)
void delayMs(unsigned long ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }
#endif
