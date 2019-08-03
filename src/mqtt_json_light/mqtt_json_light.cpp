#include "mqtt_json_light.hpp"

#include "../mqtt_driver/mqtt_driver.hpp"
#include "../light_driver/light_driver.hpp"
#include "../conf/conf.hpp"

#include <ArduinoJson.hpp>

#include <cstdint>

namespace
{
    enum class Effect
    {
        SOLID
    };

    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        void set(uint8_t r, uint8_t g, uint8_t b)
        {
            this->r = r;
            this->g = g;
            this->b = b;
        }
    };

    const char* CMD_TOPIC = "/cmd";
    const char* state_topic = "/state";

    String cmd_topic;
    // String state_topic;

    Effect effect = Effect::SOLID;
    Color color;

    ArduinoJson::DynamicJsonDocument doc(1024);

    void mqttCallback(char* topic, byte* payload, unsigned int length)
    {
        String topic_str(topic);

        if (topic_str == cmd_topic)
        {
            deserializeJson(doc, payload);

            ArduinoJson::JsonObject cmd = doc.as<ArduinoJson::JsonObject>();

            if (cmd.containsKey("state"))
            {
                String state = cmd["state"];

                if (state == String("ON"))
                {
                    lights::on();
                    lights::setRGB(color.r, color.g, color.b);
                }
                else if (state == String("OFF"))
                {
                    lights::off();
                }
            }
            if (cmd.containsKey("color"))
            {
                uint8_t r = cmd["color"]["r"].as<uint8_t>();
                uint8_t g = cmd["color"]["g"].as<uint8_t>();
                uint8_t b = cmd["color"]["b"].as<uint8_t>();

                lights::setRGB(r, g, b);

                color.set(r, g, b);
            }
            if (cmd.containsKey("effect"))
            {
                // TODO
            }
        }
    }

    void connectionCallback()
    {
        const auto prefix = conf::getPrefix();
        cmd_topic = prefix + String(CMD_TOPIC);

        Serial.printf("[HASS] Subscribing to %s\n", cmd_topic.c_str());
        mqtt::subscribe(cmd_topic.c_str());
    }
}

namespace mqttjsonlight
{
    void init()
    {
        mqtt::setMqttCallback(mqttCallback);
        mqtt::setConnectionCallback(connectionCallback);
    }
}
