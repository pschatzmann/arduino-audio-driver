/**
 * @brief We just set up the codec. Because I2C was not defined we need to
 * initilize it ourself After this you can set up and use i2s
 * @author phil schatzmann
 */

#include "AudioBoard.h"

AudioBoard board(&AudioDriverES8388, my_pins);

void setup() {
  // start I2C for the communication with the codec
  Wire.begin();
  // configure codec
  CodecConfig cfg;
  cfg.adc_input = ADC_INPUT_LINE1;
  cfg.dac_output = DAC_OUTPUT_ALL;
  cfg.i2s.bits = BIT_LENGTH_16BITS;
  cfg.i2s.rate = RATE_44K;
  // cfg.i2s.fmt = I2S_NORMAL;
  // cfg.i2s.mode = MODE_SLAVE;
  board.begin(cfg);
}

void loop() {}
