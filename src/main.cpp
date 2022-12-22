#include "config.h"
#include "mpu.hpp"
#include "web.hpp"

#include <Arduino.h>
#include <pgmspace.h>
#include <WiFi.h>

// Store whether our LED is on or not so we can blink it
bool blinkState = false;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void
print_chip_debug_info()
{
    // Chip info
    log_d("%s v%d", ESP.getChipModel(), ESP.getChipRevision());
    log_d("%u x CPU @ %lu MHz", ESP.getChipCores(), ESP.getCpuFreqMHz());
    log_d(
        "%.2f MB flash @ %lu MHz, mode: %#x", ESP.getFlashChipSize() / 1024.0 / 1024.0,
        ESP.getFlashChipSpeed() / 1000 / 1000, ESP.getFlashChipMode()
    );
    log_d("Chip ID: %llX", ESP.getEfuseMac());

    // Sketch info
    log_d("Sketch MD5: %s", ESP.getSketchMD5().c_str());
    log_d(
        "Free sketch space: %lu B/%lu B", ESP.getFreeSketchSpace(),
        ESP.getFreeSketchSpace() + ESP.getSketchSize()
    );

    // Memory info
    log_d("Free heap: %lu B/%lu B", ESP.getHeapSize(), ESP.getHeapSize());
    log_d("Free PSRAM: %lu B/%lu B", ESP.getFreePsram(), ESP.getPsramSize());

    // Library versions
    log_d("ESP-IDF %s", ESP.getSdkVersion());
    log_d(
        "Arduino v%u.%u.%u", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MAJOR,
        ESP_ARDUINO_VERSION_PATCH
    );
    log_d("WiFiManager %s", wifi_manager_version());
}

void
setup()
{
    // initialize serial communication
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    // Initial log
    print_chip_debug_info();

    // Connect to WiFi
    log_i("Attempting to connect to WiFi");

    if (!wifi_connect()) {
        log_e("WiFi connection failed.");
        ESP.restart();
    }

    log_i("Wifi successfully connected!");

#ifndef TEST_WEBSERVER
    // Configure the MPU6050
    mpu_setup();
#endif

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void
loop()
{
    if (Serial.available()) {
        char cmd;
        switch (cmd = Serial.read()) {
            case 'c':
                log_i("Clearing WiFi settings and rebooting in 5 seconds!");
                delay(5000);
                WiFi.disconnect(true, true);
                ESP.restart();
                break;

            case 'h':
                log_i("Commands: (c)lear wifi settings, (h)elp");
                break;

            default:
                log_w("Unknown command '%c'", cmd);
                break;
        }
    }

#ifndef TEST_WEBSERVER
    static unsigned long poll_miss_count = 0;

    // if programming failed, don't try to do anything
    if (!dmp_ready)
        return;

    // read a packet from FIFO
    if (mpu_data_available()) { // Get the Latest packet

        // Display yaw-pitch-roll
        float ypr[3];
        mpu_get_ypr(ypr);
        Serial.print("\typr\t");
        Serial.print(ypr[0] * 180 / M_PI);
        Serial.print("\t");
        Serial.print(ypr[1] * 180 / M_PI);
        Serial.print("\t");
        Serial.print(ypr[2] * 180 / M_PI);

        // display real acceleration, adjusted to remove gravity
        VectorInt16 aaReal;
        mpu_get_real_accel(&aaReal);
        Serial.print("\tareal\t");
        Serial.print(aaReal.x);
        Serial.print("\t");
        Serial.print(aaReal.y);
        Serial.print("\t");
        Serial.print(aaReal.z);

        // display initial world-frame acceleration, adjusted to remove gravity
        // and rotated based on known orientation from quaternion
        VectorInt16 aaWorld;
        mpu_get_world_accel(&aaWorld);
        Serial.print("\taworld\t");
        Serial.print(aaWorld.x);
        Serial.print("\t");
        Serial.print(aaWorld.y);
        Serial.print("\t");
        Serial.print(aaWorld.z);

        // Display the temperature using the MPU6050's built-in temp sensor
        Serial.printf("\ttemp\t%.1f°C", mpu_get_temp());

        static auto last_sample_time = 0;
        auto cur_time = millis();
        auto sample_time = cur_time - last_sample_time;
        last_sample_time = cur_time;

        Serial.printf("\tSample time\t%lu ms", sample_time);
        Serial.printf("\tPoll misses\t%lu", poll_miss_count);

        Serial.println();

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);

        poll_miss_count = 0;
    } else {
        poll_miss_count++;
    }
#endif
}
