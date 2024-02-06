
## Arduino Audio Driver

I was never quite happy with my [AudioKit project](https://github.com/pschatzmann/arduino-audiokit), that was a quick and dirty adaptation of the Espressif IDF Drivers for Arduino. 

In this project I finally implemented some clean and simple C++ classes. This reduced the complexity considerably and increased the flexibility tremendously!

The goal of this project is to provide an easy API to configure different audio codec chips. After setting up the codec, you can use the I2S functionality provided by your microcontroller.

Supported audio codec chips are e.g

- AC101
- ES8388
- ES8311
- CS43l22
- ES7243
- etc

While you can use this library stand alone, I recommend to use it together with my [AudioTools](https://github.com/pschatzmann/arduino-audio-tools) project which provides a nice integration with it's I2SCodecStream class.

## AudioTools

```C++

#include "AudioTools.h"
#include "AudioLibs/I2SCodecStream.h"

I2SCodecStream i2s(LyratV43);

void setup() {
  // setup i2s and codec
  auto cfg = i2s.defaultConfig();
  cfg.sample_rate = 44100;
  cfg.bits_per_sample = 16;
  cfg.channels = 1;
  i2s.begin();
  // set volume
  i2s.setVolume(0.5);
}  

```
You can use the [I2SCodecStream](https://pschatzmann.github.io/arduino-audio-tools/classaudio__tools_1_1_i2_s_codec_stream.html) like the I2SStream but it provides some additional functionality, like setting the volume via the codec. In the __constructor__ you need to provide an __audio board__ object.


## Stand-Alone: Predefined Boards

A board definition consists of a driver class and pin definitions for that specific board. You can easily use the predefined boards. E.g. the following code is setting up and starting the codec on a Lyrat board:

```C++

#include "AudioBoard.h"

void setup() {
  CodecConfig cfg;
  cfg.adc_input = ADC_INPUT_LINE1;
  cfg.dac_output = DAC_OUTPUT_ALL;
  cfg.i2s.bits = BIT_LENGTH_16BITS;
  cfg.i2s.rate = RATE_44K;
  //cfg.i2s.fmt = I2S_NORMAL;
  //cfg.i2s.mode = MODE_SLAVE; 
  LyratV43.begin(cfg);   

}

```

Check the documentation for all available boards.


## Stand-Alone: Custom Boards

You can also easily define your custom boards by defining the __driver__ and your __specifc pins__

```C++

#include "AudioBoard.h"

Pins my_pins;
AudioBoard board(my_pins, &AudioDriverES8388);

void setup() {
  // add i2c codec pins: scl, sda, port
  my_pins.addI2C(CODEC, 32, 22, 0x20);
  // add i2s pins: mclk, bclk, ws, data_out, data_in
  my_pins.addI2S(CODEC, 0, 27, 26, 25, 35);
  // add other pins: PA on gpio 21
  my_pins.addPin(PA, 21);

  // configure codec
  CodecConfig cfg;
  cfg.adc_input = ADC_INPUT_LINE1;
  cfg.dac_output = DAC_OUTPUT_ALL;
  cfg.i2s.bits = BIT_LENGTH_16BITS;
  cfg.i2s.rate = RATE_44K;
  //cfg.i2s.fmt = I2S_NORMAL;
  //cfg.i2s.mode = MODE_SLAVE; 
  board.begin(cfg);   
}

```
## Logging

The functionality has a built in logger. The default log level has been set to Warning. You can change it like this:

```C++

  LOGLEVEL_AUDIODRIVER = AudioDriverDebug; // or AudiDriverInfo, AudioDriverWarning, AudioDriverError

```
## Documentation

- [Classes and Objects](https://pschatzmann.github.io/arduino-audio-driver/html/group__audio__driver.html)

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





