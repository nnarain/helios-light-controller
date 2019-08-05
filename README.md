# ESP Light Controller

Firmware for an ESP-01 base light strip controller. Compliant with MQTT JSON Lights: https://www.home-assistant.io/components/light.mqtt/

Build
-----

This is an Arduino project. Simply open it in the Arduino IDE and upload to the ESP-01 device.

Usage
-----

The device must be configured before it is used. Configuration occurs through an AT like serial interface.

An AT command has the following structure.

```
AT+<CMD>[-[ARG1,ARG2,...]]
```

| Command | Arguments          | Example                                         |
| ------- | ------------------ | ----------------------------------------------- |
| WIFI    | ssid,pass          | AT+WIFI-MyWifi,MyWifiPassword                   |
| MQTT    | broker,port,prefix | AT+MQTT-192.168.0.10,1883,/home/lights/esplight |
| LEDS    | count              | AT+LEDS-60                                      |
| DUMP    |                    | AT+DUMP                                         |

The `DUMP` command dumps the current config to the serial port.

Hardware
--------

Included in this project under the `design` folder is a set of KiCAD files. In order of open these some minor setup needs to occur.

First install the ESP8266 KiCAD libraries.

```
$ git clone https://github.com/jdunmire/kicad-ESP8266
```

Start KiCAD. Under `Preferences > Configure Paths` add a new enviroment variable called `KIESP_LIB` and set it to `/path/to/kicad-ESP8266`.

KiCAD will now be able to find all the necessary components.
