#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "webconfig.h"
#include "wifi_cfg.h"
#include "index_html.h"
#include "favicon.h"

namespace WebConfig {
    void (*_operate)(String);
    ESP8266WebServer _server(80);

    void handleClient() {
        _server.handleClient();
    }

    void setupWifi(void (*operate)(String)) {
        _operate = operate;

        IPAddress subnet(255, 255, 255, 0);
        bool is_ap =
            _wifi_ip != INADDR_NONE &&
            _wifi_gateway == INADDR_NONE;

        // Note: here WiFi setup only as AP or STA, not both (WIFI_AP_STA).

        Serial.println("");
        if (is_ap) {
            WiFi.mode(WIFI_AP);
            Serial.println("Setting up AP:");
            Serial.print("    SSID: ");
            Serial.println(_wifi_ssid);
            Serial.print("    Password: ");
            if (strlen(_wifi_password) < 8) {
                Serial.println("");
                WiFi.softAP(_wifi_ssid);
            } else {
                Serial.println(_wifi_password);
                WiFi.softAP(_wifi_ssid, _wifi_password);
            }
            Serial.print("    IP/Gateway: ");
            Serial.println(_wifi_ip);
            WiFi.softAPConfig(_wifi_ip, _wifi_ip, subnet);
        } else {
            WiFi.mode(WIFI_STA);
            if (_wifi_ip == INADDR_NONE) {
                Serial.println("Using dynamic IP address.");
            } else {
                WiFi.config(_wifi_ip, _wifi_gateway, subnet);
            }
        }

        WiFi.begin(_wifi_ssid, _wifi_password);

        if (!is_ap) {
            int timeout = 0;
            Serial.print("Connecting to: ");
            Serial.println(_wifi_ssid);
            while (WiFi.status() != WL_CONNECTED && timeout < 20) {
                delay(1000);
                Serial.print(".");
                timeout++;
            }
            Serial.println("");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("WiFi connected using IP address: ");
                Serial.println(WiFi.localIP());
            } else {
                Serial.println("Failed to connect to WiFi.");
            }
        }

        _server.on("/", HTTP_GET, [](){
            Serial.println("GET: /");
            _server.send(200, "text/html", index_html);
        });

        _server.on("/favicon.ico", HTTP_GET, [](){
            Serial.println("GET: /favicon.ico");
            _server.send_P(200, "image/x-icon", favicon, sizeof(favicon_bytes));
        });

        _server.on("/", HTTP_POST, [](){
            Serial.println("POST: /");
            _server.send(200, "text/plain", "");
            String direction = _server.arg("d");
            Serial.print("Got request: ");
            Serial.println(direction);
            _operate(direction);
        });

        _server.begin();
        Serial.println("Server is running.");
    }
};
