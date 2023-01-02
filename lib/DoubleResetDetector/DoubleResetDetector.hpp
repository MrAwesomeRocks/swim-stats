// clang-format off
/****************************************************************************************************************************
  ESP_DoubleResetDetector.h
  For ESP8266 / ESP32 boards

  ESP_DoubleResetDetector is a library for the ESP8266/Arduino platform
  to enable trigger configure mode by resetting ESP32 / ESP8266 twice.

  Forked from DataCute https://github.com/datacute/DoubleResetDetector

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_DoubleResetDetector
  Licensed under MIT license
  Version: 1.3.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      15/12/2019 Initial coding
  1.0.1   K Hoang      30/12/2019 Now can use EEPROM or SPIFFS for both ESP8266 and ESP32. RTC still OK for ESP8266
  1.0.2   K Hoang      10/04/2020 Fix bug by left-over cpp file and in example.
  1.0.3   K Hoang      13/05/2020 Update to use LittleFS for ESP8266 core 2.7.1+
  1.1.0   K Hoang      04/12/2020 Add support to LittleFS for ESP32 using LITTLEFS Library
  1.1.1   K Hoang      28/12/2020 Suppress all possible compiler warnings
  1.1.2   K Hoang      10/10/2021 Update `platform.ini` and `library.json`
  1.2.0   K Hoang      26/11/2021 Auto detect ESP32 core and use either built-in LittleFS or LITTLEFS library
  1.2.1   K Hoang      26/11/2021 Fix compile error for ESP32 core v1.0.5-
  1.3.0   K Hoang      10/02/2022 Add support to new ESP32-S3
  1.3.1   K Hoang      04/03/2022 Add waitingForDRD() function to signal in DRD wating period
  1.3.2   K Hoang      09/09/2022 Fix ESP32 chipID for example ConfigOnDoubleReset
*****************************************************************************************************************************/
// clang-format on
#pragma once

#include <Arduino.h>
#include <EEPROM.h>

class DoubleResetDetector {
    bool waiting_for_drd_ = false;
    unsigned long timeout_;

    int flag_addr_;
    uint8_t flag_;

 public:
    static constexpr uint8_t FLAG_SET = 0xAF;
    static constexpr uint8_t FLAG_CLEAR = 0xFA;

    /**
     * @brief Construct a new Double Reset Detector object.
     *
     * @param timeout The DRD timeout (in seconds)
     * @param flag_address The address of the EEPROM flag.
     */
    explicit DoubleResetDetector(uint16_t timeout, int flag_address) :
        timeout_(timeout * 1000), flag_addr_(flag_address){};

    /**
     * @brief Check if a double reset occurred.
     *
     * @return Whether or not a double reset occurred.
     */
    [[nodiscard]] inline bool
    check() noexcept
    {
        if (!EEPROM.length()) {
            log_w("EEPROM not initialized, DRD will not work.");
            return false;
        }

        EEPROM.get(flag_addr_, flag_);
        log_v("Read flag %#2x", flag_);

        if (flag_ == FLAG_SET) {
            log_d("Double reset detected!");
            clear_flag_();

            return true;
        } else {
            log_d("No double reset detected!");

            set_flag_();
            waiting_for_drd_ = true;

            return false;
        }
    };

    [[nodiscard]] inline bool
    waiting() const noexcept
    {
        return waiting_for_drd_;
    }

    inline void
    loop() noexcept
    {
        if (waiting_for_drd_ && millis() > timeout_) {
            log_d("DRD timeout expired");
            stop();
        }
    };

    inline void
    stop() noexcept
    {
        clear_flag_();
        waiting_for_drd_ = false;
    };

 private:
    inline void
    clear_flag_() noexcept
    {
        flag_ = FLAG_CLEAR;

        if (!EEPROM.length()) {
            log_w("EEPROM not initialized, DRD will not work.");
            return;
        }

        EEPROM.put(flag_addr_, flag_);
        EEPROM.commit();

        log_d("Cleared DRD flag.");
    };

    inline void
    set_flag_() noexcept
    {
        flag_ = FLAG_SET;

        if (!EEPROM.length()) {
            log_w("EEPROM not initialized, DRD will not work.");
            return;
        }

        EEPROM.put(flag_addr_, flag_);
        EEPROM.commit();

        log_d("Set DRD flag.");
    };
};
