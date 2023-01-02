/**
 * @file data.cpp
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
#include "data.hpp"

#include "server.hpp"

#include <LittleFS.h>

// What are we doing with our MPU data.
enum DataSink {
    DATA_SINK_STREAM, // Stream with eventsource
    DATA_SINK_RECORD, // Record to file
};

// The default data sink.
#define DATA_SINK_DEFAULT DATA_SINK_STREAM

/******************************************************************************/

// Default to streaming over WiFi
static DataSink cur_data_sink = DATA_SINK_DEFAULT;

// When to stop recording
static unsigned long rec_end;

// The file we're recording to
static File rec_file;

/******************************************************************************/

StaticJsonDocument<MPU_DATA_JSON_SIZE>
mpu_data_t::to_json()
{
    StaticJsonDocument<MPU_DATA_JSON_SIZE> doc;

    // Yaw, pitch, roll
    // TODO(nino): send in radians or degrees?
    JsonArray ypr_json = doc.createNestedArray("ypr");
    ypr_json.add(degrees(ypr[0]));
    ypr_json.add(degrees(ypr[1]));
    ypr_json.add(degrees(ypr[2]));

    // Real acceleration (w/o gravity)
    JsonArray accel_json = doc.createNestedArray("accel");
    accel_json.add(accel.x);
    accel_json.add(accel.y);
    accel_json.add(accel.x);

    // Gyroscope
    JsonArray gyro_json = doc.createNestedArray("gyro");
    gyro_json.add(gyro.x);
    gyro_json.add(gyro.y);
    gyro_json.add(gyro.z);

    return doc;
}

void
data_process_measurement(mpu_data_t meas)
{
    switch (cur_data_sink) {
        case DATA_SINK_STREAM:
            web_server_send_event("mpuData", meas.to_json());
            break;

        case DATA_SINK_RECORD:
            if (millis() > rec_end) {
                rec_file.close();
                cur_data_sink = DATA_SINK_DEFAULT;

                log_i("Recording completed!");
                return;
            }
            rec_file.write(reinterpret_cast<uint8_t*>(&meas), sizeof(meas));
            break;

        default:
            log_w("Invalid data sink %lu", cur_data_sink);
            log_e("WE SHOULD NEVER BE HERE. Restarting...");
            ESP.restart();
            break;
    }
}

void
data_start_recording(uint32_t rec_len, String filename)
{
    // Prepend the dir and append the ext
    filename = "/recs/" + filename + ".dat";

    log_i("Recording to %s...", filename.c_str());

    // Open the file
    rec_file = LittleFS.open(filename, "w", true);
    if (!rec_file) {
        log_e("Could not open recording file.");
        rec_file.close();
        return;
    }

    // Set the sink mode
    cur_data_sink = DATA_SINK_RECORD;

    // Set the end time
    log_i("Starting recording for %lu ms.", rec_len);
    rec_end = millis() + rec_len;
}
