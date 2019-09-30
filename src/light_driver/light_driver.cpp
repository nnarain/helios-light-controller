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
        if (strip && enabled)
        {
            strip->service();
        }
    }

    void setRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        strip->setColor(r, g, b);
    }

    void setEffect(const String& effect)
    {
        const auto count = strip->getModeCount();

        // Find the mode by name and set it
        for (auto mode = 0u; mode < count; ++mode)
        {
            const auto mode_name = strip->getModeName(mode);

            if (effect == String(mode_name))
            {
                strip->setMode(mode);
                break;
            }
        }
    }
}