#include "light_driver.hpp"

#include <WS2812FX.h>

namespace
{
    WS2812FX* strip = nullptr;
    bool enabled = false;
}

namespace lights
{
    void init(int led_count)
    {
        strip = new WS2812FX(led_count, 2, NEO_GRB | NEO_KHZ800);
        strip->init();
        strip->setBrightness(255);
        strip->setSpeed(2000);

        strip->start();
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
            strip->service();
        }
    }

    void setRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        const auto c = Adafruit_NeoPixel::Color(r, g, b);
        strip->fill(c);
    }
}