#include <AudioBoard.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/I2SCodecStream.h>
#include "Driver.h"

using namespace audio_tools;
using namespace audio_driver;

// === Pins ===
#define SDAPIN     4
#define SCLPIN     5
#define I2CSPEED   100000
#define NAU8325ADDR 0x21
#define MCLKPIN    22
#define BCLKPIN    25
#define WSPIN      24
#define DOPIN      23
#define DIPIN     -1

// === Audio Setup ===
AudioInfo audio_info(44100, 2, 16);
SineWaveGenerator<int16_t> sine_wave(30000);
GeneratedSoundStream<int16_t> sound_stream(sine_wave);
StreamCopy copier;

TwoWire myWire = TwoWire(0);
DriverPins pins;
AudioBoard board(AudioDriverNAU8325, pins);
I2SCodecStream i2s_out(board);

void setup() {
  Serial.begin(115200);
  delay(1000);  // Allow time for serial monitor to open
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  AudioDriverLogger.begin(Serial, AudioDriverLogLevel::Debug);

  // Setup I2C and I2S pins
  pins.addI2C(PinFunction::CODEC, SCLPIN, SDAPIN, NAU8325ADDR, I2CSPEED, myWire);
  pins.addI2S(PinFunction::CODEC, MCLKPIN, BCLKPIN, WSPIN, DOPIN, DIPIN);
  pins.begin();

  // Codec config
  CodecConfig config;
  config.i2s.rate = RATE_44K;
  config.i2s.bits = BIT_LENGTH_16BITS;
  config.i2s.channels = CHANNELS2;

  // Start codec board
  if (!board.begin(config)) {
    Serial.println("[NAU8325] board.begin() failed!");
    while (1);
  }

  // Configure I2S output
  auto cfg = i2s_out.defaultConfig();
  cfg.copyFrom(audio_info);
  i2s_out.begin(cfg);
  copier.begin(i2s_out, sound_stream);

  // Start sine wave
  sine_wave.begin(audio_info, N_A4);
  Serial.println("Sinewave output started.");
}

void loop() {
  copier.copy();
}
