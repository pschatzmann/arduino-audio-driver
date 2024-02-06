#pragma once

typedef enum {
    _BIT_LENGTH_MIN = -1,
    _BIT_LENGTH_16BITS = 0x03,
    _BIT_LENGTH_18BITS = 0x02,
    _BIT_LENGTH_20BITS = 0x01,
    _BIT_LENGTH_24BITS = 0x00,
    _BIT_LENGTH_32BITS = 0x04,
    _BIT_LENGTH_MAX,
} es_bits_length_t;

typedef enum {
    _ES_MODULE_MIN = -1,
    _ES_MODULE_ADC = 0x01,
    _ES_MODULE_DAC = 0x02,
    _ES_MODULE_ADC_DAC = 0x03,
    _ES_MODULE_LINE = 0x04,
    _ES_MODULE_MAX
} es_module_t;


typedef enum {
    _DAC_OUTPUT_MIN = -1,
    _DAC_OUTPUT_LOUT1 = 0x04,
    _DAC_OUTPUT_LOUT2 = 0x08,
    _DAC_OUTPUT_SPK   = 0x09,
    _DAC_OUTPUT_ROUT1 = 0x10,
    _DAC_OUTPUT_ROUT2 = 0x20,
    _DAC_OUTPUT_ALL = 0x3c,
    _DAC_OUTPUT_MAX,
} es_dac_output_t;

typedef enum {
    _ADC_INPUT_MIN = -1,
    _ADC_INPUT_LINPUT1_RINPUT1 = 0x00,
    _ADC_INPUT_MIC1  = 0x05,
    _ADC_INPUT_MIC2  = 0x06,
    _ADC_INPUT_LINPUT2_RINPUT2 = 0x50,
    _ADC_INPUT_DIFFERENCE = 0xf0,
    _ADC_INPUT_MAX,
} es_adc_input_t;


typedef enum {
    _ES_I2S_MIN = -1,
    _ES_I2S_NORMAL = 0,
    _ES_I2S_LEFT = 1,
    _ES_I2S_RIGHT = 2,
    _ES_I2S_DSP = 3,
    _ES_I2S_MAX
} es_i2s_fmt_t;

typedef enum {
    _D2SE_PGA_GAIN_MIN = -1,
    _D2SE_PGA_GAIN_DIS = 0,
    _D2SE_PGA_GAIN_EN = 1,
    _D2SE_PGA_GAIN_MAX = 2,
} es_d2se_pga_t;


typedef enum {
    MCLK_DIV_MIN = -1,
    MCLK_DIV_1 = 1,
    MCLK_DIV_2 = 2,
    MCLK_DIV_3 = 3,
    MCLK_DIV_4 = 4,
    MCLK_DIV_6 = 5,
    MCLK_DIV_8 = 6,
    MCLK_DIV_9 = 7,
    MCLK_DIV_11 = 8,
    MCLK_DIV_12 = 9,
    MCLK_DIV_16 = 10,
    MCLK_DIV_18 = 11,
    MCLK_DIV_22 = 12,
    MCLK_DIV_24 = 13,
    MCLK_DIV_33 = 14,
    MCLK_DIV_36 = 15,
    MCLK_DIV_44 = 16,
    MCLK_DIV_48 = 17,
    MCLK_DIV_66 = 18,
    MCLK_DIV_72 = 19,
    MCLK_DIV_5 = 20,
    MCLK_DIV_10 = 21,
    MCLK_DIV_15 = 22,
    MCLK_DIV_17 = 23,
    MCLK_DIV_20 = 24,
    MCLK_DIV_25 = 25,
    MCLK_DIV_30 = 26,
    MCLK_DIV_32 = 27,
    MCLK_DIV_34 = 28,
    MCLK_DIV_7  = 29,
    MCLK_DIV_13 = 30,
    MCLK_DIV_14 = 31,
    MCLK_DIV_MAX,
} es_sclk_div_t;

typedef enum {
    LCLK_DIV_MIN = -1,
    LCLK_DIV_128 = 0,
    LCLK_DIV_192 = 1,
    LCLK_DIV_256 = 2,
    LCLK_DIV_384 = 3,
    LCLK_DIV_512 = 4,
    LCLK_DIV_576 = 5,
    LCLK_DIV_768 = 6,
    LCLK_DIV_1024 = 7,
    LCLK_DIV_1152 = 8,
    LCLK_DIV_1408 = 9,
    LCLK_DIV_1536 = 10,
    LCLK_DIV_2112 = 11,
    LCLK_DIV_2304 = 12,

    LCLK_DIV_125 = 16,
    LCLK_DIV_136 = 17,
    LCLK_DIV_250 = 18,
    LCLK_DIV_272 = 19,
    LCLK_DIV_375 = 20,
    LCLK_DIV_500 = 21,
    LCLK_DIV_544 = 22,
    LCLK_DIV_750 = 23,
    LCLK_DIV_1000 = 24,
    LCLK_DIV_1088 = 25,
    LCLK_DIV_1496 = 26,
    LCLK_DIV_1500 = 27,
    LCLK_DIV_MAX,
} es_lclk_div_t;

typedef struct {
    es_sclk_div_t sclk_div;    /*!< bits clock divide */
    es_lclk_div_t lclk_div;    /*!< WS clock divide */
} es_i2s_clock_t;

typedef void* audio_codec_dac_vol_offset;

/**
 * @brief Codec dac volume configurations
 */
typedef struct {
    float   max_dac_volume;  /*!< Codec support max volume */
    float   min_dac_volume;  /*!< Codec support min volume */
    float   board_pa_gain;   /*!< Board power amplifier gain */
    float   volume_accuracy; /*!< Codec dac volume accuracy(0.5 or 1) */
    int8_t  dac_vol_symbol;  /*!< Whether the dac volume is positively correlated with the register value */
    uint8_t zero_volume_reg; /*!< Codec register value for zero dac volume */
    uint8_t reg_value;       /*!< Record current dac volume register value */
    int     user_volume;     /*!< Record the user set volume */
    audio_codec_dac_vol_offset offset_conv_volume; /*!<  Convert user volume to dac volume offset */
} codec_dac_volume_config_t;



void delay(uint32_t);