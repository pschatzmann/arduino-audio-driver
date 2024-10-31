#include "Utils/Logger.h"
#include "Utils/etc.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Default log level is warning
int LOGLEVEL_AUDIODRIVER = 2;

void AD_LOGD(const char* fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 0) { // LOGLEVEL_AUDIODRIVER = Debug
    char log_buffer[AD_LOGLENGTH+1];
    strcpy(log_buffer, "Debug:   ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer+9, AD_LOGLENGTH-9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

void AD_LOGI(const char* fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 1) { // LOGLEVEL_AUDIODRIVER = Info, Debug
    char log_buffer[AD_LOGLENGTH+1];
    strcpy(log_buffer, "Info:    ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer+9,AD_LOGLENGTH-9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

void AD_LOGW(const char* fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 2) { // LOGLEVEL_AUDIODRIVER = Warning, Info, Debug
    char log_buffer[AD_LOGLENGTH+1];
    strcpy(log_buffer, "Warning:  ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer+9,AD_LOGLENGTH-9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

void AD_LOGE(const char* fmr, ...) {
  if (LOGLEVEL_AUDIODRIVER <= 3) { // for all levels
    char log_buffer[AD_LOGLENGTH+1];
    strcpy(log_buffer, "Error:   ");
    va_list arg;
    va_start(arg, fmr);
    vsnprintf(log_buffer+9,AD_LOGLENGTH-9, fmr, arg);
    va_end(arg);
    audioDriverLogStr(log_buffer);
  }
}

