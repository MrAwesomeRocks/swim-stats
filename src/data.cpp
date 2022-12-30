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

// Default to streaming over WiFi
static DataSink cur_data_sink = DATA_SINK_STREAM;

// When to stop recording, if we are recording
unsigned long recording_end;

void
set_data_sink(DataSink sink)
{
    cur_data_sink = sink;
}

StaticJsonDocument<192>
mpu_data_t::to_json()
{
    StaticJsonDocument<192> doc;

    // Yaw, pitch, roll
    // TODO(nino): send in radians or degrees?
    JsonArray ypr_json = doc.createNestedArray("ypr");
    ypr_json.add(degrees(ypr[0]));
    ypr_json.add(degrees(ypr[1]));
    ypr_json.add(degrees(ypr[2]));

    // Real acceleration (w/o gravity)
    JsonArray accel_json = doc.createNestedArray("accel");
    accel_json.add(accel[0]);
    accel_json.add(accel[1]);
    accel_json.add(accel[2]);

    // Temperature
    doc["temp"] = temp;

    return doc;
}

void
process_measurement(mpu_data_t meas)
{
    switch (cur_data_sink) {
        case DATA_SINK_STREAM:
            web_server_send_event("mpuData", meas.to_json());
            break;

        case DATA_SINK_RECORD:
            // TODO
            break;

        default:
            log_w("Invalid data sink.");
            log_e("WE SHOULD NEVER BE HERE.");
            break;
    }
}
