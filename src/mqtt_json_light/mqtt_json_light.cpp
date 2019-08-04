#include "mqtt_json_light.hpp"

#include "../mqtt_driver/mqtt_driver.hpp"
#include "../light_driver/light_driver.hpp"
#include "../conf/conf.hpp"

#include <ArduinoJson.hpp>
#include <FS.h>

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
    const char* STATE_TOPIC = "/state";

    const char* CURRENT_STATE_FILE = "/currrent_state";

    String cmd_topic;
    String state_topic;

    Effect effect = Effect::SOLID;
    Color color;

    ArduinoJson::DynamicJsonDocument doc(256);
    ArduinoJson::StaticJsonDocument<256> current_state;

    void updateSavedState()
    {
        auto file = SPIFFS.open(CURRENT_STATE_FILE, "w+");
        if (file)
        {
            ArduinoJson::serializeJson(current_state, file);
        }
        else
        {
            Serial.printf("[HASS] Unable to update saved state \n");
        }
        
        file.close();
    }

    void loadInitialState()
    {
        auto file = SPIFFS.open(CURRENT_STATE_FILE, "r");
        if (file)
        {
            if (ArduinoJson::deserializeJson(current_state, file))
            {
                Serial.printf("[HASS] Failed to load initial state\n");
            }
        }
        else
        {
            Serial.printf("[HASS] Failed to publish initial state\n");
        }
        file.close();
    }

    void publishState()
    {
        String output;
        ArduinoJson::serializeJson(current_state, output);

        mqtt::publish(state_topic.c_str(), output.c_str());
    }

    void mqttCallback(char* topic, byte* payload, unsigned int length)
    {
        String topic_str(topic);

        if (topic_str == cmd_topic)
        {
            if (!deserializeJson(doc, payload))
            {
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

                    current_state["state"] = state;
                }
                if (cmd.containsKey("color"))
                {
                    uint8_t r = cmd["color"]["r"].as<uint8_t>();
                    uint8_t g = cmd["color"]["g"].as<uint8_t>();
                    uint8_t b = cmd["color"]["b"].as<uint8_t>();

                    lights::setRGB(r, g, b);

                    current_state["color"] = cmd["color"];
                }
                if (cmd.containsKey("effect"))
                {
                    // TODO

                    current_state["effect"] = cmd["effect"];
                }

                updateSavedState();
                publishState();
            }
        }
    }

    void connectionCallback()
    {
        const auto prefix = conf::getPrefix();

        cmd_topic = prefix + String(CMD_TOPIC);
        state_topic = prefix + String(STATE_TOPIC);

        Serial.printf("[HASS] Command Topic: %s\n", cmd_topic.c_str());
        Serial.printf("[HASS] State Topic: %s\n", state_topic.c_str());

        Serial.printf("[HASS] Subscribing to %s\n", cmd_topic.c_str());
        mqtt::subscribe(cmd_topic.c_str());

        // Publish the saved state
        loadInitialState();
        publishState();
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
