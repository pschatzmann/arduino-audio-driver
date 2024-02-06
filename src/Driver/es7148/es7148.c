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

#include "es7148.h"


static bool codec_init_flag = 0;

// func_t AUDIO_CODEC_ES7148_DEFAULT_HANDLE = {
//     .audio_codec_initialize = es7148_codec_init,
//     .audio_codec_deinitialize = es7148_codec_deinit,
//     .audio_codec_ctrl = es7148_codec_ctrl_state_active,
//     .audio_codec_config_iface = es7148_codec_config_i2s,
//     .audio_codec_set_mute = es7148_codec_set_voice_mute,
//     .audio_codec_set_volume = es7148_codec_set_voice_volume,
//     .audio_codec_get_volume = es7148_codec_get_voice_volume,
//     .lock = NULL,
//     .handle = NULL,
// };

static bool es7148_codec_initialized()
{
    return codec_init_flag;
}

error_t es7148_codec_init(codec_config_t *cfg)
{
    if (es7148_codec_initialized()) {
        AUDIODRIVER_LOGW("The es7148 codec has been already initialized");
        return RESULT_OK;
    }
    codec_init_flag = true;
    return RESULT_OK;
}

error_t es7148_codec_deinit(void)
{
    codec_init_flag = false;
    return RESULT_OK;
}

error_t es7148_codec_ctrl_state_active(codec_mode_t mode, bool ctrl_state_active)
{
    return RESULT_OK;
}

error_t es7148_codec_config_i2s(codec_mode_t mode, I2SDefinition *iface)
{
    return RESULT_OK;
}

error_t es7148_codec_set_voice_mute(bool mute)
{
    return RESULT_OK;
}

error_t es7148_codec_set_voice_volume(int volume)
{
    int ret = 0;
    return ret;
}

error_t es7148_codec_get_voice_volume(int *volume)
{
    int ret = 0;
    return ret;
}
