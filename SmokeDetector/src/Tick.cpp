#include "Tick.h"

/* static */ volatile uint32_t Tick::value = 0;

extern "C" void vector_systick(void) { Tick::value++; }
