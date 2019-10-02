#include "wifi.hpp"
#include "../logging/logging.hpp"

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace
{
    static constexpr char* module = "WIFI";
    static constexpr int TIMEOUT_MS = 10000;
    static constexpr int DELAY_MS = 500;
    static constexpr int MAX_COUNT = TIMEOUT_MS / DELAY_MS;
}

namespace wifi
{
    void init(const char* ssid, const char* password)
    {
        logger::log(module, "Connecting to %s", ssid);

        if (ssid == nullptr || password == nullptr)
        {
            logger::log(module, "Invalid SSID or password: %s, %s", ssid, password);
            return;
        }

        logger::log(module, "Starting: %s, %s", ssid, password);
        WiFi.begin(ssid, password);

        auto count = 0;

        while (WiFi.status() != WL_CONNECTED && count++ < MAX_COUNT)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.print("\n");

        if (WiFi.status() == WL_CONNECTED)
        {
            randomSeed(micros());
            logger::log(module, "Connected");
            logger::log(module, "IP address: %s", WiFi.localIP().toString().c_str());
        }
        else
        {
            logger::log(module, "WiFi timed out trying to connect");
        }
    }

    bool connect()
    {
        return WiFi.status() == WL_CONNECTED;
    }
}