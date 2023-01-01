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
    log_i("Opening directory %s", dir.c_str());

    File rec_dir = LittleFS.open(dir);
    if (!rec_dir || !rec_dir.isDirectory()) {
        log_e("Could not open directory!");
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
        return req->send(500, "text/plain", "Overflowed JSON document!");
    }

    // Send it
    auto* res = req->beginResponseStream("application/json");
    serializeJson(doc, *res);
    req->send(res);

    log_i("Successfully listed directory");
}

bool
web_server_setup()
{
    // API paths
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

        // We should send the file converted to JSON
        log_i("Opening file %s", filename.c_str());

        File file = LittleFS.open(filename);
        if (!file || file.isDirectory()) {
            log_e("Could not open recording file \"%s\"", file.path());
            file.close();
            return req->send(404, "text/plain", "Recording not found.");
        }

        // Get file size
        uint32_t num_points = file.size() / sizeof(mpu_data_t);
        log_i("Found %lu datapoints in %s", num_points, filename.c_str());

        // Create JSON and fill it
        log_d("Creating JSON document");
        DynamicJsonDocument doc(16 + 160 * num_points + 8); // extra 8 bytes at end
        auto data = doc.createNestedArray("data");

        while (file.available()) {
            mpu_data_t mpu_data;
            file.read(reinterpret_cast<uint8_t*>(&mpu_data), sizeof(mpu_data));
            data.add(mpu_data.to_json());
        }

        // Check for overflow
        if (doc.overflowed()) {
            log_e("Overflowed JSON document!");
            return req->send(500, "text/plain", "Overflowed JSON document!");
        }

        // Send it
        auto* res = req->beginResponseStream("application/json");
        serializeJson(doc, *res);
        req->send(res);

        log_i("Successfully sent data file");
    });

    // Static data files
    server.rewrite("/index.html", "/");

    // TODO(nino): find a smart way to do last modified time
    server.serveStatic("/", LittleFS, "/")
        .setDefaultFile("index.html")
        // .setLastModified("Wed, 28 Dec 2022 11:15:13 GMT");
        .setCacheControl("public, max-age=604800, no-cache, "
                         "stale-if-error=86400, stale-while-revalidate=86400");

    // Handle server-side events
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

    // Start the server
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
