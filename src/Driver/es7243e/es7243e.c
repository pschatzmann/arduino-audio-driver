/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#include "es7243e.h"
#include "stddef.h" // NULL

//static char *TAG = "DRV7243E";
static i2c_bus_handle_t i2c_handle = NULL;
static int es7243e_addr = 0x10; //0x20 >> 1;

static error_t es7243e_write_reg(uint8_t reg_add, uint8_t data)
{
    return i2c_bus_write_bytes(i2c_handle, es7243e_addr, &reg_add, sizeof(reg_add), &data, sizeof(data));
}


error_t es7243e_adc_set_addr(int addr)
{
    es7243e_addr = addr;
    return RESULT_OK;
}

error_t es7243e_adc_init(codec_config_t *codec_cfg, void *i2c)
{
    i2c_handle = i2c;
    error_t ret = RESULT_OK;
    ret |= es7243e_write_reg(0x01, 0x3A);
    ret |= es7243e_write_reg(0x00, 0x80);
    ret |= es7243e_write_reg(0xF9, 0x00);
    ret |= es7243e_write_reg(0x04, 0x02);
    ret |= es7243e_write_reg(0x04, 0x01);
    ret |= es7243e_write_reg(0xF9, 0x01);
    ret |= es7243e_write_reg(0x00, 0x1E);
    ret |= es7243e_write_reg(0x01, 0x00);

    ret |= es7243e_write_reg(0x02, 0x00);
    ret |= es7243e_write_reg(0x03, 0x20);
    ret |= es7243e_write_reg(0x04, 0x01);
    ret |= es7243e_write_reg(0x0D, 0x00);
    ret |= es7243e_write_reg(0x05, 0x00);
    ret |= es7243e_write_reg(0x06, 0x03); // SCLK=MCLK/4
    ret |= es7243e_write_reg(0x07, 0x00); // LRCK=MCLK/256
    ret |= es7243e_write_reg(0x08, 0xFF); // LRCK=MCLK/256

    ret |= es7243e_write_reg(0x09, 0xCA);
    ret |= es7243e_write_reg(0x0A, 0x85);
    ret |= es7243e_write_reg(0x0B, 0x00);
    ret |= es7243e_write_reg(0x0E, 0xBF);
    ret |= es7243e_write_reg(0x0F, 0x80);
    ret |= es7243e_write_reg(0x14, 0x0C);
    ret |= es7243e_write_reg(0x15, 0x0C);
    ret |= es7243e_write_reg(0x17, 0x02);
    ret |= es7243e_write_reg(0x18, 0x26);
    ret |= es7243e_write_reg(0x19, 0x77);
    ret |= es7243e_write_reg(0x1A, 0xF4);
    ret |= es7243e_write_reg(0x1B, 0x66);
    ret |= es7243e_write_reg(0x1C, 0x44);
    ret |= es7243e_write_reg(0x1E, 0x00);
    ret |= es7243e_write_reg(0x1F, 0x0C);
    ret |= es7243e_write_reg(0x20, 0x1A); //PGA gain +30dB
    ret |= es7243e_write_reg(0x21, 0x1A); //PGA gain +30dB

    ret |= es7243e_write_reg(0x00, 0x80); //Slave  Mode
    ret |= es7243e_write_reg(0x01, 0x3A);
    ret |= es7243e_write_reg(0x16, 0x3F);
    ret |= es7243e_write_reg(0x16, 0x00);
    if (ret) {
        AD_LOGE("Es7243e initialize failed!");
        return RESULT_FAIL;
    }
    return ret;
}

error_t es7243e_adc_deinit(void)
{
    return RESULT_OK;
}

error_t es7243e_adc_ctrl_state_active(codec_mode_t mode, bool ctrl_state_active)
{
    error_t ret = RESULT_OK;
    if (ctrl_state_active) {
        ret |= es7243e_write_reg(0xF9, 0x00);
        ret |= es7243e_write_reg(0x04, 0x01);
        ret |= es7243e_write_reg(0x17, 0x01);
        ret |= es7243e_write_reg(0x20, 0x10);
        ret |= es7243e_write_reg(0x21, 0x10);
        ret |= es7243e_write_reg(0x00, 0x80);
        ret |= es7243e_write_reg(0x01, 0x3A);
        ret |= es7243e_write_reg(0x16, 0x3F);
        ret |= es7243e_write_reg(0x16, 0x00);
    } else {
        AD_LOGW("The codec going to stop");
        ret |= es7243e_write_reg(0x04, 0x02);
        ret |= es7243e_write_reg(0x04, 0x01);
        ret |= es7243e_write_reg(0xF7, 0x30);
        ret |= es7243e_write_reg(0xF9, 0x01);
        ret |= es7243e_write_reg(0x16, 0xFF);
        ret |= es7243e_write_reg(0x17, 0x00);
        ret |= es7243e_write_reg(0x01, 0x38);
        ret |= es7243e_write_reg(0x20, 0x00);
        ret |= es7243e_write_reg(0x21, 0x00);
        ret |= es7243e_write_reg(0x00, 0x00);
        ret |= es7243e_write_reg(0x00, 0x1E);
        ret |= es7243e_write_reg(0x01, 0x30);
        ret |= es7243e_write_reg(0x01, 0x00);
    }
    return ret;
}

error_t es7243e_adc_config_i2s(codec_mode_t mode, I2SDefinition *iface)
{
    return RESULT_OK;
}

error_t es7243e_adc_set_voice_volume(int volume)
{
    return RESULT_OK;
}

error_t es7243e_adc_get_voice_volume(int *volume)
{
    return RESULT_OK;
}
