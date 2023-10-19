#pragma once

#include <Arduino.h>

namespace WebConfig {
    void setupWifi(void (*operate)(String));
    void handleClient();
}
