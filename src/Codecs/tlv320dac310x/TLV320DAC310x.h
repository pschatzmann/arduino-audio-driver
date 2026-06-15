/**
 * @file TLV320DAC310x.h
 * @brief Header only C++ driver for the Texas Instruments TLV320DAC3100 /
 * TLV320DAC3101 audio DAC.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/tlv320dac310x.c / tlv320dac310x.h, Copyright (c) 2019 Intel
 * Corporation, Apache-2.0).
 */
#pragma once

#include <cstddef>

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Digital audio interface format
enum class TLV320DAC310xFormat {
  I2S = 0,
  DSP = 1,
  RJF = 2,  ///< Right justified
  LJF = 3,  ///< Left justified
};

/// Word length of the digital audio interface
enum class TLV320DAC310xWordLength {
  Bits16 = 0,
  Bits20 = 1,
  Bits24 = 2,
  Bits32 = 3,
};

/// Processing block / decimation filter selection (see codec_configure_filters)
enum class TLV320DAC310xProcBlock {
  DecimationA = 25,  ///< PRB_P25, used for PCM rates < 96kHz
  DecimationB = 10,  ///< PRB_P10, used for PCM rates >= 96kHz
  DecimationC = 18,  ///< PRB_P18, used for PCM rates >= 192kHz
};

/// OSR multiple, determined from the PCM sample rate
enum class TLV320DAC310xOsrMultiple {
  Multiple8 = 8,
  Multiple4 = 4,
  Multiple2 = 2,
};

/// Headphone driver common mode voltage
enum class TLV320DAC310xCmVoltage {
  V1P35 = 0,
  V1P5 = 1,
  V1P65 = 2,
  V1P8 = 3,
};

/**
 * @brief Header only C++ driver for the TLV320DAC3100 / TLV320DAC3101 audio
 * DAC.
 *
 * The register map of this chip is organized in pages: page 0 register 0 is
 * the page-select register. selectPage()/writePagedReg()/readPagedReg()
 * take care of switching pages as needed.
 *
 * The RESET pin is not handled here - the chip must be taken out of reset
 * (RESET pin driven high / inactive) by the caller before begin() is called.
 */
class TLV320DAC310x : public ZephyrDriverCommon {
 public:
  // ---- Page 0 register addresses ----
  static constexpr uint8_t PAGE_CONTROL_ADDR = 0;
  static constexpr uint8_t SOFT_RESET_ADDR = 1;
  static constexpr uint8_t NDAC_DIV_ADDR = 11;
  static constexpr uint8_t MDAC_DIV_ADDR = 12;
  static constexpr uint8_t OSR_MSB_ADDR = 13;
  static constexpr uint8_t OSR_LSB_ADDR = 14;
  static constexpr uint8_t IF_CTRL1_ADDR = 27;
  static constexpr uint8_t BCLK_DIV_ADDR = 30;
  static constexpr uint8_t OVF_FLAG_ADDR = 39;
  static constexpr uint8_t PROC_BLK_SEL_ADDR = 60;
  static constexpr uint8_t DATA_PATH_SETUP_ADDR = 63;
  static constexpr uint8_t VOL_CTRL_ADDR = 64;
  static constexpr uint8_t L_DIG_VOL_CTRL_ADDR = 65;
  static constexpr uint8_t DRC_CTRL1_ADDR = 68;
  static constexpr uint8_t L_BEEP_GEN_ADDR = 71;
  static constexpr uint8_t R_BEEP_GEN_ADDR = 72;
  static constexpr uint8_t BEEP_LEN_MSB_ADDR = 73;
  static constexpr uint8_t BEEP_LEN_MID_ADDR = 74;
  static constexpr uint8_t BEEP_LEN_LSB_ADDR = 75;
  static constexpr uint8_t VOL_MICDET_ADC_CTRL_ADDR = 116;

  // ---- Page 1 register addresses ----
  static constexpr uint8_t HEADPHONE_DRV_ADDR = 31;
  static constexpr uint8_t SPEAKER_AMP_ADDR = 32;
  static constexpr uint8_t HP_OUT_POP_RM_ADDR = 33;
  static constexpr uint8_t OUTPUT_ROUTING_ADDR = 35;
  static constexpr uint8_t HPL_ANA_VOL_CTRL_ADDR = 36;
  static constexpr uint8_t HPR_ANA_VOL_CTRL_ADDR = 37;
  static constexpr uint8_t SPL_ANA_VOL_CTRL_ADDR = 38;
  static constexpr uint8_t HPL_DRV_GAIN_CTRL_ADDR = 40;
  static constexpr uint8_t HPR_DRV_GAIN_CTRL_ADDR = 41;
  static constexpr uint8_t SPK_DRV_GAIN_CTRL_ADDR = 42;
  static constexpr uint8_t HEADPHONE_DRV_CTRL_ADDR = 44;

  // ---- Page 3 register addresses ----
  static constexpr uint8_t TIMER_MCLK_DIV_ADDR = 16;

  // ---- Bit field defines ----
  static constexpr uint8_t SOFT_RESET_ASSERT = 1;

  static constexpr uint8_t NDAC_POWER_UP = 0x80;       // BIT(7)
  static constexpr uint8_t NDAC_POWER_UP_MASK = 0x80;  // BIT(7)
  static constexpr uint8_t NDAC_DIV_MASK = 0x7F;       // BIT_MASK(7)

  static constexpr uint8_t MDAC_POWER_UP = 0x80;
  static constexpr uint8_t MDAC_POWER_UP_MASK = 0x80;
  static constexpr uint8_t MDAC_DIV_MASK = 0x7F;

  static constexpr uint32_t DAC_PROC_CLK_FREQ_MAX = 49152000;  // 49.152 MHz
  static constexpr uint32_t DAC_MOD_CLK_FREQ_MIN = 2800000;    // 2.8 MHz
  static constexpr uint32_t DAC_MOD_CLK_FREQ_MAX = 6200000;    // 6.2 MHz

  static constexpr uint8_t OSR_MSB_MASK = 0x03;  // BIT_MASK(2)
  static constexpr uint8_t OSR_LSB_MASK = 0xFF;  // BIT_MASK(8)

  static constexpr uint8_t IF_CTRL_IFTYPE_MASK = 0x03;  // BIT_MASK(2)
  static constexpr uint8_t IF_CTRL_WLEN_MASK = 0x03;    // BIT_MASK(2)
  static constexpr uint8_t IF_CTRL_BCLK_OUT = 0x08;     // BIT(3)
  static constexpr uint8_t IF_CTRL_WCLK_OUT = 0x04;     // BIT(2)

  static constexpr uint8_t BCLK_DIV_POWER_UP = 0x80;       // BIT(7)
  static constexpr uint8_t BCLK_DIV_POWER_UP_MASK = 0x80;  // BIT(7)
  static constexpr uint8_t BCLK_DIV_MASK = 0x7F;           // BIT_MASK(7)

  static constexpr uint8_t PROC_BLK_SEL_MASK = 0x1F;  // BIT_MASK(5)

  static constexpr uint8_t DAC_LR_POWERUP_DEFAULT =
      (0x80 | 0x40 | 0x10 | 0x04);  // BIT(7)|BIT(6)|BIT(4)|BIT(2)
  static constexpr uint8_t DAC_LR_POWERDN_DEFAULT = (0x10 | 0x04);  // BIT(4)|BIT(2)

  static constexpr uint8_t VOL_CTRL_UNMUTE_DEFAULT = 0;
  static constexpr uint8_t VOL_CTRL_MUTE_DEFAULT = (0x08 | 0x04);  // BIT(3)|BIT(2)

  static constexpr uint8_t BEEP_GEN_EN_BEEP = 0x80;  // BIT(7)

  static constexpr uint8_t VOL_MICDET_VOL_CTRL_PIN = 0x80;  // BIT(7)

  static constexpr uint8_t HEADPHONE_DRV_POWERUP = (0x80 | 0x40);  // BIT(7)|BIT(6)
  static constexpr uint8_t HEADPHONE_DRV_CM_MASK = (0x03 << 3);
  static constexpr uint8_t HEADPHONE_DRV_RESERVED = 0x04;  // BIT(2)

  static constexpr uint8_t SPEAKER_AMP_POWERUP = 0x80;  // BIT(7)
  static constexpr uint8_t SPEAKER_AMP_RESERVED = (0x04 | 0x02);  // BIT(2)|BIT(1)

  static constexpr uint8_t SPK_DRV_UNMUTE = 0x04;  // BIT(2)

  static constexpr uint8_t HP_OUT_POP_RM_ENABLE = 0x80;  // BIT(7)

  static constexpr uint8_t OUTPUT_ROUTING_HPL = (2 << 6);
  static constexpr uint8_t OUTPUT_ROUTING_MIXERL = (1 << 6);
  static constexpr uint8_t OUTPUT_ROUTING_HPR = (2 << 2);
  static constexpr uint8_t OUTPUT_ROUTING_MIXERR = (1 << 2);

  static constexpr uint8_t HPX_ANA_VOL_ENABLE = 0x80;  // BIT(7)
  static constexpr uint8_t HPX_ANA_VOL_MASK = 0x7F;    // BIT_MASK(7)
  static constexpr uint8_t HPX_ANA_VOL_MAX = 0;
  static constexpr uint8_t HPX_ANA_VOL_DEFAULT = 64;
  static constexpr uint8_t HPX_ANA_VOL_MIN = 127;
  static constexpr uint8_t HPX_ANA_VOL_LOW_THRESH = 105;
  static constexpr uint8_t HPX_ANA_VOL_FLOOR = 144;

  static constexpr uint8_t HPX_DRV_UNMUTE = 0x04;  // BIT(2)

  static constexpr uint8_t HEADPHONE_DRV_LINEOUT = (0x02 | 0x04);  // BIT(1)|BIT(2)

  static constexpr uint8_t TIMER_MCLK_DIV_EN_EXT = 0x80;  // BIT(7)
  static constexpr uint8_t TIMER_MCLK_DIV_MASK = 0x7F;    // BIT_MASK(7)

  // ---- Output volume range (0.5dB steps) ----
  static constexpr int OUTPUT_VOLUME_MAX = 0;
  static constexpr int OUTPUT_VOLUME_MIN = (-78 * 2);

  TLV320DAC310x() { i2c_addr = 0x18; }

  /// Required by ZephyrDriverCommon: maps the generic parameters to the
  /// TLV320DAC310x specific begin() below (using the default MCLK frequency
  /// of 12 MHz).
  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)channels;
    return begin(sample_rate, bits, 12000000, toFormat(fmt), is_master, is_master);
  }

  /**
   * @brief Initialize the codec.
   *
   * Note: the RESET pin must be driven inactive (high) by the caller before
   * calling begin().
   *
   * @param sample_rate I2S sample rate in Hz (e.g. 44100, 48000)
   * @param word_size I2S word length in bits (16, 20, 24, 32)
   * @param mclk_freq MCLK frequency in Hz as fed to the codec
   * @param fmt digital audio interface format
   * @param bclk_controller true if this device drives BCLK (master)
   * @param wclk_controller true if this device drives WCLK/LRCLK (master)
   */
  bool begin(uint32_t sample_rate = 44100, uint8_t word_size = 16,
             uint32_t mclk_freq = 12000000,
             TLV320DAC310xFormat fmt = TLV320DAC310xFormat::I2S,
             bool bclk_controller = false, bool wclk_controller = false) {
    bool rc = true;

    rc &= softReset();
    rc &= configureClocks(mclk_freq, sample_rate, bclk_controller, word_size);
    rc &= configureDai(word_size, fmt, bclk_controller, wclk_controller);
    rc &= configureFilters(sample_rate);
    rc &= configureOutput();

    return rc;
  }

  /// Soft reset of the codec
  bool softReset() { return writePagedReg(0, SOFT_RESET_ADDR, SOFT_RESET_ASSERT); }

  /**
   * @brief Configure NDAC/MDAC/OSR dividers (and optionally BCLK divider)
   * to derive the DAC processing/modulator clocks from MCLK.
   */
  bool configureClocks(uint32_t mclk_freq, uint32_t sample_rate,
                        bool bclk_controller = false, uint8_t word_size = 16) {
    bool rc = true;
    uint32_t dac_clk, mod_clk;
    uint32_t ndac, mdac, bclk_div = 0;
    int osr, osr_min, osr_max;
    TLV320DAC310xOsrMultiple osr_multiple = getOsrMultiple(sample_rate);

    if (mclk_freq <= DAC_PROC_CLK_FREQ_MAX) {
      ndac = 1;
    } else {
      ndac = mclk_freq / DAC_PROC_CLK_FREQ_MAX;
    }
    dac_clk = mclk_freq / ndac;

    /* find mdac/osr such that mod_clk = sample_rate * osr is an integer
     * divisor of dac_clk and DAC_MOD_CLK_FREQ_MIN <= mod_clk <=
     * DAC_MOD_CLK_FREQ_MAX */
    osr_min = (DAC_MOD_CLK_FREQ_MIN + sample_rate - 1) / sample_rate;
    osr_max = DAC_MOD_CLK_FREQ_MAX / sample_rate;

    int multiple = (int)osr_multiple;
    osr_max = (osr_max / multiple) * multiple;
    osr_min = ((osr_min + multiple - 1) / multiple) * multiple;

    osr = osr_max;
    mdac = 0;
    while (osr >= osr_min) {
      mod_clk = sample_rate * osr;
      mdac = dac_clk / mod_clk;
      if ((mdac * mod_clk) == dac_clk) {
        break;
      }
      osr -= multiple;
    }

    if (osr < osr_min || mdac == 0) {
      return false;
    }

    if (bclk_controller) {
      uint32_t num = (uint32_t)osr * mdac;
      uint32_t den = (uint32_t)word_size * 2U;
      bclk_div = num / den;
      if ((bclk_div * den) != num) {
        return false;
      }
      rc &= writePagedReg(0, BCLK_DIV_ADDR,
                           (uint8_t)(BCLK_DIV_POWER_UP | (bclk_div & BCLK_DIV_MASK)));
    }

    /* set NDAC, then MDAC, followed by OSR */
    rc &= writePagedReg(0, NDAC_DIV_ADDR,
                         (uint8_t)((ndac & NDAC_DIV_MASK) | NDAC_POWER_UP_MASK));
    rc &= writePagedReg(0, MDAC_DIV_ADDR,
                         (uint8_t)((mdac & MDAC_DIV_MASK) | MDAC_POWER_UP_MASK));
    rc &= writePagedReg(0, OSR_MSB_ADDR, (uint8_t)((osr >> 8) & OSR_MSB_MASK));
    rc &= writePagedReg(0, OSR_LSB_ADDR, (uint8_t)(osr & OSR_LSB_MASK));

    if (bclk_controller) {
      rc &= writePagedReg(0, BCLK_DIV_ADDR,
                           (uint8_t)((bclk_div & BCLK_DIV_MASK) | BCLK_DIV_POWER_UP));
    }

    /* MCLK divider for the internal timer, target ~1MHz */
    uint32_t mclk_div = (mclk_freq + 999999) / 1000000;
    rc &= writePagedReg(3, TIMER_MCLK_DIV_ADDR,
                         (uint8_t)(TIMER_MCLK_DIV_EN_EXT | (mclk_div & TIMER_MCLK_DIV_MASK)));

    return rc;
  }

  /// Configure the digital audio interface (format, word length, clock directions)
  bool configureDai(uint8_t word_size = 16,
                     TLV320DAC310xFormat fmt = TLV320DAC310xFormat::I2S,
                     bool bclk_controller = false, bool wclk_controller = false) {
    uint8_t val = (uint8_t)(((uint8_t)fmt & IF_CTRL_IFTYPE_MASK) << 6);

    if (bclk_controller) val |= IF_CTRL_BCLK_OUT;
    if (wclk_controller) val |= IF_CTRL_WCLK_OUT;

    TLV320DAC310xWordLength wlen;
    switch (word_size) {
      case 16:
        wlen = TLV320DAC310xWordLength::Bits16;
        break;
      case 20:
        wlen = TLV320DAC310xWordLength::Bits20;
        break;
      case 24:
        wlen = TLV320DAC310xWordLength::Bits24;
        break;
      case 32:
        wlen = TLV320DAC310xWordLength::Bits32;
        break;
      default:
        return false;
    }
    val |= (uint8_t)(((uint8_t)wlen & IF_CTRL_WLEN_MASK) << 4);

    return writePagedReg(0, IF_CTRL1_ADDR, val);
  }

  /// Select the decimation filter / processing block based on the sample rate
  bool configureFilters(uint32_t sample_rate) {
    TLV320DAC310xProcBlock proc_blk;
    if (sample_rate >= 192000) {
      proc_blk = TLV320DAC310xProcBlock::DecimationC;
    } else if (sample_rate >= 96000) {
      proc_blk = TLV320DAC310xProcBlock::DecimationB;
    } else {
      proc_blk = TLV320DAC310xProcBlock::DecimationA;
    }
    return writePagedReg(0, PROC_BLK_SEL_ADDR,
                          (uint8_t)((uint8_t)proc_blk & PROC_BLK_SEL_MASK));
  }

  /**
   * @brief Configure the output path(s) selected via setDevices(): common
   * mode voltage, pop removal, route DAC -> mixer -> headphone and/or
   * Class-D speaker, default analog volume, unmute and power up the
   * corresponding drivers.
   *
   * DAC_OUTPUT_LINE1 enables the headphone/line drivers (HPL/HPR), and
   * DAC_OUTPUT_LINE2 enables the mono Class-D speaker driver (SPK), fed from
   * DAC_L. DAC_OUTPUT_ALL enables both, DAC_OUTPUT_NONE powers both down.
   */
  bool configureOutput(TLV320DAC310xCmVoltage cm = TLV320DAC310xCmVoltage::V1P65) {
    bool rc = true;
    uint8_t val = 0;

    bool hp = output_device == DAC_OUTPUT_LINE1 || output_device == DAC_OUTPUT_ALL;
    bool spk = output_device == DAC_OUTPUT_LINE2 || output_device == DAC_OUTPUT_ALL;

    if (!hp && !spk) {
      /* unroute the DACs and power down both the headphone and speaker drivers */
      rc &= writePagedReg(1, OUTPUT_ROUTING_ADDR, 0);
      rc &= readPagedReg(1, HEADPHONE_DRV_ADDR, val);
      rc &= writePagedReg(1, HEADPHONE_DRV_ADDR,
                           (uint8_t)((val & (uint8_t)~HEADPHONE_DRV_POWERUP) |
                                     HEADPHONE_DRV_RESERVED));
      rc &= readPagedReg(1, SPEAKER_AMP_ADDR, val);
      rc &= writePagedReg(1, SPEAKER_AMP_ADDR,
                           (uint8_t)((val & (uint8_t)~SPEAKER_AMP_POWERUP) |
                                     SPEAKER_AMP_RESERVED));
      return rc;
    }

    /* set common mode voltage of the headphone driver (e.g. 1.65V = half of
     * typical 3.3V AVDD) */
    rc &= readPagedReg(1, HEADPHONE_DRV_ADDR, val);
    val &= (uint8_t)~HEADPHONE_DRV_CM_MASK;
    val |= (uint8_t)((((uint8_t)cm) << 3) & HEADPHONE_DRV_CM_MASK) | HEADPHONE_DRV_RESERVED;
    rc &= writePagedReg(1, HEADPHONE_DRV_ADDR, val);

    /* enable pop removal on power down/up */
    rc &= readPagedReg(1, HP_OUT_POP_RM_ADDR, val);
    rc &= writePagedReg(1, HP_OUT_POP_RM_ADDR, (uint8_t)(val | HP_OUT_POP_RM_ENABLE));

    /* route DAC_L/DAC_R to the mixer amplifiers so that the analog volume
     * controls have effect; DAC_L feeds both HPL and SPK (SPK is mono),
     * DAC_R only feeds HPR */
    val = 0;
    if (hp || spk) val |= OUTPUT_ROUTING_MIXERL;
    if (hp) val |= OUTPUT_ROUTING_MIXERR;
    rc &= writePagedReg(1, OUTPUT_ROUTING_ADDR, val);

    if (hp) {
      /* enable volume control on headphone out */
      rc &= writePagedReg(1, HPL_ANA_VOL_CTRL_ADDR, analogVolReg(HPX_ANA_VOL_DEFAULT));
      rc &= writePagedReg(1, HPR_ANA_VOL_CTRL_ADDR, analogVolReg(HPX_ANA_VOL_DEFAULT));

      /* set headphone outputs as line-out */
      rc &= writePagedReg(1, HEADPHONE_DRV_CTRL_ADDR, HEADPHONE_DRV_LINEOUT);

      /* unmute headphone drivers */
      rc &= writePagedReg(1, HPL_DRV_GAIN_CTRL_ADDR, HPX_DRV_UNMUTE);
      rc &= writePagedReg(1, HPR_DRV_GAIN_CTRL_ADDR, HPX_DRV_UNMUTE);

      /* power up headphone drivers */
      rc &= readPagedReg(1, HEADPHONE_DRV_ADDR, val);
      rc &= writePagedReg(1, HEADPHONE_DRV_ADDR,
                           (uint8_t)(val | HEADPHONE_DRV_POWERUP | HEADPHONE_DRV_RESERVED));
    } else {
      /* power down headphone drivers */
      rc &= readPagedReg(1, HEADPHONE_DRV_ADDR, val);
      rc &= writePagedReg(1, HEADPHONE_DRV_ADDR,
                           (uint8_t)((val & (uint8_t)~HEADPHONE_DRV_POWERUP) |
                                     HEADPHONE_DRV_RESERVED));
    }

    if (spk) {
      /* route the left analog volume control to the Class-D speaker driver */
      rc &= writePagedReg(1, SPL_ANA_VOL_CTRL_ADDR, analogVolReg(HPX_ANA_VOL_DEFAULT));

      /* unmute the Class-D speaker driver (default gain = 6dB) */
      rc &= writePagedReg(1, SPK_DRV_GAIN_CTRL_ADDR, SPK_DRV_UNMUTE);

      /* power up the Class-D speaker driver */
      rc &= readPagedReg(1, SPEAKER_AMP_ADDR, val);
      rc &= writePagedReg(1, SPEAKER_AMP_ADDR,
                           (uint8_t)(val | SPEAKER_AMP_POWERUP | SPEAKER_AMP_RESERVED));
    } else {
      /* power down the Class-D speaker driver */
      rc &= readPagedReg(1, SPEAKER_AMP_ADDR, val);
      rc &= writePagedReg(1, SPEAKER_AMP_ADDR,
                           (uint8_t)((val & (uint8_t)~SPEAKER_AMP_POWERUP) |
                                     SPEAKER_AMP_RESERVED));
    }

    return rc;
  }

  /// Power up & unmute the DAC channels and start the output
  bool startOutput() {
    bool rc = true;
    rc &= writePagedReg(0, DATA_PATH_SETUP_ADDR, DAC_LR_POWERUP_DEFAULT);
    rc &= writePagedReg(0, VOL_CTRL_ADDR, VOL_CTRL_UNMUTE_DEFAULT);
    return rc;
  }

  /// Mute and power down the DAC channels
  bool stopOutput() {
    bool rc = true;
    rc &= writePagedReg(0, VOL_CTRL_ADDR, VOL_CTRL_MUTE_DEFAULT);
    rc &= writePagedReg(0, DATA_PATH_SETUP_ADDR, DAC_LR_POWERDN_DEFAULT);
    return rc;
  }

  /// Mute the output (DAC channel mute, output stage stays powered)
  bool setOutputMute() { return writePagedReg(0, VOL_CTRL_ADDR, VOL_CTRL_MUTE_DEFAULT); }

  /// Unmute the output
  bool setOutputUnmute() { return writePagedReg(0, VOL_CTRL_ADDR, VOL_CTRL_UNMUTE_DEFAULT); }

  /// Mute / unmute the output
  bool setOutputMute(bool mute) { return mute ? setOutputMute() : setOutputUnmute(); }

  /**
   * @brief Set the headphone output (analog) volume.
   *
   * @param vol volume in units of 0.5dB, range [OUTPUT_VOLUME_MIN (-156,
   * i.e. -78dB), OUTPUT_VOLUME_MAX (0, i.e. 0dB)]
   */
  bool setOutputVolume(int vol) {
    static const uint8_t vol_array[] = {107, 108, 110, 113, 116, 120,
                                         125, 128, 132, 138, 144};
    if (vol > OUTPUT_VOLUME_MAX || vol < OUTPUT_VOLUME_MIN) return false;

    /* remove sign: 0 = 0dB .. 156 = -78dB */
    int v = -vol;
    uint8_t vol_val;
    if (v > HPX_ANA_VOL_FLOOR) {
      vol_val = HPX_ANA_VOL_FLOOR;
    } else if (v > HPX_ANA_VOL_LOW_THRESH) {
      size_t idx = 0;
      size_t n = sizeof(vol_array) / sizeof(vol_array[0]);
      while (idx < n && vol_array[idx] < (uint8_t)v) idx++;
      vol_val = (uint8_t)(HPX_ANA_VOL_LOW_THRESH + idx + 1);
    } else {
      vol_val = (uint8_t)v;
    }

    bool rc = true;
    rc &= writePagedReg(1, HPL_ANA_VOL_CTRL_ADDR, analogVolReg(vol_val));
    rc &= writePagedReg(1, HPR_ANA_VOL_CTRL_ADDR, analogVolReg(vol_val));
    return rc;
  }

  /// no-op, nothing is cached that needs re-applying
  bool applyProperties() { return true; }

  /// Stores the output device selection for use by configureOutput()
  bool setDevices(input_device_t input_device, output_device_t output_device) override {
    (void)input_device;
    this->output_device = output_device;
    return true;
  }

  /// Mutes/unmutes the output
  bool setMute(bool mute) override { return setOutputMute(mute); }

  /// Sets the output volume in % (0...100), mapped to
  /// [OUTPUT_VOLUME_MIN..OUTPUT_VOLUME_MAX] (0.5dB steps)
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    int vol = OUTPUT_VOLUME_MIN +
              (volume_percent * (OUTPUT_VOLUME_MAX - OUTPUT_VOLUME_MIN)) / 100;
    return setOutputVolume(vol);
  }

 protected:
  uint8_t active_page = 0xFF;  ///< 0xFF forces a page select on first access
  /// Output device selection set via setDevices(), used by configureOutput()
  output_device_t output_device = DAC_OUTPUT_ALL;

  /// Build the register value for the HPx analog volume control registers
  static uint8_t analogVolReg(uint8_t vol) {
    return (uint8_t)((vol & HPX_ANA_VOL_MASK) | HPX_ANA_VOL_ENABLE);
  }

  /// Map the generic I2S format to the TLV320DAC310x specific format
  static TLV320DAC310xFormat toFormat(i2s_format_t fmt) {
    switch (fmt) {
      case I2S_LEFT:
        return TLV320DAC310xFormat::LJF;
      case I2S_RIGHT:
        return TLV320DAC310xFormat::RJF;
      case I2S_DSP:
        return TLV320DAC310xFormat::DSP;
      default:
        return TLV320DAC310xFormat::I2S;
    }
  }

  static TLV320DAC310xOsrMultiple getOsrMultiple(uint32_t sample_rate) {
    if (sample_rate >= 192000) return TLV320DAC310xOsrMultiple::Multiple2;
    if (sample_rate >= 96000) return TLV320DAC310xOsrMultiple::Multiple4;
    return TLV320DAC310xOsrMultiple::Multiple8;
  }

  /// Select the active register page (cached, only writes if changed)
  bool selectPage(uint8_t page) {
    if (active_page == page) return true;
    if (!writeReg(PAGE_CONTROL_ADDR, page)) return false;
    active_page = page;
    return true;
  }

  /// Write a register on the given page
  bool writePagedReg(uint8_t page, uint8_t reg, uint8_t value) {
    if (!selectPage(page)) return false;
    return writeReg(reg, value);
  }

  /// Read a register on the given page
  bool readPagedReg(uint8_t page, uint8_t reg, uint8_t& value) {
    if (!selectPage(page)) return false;
    return readReg(reg, value);
  }

  /// Read-Modify-Write of a register on the given page
  bool updatePagedReg(uint8_t page, uint8_t reg, uint8_t mask, uint8_t value) {
    if (!selectPage(page)) return false;
    return updateReg(reg, mask, value);
  }
};

using TLV320DAC3100 = TLV320DAC310x;

}  // namespace audio_driver
