#pragma once

// maximum size of log string
#define AUDIODRIVER_LOGLENGTH 215

// Logging of method
#define LOG_METHOD __PRETTY_FUNCTION__

#ifdef __cplusplus
extern "C" {
#endif

// define supported log levels
enum AudioDriverLogLevels {AudioDriverDebug, AudioDriverInfo, AudioDriverWarning, AudioDriverError};

// define default log level
extern int LOGLEVEL_AUDIODRIVER;

void AUDIODRIVER_LOGD(const char* fmr,...);
void AUDIODRIVER_LOGI(const char* fmr,...);
void AUDIODRIVER_LOGW(const char* fmr,...);
void AUDIODRIVER_LOGE(const char* fmr,...);

#ifdef __cplusplus
}
#endif
