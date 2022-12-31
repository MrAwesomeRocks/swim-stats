#pragma once

#include <Arduino.h>

#define _UTILS__ISO8601_LEN 26 // 2022-12-31T20:06:38-0600

inline String
iso8601_str()
{
    struct tm tm;
    if (!getLocalTime(&tm))
        return "";

    char buf[_UTILS__ISO8601_LEN];
    strftime(buf, _UTILS__ISO8601_LEN, "%FT%T%z", &tm);

    return buf;
}
