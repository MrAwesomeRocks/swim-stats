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
#include "connections.hpp"

#include "config.h"

#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>

#ifdef USE_STATIC_IP
static const IPAddress static_ip(STATIC_IP_ADDR);
static const IPAddress static_gateway(STATIC_GATEWAY);
static const IPAddress static_mask(STATIC_SUBNET_MASK);
static const IPAddress static_dns(STATIC_DNS_SERVER);
#endif

bool
wifi_connect()
{
#if defined(HOSTNAME_PREFIX) || defined(AP_SSID_PREFIX) || !defined(AP_PSK)
    // Get the chip ID, we'll need it later
    String esp32_id(WIFI_getChipId(), HEX);
    esp32_id.toUpperCase();
#endif

    // Set up the WiFi in the mode we'll want it at the end
    WiFi.mode(WIFI_STA);

    // Create our WiFi manager
    WiFiManager wm;
    wm.setDebugOutput(true, ARDUHAL_LOG_COLOR_D "[------][D][WiFiManager] UNKNOWN(): ");

    // Save user preferences
    wm.setRestorePersistent(true);

#ifdef USE_STATIC_IP
    // Set a default static IP
    wm.setSTAStaticIPConfig(static_ip, static_gateway, static_mask, static_dns);
#endif

    // Show static IP options on the config portal
    wm.setShowStaticFields(true);
    wm.setShowDnsFields(true);

    // Cosmetic config portal setup
    wm.setConfigPortalTimeout(60 * 3);
    wm.setDarkMode(true);
    wm.setScanDispPerc(true);

    // Set our hostname
#ifdef HOSTNAME
    wm.setHostname(HOSTNAME);
#else
    wm.setHostname(HOSTNAME_PREFIX + esp32_id);
#endif

    // Set the access point SSID
#ifdef AP_SSID
    String ap_name = AP_SSID;
#else
    String ap_name = AP_SSID_PREFIX + esp32_id;
#endif

    // Set the access point password
#ifdef AP_PSK
    String ap_psk = AP_PSK;
#else
    String ap_psk = esp32_id;
#endif

    return wm.autoConnect(ap_name.c_str(), ap_psk.c_str());
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

bool
mdns_setup()
{
    // Get chip ID
    String esp32_id(WIFI_getChipId(), HEX);
    esp32_id.toUpperCase();

    // Create hostname
    String hostname(HOSTNAME_PREFIX + esp32_id);

    // Start mDNS
    if (!MDNS.begin(hostname.c_str()))
        return false;

    // Add web server service
    MDNS.addService("http", "tcp", 80);

    log_i("mDNS running at %s.local", hostname.c_str());
    return true;
}
