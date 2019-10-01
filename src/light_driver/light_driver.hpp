#ifndef LIGHT_DRIVER_HPP
#define LIGHT_DRIVER_HPP

#include <Arduino.h>
#include <cstdint>

namespace lights
{
    void init(int led_count);
    void on();
    void off();
    bool isOn();
    void spin();
    void setRGB(uint8_t r, uint8_t g, uint8_t b);

    void setEffect(const String& effect);

    void setSpeed(uint16_t speed);
    void setBrightness(uint8_t b);
}

#endif
