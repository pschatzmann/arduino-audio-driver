
#pragma once

#include "etc.h"
#include "Logger.h"
#include "Print.h"

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
    void setOutput(Print& out) {
        setAudioDriverLogOutput(&out);
    }
    bool begin(Print& out, AudioDriverLogLevel level) {
        setLogLevel(level);
        setOutput(out);
        return true;
    }
    void end(){
        setAudioDriverLogOutput(nullptr);
    }
};

static AudioDriverLoggerClass AudioDriverLogger;