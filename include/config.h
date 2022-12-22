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
        Tests
*/
// Uncomment to test the web server without the MPU6050
#define TEST_WEBSERVER
