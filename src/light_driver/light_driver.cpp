#include "light_driver.hpp"

#include <Adafruit_NeoPixel.h>

namespace
{
    Adafruit_NeoPixel* strip = nullptr;
    bool enabled = false;
}

namespace lights
{
    void init(int led_count)
    {
        strip = new Adafruit_NeoPixel(led_count, 2, NEO_GRB | NEO_KHZ800);
        strip->begin();
        strip->clear();
        strip->show();
    }

    void on()
    {
        enabled = true;
    }

    void off()
    {
        strip->clear();
        strip->show();

        enabled = false;
    }

    bool isOn()
    {
        return enabled;
    }

    void spin()
    {
        if (enabled)
        {
            strip->show();
        }
    }

    void setRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        const auto c = Adafruit_NeoPixel::Color(r, g, b);
        strip->fill(c);
    }
}