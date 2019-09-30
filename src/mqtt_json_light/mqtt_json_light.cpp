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
    const char* module = "HASS";

    const char* CMD_TOPIC = "/cmd";
    const char* STATE_TOPIC = "/state";

    const char* CURRENT_STATE_FILE = "/currrent_state";

    String cmd_topic;
    String state_topic;

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

                lights::setRGB(r, g, b);

                // Load the effect
                const String effect = current_state["effect"];
                logger::log(module, "Setting effect to %s", effect.c_str());
                lights::setEffect(effect);
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

        logger::log(module, "Updating state: %s", output.c_str());

        mqtt::publish(state_topic.c_str(), output.c_str(), true);
    }

    void mqttCallback(char* topic, byte* payload, unsigned int length)
    {
        String topic_str(topic);

        if (topic_str == cmd_topic)
        {
            if (!deserializeJson(doc, payload, length))
            {
                ArduinoJson::JsonObject cmd = doc.as<ArduinoJson::JsonObject>();

                if (cmd.containsKey("state"))
                {
                    String state = cmd["state"];

                    if (state == String("ON"))
                    {
                        logger::log(module, "Enabling lights");
                        lights::on();
                    }
                    else if (state == String("OFF"))
                    {
                        logger::log(module, "Disabling lights");
                        lights::off();
                    }

                    current_state["state"] = state;
                }
                if (cmd.containsKey("color"))
                {
                    current_state["color"] = cmd["color"].as<ArduinoJson::JsonObject>();

                    const auto r = cmd["color"]["r"].as<uint8_t>();
                    const auto g = cmd["color"]["g"].as<uint8_t>();
                    const auto b = cmd["color"]["b"].as<uint8_t>();

                    logger::log(module, "Setting color to %d, %d, %d", r, g, b);
                    lights::setRGB(r, g, b);
                }
                if (cmd.containsKey("effect"))
                {
                    current_state["effect"] = cmd["effect"].as<String>();

                    logger::log(module, "Setting effect to: %s", current_state["effect"].as<const char*>());
                    lights::setEffect(current_state["effect"]);
                }

                publishState();
                updateSavedState();
            }
            else
            {
                logger::log(module, "Failed to deserialize payload recieved from %s", topic);
                for (int i = 0; i < length; ++i)
                {
                    Serial.print((char)payload[i]);
                }
                Serial.println();
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

    }
}
