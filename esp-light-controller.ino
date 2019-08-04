//
// ESP8266 based light strip controller
//
// Compatible with HA MQTT Lights Component: https://www.home-assistant.io/components/light.mqtt/
//
// @author Natesh Narain
// @date July 19, 2019
//

#include "src/wifi/wifi.hpp"
#include "src/mqtt_driver/mqtt_driver.hpp"
#include "src/light_driver/light_driver.hpp"
#include "src/mqtt_json_light/mqtt_json_light.hpp"
#include "src/conf/conf.hpp"

namespace
{
  bool configuration_ready = false;
  bool system_initialized = false;
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println();

  conf::init();
}

void loop()
{
  if (!configuration_ready && conf::found())
  {
    // System configuration is now ready
    Serial.println("[MAIN] System config is ready");
    configuration_ready = true;
  }

  if (configuration_ready && !system_initialized)
  {
    // Setup wifi
    const auto ssid = conf::getSSID();
    const auto pass = conf::getPassword();

    wifi::init(ssid.c_str(), pass.c_str());

    // Setup LEDs
    const auto led_count = conf::getLedCount();

    lights::init(led_count);

    // Initialize MQTT Lights
    mqttjsonlight::init();

    // System is initialized
    system_initialized = true;
    Serial.println("[MAIN] System initialized");
  }

  if (system_initialized)
  {
    // Connect to MQTT broker
    if (!mqtt::isConnected())
    {
      const auto broker = conf::getMqttBroker();
      const auto port = conf::getMqttPort();

      mqtt::connect(broker.c_str(), port);
    }

    // spin
    mqtt::spin();
    lights::spin();
    mqttjsonlight::spin();
  }

  // Spin conf to allow runtime configuration
  conf::spin();
}
