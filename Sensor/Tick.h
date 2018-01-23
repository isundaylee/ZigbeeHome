#pragma once

#include <stddef.h>

typedef size_t tick_t;

class Tick {
public:
    static tick_t value;

    static void begin();
    static tick_t since(tick_t t);
};
