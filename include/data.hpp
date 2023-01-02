/**
 * @file data.hpp
 * @author Nino Maruszewski (nino.maruszewski@gmail.com)
 * @brief Data processing functions.
 * @version 0.1
 * @date 2022-12-29
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

#include "utils.hpp"

#include <ArduinoJson.h>
#include <helper_3dmath.h>

#define MPU_DATA_JSON_SIZE 192
#define MPU_DATA_JSON_ARR_SIZE 208

/**
 * @brief A struct for holding raw MPU data measurements
 */
struct mpu_data_t {
    float ypr[3];      // [yaw, pitch, roll]  (radians)
    VectorFloat accel; // [a_x, a_y, a_z]     (w/o gravity, m/s^2)
    VectorFloat gyro;  // [g_x, g_y, g_z]     (rad / s)

    /**
     * @brief Convert this data struct to a JSON.
     *
     * @return A new JsonObject with this struct's data.
     */
    StaticJsonDocument<MPU_DATA_JSON_SIZE> to_json();
};

/**
 * @brief Process new MPU measurements.
 *
 * @param meas The new measurements.
 */
void data_process_measurement(mpu_data_t meas);

/**
 * @brief Start recording data.
 *
 * @param recording_len How long to record for.
 * @param filename The filename to record to.
 */
void data_start_recording(uint32_t recording_len, String filename = iso8601_str());
