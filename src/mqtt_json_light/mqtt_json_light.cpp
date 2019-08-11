#include "mqtt_json_light.hpp"

#include "../mqtt_driver/mqtt_driver.hpp"
#include "../light_driver/light_driver.hpp"
#include "../conf/conf.hpp"
#include "../logging/logging.hpp"

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

    const char* module = "HASS";

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
            logger::log(module, "Unable to update saved state");
        }
        
        file.close();
    }

    void loadInitialState()
    {
        auto file = SPIFFS.open(CURRENT_STATE_FILE, "r");
        if (file)
        {
            if (!ArduinoJson::deserializeJson(current_state, file))
            {
                const String state_str = current_state["state"];

                if (state_str == "ON")
                {
                    lights::on();
                }
                else
                {
                    lights::off();
                }

                // Load the initial color
                const auto r = current_state["color"]["r"].as<uint8_t>();
                const auto g = current_state["color"]["g"].as<uint8_t>();
                const auto b = current_state["color"]["b"].as<uint8_t>();

                color.set(r, g, b);

                // Load the effect
                const String effect_str = current_state["effect"];

                if (effect_str == "solid")
                {
                    effect = Effect::SOLID;
                }
            }
            else
            {
                logger::log(module, "Failed to load initial state");
            }
            
        }
        else
        {
            logger::log(module, "Failed to publish initial state");
        }
        file.close();
    }

    void publishState()
    {
        String output;
        ArduinoJson::serializeJson(current_state, output);

        mqtt::publish(state_topic.c_str(), output.c_str(), true);
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
                    }
                    else if (state == String("OFF"))
                    {
                        lights::off();
                    }

                    current_state["state"] = state;
                }
                if (cmd.containsKey("color"))
                {
                    const auto r = cmd["color"]["r"].as<uint8_t>();
                    const auto g = cmd["color"]["g"].as<uint8_t>();
                    const auto b = cmd["color"]["b"].as<uint8_t>();

                    color.set(r, g, b);

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

        logger::log(module, "Command Topic: %s", cmd_topic.c_str());
        logger::log(module, "State Topic: %s", state_topic.c_str());

        logger::log(module, "Subscribing to %s", cmd_topic.c_str());
        mqtt::subscribe(cmd_topic.c_str());

        // Publish the saved state
        publishState();
    }

    void effectSolid()
    {
        if (lights::isOn())
        {
            lights::setRGB(color.r, color.g, color.b);
        }
    }
}

namespace mqttjsonlight
{
    void init()
    {
        mqtt::setMqttCallback(mqttCallback);
        mqtt::setConnectionCallback(connectionCallback);

        loadInitialState();
    }

    void spin()
    {
        switch (effect)
        {
        case Effect::SOLID:
            effectSolid();
            break;
        default:
            break;
        }
    }
}
