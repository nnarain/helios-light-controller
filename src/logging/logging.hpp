#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <Arduino.h>
#include <stdarg.h>

namespace logger
{
    void log(const char* module, const char* fmt, ...);
}

#endif
