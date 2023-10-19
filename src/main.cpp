#include "Arduino.h"
#include "string.h"

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

#include "wifi_cfg.h"
#include "index_html.h"
#include "favicon.h"

#include <AccelStepper.h>

// Make D4, D5, D6, D7 correspond to A, B, C, D or IN1, IN2, IN3, IN4.
// We want to have a sequence of IN1, IN3, IN2, IN4.
AccelStepper _stepmotor(AccelStepper::HALF4WIRE, D4, D6, D5, D7);

ESP8266WebServer _server(80);

static void rotate(String direction) {
    Serial.println(direction);

    if (direction == "l") {
      _stepmotor.moveTo(_stepmotor.currentPosition() - 4076);
    }
    if (direction == "r") {
      _stepmotor.moveTo(_stepmotor.currentPosition() + 4076);
    }
    Serial.println(_stepmotor.currentPosition());
}

void setupWifi() {
    IPAddress subnet(255, 255, 255, 0);
    bool is_ap = _wifi_gateway.toString() == "0.0.0.0";

    Serial.println("");
    if (is_ap) {
        Serial.println("Setting up AP:");
        Serial.print("    SSID: ");
        Serial.println(_wifi_ssid);
        WiFi.mode(WIFI_AP_STA);
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
        WiFi.config(_wifi_ip, _wifi_gateway, subnet);
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
    _server.on("/", HTTP_POST, [](){
        Serial.println("POST: /");
        _server.send(200, "text/plain", "");
        String direction = _server.arg("d");
        Serial.print("Got request: ");
        Serial.println(direction);
        rotate(direction);
    });

    _server.begin();
    Serial.println("Server is running.");
}

void setupMotor() {
    _stepmotor.setMaxSpeed(1019.0);
    _stepmotor.setAcceleration(250.0);
    _stepmotor.setSpeed(0);
    _stepmotor.moveTo(4076);
}

void setupPins() {
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    setupPins();
    setupMotor();
    setupWifi();
}

void loop() {
    _server.handleClient();
    _stepmotor.run();
}
