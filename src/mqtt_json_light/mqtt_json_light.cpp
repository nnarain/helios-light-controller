#include "mqtt_json_light.hpp"

#include "../mqtt_driver/mqtt_driver.hpp"
#include "../light_driver/light_driver.hpp"
#include "../conf/conf.hpp"
#include "../fs/fs.hpp"
#include "../logging/logging.hpp"

#include <ArduinoJson.hpp>

#include <cstdint>

namespace
{
    using strbuf = char[64];

    struct State
    {
        bool on_state;
        uint8_t r;
        uint8_t g;
        uint8_t b;
        strbuf effect;
        uint16_t speed;
        uint8_t brightness;
    };

    State state;

    const char* module = "HASS";

    const char* CMD_TOPIC = "/cmd";
    const char* STATE_TOPIC = "/state";

    const char* CURRENT_STATE_FILE = "/currrent_state";

    ArduinoJson::StaticJsonDocument<256> doc;

    String cmd_topic;
    String state_topic;

    void updateSavedState()
    {
        fs::store(CURRENT_STATE_FILE, state);
    }

    void loadInitialState()
    {
        if (!fs::load(CURRENT_STATE_FILE, state))
        {
            logger::log(module, "Failed to load initial state");
            return;
        }

        if (state.on_state)
        {
            lights::on();
        }
        else
        {
            lights::off();
        }

        lights::setRGB(state.r, state.g, state.b);
        lights::setEffect(String{state.effect});
        lights::setSpeed(state.speed);
        lights::setBrightness(state.brightness);
    }

    void publishState()
    {
        doc["state"] = (state.on_state) ? "ON" : "OFF";

        doc["color"]["r"] = state.r;
        doc["color"]["g"] = state.g;
        doc["color"]["b"] = state.b;

        doc["effect"] = state.effect;
        doc["speed"] = state.speed;
        doc["brightness"] = state.brightness;

        String output;
        ArduinoJson::serializeJson(doc, output);

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
                if (doc.containsKey("state"))
                {
                    String on_state = doc["state"];

                    if (on_state == String("ON"))
                    {
                        logger::log(module, "Enabling lights");
                        state.on_state = true;
                        lights::on();
                    }
                    else if (on_state == String("OFF"))
                    {
                        logger::log(module, "Disabling lights");
                        state.on_state = false;
                        lights::off();
                    }
                }
                if (doc.containsKey("color"))
                {
                    const auto r = doc["color"]["r"].as<uint8_t>();
                    const auto g = doc["color"]["g"].as<uint8_t>();
                    const auto b = doc["color"]["b"].as<uint8_t>();

                    logger::log(module, "Setting color to (%d, %d, %d)", r, g, b);
                    lights::setRGB(r, g, b);

                    state.r = r;
                    state.g = g;
                    state.b = b;
                }
                if (doc.containsKey("effect"))
                {
                    const auto effect = doc["effect"].as<const char*>();

                    logger::log(module, "Setting effect to: %s", effect);
                    lights::setEffect(String{effect});

                    strncpy(state.effect, effect, strlen(effect));
                }
                if (doc.containsKey("brightness"))
                {
                    const auto brightness = doc["brightness"].as<uint8_t>();
                    state.brightness = brightness;

                    lights::setBrightness(brightness);
                }
                if (doc.containsKey("speed"))
                {
                    const auto speed = doc["speed"].as<uint16_t>();
                    state.speed = speed;

                    lights::setSpeed(speed);
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
