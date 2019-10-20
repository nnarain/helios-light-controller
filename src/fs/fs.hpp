#ifndef FS_HPP
#define FS_HPP

#include "../logging/logging.hpp"

#include <FS.h>

namespace fs
{
    void init();

    template<typename T>
    bool store(const char* filename, const T& item)
    {
        auto file = SPIFFS.open(filename, "w+");

        if (file)
        {
            const auto ptr = reinterpret_cast<const uint8_t*>(&item);
            file.write(ptr, sizeof(T));

            return true;
        }
        else
        {
            logger::log("FS", "Failed to store in file: %s", filename);
        }

        return false;
    }

    template<typename T>
    bool load(const char* filename, T& item)
    {
        auto file = SPIFFS.open(filename, "r");

        if (file)
        {
            auto ptr = reinterpret_cast<uint8_t*>(&item);
            file.read(ptr, sizeof(T));

            return true;
        }
        else
        {
            logger::log("FS", "Failed to load from file: %s", filename);
        }
        

        return false;
    }
}

#endif
