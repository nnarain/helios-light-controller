#include "conf.hpp"

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.hpp>

namespace
{
    const char * log_prefix = "[CONF]";
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
            Serial.printf("%s Failed to open %s", log_prefix, configuration_file);
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
                        Serial.printf("%s %s not found\n", log_prefix, KEYS[i]);
                    }
                }
            }

            Serial.printf("[CONF] found config: %d\n", (int)found_config);
        }
        else
        {
            Serial.printf("%s Failed to open %s", log_prefix, configuration_file);
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
        Serial.println();
        Serial.printf("%s WIFI: SSID=%s, PASS=%s\n", log_prefix, ssid.c_str(), pass.c_str());
        Serial.printf("%s MQTT: BROKER=%s, PORT=%d, PREFIX=%s\n", log_prefix, broker.c_str(), port, prefix.c_str());
        Serial.printf("%s LEDS: COUNT=%d\n", log_prefix, led_count);
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
            Serial.printf("%s Error mounting file system. Run AT+FMT0 to format\n", log_prefix);
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
            // Serial.printf("%s CMD: %s\n", log_prefix, at_cmd.c_str());

            sscanf(at_cmd.c_str(), "AT+%04s-%s", cmd, arg);

            const auto cmd_str = String(cmd);

            if (cmd_str == "WIFI")
            {
                char* ssid = strtok(arg, ",");
                char* pass = strtok(0, ",");

                Serial.printf("%s WIFI: SSID=%s, PASS=%s\n\n", log_prefix, ssid, pass);

                root[KEY_SSID] = ssid;
                root[KEY_PASS] = pass;

                updateConfig();
            }
            else if (cmd_str == "LEDS")
            {
                int led_count = atoi(arg);

                Serial.printf("%s LEDS: COUNT=%d\n\n", log_prefix, led_count);

                root[KEY_COUNT] = led_count;

                updateConfig();
            }
            else if (cmd_str == "MQTT")
            {
                char* broker = strtok(arg, ",");
                char* port_s = strtok(0, ",");
                char* prefix = strtok(0, ",");

                int port = atoi(port_s);

                Serial.printf("%s MQTT: BROKER=%s, PORT=%d, PREFIX=%s\n\n", log_prefix, broker, port, prefix);

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
                Serial.printf("%s Formatting file system...\n", log_prefix);
                if (SPIFFS.format())
                {
                    Serial.printf("%s Formatting successful!\n", log_prefix);
                }
                else
                {
                    Serial.printf("%s Failed to format file system\n", log_prefix);
                }
            }
            else
            {
                Serial.printf("%s Unrecognized command: %s", log_prefix, at_cmd.c_str());
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
