# Step motor test

A software to test the 28BYJ-48 step motor using the AccelStepper library.

Copy the contents of `src/wifi_cfg.json.template` to `src/wifi_cfg.json` and configure accordingly. The "ap" boolean value defines whether the device works as an Access Point or a normal WiFi client. In the latter case, defining an "ip" of "0.0.0.0" results in a dynamic IP address, otherwise the property defines a static IP address.
