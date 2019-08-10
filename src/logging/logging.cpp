#include "logging.hpp"

namespace logging
{
    void log(const char* module, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Serial.printf("[%s]", module);
        Serial.printf(fmt, args);
        Serial.println();
        va_end(args);
    }
}
