// logging methods that can be called from C and C++
#pragma once
#include "ConfigAudioDriver.h"

// maximum size of log string
#define AD_LOGLENGTH 215

// Logging of method
#define LOG_METHOD __PRETTY_FUNCTION__
// Log trace
#define AD_TRACED() AD_LOGD(LOG_METHOD)

#if AUDIO_DRIVER_LOGGIN_ACTVIE && !defined(AUDIO_DRIVER_LOGGING_IDF)

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(ARDUINO)
#  include "Arduino.h"
#endif

inline int LOGLEVEL_AUDIODRIVER = AUDIO_DRIVER_LOG_LEVEL;

#if defined(ARDUINO)
inline Print *p_audio_driver_log_output = &Serial;
#endif

inline void audioDriverLogStr(const char *msg) {
#if defined(ARDUINO)
  if (p_audio_driver_log_output) p_audio_driver_log_output->println(msg);
#else
  printf("%s\n", msg);
#endif
}

inline void AD_LOGD(const char *fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 0) {
    char log_buffer[AD_LOGLENGTH + 1];
    strcpy(log_buffer, "Debug:   ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer + 9, AD_LOGLENGTH - 9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

inline void AD_LOGI(const char *fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 1) {
    char log_buffer[AD_LOGLENGTH + 1];
    strcpy(log_buffer, "Info:    ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer + 9, AD_LOGLENGTH - 9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

inline void AD_LOGW(const char *fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 2) {
    char log_buffer[AD_LOGLENGTH + 1];
    strcpy(log_buffer, "Warning:  ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer + 9, AD_LOGLENGTH - 9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

inline void AD_LOGE(const char *fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 3) {
    char log_buffer[AD_LOGLENGTH + 1];
    strcpy(log_buffer, "Error:   ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer + 9, AD_LOGLENGTH - 9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

#if defined(ARDUINO)
inline void setAudioDriverLogOutput(void *out) {
  p_audio_driver_log_output = (Print *)out;
}
#else
inline void setAudioDriverLogOutput(void *out) {}
#endif

#endif  // AUDIO_DRIVER_LOGGIN_ACTVIE && !defined(AUDIO_DRIVER_LOGGING_IDF)
