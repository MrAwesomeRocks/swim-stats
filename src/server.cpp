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

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Server on port 80 (HTTP)
AsyncWebServer server(80);

// Event source on /events
AsyncEventSource events("/events");

/**
 * @brief Unified template processor.
 *
 * @param var Template variable
 * @return
 */
static String
template_processor(const String& var)
{
    switch (var[0]) {
        case 'v':
            // ESP32
            /* code */
            break;

        default:
            break;
    }
    return String();
}

bool
web_server_setup()
{
    // The root path loads index.html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/index.html", "", false, template_processor);
    });

    // index.html redirects back to root
    server.on("/index.html", HTTP_ANY, [](AsyncWebServerRequest* request) {
        request->redirect("/");
    });

    // Static data files
    server.serveStatic("/", LittleFS, "/");

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
        client->send("{\"connected\":true}", NULL, millis(), 1000);
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
