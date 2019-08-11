#include "wifi.hpp"
#include "../logging/logging.hpp"

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace
{
    const char* module = "WIFI";
}

namespace wifi
{
    void init(const char* ssid, const char* password)
    {
        logger::log(module, "Connecting to %s", ssid);

        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.print("\n");

        randomSeed(micros());

        logger::log(module, "Connected");
        logger::log(module, "IP address: %s", WiFi.localIP().toString().c_str());
    }
}