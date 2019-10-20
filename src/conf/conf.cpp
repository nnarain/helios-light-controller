#include "conf.hpp"
#include "../logging/logging.hpp"
#include "../fs/fs.hpp"

#include <Arduino.h>
#include <ArduinoJson.hpp>

namespace
{
    using strbuf = char[64];

    struct Configuration
    {
        strbuf ssid;
        strbuf pass;
        strbuf broker;
        uint16_t port;
        strbuf prefix;
        uint32_t count;

        Configuration()
        {
            ssid[0] = '\0';
            pass[0] = '\0';
            broker[0] = '\0';
            port = 0;
            prefix[0] = '\0';
            count = 0;
        }
    };

    Configuration config;

    const char * module = "CONF";

    const char * CONFIGURATION_FILE = "/config";

    const char* KEY_SSID = "ssid";
    const char* KEY_PASS = "pass";
    const char* KEY_BROKER = "broker";
    const char* KEY_PORT = "port";
    const char* KEY_PREFIX = "prefix";
    const char* KEY_COUNT = "count";

    const char* KEYS[] = {
        KEY_SSID, KEY_PASS, KEY_BROKER, KEY_PORT, KEY_PREFIX, KEY_COUNT
    };

    bool found_config = false;

    void dumpConfig()
    {
        logger::log(module, "Current configuration:");
        logger::log(module, "WIFI: SSID=%s, PASS=%s", config.ssid, config.pass);
        logger::log(module, "MQTT: BROKER=%s, PORT=%d, PREFIX=%s", config.broker, config.port, config.prefix);
        logger::log(module, "LEDS: COUNT=%d", config.count);
    }
}

namespace conf
{
    void init()
    {
        found_config = fs::load(CONFIGURATION_FILE, config);
        dumpConfig();
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
                const char* ssid = strtok(arg, ",");
                const char* pass = strtok(0, ",");

                strncpy(config.ssid, ssid, sizeof(strbuf));
                strncpy(config.pass, pass, sizeof(strbuf));

                logger::log(module, "WIFI: SSID=%s, PASS=%s", ssid, pass);
            }
            else if (cmd_str == "LEDS")
            {
                const auto led_count = atoi(arg);

                config.count = led_count;

                logger::log(module, "LEDS: COUNT=%d", led_count);
            }
            else if (cmd_str == "MQTT")
            {
                const char* broker = strtok(arg, ",");
                const char* port_s = strtok(0, ",");
                const char* prefix = strtok(0, ",");

                const auto port = atoi(port_s);

                strncpy(config.broker, broker, sizeof(strbuf));
                strncpy(config.prefix, prefix, sizeof(strbuf));

                config.port = port;

                logger::log(module, "MQTT: BROKER=%s, PORT=%d, PREFIX=%s", broker, port, prefix);
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

            fs::store(CONFIGURATION_FILE, config);
        }
    }

    String getSSID()
    {
        return String{config.ssid};
    }

    String getPassword()
    {
        return String{config.pass};
    }

    String getMqttBroker()
    {
        return String{config.broker};
    }

    uint16_t getMqttPort()
    {
        return config.port;
    }

    String getPrefix()
    {
        return String{config.prefix};
    }

    int getLedCount()
    {
        return config.count;
    }
}
