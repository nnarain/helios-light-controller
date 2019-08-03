// https://diyprojects.io/esp8266-web-server-part-4-arduinojson-load-save-files-spiffs/

#ifndef CONF_HPP
#define CONF_HPP

#include <Arduino.h>

namespace conf
{
    void init();
    bool found();
    void spin();

    String getSSID();
    String getPassword();
    String getMqttBroker();
    uint16_t getMqttPort();
    int getLedCount();
    String getPrefix();
}

#endif
