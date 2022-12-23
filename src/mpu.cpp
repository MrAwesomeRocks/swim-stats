/**
 * @file mpu.cpp
 * @author Nino Maruszewski (nino.maruszewski@gmail.com)
 * @brief Interface to MPU6050 data collection.
 * @version 1.0
 * @date 2022-12-21
 *
 * Adapted from
 * https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6_using_DMP_V6.12/MPU6050_DMP6_using_DMP_V6.12.ino
 *
 * MIT License
 *
 * Copyright (c) 2022 Nino Maruszewski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include "mpu.hpp"

#include "config.h"

#include <Arduino.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps612.h>
#include <Wire.h>

MPU6050 mpu(MPU6050_ADDRESS_AD0_LOW);

// MPU control/status vars
bool dmp_ready = false;  // set true if DMP init was successful
uint8_t mpu_int_status;  // holds actual interrupt status byte from MPU
uint8_t device_status;   // status after each device operation (0 = success, !0 = error)
uint16_t packet_size;    // expected DMP packet size (default is 42 bytes)
uint16_t fifo_count;     // count of all bytes currently in FIFO
uint8_t fifo_buffer[64]; // FIFO storage buffer

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

// indicates whether MPU interrupt pin has gone high
volatile bool mpu_interrupt = false;

void IRAM_ATTR
dmp_data_ready_isr()
{
    mpu_interrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
bool
mpu_setup()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock (shouldn't cause problems)

    // initialize device
    log_i("Initializing I2C devices...");
    mpu.initialize();

    // verify connection
    log_i("Testing device connections...");
    if (mpu.testConnection())
        log_i("MPU6050 connection successful");
    else
        log_w("MPU6050 connection failed"); // TODO(nino): This seems like an error...

    // load and configure the DMP
    log_i("Initializing DMP...");
    device_status = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (device_status != 0) {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        log_e("DMP Initialization failed (code %d)\n", device_status);

        switch (device_status) {
            case 1:
                log_e("Initial DMP memory load failed.");
                break;
            case 2:
                log_e("DMP configuration updates failed.");
                break;
            default:
                log_e("Unknown DMP error.");
                break;
        }
        return false;
    }

    // Calibration Time: generate offsets and calibrate our MPU6050
    log_i("Calibrating DMP...");
    mpu.CalibrateAccel();
    mpu.CalibrateGyro();
    Serial.println();

    int16_t* offsets = mpu.GetActiveOffsets();
    log_d("DMP Offsets:");
    log_d(
        "Accel:\t%.5f,\t%.5f,\t%.5f", (float)offsets[0], (float)offsets[1],
        (float)offsets[2]
    );
    log_d(
        "Gyro:\t%.5f,\t%.5f,\t%.5f", (float)offsets[3], (float)offsets[4],
        (float)offsets[5]
    );

    // turn on the DMP, now that it's ready
    log_i("Enabling DMP...");
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    log_i(
        "Enabling interrupt detection (ESP32 external interrupt %d)...",
        digitalPinToInterrupt(INTERRUPT_PIN)
    );
    pinMode(INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmp_data_ready_isr, RISING);
    mpu_int_status = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    log_i("DMP ready! Waiting for first interrupt...");
    dmp_ready = true;

    // get expected DMP packet size for later comparison
    packet_size = mpu.dmpGetFIFOPacketSize();
    log_d("DMP packet size: %u", packet_size);

    return true;
}

bool
mpu_data_available()
{
    return mpu.dmpGetCurrentFIFOPacket(fifo_buffer);
}

/**
 * @brief Get the linear acceleration from the raw acceleration and gravity.
 *
 * Adapted from MPU6050::dmpGetLinearAccel, which uses the wrong multiplier for
 * gravity (8192 instead of 16384).
 *
 * See https://github.com/jrowberg/i2cdevlib/issues/152#issuecomment-682319598
 * for more details.
 *
 * @param v
 * @param vRaw
 * @param gravity
 * @return uint8_t
 */
static uint8_t
get_linear_accel(VectorInt16* v, VectorInt16* vRaw, VectorFloat* gravity)
{
    // get rid of the gravity component (+1g = +16384 in standard DMP FIFO packet,
    // sensitivity is 2g)
    v->x = vRaw->x - gravity->x * 16384;
    v->y = vRaw->y - gravity->y * 16384;
    v->z = vRaw->z - gravity->z * 16384;
    return 0;
}

void
mpu_get_real_accel(VectorInt16* accel_real)
{
    Quaternion q;        // [w, x, y, z]         quaternion container
    VectorInt16 accel;   // [x, y, z]            accel sensor measurements
    VectorFloat gravity; // [x, y, z]            gravity vector

    mpu.dmpGetQuaternion(&q, fifo_buffer);
    mpu.dmpGetAccel(&accel, fifo_buffer);
    mpu.dmpGetGravity(&gravity, &q);
    get_linear_accel(accel_real, &accel, &gravity);
}

void
mpu_get_world_accel(VectorInt16* accel_world)
{
    Quaternion q;           // [w, x, y, z]     quaternion container
    VectorInt16 accel;      // [x, y, z]        accel sensor measurements
    VectorFloat gravity;    // [x, y, z]        gravity vector
    VectorInt16 accel_real; // [x, y, z]        gravity-free accel sensor measurements

    mpu.dmpGetQuaternion(&q, fifo_buffer);
    mpu.dmpGetAccel(&accel, fifo_buffer);
    mpu.dmpGetGravity(&gravity, &q);
    get_linear_accel(&accel_real, &accel, &gravity);
    mpu.dmpGetLinearAccelInWorld(accel_world, &accel_real, &q);
}

void
mpu_get_ypr(float ypr[3])
{
    Quaternion q;        // [w, x, y, z]         quaternion container
    VectorFloat gravity; // [x, y, z]            gravity vector

    mpu.dmpGetQuaternion(&q, fifo_buffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
}

float
mpu_get_temp()
{
    int16_t mpu_temp = mpu.getTemperature();
    float temp_c = mpu_temp / 340.0 + 36.53;

    return temp_c;
}
