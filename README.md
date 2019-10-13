# ESP Light Controller

[![GitHub release](https://img.shields.io/github/release/nnarain/esp-light-controller.svg)](https://github.com/nnarain/esp-light-controller/releases)

Firmware for an ESP-01 base light strip controller. Compliant with MQTT JSON Lights: https://www.home-assistant.io/components/light.mqtt/

Build
-----

This is an Arduino project. Simply open it in the Arduino IDE and upload to the ESP-01 device.

Usage
-----

The device must be configured before it can be used. Configuration occurs through an AT like serial interface.

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

This repo contains schematics and PCB layouts for the ESP light controller.

Install submodules to pull in external KiCAD libraries.

```
$ git submodule update --init --recursive
```

Checkout the [release](https://github.com/nnarain/esp-light-controller/releases) page for KiCAD outputs.
