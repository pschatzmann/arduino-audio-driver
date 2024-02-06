/**
 * @brief We use a predefined AudioKitEs8388V1 board and output a sine wave via i2s
 * using the AudioTools I2SCodecStream
 * @author phil schatzmann
 */

#include "AudioTools.h" // install https://github.com/pschatzmann/arduino-audio-tools
#include "AudioLibs/I2SCodecStream.h"

AudioInfo info(44100, 2, 16);
SineWaveGenerator<int16_t> sineWave(32000); 
GeneratedSoundStream<int16_t>sound(sineWave); 
I2SCodecStream out(AudioKitEs8388V1);
StreamCopy copier(out, sound); 

// Arduino Setup
void setup(void) {
  // Setup logging
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  LOGLEVEL_AUDIODRIVER = AudioDriverInfo;

  // start I2S & codec
  Serial.println("starting I2S...");
  auto config = out.defaultConfig();
  config.copyFrom(info);
  out.begin(config);

  // set volume
  out.setVolume(1.0);

  // Setup sine wave
  sineWave.begin(info, N_B4);
  Serial.println("started...");
}

// Arduino loop - copy sound to out
void loop() { copier.copy(); }