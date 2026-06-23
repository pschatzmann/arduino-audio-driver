/**
 * @file SF32LB.h
 * @brief Header only C++ driver for the SF32LB (SiFli SF32LB5x) on-chip
 * AUDCODEC.
 *
 * Ported from the Zephyr RTOS driver
 * (drivers/audio/sf32lb_codec.c, Copyright The Zephyr Project Contributors,
 * Apache-2.0).
 */
#pragma once

#include <stdint.h>

#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/**
 * @brief SF32LB (SiFli SF32LB5x) on-chip AUDCODEC driver, ported from the
 * Zephyr RTOS driver `drivers/audio/sf32lb_codec.c`
 * (https://github.com/zephyrproject-rtos/zephyr, Apache-2.0) using the
 * register definitions from hal_sifli `cmsis/sf32lb52x/audcodec.h`.
 *
 * Unlike the other codecs in this library, the SF32LB AUDCODEC is a
 * memory mapped peripheral that is part of the SoC itself - there is no
 * I2C control bus. It therefore extends ZephyrDriver (for API consistency)
 * but does not use the wire/i2c_addr members; instead it operates on a
 * 32 bit register block whose base address is provided via begin().
 *
 * Only the register level audio path setup (analog DAC/ADC bias, clock
 * dividers for the non-PLL sample rates 8/12/16/24/32/48 kHz, channel
 * enable, volume and mute) is implemented. The fractional audio PLL
 * (needed for the 44.1 kHz family and for PLL based clocking) requires
 * factory trim/calibration data and SoC specific calibration routines
 * that are out of scope for a register level port; CODEC_CLK_USING_PLL
 * is therefore always treated as 0 (XTAL 48 MHz reference).
 */
class SF32LB : public ZephyrDriverCommon {
 public:
  /// Register offsets (byte offsets from the AUDCODEC base address)
  enum Reg : uint32_t {
    REG_ID = 0x00,
    REG_CFG = 0x04,
    REG_IRQ = 0x08,
    REG_IRQ_MSK = 0x0C,
    REG_DAC_CFG = 0x10,
    REG_ADC_CFG = 0x14,
    REG_APB_STAT = 0x18,
    REG_ADC_CH0_CFG = 0x20,
    REG_ADC_CH1_CFG = 0x24,
    REG_DAC_CH0_CFG = 0x30,
    REG_DAC_CH0_CFG_EXT = 0x34,
    REG_DAC_CH1_CFG = 0x38,
    REG_DAC_CH1_CFG_EXT = 0x3C,
    REG_ADC_CH0_ENTRY = 0x40,
    REG_ADC_CH1_ENTRY = 0x44,
    REG_DAC_CH0_ENTRY = 0x50,
    REG_DAC_CH1_ENTRY = 0x54,
    REG_DAC_CH0_DEBUG = 0x58,
    REG_DAC_CH1_DEBUG = 0x5C,
    REG_DAC_CH0_DC = 0x60,
    REG_DAC_CH1_DC = 0x64,
    REG_COMMON_CFG = 0x70,
    REG_BG_CFG0 = 0x74,
    REG_BG_CFG1 = 0x78,
    REG_BG_CFG2 = 0x7C,
    REG_REFGEN_CFG = 0x80,
    REG_PLL_CFG0 = 0x84,
    REG_PLL_CFG1 = 0x88,
    REG_PLL_CFG2 = 0x8C,
    REG_PLL_CFG3 = 0x90,
    REG_PLL_CFG4 = 0x94,
    REG_PLL_CFG5 = 0x98,
    REG_PLL_CFG6 = 0x9C,
    REG_PLL_STAT = 0xA0,
    REG_PLL_CAL_CFG = 0xA4,
    REG_PLL_CAL_RESULT = 0xA8,
    REG_ADC_ANA_CFG = 0xAC,
    REG_ADC1_CFG1 = 0xB0,
    REG_ADC1_CFG2 = 0xB4,
    REG_ADC2_CFG1 = 0xB8,
    REG_ADC2_CFG2 = 0xBC,
    REG_DAC1_CFG = 0xC0,
    REG_DAC2_CFG = 0xC4,
  };

  // ---- CFG ----
  static constexpr uint32_t CFG_ADC_ENABLE = 1UL << 0;
  static constexpr uint32_t CFG_DAC_ENABLE = 1UL << 1;
  static constexpr uint32_t CFG_ADC_EN_DLY_SEL_Msk = 0x3UL << 3;

  // ---- DAC_CFG / ADC_CFG ----
  static constexpr uint32_t DAC_CFG_OSR_SEL_Msk = 0xFUL << 0;
  static constexpr uint32_t DAC_CFG_OP_MODE_Msk = 0x3UL << 4;
  static constexpr uint32_t DAC_CFG_PATH_RESET = 1UL << 6;
  static constexpr uint32_t DAC_CFG_CLK_SRC_SEL_Msk = 0x1UL << 7;
  static constexpr uint32_t DAC_CFG_CLK_DIV_Msk = 0xFFUL << 8;

  static constexpr uint32_t ADC_CFG_OSR_SEL_Msk = 0x7UL << 0;
  static constexpr uint32_t ADC_CFG_OP_MODE_Msk = 0x3UL << 3;
  static constexpr uint32_t ADC_CFG_PATH_RESET = 1UL << 5;
  static constexpr uint32_t ADC_CFG_CLK_SRC_SEL_Msk = 0x1UL << 6;
  static constexpr uint32_t ADC_CFG_CLK_DIV_Msk = 0xFFUL << 8;

  // ---- DAC_CHx_CFG ----
  static constexpr uint32_t DAC_CH_CFG_ENABLE = 1UL << 0;
  static constexpr uint32_t DAC_CH_CFG_DOUT_MUTE = 1UL << 1;
  static constexpr uint32_t DAC_CH_CFG_DEM_MODE_Msk = 0x3UL << 2;
  static constexpr uint32_t DAC_CH_CFG_DMA_EN = 1UL << 7;
  static constexpr uint32_t DAC_CH_CFG_ROUGH_VOL_Msk = 0xFUL << 8;
  static constexpr uint32_t DAC_CH_CFG_FINE_VOL_Msk = 0xFUL << 12;
  static constexpr uint32_t DAC_CH_CFG_DATA_FORMAT = 1UL << 16;
  static constexpr uint32_t DAC_CH_CFG_SINC_GAIN_Msk = 0x1FFUL << 17;
  static constexpr uint32_t DAC_CH_CFG_DITHER_GAIN_Msk = 0x7UL << 26;
  static constexpr uint32_t DAC_CH_CFG_DITHER_EN = 1UL << 29;

  // ---- DAC_CHx_CFG_EXT ----
  static constexpr uint32_t DAC_CH_CFG_EXT_RAMP_EN = 1UL << 0;
  static constexpr uint32_t DAC_CH_CFG_EXT_RAMP_MODE = 1UL << 1;
  static constexpr uint32_t DAC_CH_CFG_EXT_ZERO_ADJUST_EN = 1UL << 2;
  static constexpr uint32_t DAC_CH_CFG_EXT_RAMP_INTERVAL_Msk = 0xFUL << 3;
  static constexpr uint32_t DAC_CH_CFG_EXT_RAMP_STAT_Msk = 0x3UL << 7;

  // ---- DAC_CHx_DEBUG ----
  static constexpr uint32_t DAC_CH_DEBUG_DATA_OUT_Msk = 0xFFFFUL << 0;
  static constexpr uint32_t DAC_CH_DEBUG_BYPASS = 1UL << 16;

  // ---- ADC_CHx_CFG ----
  static constexpr uint32_t ADC_CH_CFG_ENABLE = 1UL << 0;
  static constexpr uint32_t ADC_CH_CFG_HPF_BYPASS = 1UL << 1;
  static constexpr uint32_t ADC_CH_CFG_HPF_COEF_Msk = 0xFUL << 2;
  static constexpr uint32_t ADC_CH_CFG_STB_INV = 1UL << 6;
  static constexpr uint32_t ADC_CH_CFG_DMA_EN = 1UL << 7;
  static constexpr uint32_t ADC_CH_CFG_ROUGH_VOL_Msk = 0xFUL << 8;
  static constexpr uint32_t ADC_CH_CFG_FINE_VOL_Msk = 0xFUL << 12;
  static constexpr uint32_t ADC_CH_CFG_DATA_FORMAT = 1UL << 16;

  // ---- BG_CFG0 ----
  static constexpr uint32_t BG_CFG0_EN = 1UL << 0;
  static constexpr uint32_t BG_CFG0_LP_MODE = 1UL << 1;
  static constexpr uint32_t BG_CFG0_VREF_SEL_Msk = 0xFUL << 2;
  static constexpr uint32_t BG_CFG0_EN_SMPL = 1UL << 7;
  static constexpr uint32_t BG_CFG0_EN_RCFLT = 1UL << 8;
  static constexpr uint32_t BG_CFG0_MIC_VREF_SEL_Msk = 0x7UL << 9;
  static constexpr uint32_t BG_CFG0_EN_AMP = 1UL << 12;
  static constexpr uint32_t BG_CFG0_SET_VC = 1UL << 13;

  // ---- REFGEN_CFG ----
  static constexpr uint32_t REFGEN_CFG_EN = 1UL << 0;
  static constexpr uint32_t REFGEN_CFG_EN_CHOP = 1UL << 1;
  static constexpr uint32_t REFGEN_CFG_LV_MODE = 1UL << 5;

  // ---- PLL_CFG0 ----
  static constexpr uint32_t PLL_CFG0_ICP_SEL_Msk = 0x1FUL << 6;
  static constexpr uint32_t PLL_CFG0_EN_ANA = 1UL << 15;
  static constexpr uint32_t PLL_CFG0_EN_VCO = 1UL << 27;
  static constexpr uint32_t PLL_CFG0_EN_IARY = 1UL << 28;

  // ---- PLL_CFG1 ----
  static constexpr uint32_t PLL_CFG1_R3_SEL_Msk = 0xFUL << 0;
  static constexpr uint32_t PLL_CFG1_RZ_SEL_Msk = 0xFUL << 4;
  static constexpr uint32_t PLL_CFG1_C2_SEL_Msk = 0x7UL << 8;
  static constexpr uint32_t PLL_CFG1_CZ_SEL_Msk = 0x7UL << 11;

  // ---- PLL_CFG2 ----
  static constexpr uint32_t PLL_CFG2_RSTB = 1UL << 8;
  static constexpr uint32_t PLL_CFG2_EN_DIG = 1UL << 13;

  // ---- PLL_CFG3 ----
  static constexpr uint32_t PLL_CFG3_EN_SDM = 1UL << 30;

  // ---- PLL_CFG4 ----
  static constexpr uint32_t PLL_CFG4_EN_CLK_CHOP_DAC = 1UL << 5;
  static constexpr uint32_t PLL_CFG4_DIVA_CLK_DAC_Msk = 0x1FUL << 6;
  static constexpr uint32_t PLL_CFG4_EN_CLK_DAC = 1UL << 11;
  static constexpr uint32_t PLL_CFG4_SEL_CLK_DAC = 1UL << 12;
  static constexpr uint32_t PLL_CFG4_SEL_CLK_DAC_SOURCE_Msk = 0x3UL << 13;
  static constexpr uint32_t PLL_CFG4_SEL_CLK_DIG = 1UL << 15;
  static constexpr uint32_t PLL_CFG4_DIVA_CLK_DIG_Msk = 0x1FUL << 18;
  static constexpr uint32_t PLL_CFG4_EN_CLK_DIG = 1UL << 23;

  // ---- PLL_CFG5 ----
  static constexpr uint32_t PLL_CFG5_EN_CLK_CHOP_BG = 1UL << 7;
  static constexpr uint32_t PLL_CFG5_EN_CLK_CHOP_REFGEN = 1UL << 15;

  // ---- PLL_CFG6 ----
  static constexpr uint32_t PLL_CFG6_SEL_CLK_CHOP_MICBIAS_Msk = 0x3UL << 6;
  static constexpr uint32_t PLL_CFG6_EN_CLK_CHOP_MICBIAS = 1UL << 8;
  static constexpr uint32_t PLL_CFG6_SEL_CLK_ADC2 = 1UL << 9;
  static constexpr uint32_t PLL_CFG6_DIVA_CLK_ADC2_Msk = 0x7UL << 10;
  static constexpr uint32_t PLL_CFG6_EN_CLK_ADC2 = 1UL << 13;
  static constexpr uint32_t PLL_CFG6_SEL_CLK_ADC1 = 1UL << 14;
  static constexpr uint32_t PLL_CFG6_DIVA_CLK_ADC1_Msk = 0x7UL << 15;
  static constexpr uint32_t PLL_CFG6_EN_CLK_ADC1 = 1UL << 18;
  static constexpr uint32_t PLL_CFG6_SEL_CLK_ADC0 = 1UL << 19;
  static constexpr uint32_t PLL_CFG6_DIVA_CLK_ADC0_Msk = 0x7UL << 20;
  static constexpr uint32_t PLL_CFG6_EN_CLK_ADC0 = 1UL << 23;
  static constexpr uint32_t PLL_CFG6_SEL_CLK_ADC_SOURCE = 1UL << 24;

  // ---- ADC_ANA_CFG ----
  static constexpr uint32_t ADC_ANA_CFG_MICBIAS_CHOP_EN = 1UL << 0;
  static constexpr uint32_t ADC_ANA_CFG_MICBIAS_EN = 1UL << 1;

  // ---- ADC1_CFG1 / ADC2_CFG1 ----
  static constexpr uint32_t ADCx_CFG1_VCMST = 1UL << 3;
  static constexpr uint32_t ADCx_CFG1_VREF_SEL_Msk = 0x7UL << 6;
  static constexpr uint32_t ADCx_CFG1_GC_Msk = 0x7UL << 18;
  static constexpr uint32_t ADCx_CFG1_DACN_EN = 1UL << 21;
  static constexpr uint32_t ADCx_CFG1_DIFF_EN = 1UL << 22;
  static constexpr uint32_t ADCx_CFG1_FSP_Msk = 0x3UL << 23;

  // ---- ADC1_CFG2 / ADC2_CFG2 ----
  static constexpr uint32_t ADCx_CFG2_CLEAR = 1UL << 0;
  static constexpr uint32_t ADCx_CFG2_RSTB = 1UL << 2;
  static constexpr uint32_t ADCx_CFG2_EN = 1UL << 3;

  // ---- DAC1_CFG / DAC2_CFG ----
  static constexpr uint32_t DACx_CFG_EN_OS_DAC = 1UL << 0;
  static constexpr uint32_t DACx_CFG_SR = 1UL << 12;
  static constexpr uint32_t DACx_CFG_LP_MODE = 1UL << 14;
  static constexpr uint32_t DACx_CFG_EN_AMP = 1UL << 21;
  static constexpr uint32_t DACx_CFG_EN_VCM = 1UL << 24;
  static constexpr uint32_t DACx_CFG_EN_DAC = 1UL << 25;

  /// DAC volume range (1 dB units): -36 dB .. +54 dB, applied via
  /// rough (6 dB step) + fine (0.5 dB step) volume fields.
  static constexpr int MIN_VOLUME_DB = -36;
  static constexpr int MAX_VOLUME_DB = 54;

  /// Clock divider settings for the non-PLL (48 MHz XTAL) sample rates.
  struct DacClockConfig {
    uint32_t sample_rate;
    uint8_t clk_div;     // DAC_CFG.CLK_DIV
    uint8_t osr_sel;     // DAC_CFG.OSR_SEL
    uint8_t diva_clk_dac;  // PLL_CFG4.DIVA_CLK_DAC
  };

  struct AdcClockConfig {
    uint32_t sample_rate;
    uint8_t clk_div;       // ADC_CFG.CLK_DIV
    uint8_t osr_sel;       // ADC_CFG.OSR_SEL
    uint8_t diva_clk_adc;  // PLL_CFG6.DIVA_CLK_ADCx
    uint8_t fsp;           // ADCx_CFG1.FSP
  };

  /// Provides the (memory mapped) base address of the AUDCODEC register
  /// block. Must be called before begin().
  void setBaseAddress(uintptr_t base) { reg_base = base; }

  /// Initializes the analog references (bandgap/refgen/PLL digital clocks)
  /// and configures the DAC output path for the given sample rate.
  /// @param base AUDCODEC peripheral base address
  /// @param sample_rate one of 8000, 12000, 16000, 24000, 32000, 48000
  bool begin(uintptr_t base, uint32_t sample_rate = 48000) {
    reg_base = base;
    const DacClockConfig* dac_clk = dacClockConfig(sample_rate);
    if (dac_clk == nullptr) return false;

    pllAndReferencesOn();
    configAnalogDacPath(*dac_clk);
    configTxChannel(*dac_clk);
    configDacPath(false);
    setBit(REG_CFG, CFG_DAC_ENABLE);
    return true;
  }

  /// Configures and enables the ADC input path for the given sample rate.
  /// begin() must have been called first (for the shared bandgap/refgen).
  bool beginInput(uint32_t sample_rate = 48000) {
    const AdcClockConfig* adc_clk = adcClockConfig(sample_rate);
    if (adc_clk == nullptr) return false;

    configAnalogAdcPath(*adc_clk);
    configRxChannel(*adc_clk);
    setBit(REG_CFG, CFG_ADC_ENABLE);
    return true;
  }

  /// Starts (un-mutes) the DAC output path.
  void start() { mute(false); }

  /// Mutes and disables the DAC output path.
  void stop() {
    mute(true);
    clearBit(REG_DAC_CH0_CFG, DAC_CH_CFG_ENABLE);
    clearBit(REG_DAC_CH1_CFG, DAC_CH_CFG_ENABLE);
    setBit(REG_DAC_CFG, DAC_CFG_PATH_RESET);
    clearBit(REG_DAC_CFG, DAC_CFG_PATH_RESET);
    closeAnalogDacPath();
    clearBit(REG_CFG, CFG_DAC_ENABLE);
  }

  /// Disables the ADC input path.
  void stopInput() {
    clearBit(REG_ADC_CH0_CFG, ADC_CH_CFG_ENABLE);
    clearBit(REG_ADC_CH1_CFG, ADC_CH_CFG_ENABLE);
    setBit(REG_ADC_CFG, ADC_CFG_PATH_RESET);
    clearBit(REG_ADC_CFG, ADC_CFG_PATH_RESET);
    closeAnalogAdcPath();
    clearBit(REG_CFG, CFG_ADC_ENABLE);
  }

  /// Mutes/unmutes the DAC output (channel 0). The previous fine volume
  /// setting is restored when un-muting.
  bool setMute(bool mute_flag) override {
    mute(mute_flag);
    return true;
  }

  /// Sets the DAC output volume in dB, range MIN_VOLUME_DB..MAX_VOLUME_DB.
  bool setVolumeDb(int volume_db) {
    if (volume_db < MIN_VOLUME_DB) volume_db = MIN_VOLUME_DB;
    if (volume_db > MAX_VOLUME_DB) volume_db = MAX_VOLUME_DB;

    uint32_t rough_vol = (volume_db - MIN_VOLUME_DB) / 6;
    uint32_t fine_vol = ((volume_db - MIN_VOLUME_DB) % 6) << 1;

    uint32_t value = (rough_vol << 8) | (fine_vol << 12);
    uint32_t mask = DAC_CH_CFG_ROUGH_VOL_Msk | DAC_CH_CFG_FINE_VOL_Msk;
    updateReg32(REG_DAC_CH0_CFG, mask, value);
    last_fine_vol = fine_vol;
    return true;
  }

  /// Sets the DAC output volume in % (0...100), mapped to
  /// [MIN_VOLUME_DB..MAX_VOLUME_DB]
  bool setVolume(int volume) override {
    volume_percent = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
    int volume_db = MIN_VOLUME_DB +
                    (volume_percent * (MAX_VOLUME_DB - MIN_VOLUME_DB)) / 100;
    return setVolumeDb(volume_db);
  }

 protected:
  uintptr_t reg_base = 0;
  uint32_t last_fine_vol = 0;

  // ---- 32 bit memory mapped register access ----

  uint32_t read32(uint32_t offset) {
    return *reinterpret_cast<volatile uint32_t*>(reg_base + offset);
  }

  void write32(uint32_t offset, uint32_t value) {
    *reinterpret_cast<volatile uint32_t*>(reg_base + offset) = value;
  }

  void setBit(uint32_t offset, uint32_t mask) {
    write32(offset, read32(offset) | mask);
  }

  void clearBit(uint32_t offset, uint32_t mask) {
    write32(offset, read32(offset) & ~mask);
  }

  /// Read-Modify-Write of selected bits of a 32 bit register.
  void updateReg32(uint32_t offset, uint32_t mask, uint32_t value) {
    uint32_t old = read32(offset);
    write32(offset, (old & ~mask) | (value & mask));
  }

  // ---- helpers ported from sf32lb_codec.c ----

  static const DacClockConfig* dacClockConfig(uint32_t sample_rate) {
    static constexpr DacClockConfig table[] = {
        {48000, 5, 0, 20}, {32000, 5, 1, 20}, {24000, 10, 5, 10},
        {16000, 5, 4, 20}, {12000, 20, 7, 5}, {8000, 10, 8, 10},
    };
    for (const auto& cfg : table) {
      if (cfg.sample_rate == sample_rate) return &cfg;
    }
    return nullptr;
  }

  static const AdcClockConfig* adcClockConfig(uint32_t sample_rate) {
    static constexpr AdcClockConfig table[] = {
        {48000, 5, 0, 5, 0}, {32000, 5, 1, 5, 0}, {24000, 10, 0, 5, 2},
        {16000, 10, 1, 5, 2}, {12000, 10, 2, 5, 2}, {8000, 10, 3, 5, 2},
    };
    for (const auto& cfg : table) {
      if (cfg.sample_rate == sample_rate) return &cfg;
    }
    return nullptr;
  }

  /// Enables the bandgap reference, reference generator and PLL digital
  /// clocks (no PLL frequency calibration, see class documentation).
  void pllAndReferencesOn() {
    // bandgap
    uint32_t bg = (1UL << 0)                 // EN
                  | (0UL << 1)                // LP_MODE
                  | (0xcUL << 2)              // VREF_SEL = 3.3V
                  | (0UL << 7)                // EN_SMPL
                  | BG_CFG0_EN_RCFLT          // EN_RCFLT
                  | (4UL << 9)                // MIC_VREF_SEL
                  | BG_CFG0_EN_AMP            // EN_AMP
                  | (0UL << 13);              // SET_VC
    write32(REG_BG_CFG0, bg);
    write32(REG_BG_CFG1, 0);
    write32(REG_BG_CFG2, 0);

    setBit(REG_PLL_CFG0, PLL_CFG0_EN_IARY | PLL_CFG0_EN_VCO | PLL_CFG0_EN_ANA);
    updateReg32(REG_PLL_CFG0, PLL_CFG0_ICP_SEL_Msk, 8UL << 6);

    setBit(REG_PLL_CFG2, PLL_CFG2_EN_DIG);
    setBit(REG_PLL_CFG3, PLL_CFG3_EN_SDM);
    setBit(REG_PLL_CFG4, PLL_CFG4_EN_CLK_DIG);

    uint32_t pll_cfg1 = (3UL << 0)   // R3_SEL
                        | (1UL << 4)  // RZ_SEL
                        | (3UL << 8)  // C2_SEL
                        | (6UL << 11);  // CZ_SEL
    write32(REG_PLL_CFG1, pll_cfg1);

    refgenInit();
  }

  void refgenInit() {
    clearBit(REG_BG_CFG0, BG_CFG0_EN_SMPL);
    clearBit(REG_REFGEN_CFG, REFGEN_CFG_EN_CHOP);
    setBit(REG_REFGEN_CFG, REFGEN_CFG_EN);
    clearBit(REG_REFGEN_CFG, REFGEN_CFG_LV_MODE);
    setBit(REG_PLL_CFG5, PLL_CFG5_EN_CLK_CHOP_BG);
    setBit(REG_PLL_CFG5, PLL_CFG5_EN_CLK_CHOP_REFGEN);
    delayMs(2);
    clearBit(REG_BG_CFG0, BG_CFG0_EN_SMPL);
  }

  /// Configures the analog DAC bias/clock tree and enables the DAC amps.
  void configAnalogDacPath(const DacClockConfig& clk) {
    clearBit(REG_PLL_CFG4, PLL_CFG4_SEL_CLK_DAC);
    clearBit(REG_PLL_CFG4, PLL_CFG4_SEL_CLK_DAC_SOURCE_Msk);

    uint32_t mask = PLL_CFG4_EN_CLK_CHOP_DAC | PLL_CFG4_EN_CLK_DAC |
                     PLL_CFG4_SEL_CLK_DAC_SOURCE_Msk | PLL_CFG4_SEL_CLK_DAC |
                     PLL_CFG4_EN_CLK_DIG;
    uint32_t value = PLL_CFG4_EN_CLK_CHOP_DAC | PLL_CFG4_EN_CLK_DAC |
                      PLL_CFG4_EN_CLK_DIG;  // SEL_CLK_DAC_SOURCE=0, SEL_CLK_DAC=0
    updateReg32(REG_PLL_CFG4, mask, value);
    updateReg32(REG_PLL_CFG4, PLL_CFG4_DIVA_CLK_DIG_Msk,
                ((uint32_t)clk.diva_clk_dac) << 18);

    setBit(REG_PLL_CFG5, PLL_CFG5_EN_CLK_CHOP_BG | PLL_CFG5_EN_CLK_CHOP_REFGEN);

    clearBit(REG_PLL_CFG2, PLL_CFG2_RSTB);
    delayMs(1);
    setBit(REG_PLL_CFG2, PLL_CFG2_RSTB);

    clearBit(REG_DAC1_CFG, DACx_CFG_LP_MODE);
    clearBit(REG_DAC1_CFG, DACx_CFG_EN_OS_DAC);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_OS_DAC);
    setBit(REG_DAC1_CFG, DACx_CFG_EN_VCM);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_VCM);
    delayMs(1);

    setBit(REG_DAC1_CFG, DACx_CFG_EN_AMP);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_AMP);
    delayMs(1);

    setBit(REG_DAC1_CFG, DACx_CFG_EN_OS_DAC);
    setBit(REG_DAC2_CFG, DACx_CFG_EN_OS_DAC);
    delayMs(1);
    setBit(REG_DAC1_CFG, DACx_CFG_EN_DAC);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_DAC);
    delayMs(1);
    clearBit(REG_DAC1_CFG, DACx_CFG_SR);
    clearBit(REG_DAC2_CFG, DACx_CFG_SR);
  }

  /// Powers down the analog DAC path (mirrors close_analog_dac_path()).
  void closeAnalogDacPath() {
    setBit(REG_DAC1_CFG, DACx_CFG_SR);
    setBit(REG_DAC2_CFG, DACx_CFG_SR);
    delayMs(1);
    clearBit(REG_DAC1_CFG, DACx_CFG_EN_DAC);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_DAC);
    delayMs(1);
    clearBit(REG_DAC1_CFG, DACx_CFG_EN_VCM);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_VCM);
    delayMs(1);
    clearBit(REG_DAC1_CFG, DACx_CFG_EN_AMP);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_AMP);
    clearBit(REG_DAC1_CFG, DACx_CFG_EN_OS_DAC);
    clearBit(REG_DAC2_CFG, DACx_CFG_EN_OS_DAC);
  }

  /// Configures the digital DAC channel 0 path (DMA disabled - data is
  /// fed via DAC_CH0_ENTRY or, if bypass is used, DAC_CH0_DEBUG).
  void configTxChannel(const DacClockConfig& clk) {
    updateReg32(REG_CFG, CFG_ADC_EN_DLY_SEL_Msk, 3UL << 3);

    uint32_t dac_cfg = (((uint32_t)clk.osr_sel) << 0)   // OSR_SEL
                        | (0UL << 4)                      // OP_MODE
                        | (0UL << 7)                      // CLK_SRC_SEL (xtal)
                        | (((uint32_t)clk.clk_div) << 8);  // CLK_DIV
    write32(REG_DAC_CFG, dac_cfg);

    constexpr uint32_t sinc_gain = 0x14D;
    uint32_t ch0_cfg = DAC_CH_CFG_ENABLE | (2UL << 2)  // DEM_MODE
                       | (6UL << 8)                    // ROUGH_VOL
                       | (0UL << 12)                   // FINE_VOL
                       | DAC_CH_CFG_DATA_FORMAT |
                       (sinc_gain << 17);
    write32(REG_DAC_CH0_CFG, ch0_cfg);

    uint32_t ch0_ext = DAC_CH_CFG_EXT_RAMP_EN | DAC_CH_CFG_EXT_RAMP_MODE |
                        DAC_CH_CFG_EXT_ZERO_ADJUST_EN |
                        (2UL << 3);  // RAMP_INTERVAL
    write32(REG_DAC_CH0_CFG_EXT, ch0_ext);

    write32(REG_DAC_CH0_DEBUG, 0xFFUL);  // BYPASS=0, DATA_OUT=0xFF
  }

  /// Enables/disables the digital bypass path on both DAC channels.
  void configDacPath(bool bypass) {
    if (bypass) {
      setBit(REG_DAC_CH0_CFG, DAC_CH_CFG_DOUT_MUTE);
      write32(REG_DAC_CH0_DEBUG, DAC_CH_DEBUG_BYPASS | 0xFFUL);
      setBit(REG_DAC_CH1_CFG, DAC_CH_CFG_DOUT_MUTE);
      write32(REG_DAC_CH1_DEBUG, DAC_CH_DEBUG_BYPASS | 0xFFUL);
    } else {
      clearBit(REG_DAC_CH0_CFG, DAC_CH_CFG_DOUT_MUTE);
      write32(REG_DAC_CH0_DEBUG, 0xFFUL);
      clearBit(REG_DAC_CH1_CFG, DAC_CH_CFG_DOUT_MUTE);
      write32(REG_DAC_CH1_DEBUG, 0xFFUL);
    }
  }

  /// Configures the analog ADC bias/clock tree and enables ADC1.
  void configAnalogAdcPath(const AdcClockConfig& clk) {
    clearBit(REG_BG_CFG0, BG_CFG0_EN_SMPL);
    setBit(REG_ADC_ANA_CFG, ADC_ANA_CFG_MICBIAS_EN);
    clearBit(REG_ADC_ANA_CFG, ADC_ANA_CFG_MICBIAS_CHOP_EN);
    delayMs(2);
    clearBit(REG_BG_CFG0, BG_CFG0_EN_SMPL);

    uint32_t pll_cfg6 = (3UL << 6)   // SEL_CLK_CHOP_MICBIAS
                         | PLL_CFG6_EN_CLK_CHOP_MICBIAS |
                         (((uint32_t)clk.diva_clk_adc) << 10)  // DIVA_CLK_ADC2
                         | PLL_CFG6_EN_CLK_ADC2 |
                         (((uint32_t)clk.diva_clk_adc) << 15)  // DIVA_CLK_ADC1
                         | PLL_CFG6_EN_CLK_ADC1 | (5UL << 20)  // DIVA_CLK_ADC0
                         | PLL_CFG6_EN_CLK_ADC0 | PLL_CFG6_SEL_CLK_ADC0;
    write32(REG_PLL_CFG6, pll_cfg6);

    clearBit(REG_PLL_CFG2, PLL_CFG2_RSTB);
    delayMs(1);
    setBit(REG_PLL_CFG2, PLL_CFG2_RSTB);

    clearBit(REG_ADC1_CFG1, ADCx_CFG1_DIFF_EN);
    clearBit(REG_ADC1_CFG1, ADCx_CFG1_DACN_EN);
    updateReg32(REG_ADC1_CFG1, ADCx_CFG1_FSP_Msk, ((uint32_t)clk.fsp) << 23);

    setBit(REG_ADC1_CFG1, ADCx_CFG1_VCMST);
    setBit(REG_ADC1_CFG2, ADCx_CFG2_CLEAR);

    updateReg32(REG_ADC1_CFG1, ADCx_CFG1_GC_Msk, 0x4UL << 18);

    setBit(REG_ADC1_CFG2, ADCx_CFG2_EN);
    clearBit(REG_ADC1_CFG2, ADCx_CFG2_RSTB);

    updateReg32(REG_ADC1_CFG1, ADCx_CFG1_VREF_SEL_Msk, 0x2UL << 6);

    delayMs(20);

    setBit(REG_ADC1_CFG2, ADCx_CFG2_RSTB);
    clearBit(REG_ADC1_CFG1, ADCx_CFG1_VCMST);
    clearBit(REG_ADC1_CFG2, ADCx_CFG2_CLEAR);
  }

  /// Powers down the analog ADC path (mirrors close_analog_adc_path()).
  void closeAnalogAdcPath() {
    clearBit(REG_ADC1_CFG2, ADCx_CFG2_EN);
    clearBit(REG_ADC2_CFG2, ADCx_CFG2_EN);
    clearBit(REG_ADC_ANA_CFG, ADC_ANA_CFG_MICBIAS_EN);
  }

  /// Configures the digital ADC channel 0 path (DMA disabled).
  void configRxChannel(const AdcClockConfig& clk) {
    uint32_t adc_cfg = (((uint32_t)clk.osr_sel) << 0)    // OSR_SEL
                        | (0UL << 3)                       // OP_MODE
                        | (0UL << 6)                       // CLK_SRC_SEL (xtal)
                        | (((uint32_t)clk.clk_div) << 8);   // CLK_DIV
    write32(REG_ADC_CFG, adc_cfg);

    uint32_t ch0_cfg = ADC_CH_CFG_ENABLE | (0x7UL << 2)  // HPF_COEF
                       | (0xaUL << 8)                    // ROUGH_VOL
                       | (0UL << 12)                     // FINE_VOL
                       | ADC_CH_CFG_DATA_FORMAT;
    write32(REG_ADC_CH0_CFG, ch0_cfg);
  }

  /// Mutes/unmutes DAC channel 0 by forcing the fine volume to its mute
  /// value (0xF) and restoring the previous value on unmute (mirrors
  /// mute_dac_path()).
  void mute(bool mute_flag) {
    if (mute_flag) {
      uint32_t reg_val = read32(REG_DAC_CH0_CFG);
      last_fine_vol = (reg_val & DAC_CH_CFG_FINE_VOL_Msk) >> 12;
      updateReg32(REG_DAC_CH0_CFG, DAC_CH_CFG_FINE_VOL_Msk, 0xFUL << 12);
    } else {
      updateReg32(REG_DAC_CH0_CFG, DAC_CH_CFG_FINE_VOL_Msk, last_fine_vol << 12);
    }
  }
};

}  // namespace audio_driver
