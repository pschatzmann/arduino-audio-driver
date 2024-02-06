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

#pragma once

#include "stdbool.h"
#include "Common.h"
#include "Utils/I2C.h"
#include "Driver/DriverConstants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize es7148 chip
 *
 * @param cfg configuration of es7148
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
error_t es7148_codec_init(codec_config_t *cfg);
/**
 * @brief Deinitialize es7148 chip
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
error_t es7148_codec_deinit(void);
/**
 * The functions es7148_ctrl_state_active and es7148_config_i2s are not used by this driver.
 * They are kept here to maintain the uniformity and convenience of the interface
 * of the ADF project.
 * These settings for es7148 are burned in firmware and configuration files.
 * Default i2s configuration: 48000Hz, 16bit, Left-Right channels.
 * Use resampling to be compatible with different file types.
 *
 * @brief Control es7148 chip
 *
 * @param mode codec mode
 * @param ctrl_state_active start or stop decode or encode progress
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
error_t es7148_codec_ctrl_state_active(codec_mode_t mode, bool ctrl_state_active);
/**
 * @brief Configure es7148 codec mode and I2S interface
 *
 * @param mode codec mode
 * @param iface I2S config
 *
 * @return
 *     - RESULT_FAIL Parameter error
 *     - RESULT_OK   Success
 */
error_t es7148_codec_config_i2s(codec_mode_t mode, I2SDefinition *iface);

/**
 * @brief mute or unmute the codec
 *
 * @param mute:  true, false
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
error_t es7148_codec_set_voice_mute(bool mute);

/**
 * @brief  Set voice volume
 *
 * @param volume:  voice volume (0~100)
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
error_t es7148_codec_set_voice_volume(int volume);

/**
 * @brief Get voice volume
 *
 * @param[out] *volume:  voice volume (0~100)
 *
 * @return
 *     - RESULT_OK
 *     - RESULT_FAIL
 */
error_t es7148_codec_get_voice_volume(int *volume);

#ifdef __cplusplus
}
#endif

