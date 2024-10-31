#include "Arduino.h"
#include "Utils/etc.h"

Print *p_audio_driver_log_output = &Serial;

/// Arcuino c++ println function 
void audioDriverLogStr(const char* msg){
    if (p_audio_driver_log_output) p_audio_driver_log_output->println(msg);
}

void setAudioDriverLogOutput(void *out){
    p_audio_driver_log_output = (Print*) out;
}
