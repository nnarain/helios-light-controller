# ESP Light Controller

[![GitHub release](https://img.shields.io/github/release/nnarain/esp-light-controller.svg)](https://github.com/nnarain/esp-light-controller/releases)

Firmware for an ESP-01 base light strip controller. Compliant with MQTT JSON Lights: https://www.home-assistant.io/components/light.mqtt/

Build
-----

This is an Arduino project. Simply open it in the Arduino IDE and upload to the ESP-01 device.

Usage
-----

This can best be used with [ESPHome](https://esphome.io/). The LED output is on `GPIO2`.

Example config:

```
esphome:
  name: livingroom
  platform: ESP8266
  board: esp01

wifi:
  ssid: "<your-ssid>"
  password: "<wifi-password>"

  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "Livingroom Fallback Hotspot"
    password: "BhmK1aDsii2b"

captive_portal:

# Enable logging
logger:

# Enable Home Assistant API
api:

ota:

# Example configuration entry
light:
  - platform: neopixelbus
    name: "LivingRoom"
    type: GRB
    method: BIT_BANG
    pin: GPIO2
    num_leds: 10

```

Hardware
--------

This repo contains schematics and PCB layouts for the ESP light controller.

Install submodules to pull in external KiCAD libraries.

```
$ git submodule update --init --recursive
```

Checkout the [release](https://github.com/nnarain/esp-light-controller/releases) page for KiCAD outputs.
