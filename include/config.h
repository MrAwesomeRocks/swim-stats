#pragma once

/*
        Pin setup
*/
#define INTERRUPT_PIN 36 // Literally anything on the ESP32
#define LED_PIN       2  // Connected to DevKit LED

/*
        Logging Config
*/
// Uncomment to use the builtin ESP-IDF logger over the Arduino one
// #define USE_ESP_IDF_LOG

// Logging tag, must be defined if using the ESP-IDF logger
#ifdef USE_ESP_IDF_LOG
#  define TAG "ARDUINO"
#endif

/*
        NTP config
*/
// The NTP servers to fetch time from
#define NTP_SERVER_1 "0.pool.ntp.org"
#define NTP_SERVER_2 "1.pool.ntp.org"
#define NTP_SERVER_3 "2.pool.ntp.org"

// Our timezone
#define NTP_TZ -6

// Number of seconds between our time zone and GMT
#define NTP_GMT_OFFSET_sec NTP_TZ * 3600

// Offset for DST. Should be just one hour
#define NTP_DST_OFFSET_sec 3600

/*
        Tests
*/
// Uncomment to test the web server without the MPU6050
#define TEST_WEBSERVER
