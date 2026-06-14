
## Arduino Audio Driver

[![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue.svg)](https://www.arduino.cc/reference/en/libraries/)
[![Zephyr Library](https://img.shields.io/badge/Zephyr-Library-blue.svg)](https://github.com/pschatzmann/arduino-audio-driver)
[![IDF Component](https://img.shields.io/badge/IDF-Component-blue.svg)](https://github.com/pschatzmann/arduino-audio-driver)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-green.svg)](https://www.gnu.org/licenses/gpl-3.0)

This project replaces the obsolete [AudioKit project](https://github.com/pschatzmann/arduino-audiokit), that was a quick and dirty adaptation of the Espressif IDF Drivers for Arduino. 

In this project I finally implemented some clean and simple C++ classes. This __reduced the complexity__ considerably and __increased the flexibility__ tremendously!

The goal of this project is to provide an __easy header-only C++ API to configure different audio codec chips__. After setting up the codec, you can use the I2S functionality provided by your microcontroller.

While you can use this library stand alone, I recommend to use it together with my [AudioTools](https://github.com/pschatzmann/arduino-audio-tools) project which provides a nice integration with it's [I2SCodecStream](https://pschatzmann.github.io/arduino-audio-tools/classaudio__tools_1_1_i2_s_codec_stream.html) class. Further information can be found in the [Wiki](https://github.com/pschatzmann/arduino-audio-Codecs/wiki).



## Logging

The functionality has a built in logger. The default log level has been set to Warning. You can change it like this:

```C++

  AudioDriverLogger.begin(Serial,AudioDriverLogLevel::Info); 

```
Valid values are: Debug, Info, Warning, Error

You can deactivate the logging completly and if you are using an ESP32, you can configure the use of the built in ESP32 IDF logger in ConfigAudioDriver.h

## Documentation

- [Wiki](https://github.com/pschatzmann/arduino-audio-Codecs/wiki)
- [Drivers and Boards](https://github.com/pschatzmann/arduino-audio-driver/wiki/Drivers-and-Boards)
- [Classes and Objects](https://pschatzmann.github.io/arduino-audio-Codecs/html/group__audio__driver.html)
- [Examples](https://github.com/pschatzmann/arduino-audio-Codecs/tree/main/examples)

## Support

I spent a lot of time to provide a comprehensive and complete documentation.
So please read the documentation first and check the issues and discussions before posting any new ones on Github.

Open issues only for bugs and if it is not a bug, use a discussion: Provide enough information about 
- the selected scenario/sketch 
- what exactly you are trying to do
- your hardware
- your software versions
- what exactly your problem is

to enable others to understand and reproduce your issue.

Finally, __don't__ send me any e-mails or post questions on my personal website! 


## Installation in Arduino

You can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone https://github.com/pschatzmann/arduino-audio-driver.git

```

I recommend to use git because you can easily update to the latest version just by executing the git pull command in the project folder. 

Please consult the Wiki on how to use this library in IDF, Zephyr or PlatformIO


## Sponsor Me

This software is totally free, but you can make me happy by rewarding me with a treat

- [Buy me a coffee](https://www.buymeacoffee.com/philschatzh)
- [Paypal me](https://paypal.me/pschatzmann?country.x=CH&locale.x=en_US)
