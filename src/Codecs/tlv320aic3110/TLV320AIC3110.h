/**
 * @file TLV320AIC3110.h
 * @brief Header only C++ driver for the Texas Instruments TLV320AIC3110
 * audio codec.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/tlv320aic3110.c / tlv320aic3110.h,
 * Copyright (c) 2025 PHYTEC America LLC, Apache-2.0).
 */
#pragma once

#include <cstddef>

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Logical audio channels
enum class AIC3110Channel {
  HeadphoneLeft,
  HeadphoneRight,
  FrontLeft,   ///< Speaker left
  FrontRight,  ///< Speaker right
  All,
};

/// DAC decimation filter / processing block selection
enum AIC3110DacProcBlock {
  AIC3110_PRB_P1_DECIMATION_A = 1,
  AIC3110_PRB_P7_DECIMATION_B = 7,
  AIC3110_PRB_P17_DECIMATION_C = 17,
};

/// ADC decimation filter / processing block selection
enum AIC3110AdcProcBlock {
  AIC3110_PRB_R4_DECIMATION_A = 4,
  AIC3110_PRB_R10_DECIMATION_B = 10,
  AIC3110_PRB_R16_DECIMATION_C = 16,
};

/// Headphone/Speaker common mode output voltage
enum AIC3110CmVoltage {
  AIC3110_CM_VOLTAGE_1P35 = 0,
  AIC3110_CM_VOLTAGE_1P5 = 1,
  AIC3110_CM_VOLTAGE_1P65 = 2,
  AIC3110_CM_VOLTAGE_1P8 = 3,
};

/**
 * @brief Header only C++ driver for the TLV320AIC3110 audio codec.
 *
 * The TLV320AIC3110 uses a paged 8 bit register map (page select register
 * at page 0 / register 0). Provides soft reset, PLL/clock configuration
 * (based on a fixed MCLK/sample-rate divider table), DAI (I2S) format
 * configuration, decimation filter selection and output/input path
 * configuration including volume and mute control.
 */
class TLV320AIC3110 : public ZephyrDriverCommon {
 public:
  /// Page + register address pair
  struct RegAddr {
    uint8_t page;
    uint8_t reg;
  };

  // ---- Page 0 registers ----
  static constexpr RegAddr SOFT_RESET_ADDR{0, 1};
  static constexpr RegAddr CLOCK_GEN_MUX_ADDR{0, 4};
  static constexpr RegAddr PLL_P_R_ADDR{0, 5};
  static constexpr RegAddr PLL_J_ADDR{0, 6};
  static constexpr RegAddr PLL_D_MSB_ADDR{0, 7};
  static constexpr RegAddr PLL_D_LSB_ADDR{0, 8};
  static constexpr RegAddr NDAC_DIV_ADDR{0, 11};
  static constexpr RegAddr MDAC_DIV_ADDR{0, 12};
  static constexpr RegAddr OSR_MSB_ADDR{0, 13};
  static constexpr RegAddr OSR_LSB_ADDR{0, 14};
  static constexpr RegAddr NADC_DIV_ADDR{0, 18};
  static constexpr RegAddr MADC_DIV_ADDR{0, 19};
  static constexpr RegAddr AOSR_ADDR{0, 20};
  static constexpr RegAddr IF_CTRL1_ADDR{0, 27};
  static constexpr RegAddr BCLK_DIV_ADDR{0, 30};
  static constexpr RegAddr OVF_FLAG_ADDR{0, 39};
  static constexpr RegAddr DAC_PROC_BLK_SEL_ADDR{0, 60};
  static constexpr RegAddr ADC_PROC_BLK_SEL_ADDR{0, 61};
  static constexpr RegAddr DATA_PATH_SETUP_ADDR{0, 63};
  static constexpr RegAddr VOL_CTRL_ADDR{0, 64};
  static constexpr RegAddr L_DIG_VOL_CTRL_ADDR{0, 65};
  static constexpr RegAddr DRC_CTRL1_ADDR{0, 68};
  static constexpr RegAddr L_BEEP_GEN_ADDR{0, 71};
  static constexpr RegAddr R_BEEP_GEN_ADDR{0, 72};
  static constexpr RegAddr BEEP_LEN_MSB_ADDR{0, 73};
  static constexpr RegAddr BEEP_LEN_MIB_ADDR{0, 74};
  static constexpr RegAddr BEEP_LEN_LSB_ADDR{0, 75};
  static constexpr RegAddr MIC_ADC_FLAG_ADDR{0, 36};
  static constexpr RegAddr MIC_ADC_CTRL_ADDR{0, 81};
  static constexpr RegAddr MIC_FCTRL_ADDR{0, 82};
  static constexpr RegAddr MIC_CCTRL_ADDR{0, 83};

  // ---- Page 1 registers ----
  static constexpr RegAddr HEADPHONE_DRV_ADDR{1, 31};
  static constexpr RegAddr SPEAKER_DRV_ADDR{1, 32};
  static constexpr RegAddr HP_OUT_POP_RM_ADDR{1, 33};
  static constexpr RegAddr OUTPUT_ROUTING_ADDR{1, 35};
  static constexpr RegAddr HPL_ANA_VOL_CTRL_ADDR{1, 36};
  static constexpr RegAddr HPR_ANA_VOL_CTRL_ADDR{1, 37};
  static constexpr RegAddr SPL_ANA_VOL_CTRL_ADDR{1, 38};
  static constexpr RegAddr SPR_ANA_VOL_CTRL_ADDR{1, 39};
  static constexpr RegAddr HPL_DRV_GAIN_CTRL_ADDR{1, 40};
  static constexpr RegAddr HPR_DRV_GAIN_CTRL_ADDR{1, 41};
  static constexpr RegAddr SPL_DRV_GAIN_CTRL_ADDR{1, 42};
  static constexpr RegAddr SPR_DRV_GAIN_CTRL_ADDR{1, 43};
  static constexpr RegAddr HEADPHONE_DRV_CTRL_ADDR{1, 44};
  static constexpr RegAddr MIC_BIAS_ADDR{1, 46};
  static constexpr RegAddr MIC_PGA_ADDR{1, 47};
  static constexpr RegAddr MIC_PGAPI_ADDR{1, 48};
  static constexpr RegAddr MIC_PGAMI_ADDR{1, 49};
  static constexpr RegAddr MIC_ICM_ADDR{1, 50};

  // ---- Page 3 registers ----
  static constexpr RegAddr TIMER_MCLK_DIV_ADDR{3, 16};

  // ---- Bit definitions ----
  static constexpr uint8_t SOFT_RESET_ASSERT = 1;
  static constexpr uint8_t CLOCK_GEN_MUX_DEFAULT = 0x3;

  static constexpr uint8_t PLL_POWER_UP = (1 << 7);
  static constexpr uint8_t PLL_P_MASK = 0x7;
  static constexpr uint8_t PLL_R_MASK = 0xF;
  static uint8_t PLL_P(uint8_t val) { return ((val & PLL_P_MASK) << 4); }
  static uint8_t PLL_R(uint8_t val) { return (val & PLL_R_MASK); }

  static constexpr uint8_t NDAC_POWER_UP = (1 << 7);
  static constexpr uint8_t NDAC_DIV_MASK = 0x7F;
  static constexpr uint8_t MDAC_POWER_UP = (1 << 7);
  static constexpr uint8_t MDAC_DIV_MASK = 0x7F;
  static constexpr uint8_t OSR_MSB_MASK = 0x3;
  static constexpr uint8_t OSR_LSB_MASK = 0xFF;
  static constexpr uint8_t NADC_POWER_UP = (1 << 7);
  static constexpr uint8_t NADC_DIV_MASK = 0x7F;
  static constexpr uint8_t MADC_POWER_UP = (1 << 7);
  static constexpr uint8_t MADC_DIV_MASK = 0x7F;

  static uint8_t NDAC_DIV(uint8_t val) { return val & NDAC_DIV_MASK; }
  static uint8_t MDAC_DIV(uint8_t val) { return val & MDAC_DIV_MASK; }
  static uint8_t NADC_DIV(uint8_t val) { return val & NADC_DIV_MASK; }
  static uint8_t MADC_DIV(uint8_t val) { return val & MADC_DIV_MASK; }
  static uint8_t BCLK_DIV(uint8_t val) { return val & MDAC_DIV_MASK; }

  // IF_CTRL1 (audio interface) fields
  static constexpr uint8_t IF_CTRL_IFTYPE_I2S = 0;
  static constexpr uint8_t IF_CTRL_IFTYPE_DSP = 1;
  static constexpr uint8_t IF_CTRL_IFTYPE_RJF = 2;
  static constexpr uint8_t IF_CTRL_IFTYPE_LJF = 3;
  static uint8_t IF_CTRL_IFTYPE(uint8_t val) { return (val & 0x1) << 6; }
  static constexpr uint8_t IF_CTRL_WLEN_16 = 0;
  static constexpr uint8_t IF_CTRL_WLEN_20 = 1;
  static constexpr uint8_t IF_CTRL_WLEN_24 = 2;
  static constexpr uint8_t IF_CTRL_WLEN_32 = 3;
  static uint8_t IF_CTRL_WLEN(uint8_t val) { return (val & 0x3) << 4; }
  static constexpr uint8_t IF_CTRL_BCLK_OUT = (1 << 3);
  static constexpr uint8_t IF_CTRL_WCLK_OUT = (1 << 2);
  static constexpr uint8_t IF_CTRL_DOUT_HIGH_Z = (1 << 0);

  static constexpr uint8_t BCLK_DIV_POWER_UP = (1 << 7);

  static constexpr uint8_t DAC_LR_POWERUP_DEFAULT =
      (1 << 7) | (1 << 6) | (1 << 4) | (1 << 2);
  static constexpr uint8_t DAC_LR_POWERDN_DEFAULT = (1 << 4) | (1 << 2);

  static constexpr uint8_t VOL_CTRL_UNMUTE_DEFAULT = 0;
  static constexpr uint8_t VOL_CTRL_MUTE_DEFAULT = (1 << 3) | (1 << 2);

  static constexpr uint8_t BEEP_GEN_EN_BEEP = (1 << 7);

  // Headphone driver
  static constexpr uint8_t HEADPHONE_DRV_POWERUP = (1 << 7) | (1 << 6);
  static constexpr uint8_t HEADPHONE_DRV_CM_MASK = (0x3 << 3);
  static uint8_t HEADPHONE_DRV_CM(uint8_t val) {
    return (uint8_t)((val << 3) & HEADPHONE_DRV_CM_MASK);
  }
  static constexpr uint8_t HEADPHONE_DRV_RESERVED = (1 << 2);
  static constexpr uint8_t HEADPHONE_DRV_LINEOUT = (1 << 1) | (1 << 2);

  static constexpr uint8_t HP_OUT_POP_RM_ENABLE = (1 << 7);

  static constexpr uint8_t OUTPUT_ROUTING_HPL = (1 << 7);
  static constexpr uint8_t OUTPUT_ROUTING_HPR = (1 << 3);
  static constexpr uint8_t OUTPUT_ROUTING_MIXER = (1 << 6) | (1 << 2);

  // Analog volume (headphone / speaker), 7 bit magnitude + enable bit
  static constexpr uint8_t HPX_ANA_VOL_ENABLE = (1 << 7);
  static constexpr uint8_t HPX_ANA_VOL_MASK = 0x7F;
  static uint8_t HPX_ANA_VOL(uint8_t val) {
    return (uint8_t)((val & HPX_ANA_VOL_MASK) | HPX_ANA_VOL_ENABLE);
  }
  static constexpr uint8_t HPX_ANA_VOL_MAX = 0;
  static constexpr uint8_t HPX_ANA_VOL_DEFAULT = 0;
  static constexpr uint8_t HPX_ANA_VOL_MIN = 127;
  static constexpr uint8_t HPX_ANA_VOL_LOW_THRESH = 105;
  static constexpr uint8_t HPX_ANA_VOL_FLOOR = 144;

  static constexpr uint8_t SPX_ANA_VOL_ENABLE = (1 << 7);
  static constexpr uint8_t SPX_ANA_VOL_MASK = 0x7F;
  static uint8_t SPX_ANA_VOL(uint8_t val) {
    return (uint8_t)((val & SPX_ANA_VOL_MASK) | SPX_ANA_VOL_ENABLE);
  }
  static constexpr uint8_t SPX_ANA_VOL_DEFAULT = 0;
  static constexpr uint8_t SPX_ANA_VOL_LOW_THRESH = 105;
  static constexpr uint8_t SPX_ANA_VOL_FLOOR = 144;

  // Driver gain / mute control
  static constexpr uint8_t HPX_DRV_UNMUTE = (1 << 2);
  static constexpr uint8_t HPX_DRV_RESERVED = (1 << 1);
  static constexpr uint8_t SPX_DRV_UNMUTE = (1 << 2);
  static constexpr uint8_t SPX_DRV_RESERVED = (1 << 1);

  // Class-D speaker driver
  static constexpr uint8_t SPEAKER_DRV_POWERUP = (1 << 7) | (1 << 6);
  static constexpr uint8_t SPEAKER_DRV_RESERVED = (1 << 2) | (1 << 1);

  // Mic / ADC
  static constexpr uint8_t MIC_ADC_POWERUP = (1 << 7);
  static constexpr uint8_t MIC_FCTRL_DEFAULT = 0x0;
  static constexpr uint8_t MIC_CCTRL_DEFAULT = 0x0;
  static constexpr uint8_t MICBIAS_DEFAULT = (1 << 3) | (1 << 1);
  static constexpr uint8_t MIC_PGA_VOL_DEFAULT = 0x0;
  static constexpr uint8_t MIC_PGAPI_L_DEFAULT = (1 << 7) | (1 << 6);
  static constexpr uint8_t MIC_PGAPI_R_DEFAULT = (1 << 5) | (1 << 4);

  // Timer / MCLK divider (page 3)
  static constexpr uint8_t TIMER_MCLK_DIV_EN_EXT = (1 << 7);
  static constexpr uint8_t TIMER_MCLK_DIV_MASK = 0x7F;
  static uint8_t TIMER_MCLK_DIV_VAL(uint8_t val) {
    return val & TIMER_MCLK_DIV_MASK;
  }

  /// Output volume range, in 0.5dB steps (-39dB .. 0dB)
  static constexpr int CODEC_OUTPUT_VOLUME_MAX = 0;
  static constexpr int CODEC_OUTPUT_VOLUME_MIN = (-78 * 2);

  /// PLL/clock divider settings for a given MCLK/sample-rate combination
  struct RateDivs {
    uint32_t mclk;
    uint32_t rate;
    uint8_t pll_p;
    uint8_t pll_j;
    uint16_t pll_d;
    uint8_t dosr;
    uint8_t ndac;
    uint8_t mdac;
    uint8_t aosr;
    uint8_t nadc;
    uint8_t madc;
  };

  TLV320AIC3110() { i2c_addr = 0x18; }

  /**
   * @brief Initialize the codec: soft reset, configure the PLL/clock
   * dividers for the given MCLK and sample rate, configure the I2S
   * interface, select decimation filters and configure the default
   * output (headphone + speaker) and input (microphone) paths.
   *
   * @param mclk master clock frequency in Hz (must match an entry in the
   *             internal PLL divider table, e.g. 12000000, 24000000,
   *             25000000)
   * @param sample_rate I2S sample rate in Hz (8000 .. 192000)
   * @param word_size I2S word size in bits (16, 20, 24, 32)
   * @param bclk_master true if the codec generates BCLK (controller mode)
   */
  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits) {
    return begin(12000000, sample_rate, bits);
  }

  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)fmt;
    (void)is_master;
    (void)channels;
    return begin(sample_rate, bits);
  }

  bool begin(uint32_t mclk = 12000000, uint32_t sample_rate = 44100,
             uint8_t word_size = 16, bool bclk_master = false) {
    bool rc = true;

    rc &= softReset();
    rc &= configureClocks(mclk, sample_rate, word_size, bclk_master);
    rc &= configureDai(word_size, bclk_master, false);
    rc &= configureFilters(sample_rate);
    rc &= configureInput();
    rc &= configureOutput();

    return rc;
  }

  /// Soft reset of the codec
  bool softReset() { return writePagedReg(SOFT_RESET_ADDR, SOFT_RESET_ASSERT); }

  /// Configure the digital audio interface (I2S) word size and clock direction
  bool configureDai(uint8_t word_size, bool bclk_master, bool wclk_master) {
    uint8_t val = IF_CTRL_IFTYPE(IF_CTRL_IFTYPE_I2S);

    if (bclk_master) val |= IF_CTRL_BCLK_OUT;
    if (wclk_master) val |= IF_CTRL_WCLK_OUT;

    switch (word_size) {
      case 16:
        val |= IF_CTRL_WLEN(IF_CTRL_WLEN_16);
        break;
      case 20:
        val |= IF_CTRL_WLEN(IF_CTRL_WLEN_20);
        break;
      case 24:
        val |= IF_CTRL_WLEN(IF_CTRL_WLEN_24);
        break;
      case 32:
        val |= IF_CTRL_WLEN(IF_CTRL_WLEN_32);
        break;
      default:
        return false;
    }

    return writePagedReg(IF_CTRL1_ADDR, val);
  }

  /**
   * @brief Configure the PLL and the NDAC/MDAC/NADC/MADC/(B)OSR clock
   * dividers for the given MCLK and sample rate, using the same divider
   * table as the Zephyr driver. Also configures the BCLK divider if the
   * codec is the I2S bit clock controller.
   */
  bool configureClocks(uint32_t mclk, uint32_t sample_rate,
                        uint8_t word_size, bool bclk_master) {
    size_t count = 0;
    const RateDivs* table = pllDivTable(count);
    const RateDivs* entry = nullptr;

    for (size_t i = 0; i < count; i++) {
      if (table[i].rate == sample_rate && table[i].mclk == mclk) {
        entry = &table[i];
        break;
      }
    }
    if (entry == nullptr) return false;

    bool rc = true;
    uint8_t p = entry->pll_p;
    uint8_t r = 1;
    uint8_t j = entry->pll_j;
    uint16_t d = entry->pll_d;

    /* set the PLL dividers */
    rc &= writePagedReg(PLL_P_R_ADDR, (uint8_t)((1 << 7) | PLL_P(p) | PLL_R(r)));
    rc &= writePagedReg(PLL_J_ADDR, j);
    rc &= writePagedReg(PLL_D_MSB_ADDR, (uint8_t)(d >> 8));
    rc &= writePagedReg(PLL_D_LSB_ADDR, (uint8_t)(d & 0xFF));

    uint8_t madc = entry->madc;
    uint8_t nadc = entry->nadc;
    uint8_t aosr = entry->aosr;
    uint8_t ndac = entry->ndac;
    uint8_t mdac = entry->mdac;
    uint8_t dosr = entry->dosr;

    int bclk_div = 0;
    if (bclk_master) {
      bclk_div = (dosr * mdac) / (word_size * 2U); /* stereo */
      if ((bclk_div * word_size * 2) != (dosr * mdac)) return false;
      rc &= writePagedReg(BCLK_DIV_ADDR,
                           (uint8_t)(BCLK_DIV_POWER_UP | BCLK_DIV((uint8_t)bclk_div)));
    }

    /* Set clock gen mux and turn on PLL */
    rc &= writePagedReg(CLOCK_GEN_MUX_ADDR, CLOCK_GEN_MUX_DEFAULT);
    rc &= updatePagedReg(PLL_P_R_ADDR, PLL_POWER_UP, PLL_POWER_UP);

    /* set NDAC, then MDAC, followed by OSR */
    rc &= writePagedReg(NDAC_DIV_ADDR, (uint8_t)(NDAC_DIV(ndac) | NDAC_POWER_UP));
    rc &= writePagedReg(MDAC_DIV_ADDR, (uint8_t)(MDAC_DIV(mdac) | MDAC_POWER_UP));
    rc &= writePagedReg(OSR_MSB_ADDR, (uint8_t)((dosr >> 8) & OSR_MSB_MASK));
    rc &= writePagedReg(OSR_LSB_ADDR, (uint8_t)(dosr & OSR_LSB_MASK));

    /* set NADC, MADC, OSR */
    rc &= writePagedReg(NADC_DIV_ADDR, (uint8_t)(NADC_DIV(nadc) | NADC_POWER_UP));
    rc &= writePagedReg(MADC_DIV_ADDR, (uint8_t)(MADC_DIV(madc) | MADC_POWER_UP));
    rc &= writePagedReg(AOSR_ADDR, aosr);

    if (bclk_master) {
      rc &= writePagedReg(BCLK_DIV_ADDR,
                           (uint8_t)(BCLK_DIV((uint8_t)bclk_div) | BCLK_DIV_POWER_UP));
    }

    /* calculate MCLK divider to get ~1MHz and setup the timer clock */
    uint8_t mclk_div = (uint8_t)((mclk + 999999U) / 1000000U);
    rc &= writePagedReg(TIMER_MCLK_DIV_ADDR,
                         (uint8_t)(TIMER_MCLK_DIV_EN_EXT | TIMER_MCLK_DIV_VAL(mclk_div)));

    return rc;
  }

  /// Select the DAC/ADC decimation filter (processing block) based on sample rate
  bool configureFilters(uint32_t sample_rate) {
    uint8_t dac_proc_blk, adc_proc_blk;

    if (sample_rate >= 192000) {
      dac_proc_blk = AIC3110_PRB_P17_DECIMATION_C;
      adc_proc_blk = AIC3110_PRB_R16_DECIMATION_C;
    } else if (sample_rate >= 96000) {
      dac_proc_blk = AIC3110_PRB_P7_DECIMATION_B;
      adc_proc_blk = AIC3110_PRB_R10_DECIMATION_B;
    } else {
      dac_proc_blk = AIC3110_PRB_P1_DECIMATION_A;
      adc_proc_blk = AIC3110_PRB_R4_DECIMATION_A;
    }

    bool rc = true;
    rc &= writePagedReg(DAC_PROC_BLK_SEL_ADDR, dac_proc_blk);
    rc &= writePagedReg(ADC_PROC_BLK_SEL_ADDR, adc_proc_blk);
    return rc;
  }

  /// Power up the DAC channels and unmute them
  bool startOutput() {
    bool rc = true;
    rc &= writePagedReg(DATA_PATH_SETUP_ADDR, DAC_LR_POWERUP_DEFAULT);
    rc &= writePagedReg(VOL_CTRL_ADDR, VOL_CTRL_UNMUTE_DEFAULT);
    return rc;
  }

  /// Mute the DAC channels and power them down
  bool stopOutput() {
    bool rc = true;
    rc &= writePagedReg(VOL_CTRL_ADDR, VOL_CTRL_MUTE_DEFAULT);
    rc &= writePagedReg(DATA_PATH_SETUP_ADDR, DAC_LR_POWERDN_DEFAULT);
    return rc;
  }

  /**
   * @brief Configure the headphone and speaker output paths: set common
   * mode voltage, enable click/pop removal, route the DAC to the output
   * mixer, set the default analog volume, unmute and power up the
   * headphone and speaker drivers.
   */
  bool configureOutput() {
    bool rc = true;
    uint8_t val = 0;

    /* set common mode voltage to 1.65V (half of AVDD), AVDD is typically 3.3V */
    rc &= readPagedReg(HEADPHONE_DRV_ADDR, val);
    val &= (uint8_t)~HEADPHONE_DRV_CM_MASK;
    val |= HEADPHONE_DRV_CM(AIC3110_CM_VOLTAGE_1P65) | HEADPHONE_DRV_RESERVED;
    rc &= writePagedReg(HEADPHONE_DRV_ADDR, val);

    /* enable pop removal on power down/up */
    rc &= readPagedReg(HP_OUT_POP_RM_ADDR, val);
    rc &= writePagedReg(HP_OUT_POP_RM_ADDR, (uint8_t)(val | HP_OUT_POP_RM_ENABLE));

    /* route DAC output to mixer */
    rc &= writePagedReg(OUTPUT_ROUTING_ADDR, OUTPUT_ROUTING_MIXER);

    /* enable volume control on Headphone out and Speaker out */
    rc &= writePagedReg(HPL_ANA_VOL_CTRL_ADDR, HPX_ANA_VOL(HPX_ANA_VOL_DEFAULT));
    rc &= writePagedReg(HPR_ANA_VOL_CTRL_ADDR, HPX_ANA_VOL(HPX_ANA_VOL_DEFAULT));
    rc &= writePagedReg(SPL_ANA_VOL_CTRL_ADDR, SPX_ANA_VOL(SPX_ANA_VOL_DEFAULT));
    rc &= writePagedReg(SPR_ANA_VOL_CTRL_ADDR, SPX_ANA_VOL(SPX_ANA_VOL_DEFAULT));

    /* unmute headphone and speaker drivers */
    rc &= writePagedReg(HPL_DRV_GAIN_CTRL_ADDR, (uint8_t)(HPX_DRV_UNMUTE | HPX_DRV_RESERVED));
    rc &= writePagedReg(HPR_DRV_GAIN_CTRL_ADDR, (uint8_t)(HPX_DRV_UNMUTE | HPX_DRV_RESERVED));
    rc &= writePagedReg(SPL_DRV_GAIN_CTRL_ADDR, SPX_DRV_UNMUTE);
    rc &= writePagedReg(SPR_DRV_GAIN_CTRL_ADDR, SPX_DRV_UNMUTE);

    /* power up headphone drivers */
    rc &= readPagedReg(HEADPHONE_DRV_ADDR, val);
    val |= HEADPHONE_DRV_POWERUP | HEADPHONE_DRV_RESERVED;
    rc &= writePagedReg(HEADPHONE_DRV_ADDR, val);

    /* power up speaker drivers */
    rc &= readPagedReg(SPEAKER_DRV_ADDR, val);
    val |= SPEAKER_DRV_POWERUP | SPEAKER_DRV_RESERVED;
    rc &= writePagedReg(SPEAKER_DRV_ADDR, val);

    return rc;
  }

  /**
   * @brief Configure the microphone input path: power up the ADC, set the
   * microphone bias, unmute the input, set the PGA volume and route both
   * microphone inputs to the PGA.
   */
  bool configureInput() {
    bool rc = true;
    rc &= writePagedReg(MIC_ADC_CTRL_ADDR, MIC_ADC_POWERUP);
    rc &= writePagedReg(MIC_BIAS_ADDR, MICBIAS_DEFAULT);
    rc &= writePagedReg(MIC_FCTRL_ADDR, MIC_FCTRL_DEFAULT);
    rc &= writePagedReg(MIC_PGA_ADDR, MIC_PGA_VOL_DEFAULT);
    rc &= writePagedReg(MIC_PGAPI_ADDR, (uint8_t)(MIC_PGAPI_L_DEFAULT | MIC_PGAPI_R_DEFAULT));
    return rc;
  }

  /**
   * @brief Set the output (headphone / speaker) volume.
   *
   * @param vol volume in 0.5dB steps, range CODEC_OUTPUT_VOLUME_MIN (-156,
   *            i.e. -78dB) .. CODEC_OUTPUT_VOLUME_MAX (0, i.e. 0dB)
   * @param channel target channel(s)
   */
  bool setOutputVolume(int vol, AIC3110Channel channel = AIC3110Channel::All) {
    static const uint8_t vol_array[] = {107, 108, 110, 113, 116, 120,
                                         125, 128, 132, 138, 144};
    static constexpr size_t vol_array_len = sizeof(vol_array) / sizeof(vol_array[0]);

    if (vol > CODEC_OUTPUT_VOLUME_MAX || vol < CODEC_OUTPUT_VOLUME_MIN) return false;

    /* remove sign */
    vol = -vol;

    uint8_t vol_val;
    if (vol > HPX_ANA_VOL_FLOOR) {
      vol_val = HPX_ANA_VOL_FLOOR;
    } else if (vol > HPX_ANA_VOL_LOW_THRESH) {
      size_t vol_index = 0;
      for (; vol_index < vol_array_len; vol_index++) {
        if (vol_array[vol_index] >= (uint8_t)vol) break;
      }
      vol_val = (uint8_t)(HPX_ANA_VOL_LOW_THRESH + vol_index + 1);
    } else {
      vol_val = (uint8_t)vol;
    }

    bool rc = true;
    switch (channel) {
      case AIC3110Channel::HeadphoneLeft:
        return writePagedReg(HPL_ANA_VOL_CTRL_ADDR, HPX_ANA_VOL(vol_val));
      case AIC3110Channel::HeadphoneRight:
        return writePagedReg(HPR_ANA_VOL_CTRL_ADDR, HPX_ANA_VOL(vol_val));
      case AIC3110Channel::FrontLeft:
        return writePagedReg(SPL_ANA_VOL_CTRL_ADDR, SPX_ANA_VOL(vol_val));
      case AIC3110Channel::FrontRight:
        return writePagedReg(SPR_ANA_VOL_CTRL_ADDR, SPX_ANA_VOL(vol_val));
      case AIC3110Channel::All:
        rc &= writePagedReg(HPL_ANA_VOL_CTRL_ADDR, HPX_ANA_VOL(vol_val));
        rc &= writePagedReg(HPR_ANA_VOL_CTRL_ADDR, HPX_ANA_VOL(vol_val));
        rc &= writePagedReg(SPL_ANA_VOL_CTRL_ADDR, SPX_ANA_VOL(vol_val));
        rc &= writePagedReg(SPR_ANA_VOL_CTRL_ADDR, SPX_ANA_VOL(vol_val));
        return rc;
      default:
        return false;
    }
  }

  /// Mute / unmute the headphone and/or speaker driver(s)
  bool setOutputMute(bool mute, AIC3110Channel channel = AIC3110Channel::All) {
    uint8_t hp_val = (uint8_t)((mute ? 1U : 0U) << 2U) | HPX_DRV_RESERVED;
    uint8_t sp_val = (uint8_t)((mute ? 1U : 0U) << 2U);

    switch (channel) {
      case AIC3110Channel::HeadphoneLeft:
        return updatePagedReg(HPL_DRV_GAIN_CTRL_ADDR, HPX_DRV_UNMUTE, hp_val);
      case AIC3110Channel::HeadphoneRight:
        return updatePagedReg(HPR_DRV_GAIN_CTRL_ADDR, HPX_DRV_UNMUTE, hp_val);
      case AIC3110Channel::FrontLeft:
        return updatePagedReg(SPL_DRV_GAIN_CTRL_ADDR, SPX_DRV_UNMUTE, sp_val);
      case AIC3110Channel::FrontRight:
        return updatePagedReg(SPR_DRV_GAIN_CTRL_ADDR, SPX_DRV_UNMUTE, sp_val);
      case AIC3110Channel::All: {
        bool rc = true;
        rc &= updatePagedReg(HPL_DRV_GAIN_CTRL_ADDR, HPX_DRV_UNMUTE, hp_val);
        rc &= updatePagedReg(HPR_DRV_GAIN_CTRL_ADDR, HPX_DRV_UNMUTE, hp_val);
        rc &= updatePagedReg(SPL_DRV_GAIN_CTRL_ADDR, SPX_DRV_UNMUTE, sp_val);
        rc &= updatePagedReg(SPR_DRV_GAIN_CTRL_ADDR, SPX_DRV_UNMUTE, sp_val);
        return rc;
      }
      default:
        return false;
    }
  }

  /// Sets the output volume in % (0...100) for all channels, mapped to
  /// [CODEC_OUTPUT_VOLUME_MIN..CODEC_OUTPUT_VOLUME_MAX] (0.5dB steps)
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    int vol = CODEC_OUTPUT_VOLUME_MIN +
              (volume_percent * (CODEC_OUTPUT_VOLUME_MAX - CODEC_OUTPUT_VOLUME_MIN)) / 100;
    return setOutputVolume(vol, AIC3110Channel::All);
  }

  /// Mutes/unmutes all outputs
  bool setMute(bool mute) override { return setOutputMute(mute, AIC3110Channel::All); }

 protected:
  uint8_t current_page = 0xFF;  ///< cached page, invalid initially

  /// Selects the active register page (writes register 0 of page 0)
  bool selectPage(uint8_t page) {
    if (current_page == page) return true;
    uint8_t reg = PAGE_CONTROL_ADDR;
    if (!writeReg(reg, page)) return false;
    current_page = page;
    return true;
  }

  /// Writes a register on the given page
  bool writePagedReg(RegAddr reg, uint8_t value) {
    if (!selectPage(reg.page)) return false;
    return writeReg(reg.reg, value);
  }

  /// Reads a register on the given page
  bool readPagedReg(RegAddr reg, uint8_t& value) {
    if (!selectPage(reg.page)) return false;
    return readReg(reg.reg, value);
  }

  /// Read-Modify-Write of a register on the given page
  bool updatePagedReg(RegAddr reg, uint8_t mask, uint8_t value) {
    uint8_t old = 0;
    if (!readPagedReg(reg, old)) return false;
    uint8_t updated = (old & ~mask) | (value & mask);
    if (updated == old) return true;
    return writePagedReg(reg, updated);
  }

  static constexpr uint8_t PAGE_CONTROL_ADDR = 0;

  /// PLL/clock divider table indexed by (mclk, sample rate)
  static const RateDivs* pllDivTable(size_t& count) {
    static const RateDivs table[] = {
        /* mclk      rate    p  j  d     dosr ndac mdac aosr nadc madc */
        {12000000, 8000, 1, 8, 1920, 128, 48, 2, 128, 48, 2},
        {24000000, 8000, 2, 8, 1920, 128, 48, 2, 128, 48, 2},
        {25000000, 8000, 2, 7, 8643, 128, 48, 2, 128, 48, 2},

        {12000000, 11025, 1, 7, 5264, 128, 32, 2, 128, 32, 2},
        {24000000, 11025, 2, 7, 5264, 128, 32, 2, 128, 32, 2},
        {25000000, 11025, 2, 7, 2253, 128, 32, 2, 128, 32, 2},

        {12000000, 16000, 1, 8, 1920, 128, 24, 2, 128, 24, 2},
        {24000000, 16000, 2, 8, 1920, 128, 24, 2, 128, 24, 2},
        {25000000, 16000, 2, 7, 8643, 128, 24, 2, 128, 24, 2},

        {12000000, 22050, 1, 7, 5264, 128, 16, 2, 128, 16, 2},
        {24000000, 22050, 2, 7, 5264, 128, 16, 2, 128, 16, 2},
        {25000000, 22050, 2, 7, 2253, 128, 16, 2, 128, 16, 2},

        {12000000, 32000, 1, 8, 1920, 128, 12, 2, 128, 12, 2},
        {24000000, 32000, 2, 8, 1920, 128, 12, 2, 128, 12, 2},
        {25000000, 32000, 2, 7, 8643, 128, 12, 2, 128, 12, 2},

        {12000000, 44100, 1, 7, 5264, 128, 8, 2, 128, 8, 2},
        {24000000, 44100, 2, 7, 5264, 128, 8, 2, 128, 8, 2},
        {25000000, 44100, 2, 7, 2253, 128, 8, 2, 128, 8, 2},

        {12000000, 48000, 1, 8, 1920, 128, 8, 2, 128, 8, 2},
        {24000000, 48000, 2, 8, 1920, 128, 8, 2, 128, 8, 2},
        {25000000, 48000, 2, 7, 8643, 128, 8, 2, 128, 8, 2},

        {12000000, 88200, 1, 7, 5264, 64, 8, 2, 64, 8, 2},
        {24000000, 88200, 2, 7, 5264, 64, 8, 2, 64, 8, 2},
        {25000000, 88200, 2, 7, 2253, 64, 8, 2, 64, 8, 2},

        {12000000, 96000, 1, 8, 1920, 64, 8, 2, 64, 8, 2},
        {24000000, 96000, 2, 8, 1920, 64, 8, 2, 64, 8, 2},
        {25000000, 96000, 2, 7, 8643, 64, 8, 2, 64, 8, 2},

        {12000000, 176400, 1, 7, 5264, 32, 8, 2, 32, 8, 2},
        {24000000, 176400, 2, 7, 5264, 32, 8, 2, 32, 8, 2},
        {25000000, 176400, 2, 7, 2253, 32, 8, 2, 32, 8, 2},

        {12000000, 192000, 1, 8, 1920, 32, 8, 2, 32, 8, 2},
        {24000000, 192000, 2, 8, 1920, 32, 8, 2, 32, 8, 2},
        {25000000, 192000, 2, 7, 8643, 32, 8, 2, 32, 8, 2},
    };
    count = sizeof(table) / sizeof(table[0]);
    return table;
  }
};

}  // namespace audio_driver
