#include "conf.hpp"
#include "../logging/logging.hpp"

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.hpp>

namespace
{
    const char * module = "CONF";

    const char * configuration_file = "/config.json";

    const char* KEY_SSID = "ssid";
    const char* KEY_PASS = "pass";
    const char* KEY_BROKER = "broker";
    const char* KEY_PORT = "port";
    const char* KEY_PREFIX = "prefix";
    const char* KEY_COUNT = "count";

    const char* KEYS[] = {
        KEY_SSID, KEY_PASS, KEY_BROKER, KEY_PORT, KEY_PREFIX, KEY_COUNT
    };

    ArduinoJson::StaticJsonDocument<256> root;

    bool found_config = false;

    void updateConfig()
    {
        File conf_file = SPIFFS.open(configuration_file, "w+");
        if (conf_file)
        {
            ArduinoJson::serializeJson(root, conf_file);
        }
        else
        {
            logger::log(module, "Failed to open %s", configuration_file);
        }

        conf_file.close();
    }

    void loadConfig()
    {
        // Attempt to read the configuration from memory
        File conf_file = SPIFFS.open(configuration_file, "r");
        if (conf_file)
        {
            if (!ArduinoJson::deserializeJson(root, conf_file))
            {
                const auto length = sizeof(KEYS) / sizeof(KEYS[0]);
                for (auto i = 0; i < length; ++i)
                {
                    found_config = root.containsKey(KEYS[i]);
                    if (!found_config)
                    {
                        logger::log(module, "%s not found", KEYS[i]);
                    }
                }
            }
        }
        else
        {
            logger::log(module, "Failed to open %s", configuration_file);
        }
        conf_file.close();
    }

    void dumpConfig()
    {
        String ssid = root[KEY_SSID];
        String pass = root[KEY_PASS];
        String broker = root[KEY_BROKER];
        int port = root[KEY_PORT].as<int>();
        String prefix = root[KEY_PREFIX];
        int led_count = root[KEY_COUNT].as<int>();

        // TODO: There is probably a better way to do this.
        logger::log(module, "Current configuration:");
        logger::log(module, "WIFI: SSID=%s, PASS=%s", ssid.c_str(), pass.c_str());
        // logger::log(module, "MQTT: BROKER=%s, PORT=%d, PREFIX=%s", broker.c_str(), port, prefix.c_str());
        // logger::log(module, "LEDS: COUNT=%d", led_count);
    }
}

namespace conf
{
    void init()
    {
        if (SPIFFS.begin())
        {
            loadConfig();
            dumpConfig();
        }
        else
        {
            logger::log(module, "Error mounting file system. Run AT+FMT0 to format");
        }
    }

    bool found()
    {
        return found_config;
    }

    /**
     * AT Interface
     * ------------
     *
     * AT+WIFI-<ssid>,<pass>
     * AT+LEDS-<count>
     * AT+MQTT-<broker>,<port>
     *
     */
    void spin()
    {
        while(Serial.available() > 0)
        {
            char cmd[4];
            char arg[64];

            String at_cmd = Serial.readString();

            sscanf(at_cmd.c_str(), "AT+%04s-%s", cmd, arg);

            const auto cmd_str = String(cmd);

            if (cmd_str == "WIFI")
            {
                char* ssid = strtok(arg, ",");
                char* pass = strtok(0, ",");

                logger::log(module, "WIFI: SSID=%s, PASS=%s", ssid, pass);

                root[KEY_SSID] = ssid;
                root[KEY_PASS] = pass;

                updateConfig();
            }
            else if (cmd_str == "LEDS")
            {
                int led_count = atoi(arg);

                logger::log(module, "LEDS: COUNT=%d", led_count);

                root[KEY_COUNT] = led_count;

                updateConfig();
            }
            else if (cmd_str == "MQTT")
            {
                char* broker = strtok(arg, ",");
                char* port_s = strtok(0, ",");
                char* prefix = strtok(0, ",");

                int port = atoi(port_s);

                logger::log(module, "MQTT: BROKER=%s, PORT=%d, PREFIX=%s", broker, port, prefix);

                root[KEY_BROKER] = broker;
                root[KEY_PORT] = port;
                root[KEY_PREFIX] = prefix;

                updateConfig();
            }
            else if (cmd_str == "DUMP")
            {
                dumpConfig();
            }
            else if (cmd_str == "FMT0")
            {
                logger::log(module, "Formatting file system...");
                if (SPIFFS.format())
                {
                    logger::log(module, "Formatting successful!");
                }
                else
                {
                    logger::log(module, "Failed to format file system");
                }
            }
            else
            {
                logger::log(module, "Unrecognized command: %s", at_cmd.c_str());
            }
        }
    }

    String getSSID()
    {
        return root[KEY_SSID];
    }

    String getPassword()
    {
        return root[KEY_PASS];
    }

    String getMqttBroker()
    {
        return root[KEY_BROKER];
    }

    uint16_t getMqttPort()
    {
        uint16_t port = root[KEY_PORT].as<uint16_t>();
        return port;
    }

    int getLedCount()
    {
        int led_count = root[KEY_COUNT].as<int>();
        return led_count;
    }

    String getPrefix()
    {
        return root[KEY_PREFIX];
    }
}
