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

#include <ArduinoJson.h>

/**
 * @brief A struct for holding raw MPU data measurements
 */
struct mpu_data_t {
    float ypr[3];   // [yaw, pitch, roll]  (radians)
    float accel[3]; // [a_x, a_y, a_z]     (w/o gravity, m/s^2)
    float temp;     //                     (celsius)

    /**
     * @brief Convert this data struct to a JSON.
     *
     * @return A new JsonObject with this struct's data.
     */
    StaticJsonDocument<192> to_json();
};

/**
 * @brief What are we doing with our MPU data.
 */
enum DataSink {
    DATA_SINK_STREAM, // Stream with eventsource
    DATA_SINK_RECORD, // Record to file
};

/**
 * @brief Set the data sink object.
 *
 * @param sink The new data sink.
 */
void set_data_sink(DataSink sink);

/**
 * @brief Process new MPU measurements.
 *
 * @param meas The new measurements.
 */
void process_measurement(mpu_data_t meas);
