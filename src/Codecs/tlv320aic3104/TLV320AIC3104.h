/**
 * @file TLV320AIC3104.h
 * @brief Header only C++ driver for the Texas Instruments TLV320AIC3104
 * low-power stereo audio codec.
 *
 * Structured to mirror the arduino-audio-driver TLV320AIC3110.h driver.
 * NOTE: the AIC3104 is a member of the older "AIC3x"
 * codec family and has a materially different register map than the
 * AIC3110/TLV320AIC31xx family.
 *
 * Reference: https://www.ti.com/lit/ds/symlink/tlv320aic3104.pdf
 */
#pragma once
#include <cstddef>
#include "Codecs/ZephyrDriverCommon.h"

namespace audio_driver {

/// Logical audio channels
enum class AIC3104Channel {
  HeadphoneLeft,   ///< HPLOUT
  HeadphoneRight,  ///< HPROUT
  LineOutLeft,     ///< LEFT_LOP/M  (fully differential line output)
  LineOutRight,    ///< RIGHT_LOP/M (fully differential line output)
  All,
};

// Make it easier to represent bits from the datasheet.
// Bits D[l] - D[r] = 0b[v]  l & r are between 7 and 0 inclusive.
// For example: "D7-D5 set to 010" is `makeBits(7, 5, 0b010)` or
// `makeBits(7, 5, 2)`.
// Plain functions (not macros) are used here, unlike ESP-IDF's own
// single-arg BIT(x) macro (from esp_bit_defs.h), so this header never has
// to undef/restore anyone else's macro. They live at namespace scope
// (rather than as class members) so they are fully defined before any
// static constexpr class member that uses them in its initializer.
static constexpr uint8_t makeBits(uint8_t l, uint8_t r, uint8_t v) {
  return (uint8_t)(((v) << (r)) & ((1 << ((l) + 1)) - 1));
}
static constexpr uint8_t makeBit(uint8_t l, uint8_t v) {
  return (uint8_t)(((v) & 0x01) << (l));
}

/**
 * @brief Header only C++ driver for the TLV320AIC3104 audio codec.
 *
 * The TLV320AIC3104 uses a paged 8 bit register map (page select register
 * at page 0 / register 0, pages 0 and 1 are used by this device). Provides
 * soft reset, PLL/clock configuration (P/Q/J/D PLL plus a shared
 * ADC/DAC "NCODEC" divider), DAI (I2S) format configuration and
 * output/input path configuration including volume and mute control for
 * the headphone (HPLOUT/HPROUT) and fully differential line (LOP/M)
 * outputs, and the microphone/line input PGAs.
 */
class TLV320AIC3104 : public ZephyrDriverCommon {
 public:
  /// Page + register address pair
  struct RegAddr {
    uint8_t page;
    uint8_t reg;
  };

  static constexpr uint8_t I2C_ADDR = 0x18;

  // ---- Page 0 registers ----
  static constexpr RegAddr SOFT_RESET_ADDR{0, 1};
  static constexpr RegAddr SAMPLE_RATE_SEL_ADDR{0, 2};
  static constexpr RegAddr PLL_PROGA_ADDR{0, 3};
  static constexpr RegAddr PLL_PROGB_ADDR{0, 4};
  static constexpr RegAddr PLL_PROGC_ADDR{0, 5};
  static constexpr RegAddr PLL_PROGD_ADDR{0, 6};
  static constexpr RegAddr CODEC_DATAPATH_ADDR{0, 7};
  static constexpr RegAddr ASD_INTF_CTRLA_ADDR{0, 8};
  static constexpr RegAddr ASD_INTF_CTRLB_ADDR{0, 9};
  static constexpr RegAddr ASD_INTF_CTRLC_ADDR{0, 10};
  static constexpr RegAddr OVRF_STATUS_PLLR_ADDR{0, 11};
  static constexpr RegAddr DIGI_FILT_CTRL_ADDR{0, 12};
  static constexpr RegAddr HEADSET_BTN_PRESS_DETECTA_ADDR{0, 13};
  static constexpr RegAddr HEADSET_BTN_PRESS_DETECTB_ADDR{0, 14};
  static constexpr RegAddr LADC_VOL_ADDR{0, 15};
  static constexpr RegAddr RADC_VOL_ADDR{0, 16};
  static constexpr RegAddr MIC2_LADC_CTRL_ADDR{0, 17};
  static constexpr RegAddr MIC2_RADC_CTRL_ADDR{0, 18};
  static constexpr RegAddr LINE1L_LADC_CTRL_ADDR{0, 19};
  // 20 is reserved
  static constexpr RegAddr LINE1R_LADC_CTRL_ADDR{0, 21};
  static constexpr RegAddr LINE1R_RADC_CTRL_ADDR{0, 22};
  // 23 is reserved
  static constexpr RegAddr LINE1L_RADC_CTRL_ADDR{0, 24};
  static constexpr RegAddr MICBIAS_CTRL_ADDR{0, 25};
  static constexpr RegAddr LAGC_CTRLA_ADDR{0, 26};
  static constexpr RegAddr LAGC_CTRLB_ADDR{0, 27};
  static constexpr RegAddr LAGC_CTRLC_ADDR{0, 28};
  static constexpr RegAddr RAGC_CTRLA_ADDR{0, 29};
  static constexpr RegAddr RAGC_CTRLB_ADDR{0, 30};
  static constexpr RegAddr RAGC_CTRLC_ADDR{0, 31};
  static constexpr RegAddr LAGC_GAIN_ADDR{0, 32};
  static constexpr RegAddr RAGC_GAIN_ADDR{0, 33};
  static constexpr RegAddr LAGC_NGATE_DEBOUNCE_ADDR{0, 34};
  static constexpr RegAddr RAGC_NGATE_DEBOUNCE_ADDR{0, 35};
  static constexpr RegAddr ADC_FLAG_ADDR{0, 36};
  static constexpr RegAddr DAC_PWR_ADDR{0, 37};
  static constexpr RegAddr HPRCOM_CFG_ADDR{0, 38};
  // 39 is reserved
  static constexpr RegAddr HPCOM_CMVOLT_ADDR{0, 40};
  static constexpr RegAddr DAC_SW_CTRL_ADDR{0, 41};
  static constexpr RegAddr POP_REDUCTION_ADDR{0, 42};
  static constexpr RegAddr LDAC_VOL_ADDR{0, 43};
  static constexpr RegAddr RDAC_VOL_ADDR{0, 44};
  // 45 is reserved
  static constexpr RegAddr PGAL_HPLOUT_VOL_ADDR{0, 46};
  static constexpr RegAddr DACL1_HPLOUT_VOL_ADDR{0, 47};
  // 48 is reserved
  static constexpr RegAddr PGAR_HPLOUT_VOL_ADDR{0, 49};
  static constexpr RegAddr DACR1_HPLOUT_VOL_ADDR{0, 50};
  static constexpr RegAddr HPLOUT_CTRL_ADDR{0, 51};
  // 52 is reserved
  static constexpr RegAddr PGAL_HPLCOM_VOL_ADDR{0, 53};
  static constexpr RegAddr DACL1_HPLCOM_VOL_ADDR{0, 54};
  // 55 is reserved
  static constexpr RegAddr PGAR_HPLCOM_VOL_ADDR{0, 56};
  static constexpr RegAddr DACR1_HPLCOM_VOL_ADDR{0, 57};
  static constexpr RegAddr HPLCOM_CTRL_ADDR{0, 58};
  // 59 is reserved
  static constexpr RegAddr PGAL_HPROUT_VOL_ADDR{0, 60};
  static constexpr RegAddr DACL1_HPROUT_VOL_ADDR{0, 61};
  // 62 is reserved
  static constexpr RegAddr PGAR_HPROUT_VOL_ADDR{0, 63};
  static constexpr RegAddr DACR1_HPROUT_VOL_ADDR{0, 64};
  static constexpr RegAddr HPROUT_CTRL_ADDR{0, 65};
  // 66 is reserved
  static constexpr RegAddr PGAL_HPRCOM_VOL_ADDR{0, 67};
  static constexpr RegAddr DACL1_HPRCOM_VOL_ADDR{0, 68};
  // 69 is reserved
  static constexpr RegAddr PGAR_HPRCOM_VOL_ADDR{0, 70};
  static constexpr RegAddr DACR1_HPRCOM_VOL_ADDR{0, 71};
  static constexpr RegAddr HPRCOM_CTRL_ADDR{0, 72};
  // 73-80 are reserved
  static constexpr RegAddr PGAL_LLOPM_VOL_ADDR{0, 81};
  static constexpr RegAddr DACL1_LLOPM_VOL_ADDR{0, 82};
  // 83 is reserved
  static constexpr RegAddr PGAR_LLOPM_VOL_ADDR{0, 84};
  static constexpr RegAddr DACR1_LLOPM_VOL_ADDR{0, 85};
  static constexpr RegAddr LLOPM_CTRL_ADDR{0, 86};
  // 87 is reserved
  static constexpr RegAddr PGAL_RLOPM_VOL_ADDR{0, 88};
  static constexpr RegAddr DACL1_RLOPM_VOL_ADDR{0, 89};
  // 90 is reserved
  static constexpr RegAddr PGAR_RLOPM_VOL_ADDR{0, 91};
  static constexpr RegAddr DACR1_RLOPM_VOL_ADDR{0, 92};
  static constexpr RegAddr RLOPM_CTRL_ADDR{0, 93};
  static constexpr RegAddr POWER_STATUS_ADDR{0, 94};
  static constexpr RegAddr SHORT_DETECT_ADDR{0, 95};
  static constexpr RegAddr STICKY_IRQ_FLAGS_ADDR{0, 96};
  static constexpr RegAddr RT_IRQ_FLAGS_ADDR{0, 97};
  // 98-100 are reserved
  static constexpr RegAddr CLK_ADDR{0, 101};
  static constexpr RegAddr CLK_GEN_ADDR{0, 102};
  static constexpr RegAddr LAGC_PROG_ATTACK_ADDR{0, 103};
  static constexpr RegAddr LAGC_PROG_DECAY_ADDR{0, 104};
  static constexpr RegAddr RAGC_PROG_ATTACK_ADDR{0, 105};
  static constexpr RegAddr RAGC_PROG_DECAY_ADDR{0, 106};
  static constexpr RegAddr ADC_DIG_FILT_CTRL_ADDR{0, 107};
  static constexpr RegAddr I2C_BUS_STATUS_ADDR{0, 107};  // Yes, same address as above.
  static constexpr RegAddr ANALOG_BYPASS_CTRL_ADDR{0, 108};
  static constexpr RegAddr DAC_QUIESCENT_CURRENT_ADDR{0, 109};
  // 110-127 are reserved

  // ---- Page 1 registers ----
  //static constexpr RegAddr PAGE_SELECT_ADDR{1, 0};
  static constexpr RegAddr LCH_FX_N0_MSB_ADDR{1, 1};
  static constexpr RegAddr LCH_FX_N0_LSB_ADDR{1, 2};
  static constexpr RegAddr LCH_FX_N1_MSB_ADDR{1, 3};
  static constexpr RegAddr LCH_FX_N1_LSB_ADDR{1, 4};
  static constexpr RegAddr LCH_FX_N2_MSB_ADDR{1, 5};
  static constexpr RegAddr LCH_FX_N2_LSB_ADDR{1, 6};
  static constexpr RegAddr LCH_FX_N3_MSB_ADDR{1, 7};
  static constexpr RegAddr LCH_FX_N3_LSB_ADDR{1, 8};
  static constexpr RegAddr LCH_FX_N4_MSB_ADDR{1, 9};
  static constexpr RegAddr LCH_FX_N4_LSB_ADDR{1, 10};
  static constexpr RegAddr LCH_FX_N5_MSB_ADDR{1, 11};
  static constexpr RegAddr LCH_FX_N5_LSB_ADDR{1, 12};
  static constexpr RegAddr LCH_FX_D1_MSB_ADDR{1, 13};
  static constexpr RegAddr LCH_FX_D1_LSB_ADDR{1, 14};
  static constexpr RegAddr LCH_FX_D2_MSB_ADDR{1, 15};
  static constexpr RegAddr LCH_FX_D2_LSB_ADDR{1, 16};
  static constexpr RegAddr LCH_FX_D4_MSB_ADDR{1, 17};
  static constexpr RegAddr LCH_FX_D4_LSB_ADDR{1, 18};
  static constexpr RegAddr LCH_FX_D5_MSB_ADDR{1, 19};
  static constexpr RegAddr LCH_FX_D5_LSB_ADDR{1, 20};
  static constexpr RegAddr LCH_DEEMP_N0_MSB_ADDR{1, 21};
  static constexpr RegAddr LCH_DEEMP_N0_LSB_ADDR{1, 22};
  static constexpr RegAddr LCH_DEEMP_N1_MSB_ADDR{1, 23};
  static constexpr RegAddr LCH_DEEMP_N1_LSB_ADDR{1, 24};
  static constexpr RegAddr LCH_DEEMP_D0_MSB_ADDR{1, 25};
  static constexpr RegAddr LCH_DEEMP_D0_LSB_ADDR{1, 26};
  static constexpr RegAddr RCH_FX_N0_MSB_ADDR{1, 27};
  static constexpr RegAddr RCH_FX_N0_LSB_ADDR{1, 28};
  static constexpr RegAddr RCH_FX_N1_MSB_ADDR{1, 29};
  static constexpr RegAddr RCH_FX_N1_LSB_ADDR{1, 30};
  static constexpr RegAddr RCH_FX_N2_MSB_ADDR{1, 31};
  static constexpr RegAddr RCH_FX_N2_LSB_ADDR{1, 32};
  static constexpr RegAddr RCH_FX_N3_MSB_ADDR{1, 33};
  static constexpr RegAddr RCH_FX_N3_LSB_ADDR{1, 34};
  static constexpr RegAddr RCH_FX_N4_MSB_ADDR{1, 35};
  static constexpr RegAddr RCH_FX_N4_LSB_ADDR{1, 36};
  static constexpr RegAddr RCH_FX_N5_MSB_ADDR{1, 37};
  static constexpr RegAddr RCH_FX_N5_LSB_ADDR{1, 38};
  static constexpr RegAddr RCH_FX_D1_MSB_ADDR{1, 39};
  static constexpr RegAddr RCH_FX_D1_LSB_ADDR{1, 40};
  static constexpr RegAddr RCH_FX_D2_MSB_ADDR{1, 41};
  static constexpr RegAddr RCH_FX_D2_LSB_ADDR{1, 42};
  static constexpr RegAddr RCH_FX_D4_MSB_ADDR{1, 43};
  static constexpr RegAddr RCH_FX_D4_LSB_ADDR{1, 44};
  static constexpr RegAddr RCH_FX_D5_MSB_ADDR{1, 45};
  static constexpr RegAddr RCH_FX_D5_LSB_ADDR{1, 46};
  static constexpr RegAddr RCH_DEEMP_N0_MSB_ADDR{1, 47};
  static constexpr RegAddr RCH_DEEMP_N0_LSB_ADDR{1, 48};
  static constexpr RegAddr RCH_DEEMP_N1_MSB_ADDR{1, 49};
  static constexpr RegAddr RCH_DEEMP_N1_LSB_ADDR{1, 50};
  static constexpr RegAddr RCH_DEEMP_D0_MSB_ADDR{1, 51};
  static constexpr RegAddr RCH_DEEMP_D0_LSB_ADDR{1, 52};
  static constexpr RegAddr D3D_ATTEN_MSB{1, 53};
  static constexpr RegAddr D3D_ATTEN_LSB{1, 54};
  // 55-64 are reserved
  static constexpr RegAddr LCH_ADC_HPF_N0_MSB_ADDR{1, 65};
  static constexpr RegAddr LCH_ADC_HPF_N0_LSB_ADDR{1, 66};
  static constexpr RegAddr LCH_ADC_HPF_N1_MSB_ADDR{1, 67};
  static constexpr RegAddr LCH_ADC_HPF_N1_LSB_ADDR{1, 68};
  static constexpr RegAddr LCH_ADC_HPF_D1_MSB_ADDR{1, 69};
  static constexpr RegAddr LCH_ADC_HPF_D1_LSB_ADDR{1, 70};
  static constexpr RegAddr RCH_ADC_HPF_N0_MSB_ADDR{1, 71};
  static constexpr RegAddr RCH_ADC_HPF_N0_LSB_ADDR{1, 72};
  static constexpr RegAddr RCH_ADC_HPF_N1_MSB_ADDR{1, 73};
  static constexpr RegAddr RCH_ADC_HPF_N1_LSB_ADDR{1, 74};
  static constexpr RegAddr RCH_ADC_HPF_D1_MSB_ADDR{1, 75};
  static constexpr RegAddr RCH_ADC_HPF_D1_LSB_ADDR{1, 76};
  // 77-127 are reserved

  // WHEW!

  // ---- Bit definitions ----
  // Codec sample rate select register (NCODEC = NADC = NDAC on this device)
  static constexpr uint8_t NCODEC_1 = 0x0;
  static constexpr uint8_t NCODEC_1_5 = 0x1;
  static constexpr uint8_t NCODEC_2 = 0x2;
  static constexpr uint8_t NCODEC_2_5 = 0x3;
  static constexpr uint8_t NCODEC_3 = 0x4;
  static constexpr uint8_t NCODEC_3_5 = 0x5;
  static constexpr uint8_t NCODEC_4 = 0x6;
  static constexpr uint8_t NCODEC_4_5 = 0x7;
  static constexpr uint8_t NCODEC_5 = 0x8;
  static constexpr uint8_t NCODEC_5_5 = 0x9;
  static constexpr uint8_t NCODEC_6 = 0xA;

  // PLL registers need some mapping.
  /// PLL Q register code: Q=16 -> 0000, Q=17 -> 0001, Q=2..15 -> value itself
  static uint8_t PLL_Q_CODE(uint8_t q) {
    if (q == 16) return 0;
    if (q == 17) return 1;
    return q & 0x0F;
  }
  /// PLL P register code: P=8 -> 000, P=1..7 -> value itself
  static uint8_t PLL_P_CODE(uint8_t p) { return (p == 8) ? 0 : p & 0x07; }

  /// PLL R value lives in OVRF_STATUS_AND_PLLR_ADDR, D3-D0 (code = R - 1)
  static uint8_t PLL_R_CODE(uint8_t r) { return (r==16) ? 0 : r & 0x0F; }

  // Codec datapath setup register (7)
  static constexpr uint8_t FSREF_44100 = makeBit(7, 1);
  static constexpr uint8_t FSREF_48000 = 0;
  static constexpr uint8_t DUAL_RATE_MODE = (makeBit(6, 1)) | (makeBit(5, 1));
  static constexpr uint8_t LDAC2LCH = makeBit(3, 1);
  static constexpr uint8_t RDAC2RCH = makeBit(1, 1);

  // Audio serial data interface control register B (9)
  static constexpr uint8_t AIC3104_IFTYPE_I2S = makeBits(7, 6, 0b00);
  static constexpr uint8_t AIC3104_IFTYPE_DSP = makeBits(7, 6, 0b01);
  static constexpr uint8_t AIC3104_IFTYPE_RJF = makeBits(7, 6, 0b10);
  static constexpr uint8_t AIC3104_IFTYPE_LJF = makeBits(7, 6, 0b11);

  static constexpr uint8_t ASD_WLEN_16 = makeBits(5, 4, 0b00);
  static constexpr uint8_t ASD_WLEN_20 = makeBits(5, 4, 0b01);
  static constexpr uint8_t ASD_WLEN_24 = makeBits(5, 4, 0b10);
  static constexpr uint8_t ASD_WLEN_32 = makeBits(5, 4, 0b11);
  
  // Generic route/mute/power bits, reused across many of the mixer /
  // output-driver registers on this device (matches the pattern used by
  // the vendor Linux driver for this codec family)
  static constexpr uint8_t ROUTE_ON = makeBit(7, 1);
  static constexpr uint8_t DAC_UNMUTE = makeBit(7, 1);
  static constexpr uint8_t HP_UNMUTE = makeBit(3, 1);
  static constexpr uint8_t OUT_PWR_ON = makeBit(0, 1);
  static constexpr uint8_t ADC_PWR_ON = makeBit(2, 1);

  static constexpr uint8_t LDAC_PWR_ON = makeBit(7, 1);
  static constexpr uint8_t RDAC_PWR_ON = makeBit(6, 1);

  /// Route-volume field: bit7 = ROUTE_ON, bits6-0 = attenuation code
  /// (0 = 0 dB .. 127 = -63.5 dB in 0.5 dB steps)
  static uint8_t ROUTE_VOL(uint8_t atten) {
    return (uint8_t)(ROUTE_ON | makeBits(6, 0, atten));
  }

  /// Output volume range, in 0.5dB steps (-63.5dB .. 0dB)
  static constexpr int CODEC_OUTPUT_VOLUME_MAX = 0;
  static constexpr int CODEC_OUTPUT_VOLUME_MIN = -127;

  /// PLL divider settings for a given MCLK and fS(ref) (44.1kHz or 48kHz
  /// "reference" rate family); R is fixed at 1 for all table entries,
  /// per the common-case values given in the datasheet's Table 10-1.
  struct PllDivs {
    uint32_t clk;
    uint32_t fsref;  ///< 44100 or 48000
    uint8_t p;
    uint8_t r;
    uint8_t j;
    uint16_t d;
  };

  /// Maps an actual sample rate onto an fS(ref) family (44.1kHz or
  /// 48kHz) plus the NCODEC divider (and dual-rate flag) needed to reach
  /// it from that reference, per datasheet section 10.3.3.
  struct RateDivs {
    uint32_t rate;
    uint32_t fsref;
    uint8_t ncodec_code;
    bool dual_rate;
  };

  TLV320AIC3104() { i2c_addr = I2C_ADDR; }

  /// Initializes the codec for I2S with the given sample rate and bits per sample
  bool begin(uint32_t sample_rate, uint8_t bits, codec_mode_t mode,
             i2s_format_t fmt, bool is_master, uint8_t channels) override {
    (void)mode;
    (void)fmt;
    return begin(sample_rate, bits, channels, is_master);
  }

  bool begin(uint32_t sample_rate = 44100, uint8_t word_size = 16, uint8_t channels = 2, bool is_master = false, uint32_t mclk = 0) {
    AD_LOGI("TLV320AIC3104 begin()");
    AD_LOGD("rate:%d  size:%d  channels:%d  master:%s  mclk:%d", sample_rate, word_size, channels, is_master?"true":"false", mclk);

    bool rc = true;
    rc &= softReset();
    delayMs(10);
    rc &= configureClocks(sample_rate, word_size, channels, mclk, is_master);
    rc &= configureDai(word_size);
    rc &= configureInput();
    rc &= configureOutput();
    return rc;
  }

  /// Soft reset of the codec
  bool softReset() { 
    AD_LOGI("TLV320AIC3104 softReset()");
    return writePagedReg(SOFT_RESET_ADDR, makeBit(7, 1));
  }

  /// Configure the digital audio interface (I2S) word size and clock direction
  bool configureDai(uint8_t word_size) {
    AD_LOGI("TLV320AIC3104 configureDai()");
    bool rc = writePagedReg(ASD_INTF_CTRLA_ADDR, makeBit(5, 1)); // BCLK, WCLK are inputs, DOUT is Hi-Z

    uint8_t b = AIC3104_IFTYPE_I2S;  // The library doesn't support any other interface types yet.
    switch (word_size) {
      case 16: b |= ASD_WLEN_16; break;
      case 20: b |= ASD_WLEN_20; break;
      case 24: b |= ASD_WLEN_24; break;
      case 32: b |= ASD_WLEN_32; break;
      default: return false;
    }
    b |= makeBits(2, 0, 0b111); // Resync DAC, ADC, with soft-mute
    rc &= writePagedReg(ASD_INTF_CTRLB_ADDR, b);
    rc &= writePagedReg(ASD_INTF_CTRLC_ADDR, 0x00);  // No offset.
    return rc;
  }


  /**
   * @brief Configure the PLL for the given MCLK/fS(ref) combination, then
   * set the shared ADC/DAC NCODEC divider (and dual-rate mode) needed to
   * reach the requested sample rate from that fS(ref).
   */
  bool configureClocks(uint32_t sample_rate, uint8_t word_size, uint8_t channels, uint32_t mclk, bool is_master) {
    AD_LOGI("TLV320AIC3104 configureClocks() rate:%d  mclk:%d  is_master:%s", sample_rate, mclk, is_master?"true":"false");

    // Are we using MCLK or BCLK to derive our local clock?
    bool use_bclk = (mclk == 0);
    uint32_t clock = use_bclk ? (sample_rate * word_size * channels) : mclk;
    AD_LOGI("MCLK: %d  Using %s at %dHz", mclk, use_bclk?"BCLK":"MCLK", clock);

    // Find the appropriate PLL and other register parameters for our clockrate.
    size_t rate_count = 0;
    const RateDivs* rate_table = rateDivTable(rate_count);
    const RateDivs* rate_entry = nullptr;
    for (size_t i = 0; i < rate_count; i++) {
      if (rate_table[i].rate == sample_rate) {
        rate_entry = &rate_table[i];
        break;
      }
    }
    if (rate_entry == nullptr) {
      AD_LOGE("TLV320AIC3104::configureClocks() No rate_entry found");
      return false;
    }
    AD_LOGI("Rate Table entry found: fSref:%d  NCODEC:%d", rate_entry->fsref, rate_entry->ncodec_code);

    size_t pll_count = 0;
    const PllDivs* pll_table = pllDivTable(pll_count);
    const PllDivs* pll_entry = nullptr;
    for (size_t i = 0; i < pll_count; i++) {
      if (pll_table[i].clk == clock && pll_table[i].fsref == rate_entry->fsref) {
        pll_entry = &pll_table[i];
        break;
      }
    }
    if (pll_entry == nullptr) {
      AD_LOGE("TLV320AIC3104::configureClocks() No pll_entry found. clock:%d  fSref:%d", clock, rate_entry->fsref);
      return false;
    }
    AD_LOGI("PLL Div entry found: D:%d  J:%d  P:%d  R:%d", pll_entry->d, pll_entry->j, pll_entry->p, pll_entry->r);

    bool rc = true;
  
    // D7 is PLL enable. D6-3 is Q. D2-0 is P
    uint8_t r3 = makeBit(7, 0b1) | makeBits(6, 3, (PLL_Q_CODE(2))) | makeBits(2, 0, PLL_P_CODE(pll_entry->p));
    uint8_t r8 = makeBits(7, 6, is_master?0b11:0b00);  // BCLK and LRCLK set to input (slave) or output (master)
    // D0 is CODEC_CLKIN: 0 is PLL, 1 is CLKDIV
    uint8_t r101 = makeBit(0, 0b0);                     // PLLDIV_OUT 
    uint8_t r102 = makeBits(7, 6, (use_bclk)?0b10:0b00) // CLKDIV_IN src. 0=MCLK, 2=BCLK.
                 | makeBits(5, 4, (use_bclk)?0b10:0b00) // PLLCLK_IN src. 0=MCLK, 2=BCLK.
                 | makeBits(3, 0, 0b0010);  // reserved bits, required to be this value.
    rc &= writePagedReg(PLL_PROGB_ADDR, makeBits(7, 2, pll_entry->j));
    rc &= writePagedReg(PLL_PROGC_ADDR, makeBits(7, 0, pll_entry->d >> 6));
    rc &= writePagedReg(PLL_PROGD_ADDR, makeBits(7, 2, pll_entry->d));
    rc &= writePagedReg(OVRF_STATUS_PLLR_ADDR, makeBits(3, 0, PLL_R_CODE(pll_entry->r)));
    rc &= writePagedReg(ASD_INTF_CTRLA_ADDR, r8);
    rc &= writePagedReg(CLK_ADDR, r101);
    rc &= writePagedReg(CLK_GEN_ADDR, r102);
    rc &= writePagedReg(PLL_PROGA_ADDR, r3);
    rc &= writePagedReg(ASD_INTF_CTRLA_ADDR, makeBit(5, 1));  // Hi-z on idle

    // fS(ref) selection + shared NCODEC divider + dual-rate mode
    uint8_t datapath = (rate_entry->fsref == 44100) ? FSREF_44100 : FSREF_48000;
    datapath |= LDAC2LCH | RDAC2RCH;
    if (rate_entry->dual_rate) datapath |= DUAL_RATE_MODE;
    rc &= writePagedReg(CODEC_DATAPATH_ADDR, datapath);
    rc &= writePagedReg(SAMPLE_RATE_SEL_ADDR, 
      makeBits(7, 4, rate_entry->ncodec_code) | // ADC Sample Rate Select
      makeBits(3, 0, rate_entry->ncodec_code)   // DAC Sample Rate Select
    );
    return rc;

  }

  /// Power up the DAC channels
  bool startOutput() {
    return updatePagedReg(DAC_PWR_ADDR, (uint8_t)(LDAC_PWR_ON | RDAC_PWR_ON),
                           (uint8_t)(LDAC_PWR_ON | RDAC_PWR_ON));
  }

  /// Power down the DAC channels
  bool stopOutput() {
    return updatePagedReg(DAC_PWR_ADDR, (uint8_t)(LDAC_PWR_ON | RDAC_PWR_ON), 0);
  }

  /**
   * @brief Configure the headphone (HPLOUT/HPROUT) and/or fully
   * differential line (LOP/M) output paths, depending on the output
   * device selected via setDevices(): power up the DAC, route the DAC
   * output to the selected mixer(s) at default (0dB) volume, unmute and
   * power up the corresponding output drivers.
   */
  bool configureOutput() {
    AD_LOGI("TLV320AIC3104 configureOutput()");
    bool rc = true;
    bool hp = output_device == DAC_OUTPUT_LINE1 || output_device == DAC_OUTPUT_ALL;
    bool line = output_device == DAC_OUTPUT_LINE2 || output_device == DAC_OUTPUT_ALL;

    // DAC_QUIESCENT_CURRENT_ADDR
    //rc &= writePagedReg(DAC_QUIESCENT_CURRENT_ADDR, 0x40); // 50% increase in DAC current.

    // Setup DACs, and HP outputs
    rc &= writePagedReg(HEADSET_BTN_PRESS_DETECTB_ADDR, makeBit(7, 1));  // Sets HP for AC coupled.
    rc &= writePagedReg(DAC_PWR_ADDR, makeBit(7, 1) | makeBit(6, 1) | makeBits(5, 4, 0b10));  // Both DACs on, HPLCOM independent
    rc &= writePagedReg(HPRCOM_CFG_ADDR, makeBits(5, 3, 0b010) | makeBits(2, 1, 0b10)); //HPRCOM independent, Short circuit protection enabled, current limit.
    //rc &= writePagedReg(DAC_SW_CTRL_ADDR, 0x00); // Defaults to: LDAC to DAC_L1, RDAC to DAC_R1, independent volumes
    rc &= writePagedReg(HPCOM_CMVOLT_ADDR, makeBits(7, 6, 0b11)); // 1.8v common mode, soft step
    rc &= writePagedReg(POP_REDUCTION_ADDR, makeBits(7, 4, 0b1001) | makeBits(3, 2, 0b01) | makeBit(1, 1)); // 800ms power-on time, 1ms ramp-up step, use band-gap for common mode reference

    // Mute while making other changes
    rc &= setOutputMute(true, AIC3104Channel::All);
    rc &= setDACMute(false);

    // route DAC_L1/DAC_R1 to the headhpones output mixer at 0dB
    rc &= writePagedReg(DACL1_HPLOUT_VOL_ADDR, ROUTE_VOL(0));
    rc &= writePagedReg(DACR1_HPROUT_VOL_ADDR, ROUTE_VOL(0));

    // route DAC_L1/DAC_R1 to the fully differential line output mixer at 0dB 
    rc &= writePagedReg(DACL1_LLOPM_VOL_ADDR, ROUTE_VOL(0));
    rc &= writePagedReg(DACR1_RLOPM_VOL_ADDR, ROUTE_VOL(0));

    // power up/down the output drivers to match the requested devices
    rc &= updatePagedReg(HPLOUT_CTRL_ADDR, OUT_PWR_ON, hp ? OUT_PWR_ON : 0);
    rc &= updatePagedReg(HPROUT_CTRL_ADDR, OUT_PWR_ON, hp ? OUT_PWR_ON : 0);
    rc &= updatePagedReg(LLOPM_CTRL_ADDR, OUT_PWR_ON, line ? OUT_PWR_ON : 0);
    rc &= updatePagedReg(RLOPM_CTRL_ADDR, OUT_PWR_ON, line ? OUT_PWR_ON : 0);

    // Unmute selected outputs
    rc &= setOutputMute(!hp, AIC3104Channel::HeadphoneLeft);
    rc &= setOutputMute(!hp, AIC3104Channel::HeadphoneRight);
    rc &= setOutputMute(!line, AIC3104Channel::LineOutLeft);
    rc &= setOutputMute(!line, AIC3104Channel::LineOutRight);

    return rc;
  }

  /**
   * @brief Configure the microphone/line input path to the 
   * Left/Right ADC PGA at 0dB and power up the ADC channels,
   * set the microphone bias, and set the ADC PGA to its default gain, 
   * unmuted.
   * ADC_INPUT_DIFFERENCE: Balanced inputs on Line1/Mic1, with Mic gain.
   * ADC_INPUT_LINE1: Single ended inputs on Line1/Mic1, Line leve.
   * ADC_INPUT_LINE2: Single ended inputs on Line2/Mic2, Line level
   * ADC_INPUT_LINE3: Single ended inputs on Line2/Mic2, Mic gain.
   * ADC_INPUT_ALL: Singled ended line inputs on Line1, Single ended mono Mic input on Mic2, Line2 off.
   * No cross-mixing (eg: left to right)
   */
  bool configureInput() {
    AD_LOGI("TLV320AIC3104 configureInput()");
    bool rc = true;

    // Default to powered up, but muted
    uint8_t left1_reg = makeBits(6, 3, 0b1111) | makeBit(2, 1);   // 19
    uint8_t right1_reg = makeBits(6, 3, 0b1111) | makeBit(2, 1);  // 22
    uint8_t left2_reg = 0xFF;   // 17
    uint8_t right2_reg = 0xFF;  // 18
    uint8_t pga_gain_reg = 0;   // 15, and 16
    switch (input_device) {
      case ADC_INPUT_DIFFERENCE:  // Balanced inputs on Line1/Mic1, with Mic gain and bias.
        left1_reg = 
          makeBit(7, 1) |      // Fully differential input
          makeBits(6, 3, 0) |  // Input pad: -0dB
          makeBit(2, 1) |     // ADC powered up
          makeBits(1, 0, 0);    // ADC PGA soft stepping, 1/sample
        right1_reg = left1_reg;
        pga_gain_reg = 
          makeBit(7, 0) |             // Not muted
          makeBits(6, 0, 0b0110000);  // 24dB PGA Gain about right for a microphone
        break;
      case ADC_INPUT_LINE1:    // Single ended inputs on Line1/Mic1, Line level.
        left1_reg = 
          makeBit(7, 0) |      // Single ended input
          makeBits(6, 3, 0) |  // Input pad: -0dB
          makeBit(2, 1) |     // ADC powered up
          makeBits(1, 0, 0);    // ADC PGA soft stepping, 1/sample
        right1_reg = left1_reg;
        pga_gain_reg = 
          makeBit(7, 0) |             // Not muted
          makeBits(6, 0, 0b0000000);  // 0dB PGA Gain for line level input
        break;
      case ADC_INPUT_LINE2:    // ADC_INPUT_LINE2: Single ended inputs on Line2/Mic2, Line level
        left2_reg = 
          makeBits(7, 4, 0b0000) | // Mic2L/Line2L ---> Left ADC
          makeBits(3, 0, 0b1111);  // Mic2R/Line2R -X-> Left ADC
        right2_reg =
          makeBits(7, 4, 0b1111) | // Mic2L/Line2L -X-> Right ADC
          makeBits(3, 0, 0b0000);  // Mic2R/Line2R ---> Right ADC
        pga_gain_reg = 
          makeBit(7, 0) |             // Not muted
          makeBits(6, 0, 0b0000000);  // 0dB PGA Gain for line level input
        break;
      case ADC_INPUT_LINE3:  // ADC_INPUT_LINE3: Single ended inputs on Line2/Mic2, Mic gain and bias.
        left2_reg = 
          makeBits(7, 4, 0b0000) | // Mic2L/Line2L ---> Left ADC
          makeBits(3, 0, 0b1111);  // Mic2R/Line2R -X-> Left ADC
        right2_reg =
          makeBits(7, 4, 0b1111) | // Mic2L/Line2L -X-> Right ADC
          makeBits(3, 0, 0b0000);  // Mic2R/Line2R ---> Right ADC
        pga_gain_reg = 
          makeBit(7, 0) |             // Not muted
          makeBits(6, 0, 0b0110000);  // 24dB PGA Gain about right for a microphone
          break;
        case ADC_INPUT_ALL: // Singled ended line inputs on Line1, Single ended mono Mic input on Mic2, Line2 off.
        default: 
        left1_reg = 
          makeBit(7, 0) |      // Single ended input
          makeBits(6, 3, 0b1000) |  // Input pad: -12dB to equalize Line Input with Mic gain, a little anyway.
          makeBit(2, 1) |     // ADC powered up
          makeBits(1, 0, 0);    // ADC PGA soft stepping, 1/sample
        right1_reg = left1_reg;
        left2_reg = 
          makeBits(7, 4, 0b0000) | // Mic2L/Line2L ---> Left ADC
          makeBits(3, 0, 0b0000);  // Mic2R/Line2R ---> Left ADC
        right2_reg =
          makeBits(7, 4, 0b1111) | // Mic2L/Line2L -X-> Right ADC
          makeBits(3, 0, 0b1111);  // Mic2R/Line2R -X-> Right ADC
        pga_gain_reg = 
          makeBit(7, 0) |             // Not muted
          makeBits(6, 0, 0b0110000);  // 24dB PGA Gain about right for a microphone
          break;
    }
    rc &= writePagedReg(LINE1L_LADC_CTRL_ADDR, left1_reg);
    rc &= writePagedReg(LINE1R_RADC_CTRL_ADDR, right1_reg);
    rc &= writePagedReg(MIC2_LADC_CTRL_ADDR, left2_reg);
    rc &= writePagedReg(MIC2_RADC_CTRL_ADDR, right2_reg);

    rc &= writePagedReg(MICBIAS_CTRL_ADDR, makeBits(7, 6, 0b10));  // Bias 2.5v

    rc &= writePagedReg(LADC_VOL_ADDR, pga_gain_reg);
    rc &= writePagedReg(RADC_VOL_ADDR, pga_gain_reg);
    return rc;
  }

  /**
   * @brief Set the output (headphone / line out) volume.
   *
   * @param vol volume in 0.5dB steps, range CODEC_OUTPUT_VOLUME_MIN (-127,
   * i.e. -63.5dB) .. CODEC_OUTPUT_VOLUME_MAX (0, i.e. 0dB)
   * @param channel target channel(s)
   */
  bool setOutputVolume(int vol, AIC3104Channel channel = AIC3104Channel::All) {
    if (vol > CODEC_OUTPUT_VOLUME_MAX || vol < CODEC_OUTPUT_VOLUME_MIN) return false;
    uint8_t atten = (uint8_t)(-vol);  ///< 0 (0dB) .. 127 (-63.5dB)

    switch (channel) {
      case AIC3104Channel::HeadphoneLeft:
        return writePagedReg(DACL1_HPLOUT_VOL_ADDR, ROUTE_VOL(atten));
      case AIC3104Channel::HeadphoneRight:
        return writePagedReg(DACR1_HPROUT_VOL_ADDR, ROUTE_VOL(atten));
      case AIC3104Channel::LineOutLeft:
        return writePagedReg(DACL1_LLOPM_VOL_ADDR, ROUTE_VOL(atten));
      case AIC3104Channel::LineOutRight:
        return writePagedReg(DACR1_RLOPM_VOL_ADDR, ROUTE_VOL(atten));
      case AIC3104Channel::All: {
        bool rc = true;
        rc &= writePagedReg(DACL1_HPLOUT_VOL_ADDR, ROUTE_VOL(atten));
        rc &= writePagedReg(DACR1_HPROUT_VOL_ADDR, ROUTE_VOL(atten));
        rc &= writePagedReg(DACL1_LLOPM_VOL_ADDR, ROUTE_VOL(atten));
        rc &= writePagedReg(DACR1_RLOPM_VOL_ADDR, ROUTE_VOL(atten));
        return rc;
      }
      default:
        return false;
    }
  }

  bool setDACMute(bool mute) {
    // makeBit(7, 1) is muted, makeBit(7, 0) is unmuted
    uint8_t dac_val = mute ? DAC_UNMUTE : 0;
    bool rc = true;
    rc &= updatePagedReg(LDAC_VOL_ADDR, DAC_UNMUTE, dac_val);
    rc &= updatePagedReg(RDAC_VOL_ADDR, DAC_UNMUTE, dac_val);
    return rc;
  }

  /// Mute / unmute the headphone and/or line output driver(s)
  bool setOutputMute(bool mute, AIC3104Channel channel = AIC3104Channel::All) {
    uint8_t hp_val = mute ? 0 : HP_UNMUTE;
    bool rc = true;
    switch (channel) {
      case AIC3104Channel::HeadphoneLeft:
        rc &= updatePagedReg(HPLOUT_CTRL_ADDR, HP_UNMUTE, hp_val);
        return rc;
      case AIC3104Channel::HeadphoneRight:
        rc &= updatePagedReg(HPROUT_CTRL_ADDR, HP_UNMUTE, hp_val);
        return rc;
      case AIC3104Channel::LineOutLeft:
        rc &= updatePagedReg(LLOPM_CTRL_ADDR, HP_UNMUTE, hp_val);
        return rc;
      case AIC3104Channel::LineOutRight:
        rc &= updatePagedReg(RLOPM_CTRL_ADDR, HP_UNMUTE, hp_val);
        return rc;
      case AIC3104Channel::All: {
        rc &= updatePagedReg(HPLOUT_CTRL_ADDR, HP_UNMUTE, hp_val);
        rc &= updatePagedReg(HPROUT_CTRL_ADDR, HP_UNMUTE, hp_val);
        rc &= updatePagedReg(LLOPM_CTRL_ADDR, HP_UNMUTE, hp_val);
        rc &= updatePagedReg(RLOPM_CTRL_ADDR, HP_UNMUTE, hp_val);
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
    return setOutputVolume(vol, AIC3104Channel::All);
  }

  /// Mutes/unmutes all outputs
  bool setMute(bool mute) override { return setOutputMute(mute, AIC3104Channel::All); }

  /// Stores the output device selection for use by configureOutput()
  bool setDevices(input_device_t input_device, output_device_t output_device) override {
    this->input_device = input_device;
    this->output_device = output_device;
    return true;
  }

  // The *PagedReg() methods are public to allow for more complex
  // configuration of the chip.  It's capable of a lot that is not implemented
  // generally by this driver.

  /// Writes a register on the given page
  bool writePagedReg(RegAddr reg, uint8_t value) {
    if (!selectPage(reg.page)) return false;
    return writeReg(reg.reg, value);
  }

  /// Reads a register on the given page
  bool readPagedReg(RegAddr reg, uint8_t& value) {
    if (!selectPage(reg.page)) return false;
    // https://github.com/palmerr23/tlv320AIC3104/blob/2d53fc2c95a07494dad87241c47c2086245e0c8b/src/tlv320aic3104_comms.h#L91
    return readReg(reg.reg, value, false); // stopBit = false, or TLV will enter auto-increment mode and return value of reg+1
    // Requires mods to:
    // * ZephyrDriverCommon.h readReg()
    // * API_I2C.h i2c_bus_read_bytes()
    // Both need to take a bool for an optional stopBit, and pass it to p_wire->endTransmission(stopBit)
  }

  /// Read-Modify-Write of a register on the given page
  bool updatePagedReg(RegAddr reg, uint8_t mask, uint8_t value) {
    uint8_t old = 0;
    if (!readPagedReg(reg, old)) return false;
    uint8_t updated = (old & ~mask) | (value & mask);
    if (updated == old) return true;
    return writePagedReg(reg, updated);
  }

protected:
  uint8_t current_page = 0xFF;  ///< cached page, invalid initially

  /// Output and input device selections set via setDevices(), 
  // used by configureOutput() and configureInputs()
  output_device_t output_device = DAC_OUTPUT_ALL;
  input_device_t input_device = ADC_INPUT_ALL;

  /// Selects the active register page (writes register 0 of page 0)
  bool selectPage(uint8_t page) {
    if (current_page == page) return true;
    uint8_t reg = PAGE_CONTROL_ADDR;
    if (!writeReg(reg, page)) return false;
    // pg47, 10.5.2. Recommended to read the page register back after writing.
    uint8_t new_page = 0xFF;
    // Ignoring errors. I don't want to succeed in write, but not update current_page because 
    // of a read error. new_page will EITHER be the new page (the read) actually succeeded, 
    // or it'll still be 0xFF which will trigger an attempt to write again next time.
    readReg(reg, new_page); 
    current_page = new_page;
    return true;
  }

  static constexpr uint8_t PAGE_CONTROL_ADDR = 0;

  /// PLL divider table indexed by (mclk, fsref), values taken from
  /// datasheet Table 10-1 "Typical MCLK Rates" (R = 1 in all cases)
  static const PllDivs* pllDivTable(size_t& count) {
    static const PllDivs table[] = {
        /* clk     fsref  p  r   j   d */
        {352800,   44100, 1, 16, 16, 0}, // BCLK 11025x16x2, 256
        {705600,   44100, 1, 16, 8,  0}, // BCLK 22050x16x2, 128
        {1411200,  44100, 1, 16, 4,  0}, // BCLK 44100x16x2, 64
        {2822400,  44100, 1, 1,  32, 0},
        {5644800,  44100, 1, 1,  16, 0},
        {11289600, 44100, 1, 1,  8,  0},     //44.1*256
        {12000000, 44100, 1, 1,  7,  5264},
        {13000000, 44100, 1, 1,  6,  9474},
        {16000000, 44100, 1, 1,  5,  6448},
        {19200000, 44100, 1, 1,  4,  7040},
        {19680000, 44100, 1, 1,  4,  5893},
        {48000000, 44100, 4, 1,  7,  5264},

        {256000,   48000, 1, 16, 24, 0},  // BCLK  8000x16x2,  384
        {512000,   48000, 1, 16, 12, 0},  // BCLK  16000x16x2, 192
        {768000,   48000, 1, 16, 8,  0},  // BCLK  24000x16x2, 128
        {1024000,  48000, 1, 16, 6,  0},  // BCLK  32000x16x2, 96
        {1536000,  48000, 1, 16, 4,  0},  // BCLK  48000x16x2, 64
        {2048000,  48000, 1, 1,  48, 0},
        {3072000,  48000, 1, 1,  32, 0},
        {4096000,  48000, 1, 1,  24, 0},
        {6144000,  48000, 1, 1,  16, 0},
        {8192000,  48000, 1, 1,  12, 0},
        {12000000, 48000, 1, 1,  8,  1920},
        {12288000, 48000, 1, 1,  6,  0},     //48*256
        {13000000, 48000, 1, 1,  7,  5618},
        {16000000, 48000, 1, 1,  6,  1440},
        {19200000, 48000, 1, 1,  5,  1200},
        {19680000, 48000, 1, 1,  4,  9951},
        {48000000, 48000, 4, 1,  8,  1920},
    };
    count = sizeof(table) / sizeof(table[0]);
    return table;
  }

  /// Maps a sample rate to its fS(ref) family and NCODEC divider, per
  /// datasheet section 10.3.3 ("Audio Data Converters")
  static const RateDivs* rateDivTable(size_t& count) {
    static const RateDivs table[] = {
        //rate  fsref  ncodec_code  dual_rate
        {8000,  48000, NCODEC_6, false},
        {11025, 44100, NCODEC_4, false},
        {16000, 48000, NCODEC_3, false},
        {22050, 44100, NCODEC_2, false},
        {24000, 48000, NCODEC_2, false},
        {32000, 48000, NCODEC_1_5, false},
        {44100, 44100, NCODEC_1, false},
        {48000, 48000, NCODEC_1, false},
        {88200, 44100, NCODEC_1, true},
        {96000, 48000, NCODEC_1, true},
    };
    count = sizeof(table) / sizeof(table[0]);
    return table;
  }

#ifdef TLV320AIC3104_DEBUG
public:
  // Debugging stuff

  // Start and end are inclusive. Doesn't matter whether you count from 
  // the left or the right, it will do the right thing.
  uint32_t extractBits(uint32_t val, uint8_t _start, uint8_t _end) {
    uint8_t start = (_start > _end) ? _start : _end;   // max()
    uint8_t end = (_start < _end) ? _start : _end;     // min()
    return ((val) & ~(0xFFFFFFFF << start+1)) >> end;
  }
  uint32_t extractBit(uint32_t val, uint8_t _start) {
    return extractBits(val, _start, _start);
  }

  void dumpRegisters() {
    AD_LOGI("Dumping Registers...");
    AD_LOGI("[");
    uint8_t val = 0;
    bool ret;
    RegAddr regaddr;
    regaddr.page = 0;
    for (uint8_t reg = 2; reg <= 109; reg++) {
      regaddr.reg = reg;
      ret = readPagedReg(regaddr, val);
      AD_LOGI("   { \"reg\": %d, \"val\": %d }%s", reg, val, reg==109?"":",");
    }
    AD_LOGI("]");
  }

  void printPLLConfig() {
    uint8_t pll_a, pll_b, pll_c, pll_d, pll_r, datapath, rate, clk_sel;

    readPagedReg(PLL_PROGA_ADDR, pll_a);
    readPagedReg(PLL_PROGB_ADDR, pll_b);
    readPagedReg(PLL_PROGC_ADDR, pll_c);
    readPagedReg(PLL_PROGD_ADDR, pll_d);
    readPagedReg(OVRF_STATUS_PLLR_ADDR, pll_r);
    readPagedReg(CODEC_DATAPATH_ADDR, datapath);
    readPagedReg(SAMPLE_RATE_SEL_ADDR, rate);
    readPagedReg(CLK_GEN_ADDR, clk_sel);

    // test cases
    /*
    Serial.printf("%d == 3\r\n", extractBits(0xFF, 7, 6));
    Serial.printf("%d == 3\r\n", extractBits(0xFF, 1, 0));
    Serial.printf("%d == 255\r\n", extractBits(0xFF, 7, 0));
    Serial.printf("%d == 0\r\n", extractBits(0xFC, 1, 0));
    Serial.printf("%d == 0\r\n", extractBits(0xF3, 3, 2));
    Serial.printf("%d == 1\r\n", extractBit(0x08, 3));
    Serial.printf("%d == 0\r\n", extractBit(0x08, 2));
    Serial.printf("%d == 0\r\n", extractBit(0x08, 4));
    */

    AD_LOGI("printPLLConfig():");
    AD_LOGI("Raw registers:");
    AD_LOGI("PLL A:%02X  B:%02X  C:%02X  D:%02X  R:%02X\r\n",
      pll_a, pll_b, pll_c, pll_d, pll_r);
    AD_LOGI("CODEC Datapath:%02X  SampleRate:%02X  Clock Select:%02X\r\n",
      datapath, rate, clk_sel);
    
    // Decode
    uint8_t P = extractBits(pll_a, 2, 0);
    if (P < 1) P+= 8;
    uint8_t Q = extractBits(pll_a, 6, 3);
    if (Q < 2) Q += 16;
    uint8_t J = extractBits(pll_b, 7, 2);
    uint16_t D = ((uint16_t)pll_c << 6) + extractBits(pll_d, 7, 2);
    uint8_t R = extractBits(pll_r, 3, 0);
    const char *pll_enabled = extractBit(pll_a, 7) ? "en" : "dis";
    const char *fSref = extractBit(datapath, 7) ? "44.1kHz" : "48kHz";
    const char *ADC_DR = extractBit(datapath, 6) ? "en" : "dis";
    const char *DAC_DR = extractBit(datapath, 5) ? "en" : "dis";
    uint8_t LDAC_DP = extractBits(datapath, 4, 3);
    uint8_t RDAC_DP = extractBits(datapath, 2, 1);
    uint8_t NADC = extractBits(rate, 7, 4);
    uint8_t NDAC = extractBits(rate, 3, 0);

    AD_LOGI("P:%d  Q:%d  J:%d  D:%d  R:%d\r\n", P, Q, J, D, R);
    AD_LOGI("PLL:%sabled  fS(ref):%s  ADC_DR:%s  DAC_DR:%s\r\n", pll_enabled, fSref, ADC_DR, DAC_DR);
    AD_LOGI("LDAC_DP:%d  RDAC_DP:%d  NADC:%d  NDAC:%d\r\n", LDAC_DP, RDAC_DP, NADC, NDAC);
  }
#endif // TLV320AIC3104_DEBUG
};

}  // namespace audio_driver
