#include "fs.hpp"

namespace fs
{
    void init()
    {
        if (!SPIFFS.begin())
        {
            logger::log("FS", "Failed to initialize file system");
        }
    }
}