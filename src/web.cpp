/**
 * @file web.cpp
 * @author Nino Maruszewski (nino.maruszewski@gmail.com)
 * @brief Web utilities.
 * @version 1.0
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
#include "web.hpp"

#include <Arduino.h>
#include <WiFiManager.h>

bool
wifi_connect()
{
    WiFi.mode(WIFI_STA);

    WiFiManager wm;

    wm.setDarkMode(true);
    wm.setRestorePersistent(true);
    wm.setDebugOutput(true, ARDUHAL_LOG_COLOR_D "[------][D][WiFiManager] UNKNOWN(): ");

    wm.setSTAStaticIPConfig(
        IPAddress(192, 168, 1, 150), IPAddress(192, 168, 1, 1),
        IPAddress(255, 255, 255, 0), IPAddress(1, 1, 1, 1)
    );
    wm.setShowStaticFields(true);
    wm.setShowDnsFields(true);

    wm.setConfigPortalTimeout(60 * 3);
    wm.setScanDispPerc(true);

    return wm.autoConnect();
}

const char*
wifi_manager_version()
{
    return WM_VERSION_STR;
}

void
wifi_print_status()
{
    const char* modes[] = {"NULL", "STA", "AP", "STA+AP"};

    log_d("Wifi mode: %s", modes[WiFi.getMode()]);

    const char* status;
    switch (WiFi.status()) {
        case WL_IDLE_STATUS:
            status = "IDLE";
            break;
        case WL_NO_SSID_AVAIL:
            status = "NO SSID AVAILABLE";
            break;
        case WL_SCAN_COMPLETED:
            status = "SCAN COMPLETED";
            break;
        case WL_CONNECTED:
            status = "CONNECTED";
            break;
        case WL_CONNECT_FAILED:
            status = "CONNECTION FAILED";
            break;
        case WL_CONNECTION_LOST:
            status = "CONNECTION LOST";
            break;
        case WL_DISCONNECTED:
            status = "DISCONNECTED";
            break;
        default:
            status = "UNKNOWN";
            break;
    }
    log_d("Status: %s", status);

    log_d("Mac address: %s", WiFi.macAddress().c_str());

    String ssid = WiFi.SSID();
    log_d("SSID (%lu): %s", ssid.length(), ssid.c_str());

    String pass = WiFi.psk();
    log_d("Passphrase (%lu): %s", pass.length(), pass.c_str());

    log_d("Channel: %u (primary)", WiFi.channel());
    log_d("BSSID: %s", WiFi.BSSIDstr().c_str());
    log_d("Strength: %d dB", WiFi.RSSI());

    log_d("IPv4: %s", WiFi.localIP().toString().c_str());
    log_d("IPv6: %s", WiFi.localIPv6().toString().c_str());
    log_d("Gateway: %s", WiFi.gatewayIP().toString().c_str());
    log_d("Subnet Mask: %s", WiFi.subnetMask().toString().c_str());
    log_d("DNS: %s", WiFi.dnsIP().toString().c_str());

    log_d("Broadcast IP: %s", WiFi.broadcastIP().toString().c_str());
    log_d("Network ID: %s", WiFi.networkID().toString().c_str());
}
