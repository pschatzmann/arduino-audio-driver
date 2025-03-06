
#pragma once

#include "etc.h"
#include "Logger.h"
#ifdef ARDUINO
#  include "Print.h"
#endif

// define supported log levels
enum class AudioDriverLogLevel {Debug=0, Info, Warning, Error};

/***
 * C++ API to configure logging
 */
class AudioDriverLoggerClass {
public:
    void setLogLevel(AudioDriverLogLevel level){
        LOGLEVEL_AUDIODRIVER = (int) level;
    }
#ifdef ARDUINO
    void setOutput(Print& out) {
        setAudioDriverLogOutput(&out);
    }
    bool begin(Print& out, AudioDriverLogLevel level) {
        setLogLevel(level);
        setOutput(out);
        return true;
    }
#endif
    void end(){
        setAudioDriverLogOutput(nullptr);
    }
};

static AudioDriverLoggerClass AudioDriverLogger;