/**
 * @brief We just set up the codec for a predefined board (AudioDriverES8388)
 * @author phil schatzmann
 */

#include "AudioBoard.h"

void setup() {
  // configure codec
  CodecConfig cfg;
  cfg.adc_input = ADC_INPUT_LINE1;
  cfg.dac_output = DAC_OUTPUT_ALL;
  cfg.i2s.bits = BIT_LENGTH_16BITS;
  cfg.i2s.rate = RATE_44K;
  // cfg.i2s.fmt = I2S_NORMAL;
  // cfg.i2s.mode = MODE_SLAVE;
  AudioDriverES8388.begin(cfg);
}

void loop() {}
