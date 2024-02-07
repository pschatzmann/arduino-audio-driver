
## Arduino Audio Driver

I was never quite happy with my [AudioKit project](https://github.com/pschatzmann/arduino-audiokit), that was a quick and dirty adaptation of the Espressif IDF Drivers for Arduino. 

In this project I finally implemented some clean and simple C++ classes. This __reduced the complexity__ considerably and __increased the flexibility__ tremendously!

The goal of this project is to provide an __easy API to configure different audio codec chips__. After setting up the codec, you can use the I2S functionality provided by your microcontroller.

Supported audio codec chips are e.g

- AC101
- ES8388
- ES8311
- CS43l22
- ES7243
- etc

While you can use this library stand alone, I recommend to use it together with my [AudioTools](https://github.com/pschatzmann/arduino-audio-tools) project which provides a nice integration with it's [I2SCodecStream](https://pschatzmann.github.io/arduino-audio-tools/classaudio__tools_1_1_i2_s_codec_stream.html) class. Further information can be found in the [Wiki](https://github.com/pschatzmann/arduino-audio-driver/wiki).


## Documentation

- [Wiki](https://github.com/pschatzmann/arduino-audio-driver/wiki)
- [Classes and Objects](https://pschatzmann.github.io/arduino-audio-driver/html/group__audio__driver.html)
- [Examples](https://github.com/pschatzmann/arduino-audio-driver/tree/main/examples)

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


## Sponsor Me

This software is totally free, but you can make me happy by rewarding me with a treat

- [Buy me a coffee](https://www.buymeacoffee.com/philschatzh)
- [Paypal me](https://paypal.me/pschatzmann?country.x=CH&locale.x=en_US)




