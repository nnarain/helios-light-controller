#ifndef LIGHT_DRIVER_HPP
#define LIGHT_DRIVER_HPP

#include <cstdint>

namespace lights
{
    void init(int led_count);
    void on();
    void off();
    bool isOn();
    void spin();
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
}

#endif
