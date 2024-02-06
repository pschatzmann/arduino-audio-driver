/**
 * @brief We define a custom board with the i2c and i2s pins and output a sine
 * with the help of the AudioTools I2SCodecStream
 * @author phil schatzmann
 */

#include "AudioTools.h" // install https://github.com/pschatzmann/arduino-audio-tools
#include "AudioLibs/I2SCodecStream.h"

AudioInfo info(44100, 2, 16);
SineWaveGenerator<int16_t> sineWave(32000);
GeneratedSoundStream<int16_t> sound(sineWave);
DriverPins my_pins;
AudioBoard board(&AudioDriverES8388, my_pins);
I2SCodecStream out(board);
StreamCopy copier(out, sound);

void setup() {
  // Setup logging
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  LOGLEVEL_AUDIODRIVER = AudioDriverInfo;

  // setup pins
  // - add i2c codec pins: scl, sda, port, (I2C object)
  my_pins.addI2C(CODEC, 32, 22, 0x20, 100000, Wire);
  // - add i2s pins: mclk, bclk, ws, data_out, data_in
  my_pins.addI2S(CODEC, 0, 14, 15, 22);

  
  // start I2S & codec with i2c and i2s configured above
  Serial.println("starting I2S...");
  auto config = out.defaultConfig();
  config.copyFrom(info);
  out.begin(config);

  // Setup sine wave
  sineWave.begin(info, N_B4);
}

// Arduino loop - copy sound to out
void loop() { copier.copy(); }