#include "config.h"
#include "mpu.hpp"

#include <Arduino.h>

// Store whether our LED is on or not so we can blink it
bool blinkState = false;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void
setup()
{
    // initialize serial communication
    Serial.begin(115200);

#ifndef TEST_WEBSERVER
    // Configure the MPU6050
    mpu_setup();
#endif

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void
loop()
{
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
