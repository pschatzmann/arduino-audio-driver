#include <AudioBoard.h>
#include <ConfigAudioDriver.h>
#include <Driver.h>
#include <DriverCommon.h>
#include <DriverPins.h>

#include <AudioTools.h>         // https://github.com/pschatzmann/arduino-audio-tools
#include "Driver.h"             // Your driver includes AudioDriverNAU8325Class

// I2C settings for your ESP32 + NAU8325
#define SDAPIN          4
#define SCLPIN          5
#define I2CSPEED   100000
#define NAU8325ADDR 0x21  // or 0x10 depending on your board

// I2S pin configuration for NAU8325
#define MCLKPIN        22
#define BCLKPIN        25
#define WSPIN           24
#define DOPIN          23
#define DIPIN          -1   // NAU8325 does not use input

// Audio stream info
AudioInfo audio_info(44100, 2, 16);                   // Sample rate, channels, bit depth
SineWaveGenerator<int16_t> sine_wave(30000);          // Amplitude
GeneratedSoundStream<int16_t> sound_stream(sine_wave);// Generates audio
DriverPins my_pins;                                   // I2C/I2S pin setup

// Use your custom NAU8325 class
audio_driver::AudioDriverNAU8325Class nau_driver;
AudioBoard audio_board(nau_driver, my_pins);          

// I2S codec stream (output to NAU8325)
I2SCodecStream sound_stream(audio_board);
StreamCopy copier(i2s_out_stream, sound_stream);      // Copy sine → I2S

// Optional custom I2C bus instance (if needed)
TwoWire myWire = TwoWire(0);

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  AudioDriverLogger.begin(Serial, AudioDriverLogLevel::Info);

  delay(1000);
  Serial.println("Starting NAU8325 AudioBoard Setup...");

  // Set I2C Pins for NAU8325
  my_pins.addI2C(PinFunction::CODEC, SCLPIN, SDAPIN, NAU8325ADDR, I2CSPEED, myWire);

  // Set I2S Pins
  my_pins.addI2S(PinFunction::CODEC, MCLKPIN, BCLKPIN, WSPIN, DOPIN, DIPIN);

  // Apply pin settings
  my_pins.begin();

  // Init board + codec
  if (!audio_board.begin()) {
    Serial.println(" NAU8325 board begin failed");
    while (1);
  }

  // Setup I2S codec stream
  auto i2s_config = sound_stream.defaultConfig();
  i2s_config.copyFrom(audio_info);
  sound_stream.begin(i2s_config);

  // Begin sine wave at 440 Hz
  sine_wave.begin(audio_info, N_A4);

  Serial.println("Setup complete. Sine wave playing...");
}

void loop() {
  copier.copy();  // Continuously stream sine wave to NAU8325
}
