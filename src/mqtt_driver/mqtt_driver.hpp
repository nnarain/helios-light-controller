
#ifndef MQTT_DRIVER_HPP
#define MQTT_DRIVER_HPP

#include <Arduino.h>
#include <cstdint>

namespace mqtt
{
    using MqttCallback = void(*)(char*, byte*, unsigned int);
    using ConnectionCallback = void(*)();

    void connect(const char* broker, uint16_t port);
    bool isConnected();
    void spin();

    void subscribe(const char* topic);
    void publish(const char* topic, const char* payload);

    void setMqttCallback(MqttCallback fn);
    void setConnectionCallback(ConnectionCallback fn);
}

#endif
