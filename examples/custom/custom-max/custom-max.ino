/**
 * @brief We just set up the codec and the i2c pins, so that the wire is
 * automatically initialized with the indicated pins.
 * After this you can set up and use i2s
 * @author phil schatzmann
 */

#include "AudioBoard.h"

AudioBoard board(&AudioDriverES8388);

void setup() {
  // add i2c codec pins: scl, sda, port
  my_pins.addI2C(CODEC, 32, 22, 0x20);
  // example add other pins: PA on gpio 21
  my_pins.addPin(PA, 21);

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
