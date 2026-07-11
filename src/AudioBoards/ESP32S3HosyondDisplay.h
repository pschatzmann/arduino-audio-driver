#pragma once
/**
 * @file ESP32S3HosyondDisplay.h
 * @brief Device definition for the Hosyond 2.8inch ESP32-S3 Display
 * @author Rick Wargo
 *
 * Information about this board can be found at https://www.lcdwiki.com/2.8inch_ESP32-S3_Display.
 * The product page is accessible via the Hosyond site at https://hosyond.com/#:~:text=ESP32%20Display.
 *
 * This will not work for the Hosyond 3.5inch display as the pins are different. Fortunately, both boards
 * share the ES8311 for audio, however, this board uses an FM8002E for the power amplifier, compared to
 * SC8002B. The PA's are pin compatible and are also enabled low via the Hosyond boards, so it should
 * only be another pin remapping; the difference being the SC8002B can driver a higher output up to 3W.
 */

#include "AudioBoard.h"
#include "SD_MMC.h"

#if !defined(IS_ZEPHYR)

#define HOSYOND_SD_MMC_SCK          38
#define HOSYOND_SD_MMC_CMD          40    // MOSI
#define HOSYOND_SD_MMC_D0           39    // MISO
#define HOSYOND_SD_MMC_D1           41
#define HOSYOND_SD_MMC_D2           48
#define HOSYOND_SD_MMC_D3           47    // CS
#define HOSYOND_SD_CS               HOSYOND_SD_MMC_D3
#define HOSYOND_BOOT_BUTTON         0
#define HOSYOND_RGB_LED             42

#define HOSYOND_I2S_MCK             4
#define HOSYOND_I2S_BCK             5
#define HOSYOND_I2S_DOUT            8
#define HOSYOND_I2S_DIN             6
#define HOSYOND_I2S_WS              7

#define HOSYOND_I2C_SCL             15            // GPIO number used for I2C master clock
#define HOSYOND_I2C_SDA             16            // GPIO number used for I2C master data
#define HOSYOND_I2C_NUM             0             // I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip
#define HOSYOND_I2C_SPEED           400000        // I2C master clock frequency

#define HOSYOND_AUDIO_PA_ENABLE     1

#define HOSYOND_BATTERY             9
#define HOSYOND_UART0_RX            43
#define HOSYOND_UART0_TX            44
#define HOSYOND_GPIO_EXPANDER_1     2
#define HOSYOND_GPIO_EXPANDER_2     14
#define HOSYOND_GPIO_EXPANDER_3     14
#define HOSYOND_GPIO_EXPANDER_4     21

#endif

namespace audio_driver
{
    /**
     * @brief Pins definition for ESP32-S3 Hosyond Display with ES8311 AudioDriver & FM8002E Audio Amplifier
     * @author Rick Wargo
     */

    class PinsESP32S3HosyondDisplayClass : public DriverDeviceInfo
    {
    public:
        PinsESP32S3HosyondDisplayClass()
        {
            // add i2c codec pins: scl, sda, port, frequency
            addI2C(PinFunction::CODEC, HOSYOND_I2C_SCL, HOSYOND_I2C_SDA, HOSYOND_I2C_NUM, HOSYOND_I2C_SPEED);

            // add i2s pins: mclk, bck, ws,data_out, data_in ,(port)
            addI2S(PinFunction::CODEC, HOSYOND_I2S_MCK, HOSYOND_I2S_BCK, HOSYOND_I2S_WS, HOSYOND_I2S_DOUT, HOSYOND_I2S_DIN);

            // add other pins
            addPin(PinFunction::PA, HOSYOND_AUDIO_PA_ENABLE, PinLogic::OutputActiveLow);      // Audio output enable signal, low level enable, high level disable
            addPin(PinFunction::KEY, HOSYOND_BOOT_BUTTON, PinLogic::InputActiveLow, 1); // Boot Key button

            // Additional GPIO Expansion Pins - can be used as a normal IO
            addPin(PinFunction::EXPANDER, HOSYOND_GPIO_EXPANDER_1, PinLogic::Inactive, 0);
            addPin(PinFunction::EXPANDER, HOSYOND_GPIO_EXPANDER_2, PinLogic::Inactive, 1);
            addPin(PinFunction::EXPANDER, HOSYOND_GPIO_EXPANDER_3, PinLogic::Inactive, 2);
            addPin(PinFunction::EXPANDER, HOSYOND_GPIO_EXPANDER_4, PinLogic::Inactive, 3);

            // The following are here for definition purposes
            // addPin(PinFunction::EXPANDER, HOSYOND_BATTERY, PinLogic::Input, 4);   // Battery voltage ADC value acquisition input signal
            // addPin(PinFunction::LED, HOSYOND_RGB_LED, PinLogic::Output, 1);       // Single-line RGB three-color LED light, which can control the internal red, green, and blue three kinds of light beads separately according to different signals
        }

        bool begin() override
        {
            bool rc = DriverDeviceInfo::begin();

            if (sdmmc_active) {
                setSDMMCActive(true);
                rc &= SD_MMC.setPins(HOSYOND_SD_MMC_SCK, HOSYOND_SD_MMC_CMD, HOSYOND_SD_MMC_D0, HOSYOND_SD_MMC_D1, HOSYOND_SD_MMC_D2, HOSYOND_SD_MMC_D3);
                rc &= SD_MMC.begin();
            }
            return rc;
        }
    };

    /// @ingroup audio_driver
    static PinsESP32S3HosyondDisplayClass PinsESP32S3HosyondDisplay;
    /// @ingroup audio_driver
    static AudioBoard ESP32S3HosyondDisplay{AudioDriverES8311, PinsESP32S3HosyondDisplay};
} // namespace audio_driver$
