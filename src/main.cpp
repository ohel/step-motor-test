#include <Arduino.h>
#include <string.h>
#include <AccelStepper.h>
#include "webconfig.h"

// Make D4, D5, D6, D7 correspond to A, B, C, D or IN1, IN2, IN3, IN4.
// We want to have a sequence of IN1, IN3, IN2, IN4.
AccelStepper _stepmotor(AccelStepper::HALF4WIRE, D4, D6, D5, D7);

void rotate(String direction) {
    Serial.println(direction);

    if (direction == "l") {
      _stepmotor.moveTo(_stepmotor.currentPosition() - 4076);
    }
    if (direction == "r") {
      _stepmotor.moveTo(_stepmotor.currentPosition() + 4076);
    }
    Serial.println(_stepmotor.currentPosition());
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
    WebConfig::setupWifi(rotate);
}

void loop() {
    WebConfig::handleClient();
    _stepmotor.run();
}
