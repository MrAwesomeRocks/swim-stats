#pragma once

#include <Arduino.h>

// espressif/arduino-esp32 - examples/ResetReason/ResetReason.ino
#ifdef ESP_IDF_VERSION_MAJOR  // IDF 4+
#  if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
#    include "esp32/rom/rtc.h"
#  elif CONFIG_IDF_TARGET_ESP32S2
#    include "esp32s2/rom/rtc.h"
#  elif CONFIG_IDF_TARGET_ESP32C3
#    include "esp32c3/rom/rtc.h"
#  elif CONFIG_IDF_TARGET_ESP32S3
#    include "esp32s3/rom/rtc.h"
#  else
#    error Target CONFIG_IDF_TARGET is not supported
#  endif
#else // ESP32 Before IDF 4.0
#  include "rom/rtc.h"
#endif

#define _UTILS__ISO8601_LEN 26 // 2022-12-31T20:06:38-0600

inline String
iso8601_str() noexcept
{
    struct tm tm;
    if (!getLocalTime(&tm))
        return "";

    char buf[_UTILS__ISO8601_LEN];
    strftime(buf, _UTILS__ISO8601_LEN, "%FT%T%z", &tm);

    return buf;
}

inline const char*
get_reset_reason(uint8_t core) noexcept
{
    RESET_REASON reason = rtc_get_reset_reason(core);
    switch (reason) {
        case POWERON_RESET:
            return "Vbat power on reset (POWERON_RESET)";
        case SW_RESET:
            return "Software reset digital core (SW_RESET)";
        case OWDT_RESET:
            return "Legacy watch dog reset digital core (OWDT_RESET)";
        case DEEPSLEEP_RESET:
            return "Deep Sleep reset digital core (DEEPSLEEP_RESET)";
        case SDIO_RESET:
            return "Reset by SLC module, reset digital core (SDIO_RESET)";
        case TG0WDT_SYS_RESET:
            return "Timer Group0 Watch dog reset digital core (TG0WDT_SYS_RESET)";
        case TG1WDT_SYS_RESET:
            return "Timer Group1 Watch dog reset digital core (TG1WDT_SYS_RESET)";
        case RTCWDT_SYS_RESET:
            return "RTC Watch dog Reset digital core (RTCWDT_SYS_RESET)";
        case INTRUSION_RESET:
            return "Instrusion tested to reset CPU (INTRUSION_RESET)";
        case TGWDT_CPU_RESET:
            return "Time Group reset CPU (TGWDT_CPU_RESET)";
        case SW_CPU_RESET:
            return "Software reset CPU (SW_CPU_RESET)";
        case RTCWDT_CPU_RESET:
            return "RTC Watch dog Reset CPU (RTCWDT_CPU_RESET)";
        case EXT_CPU_RESET:
            return "for APP CPU, reset by PRO CPU (EXT_CPU_RESET)";
        case RTCWDT_BROWN_OUT_RESET:
            return "Reset when the vdd voltage is not stable (RTCWDT_BROWN_OUT_RESET)";
        case RTCWDT_RTC_RESET:
            return "RTC Watch dog reset digital core and rtc module (RTCWDT_RTC_RESET)";

        [[fallthrough]] case NO_MEAN:
        default:
            return "No meaning for reset reason (NO_MEAN)";
    }
}
