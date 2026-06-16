#pragma once

#include "DriverCommon.h"
#include "Platforms/API_I2C.h"
#include "Platforms/Logger.h"
#include "stdint.h"


namespace audio_driver {

struct SrcAttr {
  int param;
  uint8_t val;
};

struct OsrAttr {
  int osr;
  uint8_t clk_src;
};

struct SRateAttr {
  int fs;
  uint8_t range;
  bool max;
  uint32_t mclk_src[3];
};

struct RegDefault {
  uint16_t reg;
  uint16_t val;
};

enum AudioI2SFormat { I2S_STD, LEFT_JUSTIFIED, RIGHT_JUSTIFIED, DSP_A, DSP_B };
enum ClockPolarity { NORMAL_BCLK, INVERTED_BCLK };
enum OversamplingMode { OSR_64 = 2, OSR_128 = 1, OSR_256 = 0, OSR_32 = 4 };

enum ClockRatio {
  NAU8325_MCLK_FS_RATIO_256 = 0,
  NAU8325_MCLK_FS_RATIO_400 = 1,
  NAU8325_MCLK_FS_RATIO_500 = 2,
  NAU8325_MCLK_FS_RATIO_NUM = 3
};

class PCBCUPID_NAU8325 {
 public:
  static constexpr uint16_t NAU8325_I2C_ADDR = 0x21;
  static constexpr uint16_t NAU8325_R00_HARDWARE_RST = 0x00;
  static constexpr uint16_t NAU8325_R01_SOFTWARE_RST = 0x01;
  static constexpr uint16_t NAU8325_R02_DEVICE_ID = 0x02;
  static constexpr uint32_t NAU8325_R03_CLK_CTRL = 0x03;
  static constexpr uint16_t NAU8325_R04_ENA_CTRL = 0x04;
  static constexpr uint16_t NAU8325_R05_INTERRUPT_CTRL = 0x05;
  static constexpr uint16_t NAU8325_R06_INT_CLR_STATUS = 0x06;
  static constexpr uint16_t NAU8325_R09_IRQOUT = 0x09;
  static constexpr uint16_t NAU8325_R0A_IO_CTRL = 0x0a;
  static constexpr uint16_t NAU8325_R0B_PDM_CTRL = 0x0b;
  static constexpr uint16_t NAU8325_R0C_TDM_CTRL = 0x0c;
  static constexpr uint16_t NAU8325_R0D_I2S_PCM_CTRL1 = 0x0d;
  static constexpr uint16_t NAU8325_R0E_I2S_PCM_CTRL2 = 0x0e;
  static constexpr uint16_t NAU8325_R0F_L_TIME_SLOT = 0x0f;
  static constexpr uint16_t NAU8325_R10_R_TIME_SLOT = 0x10;
  static constexpr uint16_t NAU8325_R11_HPF_CTRL = 0x11;
  static constexpr uint16_t NAU8325_R12_MUTE_CTRL = 0x12;
  static constexpr uint16_t NAU8325_R13_DAC_VOLUME = 0x13;
  static constexpr uint16_t NAU8325_R1D_DEBUG_READ1 = 0x1d;
  static constexpr uint16_t NAU8325_R1F_DEBUG_READ2 = 0x1f;
  static constexpr uint16_t NAU8325_R22_DEBUG_READ3 = 0x22;
  static constexpr uint16_t NAU8325_R29_DAC_CTRL1 = 0x29;
  static constexpr uint16_t NAU8325_R2A_DAC_CTRL2 = 0x2a;
  static constexpr uint16_t NAU8325_R2C_ALC_CTRL1 = 0x2c;
  static constexpr uint16_t NAU8325_R2D_ALC_CTRL2 = 0x2d;
  static constexpr uint16_t NAU8325_R2E_ALC_CTRL3 = 0x2e;
  static constexpr uint16_t NAU8325_R2F_ALC_CTRL4 = 0x2f;
  static constexpr uint32_t NAU8325_R40_CLK_DET_CTRL = 0x40;
  static constexpr uint16_t NAU8325_R49_TEST_STATUS = 0x49;
  static constexpr uint16_t NAU8325_R4A_ANALOG_READ = 0x4a;
  static constexpr uint16_t NAU8325_R50_MIXER_CTRL = 0x50;
  static constexpr uint16_t NAU8325_R55_MISC_CTRL = 0x55;
  static constexpr uint16_t NAU8325_R60_BIAS_ADJ = 0x60;
  static constexpr uint16_t NAU8325_R61_ANALOG_CONTROL_1 = 0x61;
  static constexpr uint16_t NAU8325_R62_ANALOG_CONTROL_2 = 0x62;
  static constexpr uint16_t NAU8325_R63_ANALOG_CONTROL_3 = 0x63;
  static constexpr uint16_t NAU8325_R64_ANALOG_CONTROL_4 = 0x64;
  static constexpr uint16_t NAU8325_R65_ANALOG_CONTROL_5 = 0x65;
  static constexpr uint16_t NAU8325_R66_ANALOG_CONTROL_6 = 0x66;
  static constexpr uint16_t NAU8325_R69_CLIP_CTRL = 0x69;
  static constexpr uint16_t NAU8325_R73_RDAC = 0x73;
  static constexpr uint32_t NAU8325_REG_MAX = NAU8325_R73_RDAC;
  static constexpr uint16_t NAU8325_VMIDEN_SFT = 0x03;
  static constexpr uint32_t NAU8325_REG_ADDR_LEN = 16;
  static constexpr uint32_t NAU8325_REG_DATA_LEN = 16;
  static constexpr uint32_t NAU8325_CLK_DAC_SRC_SFT = 12;
  static constexpr uint32_t NAU8325_CLK_DAC_SRC_MASK = (0x3 << NAU8325_CLK_DAC_SRC_SFT);
  static constexpr uint32_t NAU8325_CLK_MUL_SRC_SFT = 6;
  static constexpr uint32_t NAU8325_CLK_MUL_SRC_MASK = (0x3 << NAU8325_CLK_MUL_SRC_SFT);
  static constexpr uint32_t NAU8325_MCLK_SEL_SFT = 3;
  static constexpr uint32_t NAU8325_MCLK_SEL_MASK = (0x7 << NAU8325_MCLK_SEL_SFT);
  static constexpr uint32_t NAU8325_MCLK_SRC_MASK = 0x7;
  static constexpr uint16_t NAU8325_DAC_LEFT_CH_EN_SFT = 3;
  static constexpr uint16_t NAU8325_DAC_LEFT_CH_EN = (0x1 << NAU8325_DAC_LEFT_CH_EN_SFT);
  static constexpr uint16_t NAU8325_DAC_RIGHT_CH_EN_SFT = 2;
  static constexpr uint16_t NAU8325_DAC_RIGHT_CH_EN = (0x1 << NAU8325_DAC_RIGHT_CH_EN_SFT);
  static constexpr uint16_t NAU8325_ENA_CTRL_DACR_EN = 0x0008;
  static constexpr uint16_t NAU8325_ENA_CTRL_DACL_EN = 0x0004;
  static constexpr uint16_t NAU8325_ENA_CTRL_DAC_EN_MASK = 0x000C;
  static constexpr uint16_t NAU8325_ARP_DWN_INT_SFT = 12;
  static constexpr uint16_t NAU8325_ARP_DWN_INT_MASK = (0x1 << NAU8325_ARP_DWN_INT_SFT);
  static constexpr uint16_t NAU8325_CLIP_INT_SFT = 11;
  static constexpr uint16_t NAU8325_CLIP_INT_MASK = (0x1 << NAU8325_CLIP_INT_SFT);
  static constexpr uint16_t NAU8325_LVD_INT_SFT = 10;
  static constexpr uint16_t NAU8325_LVD_INT_MASK = (0x1 << NAU8325_LVD_INT_SFT);
  static constexpr uint16_t NAU8325_PWR_INT_DIS_SFT = 8;
  static constexpr uint16_t NAU8325_PWR_INT_DIS = (0x1 << NAU8325_PWR_INT_DIS_SFT);
  static constexpr uint16_t NAU8325_OCP_OTP_SHTDWN_INT_SFT = 4;
  static constexpr uint16_t NAU8325_OCP_OTP_SHTDWN_INT_MASK = (0x1 << NAU8325_OCP_OTP_SHTDWN_INT_SFT);
  static constexpr uint16_t NAU8325_CLIP_INT_DIS_SFT = 3;
  static constexpr uint16_t NAU8325_CLIP_INT_DIS = (0x1 << NAU8325_CLIP_INT_DIS_SFT);
  static constexpr uint16_t NAU8325_LVD_INT_DIS_SFT = 2;
  static constexpr uint16_t NAU8325_LVD_INT_DIS = (0x1 << NAU8325_LVD_INT_DIS_SFT);
  static constexpr uint16_t NAU8325_PWR_INT_MASK = 0x1;
  static constexpr uint16_t NAU8325_INT_STATUS_MASK = 0x7f;
  static constexpr uint16_t NAU8325_IRQOUT_SEL_SEF = 14;
  static constexpr uint16_t NAU8325_IRQOUT_SEL_MASK = (0x3 << NAU8325_IRQOUT_SEL_SEF);
  static constexpr uint16_t NAU8325_DEM_DITH_SFT = 13;
  static constexpr uint16_t NAU8325_DEM_DITH_EN = (0x1 << NAU8325_DEM_DITH_SFT);
  static constexpr uint16_t NAU8325_GAINZI3_SFT = 4;
  static constexpr uint16_t NAU8325_GAINZI3_MASK = (0x1 << NAU8325_GAINZI3_SFT);
  static constexpr uint16_t NAU8325_GAINZI2_MASK = 0x7;
  static constexpr uint16_t NAU8325_IRQ_PL_SFT = 7;
  static constexpr uint16_t NAU8325_IRQ_PL_ACT_HIGH = (0x1 << NAU8325_IRQ_PL_SFT);
  static constexpr uint16_t NAU8325_IRQ_PS_SFT = 6;
  static constexpr uint16_t NAU8325_IRQ_PS_UP = (0x1 << NAU8325_IRQ_PS_SFT);
  static constexpr uint16_t NAU8325_IRQ_PE_SFT = 5;
  static constexpr uint16_t NAU8325_IRQ_PE_EN = (0x1 << NAU8325_IRQ_PE_SFT);
  static constexpr uint16_t NAU8325_IRQ_DS_SFT = 4;
  static constexpr uint16_t NAU8325_IRQ_DS_HIGH = (0x1 << NAU8325_IRQ_DS_SFT);
  static constexpr uint16_t NAU8325_IRQ_OUTPUT_SFT = 3;
  static constexpr uint16_t NAU8325_IRQ_OUTPUT_EN = (0x1 << NAU8325_IRQ_OUTPUT_SFT);
  static constexpr uint16_t NAU8325_IRQ_PIN_DEBUG_SFT = 2;
  static constexpr uint16_t NAU8325_IRQ_PIN_DEBUG_EN = (0x1 << NAU8325_IRQ_PIN_DEBUG_SFT);
  static constexpr uint16_t NAU8325_PDM_LCH_EDGE_SFT = 1;
  static constexpr uint16_t NAU8325_PDM_LCH_EDGE__MASK = (0x1 << NAU8325_PDM_LCH_EDGE_SFT);
  static constexpr uint16_t NAU8325_PDM_MODE_EN = 0x1;
  static constexpr uint16_t NAU8325_TDM_SFT = 15;
  static constexpr uint16_t NAU8325_TDM_EN = (0x1 << NAU8325_TDM_SFT);
  static constexpr uint16_t NAU8325_PCM_OFFSET_CTRL_SFT = 14;
  static constexpr uint16_t NAU8325_PCM_OFFSET_CTRL_EN = (0x1 << NAU8325_PCM_OFFSET_CTRL_SFT);
  static constexpr uint16_t NAU8325_DAC_LEFT_SFT = 6;
  static constexpr uint16_t NAU8325_NAU8325_DAC_LEFT_MASK = (0x7 << NAU8325_DAC_LEFT_SFT);
  static constexpr uint16_t NAU8325_DAC_RIGHT_SFT = 3;
  static constexpr uint16_t NAU8325_DAC_RIGHT_MASK = (0x7 << NAU8325_DAC_RIGHT_SFT);
  static constexpr uint16_t NAU8325_DACCM_CTL_SFT = 14;
  static constexpr uint16_t NAU8325_DACCM_CTL_MASK = (0x3 << NAU8325_DACCM_CTL_SFT);
  static constexpr uint16_t NAU8325_CMB8_0_SFT = 13;
  static constexpr uint16_t NAU8325_CMB8_0_MASK = (0x1 << NAU8325_CMB8_0_SFT);
  static constexpr uint16_t NAU8325_UA_OFFSET_SFT = 12;
  static constexpr uint16_t NAU8325_UA_OFFSET_MASK = (0x1 << NAU8325_UA_OFFSET_SFT);
  static constexpr uint16_t NAU8325_I2S_BP_SFT = 7;
  static constexpr uint16_t NAU8325_I2S_BP_MASK = (0x1 << NAU8325_I2S_BP_SFT);
  static constexpr uint16_t NAU8325_I2S_BP_INV = (0x1 << NAU8325_I2S_BP_SFT);
  static constexpr uint16_t NAU8325_I2S_PCMB_SFT = 6;
  static constexpr uint16_t NAU8325_I2S_PCMB_EN = (0x1 << NAU8325_I2S_PCMB_SFT);
  static constexpr uint16_t NAU8325_I2S_DACPSHS0_SFT = 4;
  static constexpr uint16_t NAU8325_I2S_DACPSHS0_MASK = (0x3 << NAU8325_I2S_DACPSHS0_SFT);
  static constexpr uint16_t NAU8325_I2S_DL_SFT = 2;
  static constexpr uint16_t NAU8325_I2S_DL_MASK = (0x3 << NAU8325_I2S_DL_SFT);
  static constexpr uint16_t NAU8325_I2S_DL_32 = (0x3 << NAU8325_I2S_DL_SFT);
  static constexpr uint16_t NAU8325_I2S_DL_24 = (0x2 << NAU8325_I2S_DL_SFT);
  static constexpr uint16_t NAU8325_I2S_DL_20 = (0x1 << NAU8325_I2S_DL_SFT);
  static constexpr uint16_t NAU8325_I2S_DL_16 = (0x0 << NAU8325_I2S_DL_SFT);
  static constexpr uint16_t NAU8325_I2S_DF_MASK = 0x3;
  static constexpr uint16_t NAU8325_I2S_DF_RIGTH = 0x0;
  static constexpr uint16_t NAU8325_I2S_DF_LEFT = 0x1;
  static constexpr uint16_t NAU8325_I2S_DF_I2S = 0x2;
  static constexpr uint16_t NAU8325_I2S_DF_PCM_AB = 0x3;
  static constexpr uint16_t NAU8325_PCM_TS_SFT = 10;
  static constexpr uint16_t NAU8325_PCM_TS_EN = (0x1 << NAU8325_PCM_TS_SFT);
  static constexpr uint16_t NAU8325_PCM8BIT0_SFT = 8;
  static constexpr uint16_t NAU8325_PCM8BIT0_MASK = (0x3 << NAU8325_PCM8BIT0_SFT);
  static constexpr uint16_t NAU8325_SHORT_FS_DET_SFT = 9;
  static constexpr uint16_t NAU8325_SHORT_FS_DET_DIS = (0x1 << NAU8325_SHORT_FS_DET_SFT);
  static constexpr uint16_t NAU8325_TSLOT_L0_MASK = 0x1ff;
  static constexpr uint16_t NAU8325_TSLOT_R0_MASK = 0x1ff;
  static constexpr uint16_t NAU8325_DAC_HPF_SFT = 15;
  static constexpr uint16_t NAU8325_DAC_HPF_EN = (0x1 << NAU8325_DAC_HPF_SFT);
  static constexpr uint16_t NAU8325_DAC_HPF_APP_SFT = 4;
  static constexpr uint16_t NAU8325_DAC_HPF_APP_MASK = (0x1 << NAU8325_DAC_HPF_APP_SFT);
  static constexpr uint32_t NAU8325_DAC_HPF_FCUT_SFT = 0;
  static constexpr uint16_t NAU8325_DAC_HPF_FCUT_MASK = (0xf << NAU8325_DAC_HPF_FCUT_SFT);
  static constexpr uint16_t NAU8325_SOFT_MUTE_SFT = 9;
  static constexpr uint16_t NAU8325_SOFT_MUTE = (0x1 << NAU8325_SOFT_MUTE_SFT);
  static constexpr uint16_t NAU8325_DAC_ZC_SFT = 8;
  static constexpr uint16_t NAU8325_DAC_ZC_EN = (0x1 << NAU8325_DAC_ZC_SFT);
  static constexpr uint16_t NAU8325_UNMUTE_CTL_SFT = 6;
  static constexpr uint16_t NAU8325_UNMUTE_CTL_MASK = (0x3 << NAU8325_UNMUTE_CTL_SFT);
  static constexpr uint16_t NAU8325_ANA_MUTE_SFT = 4;
  static constexpr uint16_t NAU8325_ANA_MUTE_MASK = (0x3 << NAU8325_ANA_MUTE_SFT);
  static constexpr uint16_t NAU8325_AUTO_MUTE_SFT = 2;
  static constexpr uint16_t NAU8325_AUTO_MUTE_DIS = (0x1 << NAU8325_AUTO_MUTE_SFT);
  static constexpr uint16_t NAU8325_DAC_VOLUME_L_SFT = 8;
  static constexpr uint16_t NAU8325_DAC_VOLUME_L_EN = (0xff << NAU8325_DAC_VOLUME_L_SFT);
  static constexpr uint32_t NAU8325_DAC_VOLUME_R_SFT = 0;
  static constexpr uint16_t NAU8325_DAC_VOLUME_R_EN = (0xff << NAU8325_DAC_VOLUME_R_SFT);
  static constexpr uint32_t NAU8325_DAC_VOL_MAX = 0xfe;
  static constexpr uint16_t NAU8325_OSR100_MASK = (0x1 << 7);
  static constexpr uint16_t NAU8325_MIPS500_MASK = (0x1 << 6);
  static constexpr uint16_t NAU8325_SHUTDWNDRVR_R_MASK = (0x1 << 5);
  static constexpr uint16_t NAU8325_SHUTDWNDRVR_L_MASK = (0x1 << 4);
  static constexpr uint16_t NAU8325_MUTEB_MASK = (0x1 << 3);
  static constexpr uint16_t NAU8325_PDOSCB_MASK = (0x1 << 2);
  static constexpr uint16_t NAU8325_POWERDOWN1B_D_MASK = (0x1 << 1);
  static constexpr uint16_t NAU8325_R_CHANNEL_Vol_SFT = 8;
  static constexpr uint16_t NAU8325_R_CHANNEL_Vol_MASK = (0x1f << NAU8325_R_CHANNEL_Vol_SFT);
  static constexpr uint16_t NAU8325_L_CHANNEL_Vol_MASK = 0x1f;
  static constexpr uint16_t NAU8325_PGAL_GAIN_MASK = (0x1f << 8);
  static constexpr uint16_t NAU8325_CLIP_MASK = (0x1 << 7);
  static constexpr uint16_t NAU8325_SCAN_MODE_MASK = (0x1 << 6);
  static constexpr uint16_t NAU8325_SDB_MASK = (0x1 << 5);
  static constexpr uint16_t NAU8325_TALARM_MASK = (0x1 << 4);
  static constexpr uint16_t NAU8325_SHORTR_MASK = (0x1 << 3);
  static constexpr uint16_t NAU8325_SHORTL_MASK = (0x1 << 2);
  static constexpr uint16_t NAU8325_TMDET_MASK = 0x3;
  static constexpr uint32_t NAU8325_DAC_OVERSAMPLE_SFT = 0;
  static constexpr uint16_t NAU8325_DAC_OVERSAMPLE_MASK = (0x7 << NAU8325_DAC_OVERSAMPLE_SFT);
  static constexpr uint16_t NAU8325_DAC_OVERSAMPLE_256 = (0x0 << NAU8325_DAC_OVERSAMPLE_SFT);
  static constexpr uint16_t NAU8325_DAC_OVERSAMPLE_128 = (0x1 << NAU8325_DAC_OVERSAMPLE_SFT);
  static constexpr uint16_t NAU8325_DAC_OVERSAMPLE_64 = (0x2 << NAU8325_DAC_OVERSAMPLE_SFT);
  static constexpr uint16_t NAU8325_DAC_OVERSAMPLE_32 = (0x4 << NAU8325_DAC_OVERSAMPLE_SFT);
  static constexpr uint16_t NAU8325_ALC_MAXGAIN_SFT = 8;
  static constexpr uint32_t NAU8325_ALC_MAXGAIN_MAX = 7;
  static constexpr uint16_t NAU8325_ALC_MAXGAIN_MASK = (0x7 << NAU8325_ALC_MAXGAIN_SFT);
  static constexpr uint32_t NAU8325_ALC_MINGAIN_MAX = 7;
  static constexpr uint32_t NAU8325_ALC_MINGAIN_SFT = 4;
  static constexpr uint32_t NAU8325_ALC_MINGAIN_MASK = (0x7 << NAU8325_ALC_MINGAIN_SFT);
  static constexpr uint16_t NAU8325_ALC_DCY_SFT = 12;
  static constexpr uint32_t NAU8325_ALC_DCY_MAX = 0xf;
  static constexpr uint16_t NAU8325_ALC_DCY_MASK = (0xf << NAU8325_ALC_DCY_SFT);
  static constexpr uint16_t NAU8325_ALC_ATK_SFT = 8;
  static constexpr uint32_t NAU8325_ALC_ATK_MAX = 0xf;
  static constexpr uint16_t NAU8325_ALC_ATK_MASK = (0xf << NAU8325_ALC_ATK_SFT);
  static constexpr uint16_t NAU8325_ALC_HLD_SFT = 4;
  static constexpr uint32_t NAU8325_ALC_HLD_MAX = 0xf;
  static constexpr uint16_t NAU8325_ALC_HLD_MASK = (0xf << NAU8325_ALC_HLD_SFT);
  static constexpr uint32_t NAU8325_ALC_LVL_SFT = 0;
  static constexpr uint32_t NAU8325_ALC_LVL_MAX = 0xf;
  static constexpr uint16_t NAU8325_ALC_LVL_MASK = (0xf << NAU8325_ALC_LVL_SFT);
  static constexpr uint16_t NAU8325_ALC_EN_SFT = 15;
  static constexpr uint16_t NAU8325_ALC_EN = (0x1 << NAU8325_ALC_EN_SFT);
  static constexpr uint16_t NAU8325_TEMP_COMP_ACT2_MASK = (0x1 << 5);
  static constexpr uint16_t NAU8325_LPF_IN1_EN_SFT = 15;
  static constexpr uint16_t NAU8325_LPF_IN1_EN = (0x1 << NAU8325_LPF_IN1_EN_SFT);
  static constexpr uint16_t NAU8325_LPF_IN1_TC_SFT = 12;
  static constexpr uint16_t NAU8325_LPF_IN1_TC_MASK = (0x7 << NAU8325_LPF_IN1_TC_SFT);
  static constexpr uint16_t NAU8325_LPF_IN2_EN_SFT = 11;
  static constexpr uint16_t NAU8325_LPF_IN2_EN = (0x1 << NAU8325_LPF_IN2_EN_SFT);
  static constexpr uint16_t NAU8325_LPF_IN2_TC_SFT = 8;
  static constexpr uint16_t NAU8325_LPF_IN2_TC_MASK = (0x7 << NAU8325_LPF_IN2_TC_SFT);
  static constexpr uint16_t NAU8325_APWRUP_SFT = 15;
  static constexpr uint16_t NAU8325_APWRUP_EN = (0x1 << NAU8325_APWRUP_SFT);
  static constexpr uint32_t NAU8325_CLKPWRUP_SFT = 14;
  static constexpr uint32_t NAU8325_CLKPWRUP_DIS = (0x1 << NAU8325_CLKPWRUP_SFT);
  static constexpr uint16_t NAU8325_PWRUP_DFT_SFT = 13;
  static constexpr uint16_t NAU8325_PWRUP_DFT = (0x1 << NAU8325_PWRUP_DFT_SFT);
  static constexpr uint16_t NAU8325_REG_SRATE_SFT = 10;
  static constexpr uint16_t NAU8325_REG_SRATE_MASK = (0x7 << NAU8325_REG_SRATE_SFT);
  static constexpr uint16_t NAU8325_REG_ALT_SRATE_SFT = 9;
  static constexpr uint16_t NAU8325_REG_ALT_SRATE_EN = (0x1 << NAU8325_REG_ALT_SRATE_SFT);
  static constexpr uint32_t NAU8325_REG_DIV_MAX = (0x1 << 8);
  static constexpr uint16_t NAU8325_BIAS_VMID_SEL_SFT = 4;
  static constexpr uint16_t NAU8325_BIAS_VMID_SEL_MASK = (0x3 << NAU8325_BIAS_VMID_SEL_SFT);
  static constexpr uint16_t NAU8325_VMDFSTENB_SFT = 14;
  static constexpr uint16_t NAU8325_VMDFSTENB_MASK = (0x3 << NAU8325_VMDFSTENB_SFT);
  static constexpr uint16_t NAU8325_CLASSDEN_SFT = 12;
  static constexpr uint16_t NAU8325_CLASSDEN_MASK = (0x3 << NAU8325_CLASSDEN_SFT);
  static constexpr uint32_t NAU8325_DACCLKEN_R_SFT = 10;
  static constexpr uint32_t NAU8325_DACCLKEN_R_MASK = (0x3 << NAU8325_DACCLKEN_R_SFT);
  static constexpr uint16_t NAU8325_DACEN_R_SFT = 8;
  static constexpr uint16_t NAU8325_DACEN_R_MASK = (0x3 << NAU8325_DACEN_R_SFT);
  static constexpr uint32_t NAU8325_DACCLKEN_SFT = 6;
  static constexpr uint32_t NAU8325_DACCLKEN_MASK = (0x3 << NAU8325_DACCLKEN_SFT);
  static constexpr uint16_t NAU8325_DACEN_SFT = 4;
  static constexpr uint16_t NAU8325_DACEN_MASK = (0x3 << NAU8325_DACEN_SFT);
  static constexpr uint16_t NAU8325_BIASEN_SFT = 2;
  static constexpr uint16_t NAU8325_BIASEN_MASK = (0x3 << NAU8325_BIASEN_SFT);
  static constexpr uint16_t NAU8325_VMIDEN_MASK = 0x3;
  static constexpr uint16_t NAU8325_ANALOG_CTRL1_VMID_EN = (0x3 << 0);
  static constexpr uint16_t NAU8325_ANALOG_CTRL1_CLASSD_EN = (0x3 << 12);
  static constexpr uint16_t NAU8325_ANALOG_CTRL1_EN_MASK = (NAU8325_ANALOG_CTRL1_VMID_EN | NAU8325_ANALOG_CTRL1_CLASSD_EN);
  static constexpr uint16_t NAU8325_PWMMOD_SFT = 14;
  static constexpr uint16_t NAU8325_PWMMOD_MASK = (0x3 << NAU8325_PWMMOD_SFT);
  static constexpr uint16_t NAU8325_DACTEST_SFT = 8;
  static constexpr uint16_t NAU8325_DACTEST_MASK = (0x7 << NAU8325_DACTEST_SFT);
  static constexpr uint16_t NAU8325_DACREFCAP_SFT = 6;
  static constexpr uint16_t NAU8325_DACREFCAP_MASK = (0x3 << NAU8325_DACREFCAP_SFT);
  static constexpr uint16_t NAU8325_POWER_DOWN_L_SFT = 8;
  static constexpr uint16_t NAU8325_POWER_DOWN_L_MASK = (0x3 << NAU8325_POWER_DOWN_L_SFT);
  static constexpr uint16_t NAU8325_POWER_DOWN_R_SFT = 6;
  static constexpr uint16_t NAU8325_POWER_DOWN_R_MASK = (0x3 << NAU8325_POWER_DOWN_R_SFT);
  static constexpr uint16_t NAU8325_CLASSD_FINE_SFT = 3;
  static constexpr uint16_t NAU8325_CLASSD_FINE_MASK = (0x7 << NAU8325_CLASSD_FINE_SFT);
  static constexpr uint16_t NAU8325_CLASSD_COARSE_GAIN_MASK = 0x7;
  static constexpr uint16_t NAU8325_CLASSD_OCPN_SFT = 8;
  static constexpr uint16_t NAU8325_CLASSD_OCPN_MASK = (0xf << NAU8325_CLASSD_OCPN_SFT);
  static constexpr uint16_t NAU8325_CLASSD_OCPP_SFT = 4;
  static constexpr uint16_t NAU8325_CLASSD_OCPP_MASK = (0xf << NAU8325_CLASSD_OCPP_SFT);
  static constexpr uint16_t NAU8325_CLASSD_SLEWN_MASK = 0xf;
  static constexpr uint32_t NAU8325_MCLK_RANGE_SFT = 15;
  static constexpr uint32_t NAU8325_MCLK_RANGE_EN = (0x1 << NAU8325_MCLK_RANGE_SFT);
  static constexpr uint32_t NAU8325_MCLK8XEN_SFT = 4;
  static constexpr uint32_t NAU8325_MCLK8XEN_EN = (0x1 << NAU8325_MCLK8XEN_SFT);
  static constexpr uint32_t NAU8325_MCLK4XEN_EN = (0x1 << 3);
  static constexpr uint16_t NAU8325_VBATLOW_SFT = 14;
  static constexpr uint16_t NAU8325_VBATLOW_MASK = (0x3 << NAU8325_VBATLOW_SFT);
  static constexpr uint16_t NAU8325_VDDSPK_LIM_SFT = 8;
  static constexpr uint16_t NAU8325_VDDSPK_LIM_EN = (0x1 << NAU8325_VDDSPK_LIM_SFT);
  static constexpr uint16_t NAU8325_VDDSPK_LIM_MASK = (0x7 << 9);
  static constexpr uint16_t NAU8325_ANALOG_CTRL6_SPKL_EN = (0x1 << 2);
  static constexpr uint16_t NAU8325_ANALOG_CTRL6_SPKR_EN = (0x1 << 1);
  static constexpr uint16_t NAU8325_ANALOG_CTRL6_EN_MASK = (NAU8325_ANALOG_CTRL6_SPKL_EN | NAU8325_ANALOG_CTRL6_SPKR_EN);
  static constexpr uint16_t NAU8325_ANTI_CLIP_SFT = 15;
  static constexpr uint16_t NAU8325_ANTI_CLIP_EN = (0x1 << NAU8325_ANTI_CLIP_SFT);
  static constexpr uint32_t NAU8325_CLK_DAC_DELAY_SFT = 4;
  static constexpr uint32_t NAU8325_CLK_DAC_DELAY_EN = (0xf << NAU8325_CLK_DAC_DELAY_SFT);
  static constexpr uint16_t NAU8325_DACVREFSEL_SFT = 2;
  static constexpr uint16_t NAU8325_DACVREFSEL_MASK = (0x3 << NAU8325_DACVREFSEL_SFT);
  static constexpr uint32_t CLK_DA_AD_MAX = 6144000;
  static constexpr uint32_t MASTER_CLK_MIN = 2048000;
  static constexpr uint32_t MASTER_CLK_MAX = 49152000;
  PCBCUPID_NAU8325(i2c_bus_handle_t wire);

  static const RegDefault reg_defaults[];

  // Static constant lookup tables (C++17 inline static)
  inline static const SrcAttr mclk_n1_div[] = {
      {1, 0x0}, {2, 0x1}, {3, 0x2}};

  inline static const SrcAttr mclk_n2_div[] = {
      {0, 0x0}, {1, 0x1}, {2, 0x2}, {3, 0x3}, {4, 0x4}};

  inline static const SrcAttr mclk_n3_mult[] = {
      {0, 0x1}, {1, 0x2}, {2, 0x3}, {3, 0x4}};

  inline static const OsrAttr osr_dac_sel[] = {
      {64, 2},   // OSR 64, SRC 1/4
      {256, 0},  // OSR 256, SRC 1
      {128, 1},  // OSR 128, SRC 1/2
      {0, 0},
      {32, 3}    // OSR 32, SRC 1/8
  };

  inline static const SRateAttr target_srate_table[] = {
      {48000, 2, true,  {12288000, 19200000, 24000000}},
      {16000, 1, false, {4096000,  6400000,  8000000}},
      {8000,  0, false, {2048000,  3200000,  4000000}},
      {44100, 2, true,  {11289600, 17640000, 22050000}},
      {64000, 3, false, {16384000, 25600000, 32000000}},
      {96000, 3, true,  {24576000, 38400000, 48000000}},
      {12000, 0, true,  {3072000,  4800000,  6000000}},
      {24000, 1, true,  {6144000,  9600000,  12000000}},
      {32000, 2, false, {8192000,  12800000, 16000000}}};

  /*
    @brief
    fs- frame rate in Hz (e.g., 8000, 16000, 44100, 48000)
    bits_per_sample- bits per sample (e.g., 16, 24)
    ratio- MCLK to FS ratio (e.g., 256, 384, 512, 640)
    @return
    true if initialization is successful, false otherwise
    @note
    The begin() function initializes the NAU8325 codec with the specified
    frame rate, bits per sample, and MCLK to FS ratio. It configures the codec
    for audio playback and sets up the necessary registers. The function returns
    true if the initialization is successful, or false if there is an error.
    The default values for bits_per_sample and ratio are 16 and 256,
    respectively. If only fs is provided, it uses the default values for
    bits_per_sample and ratio. If no parameters are provided, it uses default
    values for fs, bits_per_sample, and ratio. Example: PCBCUPID_NAU8325 codec;
    codec.begin(44100, 16, 256); // Initialize with 44.1kHz, 16 bits, and
    MCLK/FS ratio of 256 codec.begin(16000, 24); // Initialize with 16kHz, 24
    bits, and default MCLK/FS ratio of 256 codec.begin(); // Initialize with
    default values (e.g., 8000Hz, 16 bits, 256 ratio)
    @note
    The begin() function must be called before using the codec for audio
    playback. It sets up the codec's internal registers and prepares it for
    audio processing. If the initialization fails, the codec will not function
    correctly, and audio playback may not work as expected.
  */

  bool begin(uint32_t fs, uint8_t bits_per_sample, uint16_t ratio);
  bool begin(uint32_t fs, uint8_t bits_per_sample);
  bool begin();

  uint16_t readDeviceId();
  void resetChip();

  void softMute(bool enable);
  void powerOn();
  void powerOff();

  void setPowerUpDefault(bool enable);
  void setOversampling(OversamplingMode mode);
  void setVolume(uint8_t left, uint8_t right);  // public

 private:
  i2c_bus_handle_t i2c;
  uint8_t i2c_addr = NAU8325_I2C_ADDR;
  uint32_t fs;
  uint32_t mclk;

  bool clock_detection;

  void initRegisters();  // Initialize codec

  bool setSysClock(uint32_t freq);  // setup MCLK for I2S
  bool configureAudio(uint32_t fs, uint32_t mclk, uint8_t bits_per_sample);
  bool chooseClockSource(int fs, int mclk, const SRateAttr*& srate, int& n1_sel,
                         int& mult_sel, int& n2_sel);
  bool configureClocks(int fs, int mclk);
  bool applySampleRateClocks(const SRateAttr* srate, int n1_sel,
                             int mclk_mult_sel, int n2_sel);
  int getMclkRatioAndN2Index(const SRateAttr* srate, int mclk_hz,
                             int& n2_sel_out);
  const SRateAttr* getSRateAttr(int fs);
  const OsrAttr* getCurrentOSR();
  bool setI2SFormat(AudioI2SFormat format, ClockPolarity polarity);

  void enableDAPMBlocks();  // analog path power enable

  // I2C register controls
  bool readRegister(uint16_t reg, uint16_t& value);
  bool writeRegister(uint16_t reg, uint16_t value);
  bool writeRegisterBits(uint16_t reg, uint16_t mask, uint16_t value);
};

// Namespace-level state variables (static for internal linkage per TU)
static uint16_t set_ratio = 0;  // Clock ratio for enabling the I2S device
static bool alc_enable = true;
static bool clock_detection = true;
static bool clock_det_data = true;
static uint32_t dac_vref_microvolt = 2880000;
static uint32_t vref_impedance_ohms = 125000;

// ---------------------------------------------------------------------------
// Method implementations
// ---------------------------------------------------------------------------

inline PCBCUPID_NAU8325::PCBCUPID_NAU8325(i2c_bus_handle_t wire)
    : i2c(wire), i2c_addr(NAU8325_I2C_ADDR) {}

inline bool PCBCUPID_NAU8325::begin(uint32_t fs, uint8_t bits_per_sample,
                                    uint16_t ratio) {
  AD_LOGI("[NAU8325] beginDynamic() starting...");
  AD_LOGI("  fs = %lu Hz", fs);
  AD_LOGI("  ratio = %u Hz", ratio);
  AD_LOGI("  bits = %u", bits_per_sample);

  if (bits_per_sample != 16 && bits_per_sample != 24 && bits_per_sample != 32) {
    AD_LOGI("[NAU8325] Unsupported bit width: %u\n", bits_per_sample);
    return false;
  }

  set_ratio = ratio;
  uint32_t mclk = fs * ratio;
  resetChip();
  delayMs(5);

  if (!setSysClock(mclk)) return false;

  if (!configureAudio(fs, mclk, bits_per_sample)) return false;

  if (!setI2SFormat(I2S_STD, NORMAL_BCLK)) return false;

  // ***********Analog + ALC + Vref Config ***********
  initRegisters();

  // Wait for analog to settle
  delayMs(50);

  // Enable Analog/Output Blocks (DAPM Equivalent)
  enableDAPMBlocks();

  // Set default volume AFTER analog path is ready
  setVolume(0xFD, 0xFD);

  softMute(false);

  /*default powerOn*/
  powerOn();

  AD_LOGI("[NAU8325] beginDynamic() complete - sound should play");
  return true;
}

/*If the user didn't provide the ratio it will call this function with default
 * ratio*/
inline bool PCBCUPID_NAU8325::begin(uint32_t fs, uint8_t bits) {
  return begin(fs, bits,
               256);  // Passing 256 as the default ratio for clock division
}

/*if the user didn't pass any of the parameter like fs,bits,ratio*/
inline bool PCBCUPID_NAU8325::begin() {
  return begin(44100, 16, 256);  // FS - 44100, BitRate - 16, Ratio 256
}

inline void PCBCUPID_NAU8325::enableDAPMBlocks() {
  // Enable DAC L/R in ENA_CTRL (R04)
  writeRegisterBits(NAU8325_R04_ENA_CTRL, NAU8325_ENA_CTRL_DAC_EN_MASK,
                    NAU8325_ENA_CTRL_DAC_EN_MASK);

  // Enable analog power-up blocks (e.g., VMID, Class-D) in ANALOG_CONTROL_1
  // (R61)
  writeRegisterBits(NAU8325_R61_ANALOG_CONTROL_1, NAU8325_ANALOG_CTRL1_EN_MASK,
                    NAU8325_ANALOG_CTRL1_EN_MASK);

  // Enable speaker path routing (ANALOG_CONTROL_6 R66)
  writeRegisterBits(NAU8325_R66_ANALOG_CONTROL_6, NAU8325_ANALOG_CTRL6_EN_MASK,
                    NAU8325_ANALOG_CTRL6_EN_MASK);
}

inline void PCBCUPID_NAU8325::resetChip() {
  writeRegister(0x0000, 0x0001);
  delayMs(2);
  writeRegister(0x0000, 0x0000);
  delayMs(2);
}

inline uint16_t PCBCUPID_NAU8325::readDeviceId() {
  uint16_t id;
  if (readRegister(NAU8325_R02_DEVICE_ID, id)) {
    return id;
  } else {
    return 0xFFFF;  // Invalid or failed read
  }
}

inline bool PCBCUPID_NAU8325::applySampleRateClocks(const SRateAttr* srate,
                                                    int n1_sel,
                                                    int mclk_mult_sel,
                                                    int n2_sel) {
  if (!srate || n1_sel < 0 || n1_sel >= 3 || n2_sel < 0 || n2_sel >= 5) {
    AD_LOGI("[NAU8325] Invalid N1/N2 divider index.");
    return false;
  }

  // --- Debug ---
  AD_LOGI("--- Debugging clock dividers ---");
  AD_LOGI("n1_sel=%d (val=%d)", n1_sel, mclk_n1_div[n1_sel].val);
  AD_LOGI("n2_sel=%d (val=%d)", n2_sel, mclk_n2_div[n2_sel].val);
  AD_LOGI("mult_sel=%d", mclk_mult_sel);

  // Set sample rate range and max mode
  writeRegisterBits(NAU8325_R40_CLK_DET_CTRL,
                    NAU8325_REG_SRATE_MASK | NAU8325_REG_DIV_MAX,
                    (srate->range << NAU8325_REG_SRATE_SFT) |
                        (srate->max ? NAU8325_REG_DIV_MAX : 0));
  // Batch update CLK_CTRL (0x03)
  uint16_t clk_ctrl_val = 0;
  clk_ctrl_val |= mclk_n2_div[n2_sel].val;  // MCLK_SRC
  clk_ctrl_val |= mclk_n1_div[n1_sel].val << NAU8325_CLK_MUL_SRC_SFT;  // N1
  if (mclk_mult_sel >= 0 && mclk_mult_sel <= 3)
    clk_ctrl_val |= mclk_n3_mult[mclk_mult_sel].val
                    << NAU8325_MCLK_SEL_SFT;  // N3

  clk_ctrl_val |= 0x1000;  //  DAC_CLK = MCLK/2 (required!)

  writeRegister(NAU8325_R03_CLK_CTRL, clk_ctrl_val);

  writeRegister(NAU8325_R03_CLK_CTRL, clk_ctrl_val);  // Now written only once

  // Configure ANALOG_CONTROL_5 for 4x/8x clock enable
  switch (mclk_mult_sel) {
    case 2:
      writeRegisterBits(NAU8325_R65_ANALOG_CONTROL_5, NAU8325_MCLK4XEN_EN,
                        NAU8325_MCLK4XEN_EN);
      break;
    case 3:
      writeRegisterBits(NAU8325_R65_ANALOG_CONTROL_5,
                        NAU8325_MCLK4XEN_EN | NAU8325_MCLK8XEN_EN,
                        NAU8325_MCLK4XEN_EN | NAU8325_MCLK8XEN_EN);
      break;
    default:
      writeRegisterBits(NAU8325_R65_ANALOG_CONTROL_5,
                        NAU8325_MCLK4XEN_EN | NAU8325_MCLK8XEN_EN, 0);
      break;
  }

  return true;
}

inline int PCBCUPID_NAU8325::getMclkRatioAndN2Index(const SRateAttr* srate,
                                                    int mclk_hz,
                                                    int& n2_sel_out) {
  int ratio = NAU8325_MCLK_FS_RATIO_NUM;  // Default: not found

  for (int i = 0; i < 5; i++) {  // mclk_n2_div[] has 5 entries
    int div = mclk_n2_div[i].param;
    int mclk_src = mclk_hz >> div;

    if (srate->mclk_src[NAU8325_MCLK_FS_RATIO_256] == (uint32_t)mclk_src) {
      ratio = NAU8325_MCLK_FS_RATIO_256;
      n2_sel_out = i;
      break;
    }
    if (srate->mclk_src[NAU8325_MCLK_FS_RATIO_400] == (uint32_t)mclk_src) {
      ratio = NAU8325_MCLK_FS_RATIO_400;
      n2_sel_out = i;
      break;
    }
    if (srate->mclk_src[NAU8325_MCLK_FS_RATIO_500] == (uint32_t)mclk_src) {
      ratio = NAU8325_MCLK_FS_RATIO_500;
      n2_sel_out = i;
      break;
    }
  }

  return ratio;
}

inline const SRateAttr* PCBCUPID_NAU8325::getSRateAttr(int fs) {
  for (size_t i = 0;
       i < sizeof(target_srate_table) / sizeof(target_srate_table[0]); ++i) {
    if (target_srate_table[i].fs == fs) {
      return &target_srate_table[i];
    }
  }
  return nullptr;
}

inline bool PCBCUPID_NAU8325::chooseClockSource(int fs, int mclk,
                                                const SRateAttr*& srate,
                                                int& n1_sel, int& mult_sel,
                                                int& n2_sel) {
  if (fs <= 0 || mclk <= 0) {
    AD_LOGI("[NAU8325] Invalid fs or mclk.");
    return false;
  }

  srate = getSRateAttr(fs);
  if (!srate) {
    AD_LOGI("[NAU8325] Unsupported fs: %d\n", fs);
    return false;
  }

  // First try direct N2 mapping
  int ratio = getMclkRatioAndN2Index(srate, mclk, n2_sel);
  if (ratio != NAU8325_MCLK_FS_RATIO_NUM) {
    n1_sel = 0;
    mult_sel = -1;  // Bypass
    AD_LOGI("[NAU8325] Direct match: fs=%d, mclk=%d → N2=%d (Ratio=%d)\n", fs,
            mclk, n2_sel, ratio);
    return true;
  }

  // Try all N1 and N3 combinations
  int mclk_max = 0;
  int best_n1 = -1, best_mult = -1, best_n2 = -1;

  for (int i = 0; i < 3; ++i)  // N1 options
  {
    for (int j = 0; j < 4; ++j)  // N3 multiplier options
    {
      int m = (mclk << mclk_n3_mult[j].param) / mclk_n1_div[i].param;
      int r = getMclkRatioAndN2Index(srate, m, n2_sel);

      if (r != NAU8325_MCLK_FS_RATIO_NUM && (m > mclk_max || best_n1 < i)) {
        mclk_max = m;
        best_n1 = i;
        best_mult = j;
        best_n2 = n2_sel;
      }
    }
  }

  if (mclk_max > 0) {
    n1_sel = best_n1;
    mult_sel = best_mult;
    n2_sel = best_n2;

    AD_LOGI(
        "[NAU8325] Matched via N1/N3 combo: fs=%d, mclk=%d → N1=%d, "
        "N3(mult)=%d, N2=%d\n",
        fs, mclk, n1_sel, mult_sel, n2_sel);
    AD_LOGI("[NAU8325] chooseClockSource(): fs=%d, mclk=%d\n", fs, mclk);

    return true;
  }

  AD_LOGI("[NAU8325] Failed to match MCLK %d Hz with fs %d Hz\n", mclk, fs);
  return false;
}

inline bool PCBCUPID_NAU8325::configureClocks(int fs, int mclk) {
  const SRateAttr* srate = nullptr;
  int n1_sel = 0, mult_sel = -1, n2_sel = 0;

  if (!chooseClockSource(fs, mclk, srate, n1_sel, mult_sel, n2_sel)) {
    AD_LOGI("[NAU8325] Failed to choose clock source for fs=%d, mclk=%d\n", fs,
            mclk);
    return false;
  }

  if (!applySampleRateClocks(srate, n1_sel, mult_sel, n2_sel)) {
    AD_LOGI("[NAU8325] Failed to apply sample rate clocks.");
    return false;
  }
  AD_LOGI("[NAU8325] configureClocks(): fs=%u, mclk=%u\n", fs, mclk);

  return true;
}

inline const OsrAttr* PCBCUPID_NAU8325::getCurrentOSR() {
  uint16_t value;
  if (!readRegister(NAU8325_R29_DAC_CTRL1, value)) {
    return nullptr;
  }

  uint8_t osr_index = value & NAU8325_DAC_OVERSAMPLE_MASK;

  if (osr_index >= sizeof(osr_dac_sel) / sizeof(osr_dac_sel[0])) {
    return nullptr;
  }

  return &osr_dac_sel[osr_index];
}

inline bool PCBCUPID_NAU8325::configureAudio(uint32_t fs, uint32_t mclk,
                                             uint8_t bits_per_sample) {
  this->fs = fs;
  this->mclk = mclk;

  // Configure clock tree
  if (!configureClocks(fs, mclk)) {
    return false;
  }

  //  Optional: set oversampling mode explicitly (OSR = 128)
  writeRegisterBits(NAU8325_R29_DAC_CTRL1, NAU8325_DAC_OVERSAMPLE_MASK,
                    NAU8325_DAC_OVERSAMPLE_128);

  const OsrAttr* osr = getCurrentOSR();
  if (!osr || osr->osr == 0 || (fs * osr->osr > CLK_DA_AD_MAX)) {
    AD_LOGI("[NAU8325] Invalid OSR or fs × OSR exceeds max.");
    return false;
  }

  // Set DAC clock source
  writeRegisterBits(NAU8325_R03_CLK_CTRL, NAU8325_CLK_DAC_SRC_MASK,
                    osr->clk_src << NAU8325_CLK_DAC_SRC_SFT);

  // Configure I2S word length
  uint16_t val_len = 0;
  switch (bits_per_sample) {
    case 16:
      val_len = NAU8325_I2S_DL_16;
      break;
    case 20:
      val_len = NAU8325_I2S_DL_20;
      break;
    case 24:
      val_len = NAU8325_I2S_DL_24;
      break;
    case 32:
      val_len = NAU8325_I2S_DL_32;
      break;
    default:
      AD_LOGI("[NAU8325] Invalid bit width: %u\n", bits_per_sample);
      return false;
  }

  writeRegisterBits(NAU8325_R0D_I2S_PCM_CTRL1, NAU8325_I2S_DL_MASK, val_len);

  AD_LOGI("[NAU8325] configureAudio(): fs=%u, mclk=%u, bits=%u\n", fs, mclk,
          bits_per_sample);

  return true;
}

inline bool PCBCUPID_NAU8325::setI2SFormat(AudioI2SFormat format,
                                           ClockPolarity polarity) {
  uint16_t ctrl1_val = 0;

  // Set BCLK polarity
  if (polarity == INVERTED_BCLK) {
    ctrl1_val |= NAU8325_I2S_BP_INV;
  }

  // Set data format
  switch (format) {
    case I2S_STD:
      ctrl1_val |= NAU8325_I2S_DF_I2S;
      break;
    case LEFT_JUSTIFIED:
      ctrl1_val |= NAU8325_I2S_DF_LEFT;
      break;
    case RIGHT_JUSTIFIED:
      ctrl1_val |= NAU8325_I2S_DF_RIGTH;
      break;
    case DSP_A:
      ctrl1_val |= NAU8325_I2S_DF_PCM_AB;
      break;
    case DSP_B:
      ctrl1_val |= NAU8325_I2S_DF_PCM_AB | NAU8325_I2S_PCMB_EN;
      break;
    default:
      return false;
  }

  // Apply to I2S_PCM_CTRL1
  return writeRegisterBits(
      NAU8325_R0D_I2S_PCM_CTRL1,
      NAU8325_I2S_DF_MASK | NAU8325_I2S_BP_MASK | NAU8325_I2S_PCMB_EN,
      ctrl1_val);
}

inline bool PCBCUPID_NAU8325::setSysClock(uint32_t freq) {
  if (freq < MASTER_CLK_MIN || freq > MASTER_CLK_MAX) {
    AD_LOGI("[NAU8325] Invalid MCLK: %u Hz (allowed: %u - %u)\n", freq,
            MASTER_CLK_MIN, MASTER_CLK_MAX);
    return false;
  }

  this->mclk = freq;
  AD_LOGI("[NAU8325] MCLK set to %u Hz\n", mclk);
  return true;
}

inline void PCBCUPID_NAU8325::setPowerUpDefault(bool enable) {
  writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, NAU8325_PWRUP_DFT,
                    enable ? NAU8325_PWRUP_DFT : 0);
}

inline void PCBCUPID_NAU8325::setOversampling(OversamplingMode mode) {
  writeRegisterBits(NAU8325_R29_DAC_CTRL1, NAU8325_DAC_OVERSAMPLE_MASK, mode);
}

inline void PCBCUPID_NAU8325::setVolume(uint8_t left, uint8_t right) {
  uint16_t value = ((left & 0xFF) << 8) | (right & 0xFF);
  writeRegister(NAU8325_R13_DAC_VOLUME, value);
}

inline void PCBCUPID_NAU8325::softMute(bool enable) {
  writeRegisterBits(NAU8325_R12_MUTE_CTRL, NAU8325_SOFT_MUTE,
                    enable ? NAU8325_SOFT_MUTE : 0);
  delayMs(30);
}

inline void PCBCUPID_NAU8325::initRegisters() {
  // Set ALC parameters default timing and max gain
  writeRegisterBits(NAU8325_R2C_ALC_CTRL1, NAU8325_ALC_MAXGAIN_MASK,
                    0x7 << NAU8325_ALC_MAXGAIN_SFT);
  writeRegisterBits(
      NAU8325_R2D_ALC_CTRL2,
      NAU8325_ALC_DCY_MASK | NAU8325_ALC_ATK_MASK | NAU8325_ALC_HLD_MASK,
      (0x5 << NAU8325_ALC_DCY_SFT) | (0x3 << NAU8325_ALC_ATK_SFT) |
          (0x5 << NAU8325_ALC_HLD_SFT));

  /* Enable ALC to avoid signal distortion when battery low. */
  if (alc_enable) {
    writeRegisterBits(NAU8325_R2E_ALC_CTRL3, NAU8325_ALC_EN, NAU8325_ALC_EN);
  }

  // Clock detection bits
  uint16_t clk_det_mask = NAU8325_CLKPWRUP_DIS | NAU8325_PWRUP_DFT;

  if (clock_detection)
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, clk_det_mask, 0);
  else
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, clk_det_mask,
                      NAU8325_CLKPWRUP_DIS);

  if (clock_det_data)
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, NAU8325_APWRUP_EN,
                      NAU8325_APWRUP_EN);
  else
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, NAU8325_APWRUP_EN, 0);

  // DAC Vref configuration
  uint16_t vref_val = 0xFFFF;
  if (dac_vref_microvolt == 1800000)
    vref_val = 0 << NAU8325_DACVREFSEL_SFT;
  else if (dac_vref_microvolt == 2700000)
    vref_val = 1 << NAU8325_DACVREFSEL_SFT;
  else if (dac_vref_microvolt == 2880000)
    vref_val = 2 << NAU8325_DACVREFSEL_SFT;
  else if (dac_vref_microvolt == 3060000)
    vref_val = 3 << NAU8325_DACVREFSEL_SFT;

  if (vref_val != 0xFFFF) {
    writeRegisterBits(NAU8325_R73_RDAC, NAU8325_DACVREFSEL_MASK, vref_val);
  } else {
    AD_LOGI("[NAU8325] Invalid DAC Vref: %lu uV\n", dac_vref_microvolt);
  }

  /* DAC Reference Voltage Decoupling Capacitors. */
  writeRegisterBits(NAU8325_R63_ANALOG_CONTROL_3,
                    NAU8325_CLASSD_COARSE_GAIN_MASK, 0x4);
  /* Auto-Att Min Gain 0dB, Class-D N Driver Slew Rate -25%. */
  writeRegisterBits(NAU8325_R64_ANALOG_CONTROL_4, NAU8325_CLASSD_SLEWN_MASK,
                    0x7);

  // VMID resistor selection
  uint16_t vref_imp_val = 0xFFFF;
  if (vref_impedance_ohms == 0)
    vref_imp_val = 0 << NAU8325_BIAS_VMID_SEL_SFT;
  else if (vref_impedance_ohms == 25000)
    vref_imp_val = 1 << NAU8325_BIAS_VMID_SEL_SFT;
  else if (vref_impedance_ohms == 125000)
    vref_imp_val = 2 << NAU8325_BIAS_VMID_SEL_SFT;
  else if (vref_impedance_ohms == 2500)
    vref_imp_val = 3 << NAU8325_BIAS_VMID_SEL_SFT;

  if (vref_imp_val != 0xFFFF) {
    writeRegisterBits(NAU8325_R60_BIAS_ADJ, NAU8325_BIAS_VMID_SEL_MASK,
                      vref_imp_val);
  } else {
    AD_LOGI("[NAU8325] Invalid VMID impedance: %lu ohms\n",
            vref_impedance_ohms);
  }

  // Enable VMID, BIAS, DACs, etc., Voltage / current Amps
  writeRegisterBits(
      NAU8325_R61_ANALOG_CONTROL_1,
      NAU8325_DACEN_MASK | NAU8325_DACCLKEN_MASK | NAU8325_DACEN_R_MASK |
          NAU8325_DACCLKEN_R_MASK | NAU8325_CLASSDEN_MASK |
          NAU8325_VMDFSTENB_MASK | NAU8325_BIASEN_MASK | NAU8325_VMIDEN_MASK,
      (0x1 << NAU8325_DACEN_SFT) | (0x1 << NAU8325_DACCLKEN_SFT) |
          (0x1 << NAU8325_DACEN_R_SFT) | (0x1 << NAU8325_DACCLKEN_R_SFT) |
          (0x1 << NAU8325_CLASSDEN_SFT) | (0x1 << NAU8325_VMDFSTENB_SFT) |
          (0x1 << NAU8325_BIASEN_SFT) | (1 << NAU8325_VMIDEN_SFT));

  /* Enable ALC to avoid signal distortion when battery low. */
  if (alc_enable) {
    writeRegisterBits(NAU8325_R2E_ALC_CTRL3, NAU8325_ALC_EN, NAU8325_ALC_EN);
  }

  // Clock detection bits
  uint16_t clk_det_mask1 = NAU8325_CLKPWRUP_DIS | NAU8325_PWRUP_DFT;

  if (clock_detection)
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, clk_det_mask1, 0);
  else
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, clk_det_mask1,
                      NAU8325_CLKPWRUP_DIS);

  if (clock_det_data)
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, NAU8325_APWRUP_EN,
                      NAU8325_APWRUP_EN);
  else
    writeRegisterBits(NAU8325_R40_CLK_DET_CTRL, NAU8325_APWRUP_EN, 0);

  // Set default OSR = 128
  writeRegisterBits(NAU8325_R29_DAC_CTRL1, NAU8325_DAC_OVERSAMPLE_MASK,
                    NAU8325_DAC_OVERSAMPLE_128);
}

inline void PCBCUPID_NAU8325::powerOn() {
  // Clear soft mute to allow audio output
  softMute(false);

  // Enable DACs, clocks, bias, class-D
  writeRegisterBits(
      NAU8325_R61_ANALOG_CONTROL_1,
      NAU8325_DACEN_MASK | NAU8325_DACCLKEN_MASK | NAU8325_DACEN_R_MASK |
          NAU8325_DACCLKEN_R_MASK | NAU8325_CLASSDEN_MASK |
          NAU8325_VMDFSTENB_MASK | NAU8325_BIASEN_MASK | NAU8325_VMIDEN_MASK,
      (3 << NAU8325_DACEN_SFT) | (3 << NAU8325_DACCLKEN_SFT) |
          (3 << NAU8325_DACEN_R_SFT) | (3 << NAU8325_DACCLKEN_R_SFT) |
          (3 << NAU8325_CLASSDEN_SFT) | (3 << NAU8325_VMDFSTENB_SFT) |
          (3 << NAU8325_BIASEN_SFT) | 0x03);  // VMID_EN

  // If clock detection is disabled, enable PWRUP_DFT
  setPowerUpDefault(true);

  delayMs(30);  // Let analog circuitry stabilize
}

inline void PCBCUPID_NAU8325::powerOff() {
  // Soft mute first to prevent pop
  softMute(true);

  // Disable analog and DAC power
  writeRegisterBits(NAU8325_R61_ANALOG_CONTROL_1,
                    NAU8325_DACEN_MASK | NAU8325_DACCLKEN_MASK |
                        NAU8325_DACEN_R_MASK | NAU8325_DACCLKEN_R_MASK |
                        NAU8325_CLASSDEN_MASK | NAU8325_VMDFSTENB_MASK |
                        NAU8325_BIASEN_MASK | NAU8325_VMIDEN_MASK,
                    0x0000);

  // If clock detection is disabled, clear PWRUP_DFT
  setPowerUpDefault(false);

  delayMs(30);  // Let output fade out
}

inline bool PCBCUPID_NAU8325::writeRegisterBits(
    uint16_t reg, uint16_t mask,
    uint16_t value)  // mask ---> is to set the the particular bit renaining bit
                     // untouched.
{
  uint16_t current;
  if (!readRegister(reg, current)) return false;

  uint16_t new_value = (current & ~mask) | (value & mask);
  return writeRegister(reg, new_value);
}

inline bool PCBCUPID_NAU8325::writeRegister(uint16_t reg, uint16_t val) {
  AD_LOGI("[WRITE] reg=0x%04X val=0x%04X\n", reg, val);

  uint8_t regBuf[2] = {(uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF)};
  uint8_t dataBuf[2] = {(uint8_t)((val >> 8) & 0xFF), (uint8_t)(val & 0xFF)};

  return i2c_bus_write_bytes(this->i2c, i2c_addr, regBuf, sizeof(regBuf),
                             dataBuf, sizeof(dataBuf)) == RESULT_OK;
}

inline bool PCBCUPID_NAU8325::readRegister(uint16_t reg, uint16_t& value) {
  uint8_t regBuf[2] = {(uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF)};
  uint8_t dataBuf[2] = {0, 0};

  if (i2c_bus_read_bytes(this->i2c, i2c_addr, regBuf, sizeof(regBuf), dataBuf,
                         sizeof(dataBuf)) != RESULT_OK) {
    return false;
  }

  value = ((uint16_t)dataBuf[0] << 8) | dataBuf[1];
  return true;
}

}  // namespace audio_driver

