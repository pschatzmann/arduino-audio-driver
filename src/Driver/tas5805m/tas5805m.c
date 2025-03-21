/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdint.h>
#include "tas5805m.h"
#include "tas5805m_reg_cfg.h"
#include "Platforms/etc.h"
// 0x5c>>1 = 0x2E

#define TAS5805M_ADDR        0x2E    
//#define TAS5805M_RST_GPIO      get_pa_enable_gpio()
#define TAS5805M_VOLUME_MAX    100
#define TAS5805M_VOLUME_MIN    0

#define TAS5805M_ASSERT(a, format, b, ...) \
    if ((a) != 0) { \
        AD_LOGE( format, ##__VA_ARGS__); \
        return b;\
    }

error_t tas5805m_ctrl(codec_mode_t mode, bool ctrl_state_active);
error_t tas5805m_conig_iface(codec_mode_t mode, I2SDefinition *iface);
static i2c_bus_handle_t     i2c_handler;
static int power_pin = -1;

void tas5805m_set_power_pin(int pin) {
   power_pin = pin;
}

// /*
//  * i2c default configuration
//  */
// static i2c_config_t i2c_cfg = {
//     .mode = I2C_MODE_MASTER,
//     .sda_pullup_en = GPIO_PULLUP_ENABLE,
//     .scl_pullup_en = GPIO_PULLUP_ENABLE,
//     .master.clk_speed = I2C_CLOCK_SPEED,
// };

// /*
//  * Operate fuction of PA
//  */
// func_t AUDIO_CODEC_TAS5805M_DEFAULT_HANDLE = {
//     .audio_codec_initialize = tas5805m_init,
//     .audio_codec_deinitialize = tas5805m_deinit,
//     .audio_codec_ctrl = tas5805m_ctrl,
//     .audio_codec_config_iface = tas5805m_conig_iface,
//     .audio_codec_set_mute = tas5805m_set_mute,
//     .audio_codec_set_volume = tas5805m_set_volume,
//     .audio_codec_get_volume = tas5805m_get_volume,
//     .lock = NULL,
//     .handle = NULL,
// };

static error_t tas5805m_transmit_registers(const tas5805m_cfg_reg_t *conf_buf, int size)
{
    int i = 0;
    error_t ret = RESULT_OK;
    while (i < size) {
        switch (conf_buf[i].offset) {
            case CFG_META_SWITCH:
                // Used in legacy applications.  Ignored here.
                break;
            case CFG_META_DELAY:
                delay(conf_buf[i].value);
                break;
            case CFG_META_BURST:
                ret = i2c_bus_write_bytes(i2c_handler, TAS5805M_ADDR, (unsigned char *)(&conf_buf[i + 1].offset), 1, (unsigned char *)(&conf_buf[i + 1].value), conf_buf[i].value);
                i +=  (conf_buf[i].value / 2) + 1;
                break;
            case CFG_END_1:
                if (CFG_END_2 == conf_buf[i + 1].offset && CFG_END_3 == conf_buf[i + 2].offset) {
                    AD_LOGI( "End of tms5805m reg: %d", i);
                }
                break;
            default:
                ret = i2c_bus_write_bytes(i2c_handler, TAS5805M_ADDR, (unsigned char *)(&conf_buf[i].offset), 1, (unsigned char *)(&conf_buf[i].value), 1);
                break;
        }
        i++;
    }
    if (ret != RESULT_OK) {
        AD_LOGE( "Fail to load configuration to tas5805m");
        return RESULT_FAIL;
    }
    AD_LOGI( "%s:  write %d reg done", __FUNCTION__, i);
    return ret;
}

error_t tas5805m_init(codec_config_t *codec_cfg,  void* i2c)
{
    i2c_handler = i2c;
    error_t ret = RESULT_OK;
    AD_LOGI( "Power ON CODEC with GPIO %d", power_pin);
    pinMode(power_pin, OUTPUT);
    digitalWrite(power_pin, 0);
    delay(20);
    digitalWrite(power_pin, 1);
    delay(20);

    ret |= tas5805m_transmit_registers(tas5805m_registers, sizeof(tas5805m_registers) / sizeof(tas5805m_registers[0]));

    TAS5805M_ASSERT(ret, "Fail to iniitialize tas5805m PA", RESULT_FAIL);
    return ret;
}

error_t tas5805m_set_volume(int vol)
{
    int vol_idx = 0;

    if (vol < TAS5805M_VOLUME_MIN) {
        vol = TAS5805M_VOLUME_MIN;
    }
    if (vol > TAS5805M_VOLUME_MAX) {
        vol = TAS5805M_VOLUME_MAX;
    }
    vol_idx = vol / 5;

    uint8_t cmd[2] = {0, 0};
    error_t ret = RESULT_OK;

    cmd[0] = MASTER_VOL_REG_ADDR;
    cmd[1] = tas5805m_volume[vol_idx];
    ret = i2c_bus_write_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);
    AD_LOGW("volume = 0x%x", cmd[1]);
    return ret;
}

error_t tas5805m_get_volume(int *value)
{
    /// FIXME: Got the digit volume is not right.
    uint8_t cmd[2] = {MASTER_VOL_REG_ADDR, 0x00};
    error_t ret = i2c_bus_read_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);
    TAS5805M_ASSERT(ret, "Fail to get volume", RESULT_FAIL);
    unsigned i;
    for (i = 0; i < sizeof(tas5805m_volume); i++) {
        if (cmd[1] >= tas5805m_volume[i])
            break;
    }
    AD_LOGI( "Volume is %d", i * 5);
    *value = 5 * i;
    return ret;
}

error_t tas5805m_set_mute(bool enable)
{
    error_t ret = RESULT_OK;
    uint8_t cmd[2] = {TAS5805M_REG_03, 0x00};
    ret |= i2c_bus_read_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);

    if (enable) {
        cmd[1] |= 0x8;
    } else {
        cmd[1] &= (~0x08);
    }
    ret |= i2c_bus_write_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);

    TAS5805M_ASSERT(ret, "Fail to set mute", RESULT_FAIL);
    return ret;
}

error_t tas5805m_get_mute(int *value)
{
    error_t ret = RESULT_OK;
    uint8_t cmd[2] = {TAS5805M_REG_03, 0x00};
    ret |= i2c_bus_read_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);

    TAS5805M_ASSERT(ret, "Fail to get mute", RESULT_FAIL);
    *value = (cmd[1] & 0x08) >> 4;
    AD_LOGI( "Get mute value: 0x%x", *value);
    return ret;
}

error_t tas5805m_set_mute_fade(int value)
{
    error_t ret = 0;
    unsigned char cmd[2] = {MUTE_TIME_REG_ADDR, 0x00};
    /* Time for register value
    *   000: 11.5 ms
    *   001: 53 ms
    *   010: 106.5 ms
    *   011: 266.5 ms
    *   100: 0.535 sec
    *   101: 1.065 sec
    *   110: 2.665 sec
    *   111: 5.33 sec
    */
    if (value <= 12) {
        cmd[1] = 0;
    } else if (value <= 53) {
        cmd[1] = 1;
    } else if (value <= 107) {
        cmd[1] = 2;
    } else if (value <= 267) {
        cmd[1] = 3;
    } else if (value <= 535) {
        cmd[1] = 4;
    } else if (value <= 1065) {
        cmd[1] = 5;
    } else if (value <= 2665) {
        cmd[1] = 6;
    } else {
        cmd[1] = 7;
    }
    cmd[1] |= (cmd[1] << 4);

    ret |= i2c_bus_write_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);
    TAS5805M_ASSERT(ret, "Fail to set mute fade", RESULT_FAIL);
    AD_LOGI( "Set mute fade, value:%d, 0x%x", value, cmd[1]);
    return ret;
}

error_t tas5805m_set_damp_mode(int value)
{
    unsigned char cmd[2] = {0};
    cmd[0] = TAS5805M_REG_02;
    cmd[1] = 0x10 | value;
    return i2c_bus_write_bytes(i2c_handler, TAS5805M_ADDR, &cmd[0], 1, &cmd[1], 1);
}

error_t tas5805m_deinit(void)
{
    // TODO
    return RESULT_OK;
}

error_t tas5805m_ctrl(codec_mode_t mode, bool ctrl_state_active)
{
    // TODO
    return RESULT_OK;
}

error_t tas5805m_conig_iface(codec_mode_t mode, I2SDefinition *iface)
{
    //TODO
    return RESULT_OK;
}
