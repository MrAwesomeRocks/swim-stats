/**
 * @file server.cpp
 * @author Nino Maruszewski (nino.maruszewski@gmail.com)
 * @brief Web server functions.
 * @version 0.1
 * @date 2022-12-22
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
#include "server.hpp"

#include "data.hpp"

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Server on port 80 (HTTP)
static AsyncWebServer server(80);

// Event source on /events
static AsyncEventSource events("/events");

static void
list_dir(String dir, AsyncWebServerRequest* req)
{
    // Open dir
    log_i("Opening directory \"%s\"", dir.c_str());

    File rec_dir = LittleFS.open(dir);
    if (!rec_dir) {
        // No files in directory - it doesn't exist
        log_w("Directory %s does not exist", dir);
        rec_dir.close();
        return req->send(200, "application/json", "{\"files\":[]}");
    }

    if (!rec_dir.isDirectory()) {
        log_e("Directory \"%s\" not a directory", dir.c_str());
        rec_dir.close();
        return req->send(500, "text/plain", "Could not open directory.");
    }

    // Allocate JSON
    log_d("Creating JSON document");

    DynamicJsonDocument doc(1024);
    auto files = doc.createNestedArray("files");

    // Go through files
    log_i("Walking files!");

    File f;
    while (f = rec_dir.openNextFile()) {
        char* filename = strdup(f.name());
        log_d("Found file %s", filename);

        files.add(filename);
        f.close();
    }

    // Check for overflow
    if (doc.overflowed()) {
        log_e("Overflowed JSON document!");
        return req->send(507, "text/plain", "Overflowed JSON document!");
    }

    // Send it
    auto* res = req->beginResponseStream("application/json");
    serializeJson(doc, *res);
    req->send(res);

    log_i("Successfully listed directory");
}

static void
send_jsonified_data_file(String filename, AsyncWebServerRequest* req)
{
    // We should send the file converted to JSON
    log_i("Opening file \"%s\"", filename.c_str());

    File file = LittleFS.open(filename);
    if (!file || file.isDirectory()) {
        log_e("Could not open recording file \"%s\"", filename);
        file.close();
        return req->send(404, "text/plain", "Recording not found.");
    }

    // Get file size
    uint32_t num_points = file.size() / sizeof(mpu_data_t);
    log_i("Found %lu datapoints in %s", num_points, filename.c_str());

    auto* res = req->beginChunkedResponse(
        "application/json",
        [file](uint8_t* buf, size_t max_len, size_t idx) mutable -> size_t {
            // Write up to "maxLen" bytes into "buffer" and return the amount written.
            // index equals the amount of bytes that have been already sent
            // You will be asked for more data until 0 is returned
            // Keep in mind that you can not delay or yield waiting for more data!
            if (!file) // finished
                return 0;

            size_t written = 0;

            if (idx == 0) { // at start
                const char header[] = "{\"data\":[";
                size_t header_len = sizeof(header) - 1; // don't want NUL

                memcpy(buf, header, header_len);
                written += header_len;
            }

            if (!file.available()) {
                buf[0] = ']';
                buf[1] = '}';
                log_d("Finished JSON, closing file");
                file.close();
                return 2;
            }

            while (file.available()) {
                mpu_data_t mpu_data;
                file.read(reinterpret_cast<uint8_t*>(&mpu_data), sizeof(mpu_data));
                auto doc = mpu_data.to_json();

                size_t json_size = measureJson(doc) + 1; // json + comma
                if (json_size > max_len - written)
                    return written;

                serializeJson(doc, buf + written, json_size);
                written += json_size;
                buf[written - 1] = ',';
            }
            log_d("Finished writing file");
            return --written; // drop trailing comma
        }
    );
    req->send(res);

#if 0
    // Create JSON and fill it
    log_d("Creating JSON document");

    uint32_t doc_size =
        16                                    // for the "data" entry
        + MPU_DATA_JSON_ARR_SIZE * num_points // MPU_DATA_JSON_ARR_SIZE bytes per entry
        + 8;                                  // extra 8 bytes at end for safety
    if (doc_size > ESP.getMaxAllocHeap()) {
        log_e("JSON doc too big! %lu > %lu", doc_size, ESP.getFreeHeap());
        return req->send(507);
    }

    DynamicJsonDocument doc(doc_size);
    auto data = doc.createNestedArray("data");

    while (file.available()) {
        mpu_data_t mpu_data;
        file.read(reinterpret_cast<uint8_t*>(&mpu_data), sizeof(mpu_data));
        data.add(mpu_data.to_json());
    }

    // Check for overflow
    if (doc.overflowed()) {
        log_e("Overflowed JSON document! Size: %lu", doc.memoryUsage());
        return req->send(507, "text/plain", "Overflowed JSON document!");
    }

    // Send it
    size_t buf_size = measureJson(doc);
    auto* buf = (char*)malloc(buf_size);
    if (!buf) {
        log_e("JSON doc too big! %lu > %lu", buf_size, ESP.getMaxAllocHeap());
        return req->send(507);
    }

    serializeJson(doc, buf, buf_size);
    auto* res = req->beginChunkedResponse(
        "application/json",
        [buf, buf_size](uint8_t* out_buf, size_t len, size_t idx) -> size_t {
            // Write up to "maxLen" bytes into "buffer" and return the amount written.
            // index equals the amount of bytes that have been already sent
            // You will be asked for more data until 0 is returned
            // Keep in mind that you can not delay or yield waiting for more data!

            size_t available = buf_size - idx;
            size_t to_copy = min(available, len);

            log_d("%lu bytes of output buffer", len);
            log_d("Copying %lu/%lu bytes, starting at %lu", to_copy, available, idx);

            if (to_copy == 0) {
                free(buf); // We're done
                return 0;
            }

            strncpy((char*)out_buf, buf + idx, to_copy);
            return to_copy;
        }
    );
    req->send(res);

    log_i("Successfully sent data file");
#endif
}

bool
web_server_setup()
{
    /**
     * API paths
     */
    server.addHandler(new AsyncCallbackJsonWebHandler(
        "/recordings/start",
        [](AsyncWebServerRequest* req, JsonVariant& json_var) {
            const JsonObject& json = json_var.as<JsonObject>();

            log_i("Request body:");
            serializeJsonPretty(json, Serial);

            uint32_t rec_time = json["time"];
            if (!rec_time)
                return req->send(422, "text/plain", "JSON \"time\" key missing");

            data_start_recording(rec_time);
            return req->send(200, "text/plain", "Recording started");
        }
    ));

    server.on("/recordings", HTTP_GET, [](AsyncWebServerRequest* req) {
        // Check if we should list the directory
        if (req->url().length() <= 12) // "/recordings" or "/recordings/"
            return list_dir("/recs", req);

        // Send a specific file
        String filename = "/recs/" + req->url().substring(12);

        // Check if we should send the raw data file
        if (req->hasParam("raw")) {
            log_i("Raw file requested.");
            return req->send(LittleFS, filename, "", true);
        }
        return send_jsonified_data_file(filename, req);
    });

    server.on("/recordings", HTTP_DELETE, [](AsyncWebServerRequest* req) {
        StaticJsonDocument<16> doc;
        doc["success"] = data_clear_recordings();

        // Send it
        auto* res = req->beginResponseStream("application/json");
        serializeJson(doc, *res);
        req->send(res);

        if (doc["success"])
            log_i("Successfully cleared recordings");
        else
            log_e("Failed to clear recordings");
    });

    /**
     * Static data files
     */
    // Redirect index.html to root
    server.rewrite("/index.html", "/");

    // Get last modified time
    File index = LittleFS.open("/index.html.gz");
    time_t last_modified = index.getLastWrite();
    index.close();

    // Setup static file handler
    server.serveStatic("/", LittleFS, "/")
        .setDefaultFile("index.html")
        .setLastModified(gmtime(&last_modified))
        .setCacheControl("public, max-age=604800, no-cache, "
                         "stale-if-error=86400, stale-while-revalidate=86400");

    /**
     * Server-side events
     */
    events.onConnect([](AsyncEventSourceClient* client) {
        IPAddress ip = client->client()->localIP();

        if (client->lastId())
            log_i(
                "SSE client %s reconnected. Last message received: %u",
                ip.toString().c_str(), client->lastId()
            );
        else
            log_i("New SSE client: %s", ip.toString().c_str());

        // send event with message {"connected":true}, id current millis
        // and set reconnect delay to 1 second
        client->send("", NULL, millis(), 1000);
    });
    server.addHandler(&events);

    /**
     * Start the server
     */
    server.begin();

    return true; // success
}

void
web_server_send_event(const char* name, const JsonDocument& json)
{
    String msg;
    serializeJson(json, msg);

    events.send(msg.c_str(), name, millis());
    // log_d("Free heap: %lu B/%lu B", ESP.getFreeHeap(), ESP.getHeapSize());
}
