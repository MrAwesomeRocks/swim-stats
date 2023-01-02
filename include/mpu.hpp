/**
 * @file mpu.hpp
 * @author Nino Maruszewski (nino.maruszewski@gmail.com)
 * @brief Interface to MPU6050 data collection.
 * @version 1.0
 * @date 2022-12-21
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
#pragma once

#include <MPU6050_6Axis_MotionApps612.h>

/**
 * @brief Set to true if the MPU6050 initialized correctly.
 */
extern bool dmp_ready;

/**
 * @brief Setup and initialize the MPU6050 and its DMP.
 *
 * 1. Starts the I2C bus
 * 2. Initializes the MPU6050
 * 3. Initializes the DMP.
 * 4. Calibrates the DMP.
 * 5. Attaches an ISR for data ready.
 *
 * @return bool Whether or not the MPU6050 intialized successfully.
 */
bool mpu_setup();

/**
 * @brief Poll the MPU6050 to see if data is available.
 *
 * If the data is available, updates the internal FIFO data buffer.
 *
 * @return bool If there is new DMP data available.
 */
bool mpu_data_available();

/**
 * @brief Get the real acceleration (without gravity).
 *
 * @param accel_real Container to save the accelearation to.
 */
void mpu_get_real_accel(VectorInt16* accel_real);

/**
 * @brief Get the real acceleration (without gravity).
 *
 * @param accel_real Container to save the accelearation to.
 */
void mpu_get_real_accel(VectorFloat* accel_real);

/**
 * @brief Get the real acceleration, in the world frame of reference.
 *
 * This rotates the acceleration to match the initial MPU6050 orientation.
 *
 * @param accel_world Container to save the acceration to.
 */
void mpu_get_world_accel(VectorInt16* accel_world);

/**
 * @brief Get the yaw/pitch/roll orientation of the MPU6050.
 *
 * @param ypr Container to save the YPR data to.
 */
void mpu_get_ypr(float ypr[3]);

/**
 * @brief Get the gyroscope reading.
 *
 * @param accel_real Container to save the gyroscope reading to.
 */
void mpu_get_gyro(VectorInt16* gyro);

/**
 * @brief Get the gyroscope reading.
 *
 * @param accel_real Container to save the gyroscope reading to.
 */
void mpu_get_gyro(VectorFloat* gyro);

/**
 * @brief Get the temperature from the MPU6050's built-in temp sensor.
 *
 * Theoretically good for two decimal places of accuracy, realistically you can
 * only use one.
 *
 * @return float The temperature in degrees celsius.
 */
float mpu_get_temp();

/**
 * @brief Convert an MPU acceleration integer measurement to a value in m/s.
 *
 * @param mpu_meas The MPU integer measurement.
 * @return The value in m/s.
 */
inline float
mpu_accel_to_mps(int16_t mpu_meas)
{
    return mpu_meas * 9.81 / 16384.0;
}

/**
 * @brief Convert an MPU gyro measurement to a value in °/s
 *
 * @param mpu_meas The MPU integer measurement.
 * @return The value in degrees per second.
 */
inline float
mpu_gyro_to_dps(int16_t mpu_meas)
{
    return mpu_meas * 2000.0 / INT16_MAX;
}
