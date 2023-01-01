#pragma once

/*
        Pin setup
*/
#define INTERRUPT_PIN 36 // Literally anything on the ESP32
#define LED_PIN       2  // Connected to DevKit LED

/*
        MPU config
*/
// Sample rate (in ms)
// Should be a multiple of 10
#define MPU_SAMPLE_RATE 50

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
        WiFi Config
*/
#define __IP(a, b, c, d) a, b, c, d // Bit nicer representation and some arg checking

// Uncomment to use a static IP
#define USE_STATIC_IP

// Static IP config
#ifdef USE_STATIC_IP

// The static IP address
#  define STATIC_IP_ADDR __IP(192, 168, 1, 150)

// Static gateway
#  define STATIC_GATEWAY __IP(192, 168, 1, 1)

// Static subnet mask
#  define STATIC_SUBNET_MASK __IP(255, 255, 255, 0)

// Static DNS server
#  define STATIC_DNS_SERVER __IP(1, 1, 1, 1)

#endif

// Configuration of the hostname of the ESP32
// Either set a static hostname, or set a prefix and use the ESP32's chip ID
// as the suffix.
// So uncomment one of the two lines below
// #define HOSTNAME "swim-stats"
#define HOSTNAME_PREFIX "swim-stats-"

// Configuration of the WiFi manager SSID of the ESP32
// Either set a static SSID, or set a prefix and use the ESP32's chip ID
// as the suffix.
// So uncomment one of the two lines below
// #define AP_SSID "SwimStats"
#define AP_SSID_PREFIX "SwimStats_"

// Set the SSID password
// If not defined, will use the ESP32 chip ID (same one as the SSID)
// #define AP_PSK "123456"

// Uncomment to run a mDNS resolver for nicer URLs
// Uses the hostname as the URL.
#define USE_mDNS

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
// #define TEST_WEBSERVER
