#include "logging.hpp"

#include <Arduino.h>
#include <stdarg.h>

namespace logger
{
    void log(const char* module, const char* fmt, ...)
    {
        char buffer[64];

        va_list args;
        va_start(args, fmt);
        vsprintf(buffer, fmt, args);

        Serial.printf("[%s] ", module);
        Serial.printf("%s\n", buffer);

        va_end(args);
    }
}
