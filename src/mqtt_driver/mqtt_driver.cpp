#include "mqtt_driver.hpp"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

namespace
{
    WiFiClient wifi;
    PubSubClient client(wifi);

    mqtt::ConnectionCallback connection_callback_ = nullptr;
}

namespace mqtt
{
    void connect(const char* broker, uint16_t port)
    {
        client.setServer(broker, port);

        while(!client.connected()) 
        {
            Serial.println("[MQTT] Attempting MQTT connection...");
            // Random client ID
            String clientId = "esp-light:";
            clientId += String(random(0xFFFF), HEX);

            // attempt connection
            if (client.connect(clientId.c_str())) {
                Serial.println("[MQTT] MQTT connected");

                if (connection_callback_ != nullptr)
                {
                    connection_callback_();
                }
            }
            else {
                Serial.print("[MQTT] mqtt connection failed: ");
                Serial.println(client.state());
                delay(5000);
            }
        }
    }

    bool isConnected()
    {
        return client.connected();
    }

    void spin()
    {
        client.loop();
    }

    void subscribe(const char *topic)
    {
        client.subscribe(topic);
    }

    void publish(const char* topic, const char* payload, bool retained)
    {
        client.publish(topic, payload, retained);
    }

    void setMqttCallback(MqttCallback fn)
    {
        client.setCallback(fn);
    }

    void setConnectionCallback(ConnectionCallback fn)
    {
        connection_callback_ = fn;
    }
}
