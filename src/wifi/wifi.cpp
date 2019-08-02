#include "wifi.hpp"
#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace wifi
{
    void init(const char* ssid, const char* password)
    {
        Serial.print("[WIFI] Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }

        randomSeed(micros());

        Serial.println("");
        Serial.println("[WIFI] Connected");
        Serial.print("[WIFI] IP address: ");
        Serial.println(WiFi.localIP());
    }
}