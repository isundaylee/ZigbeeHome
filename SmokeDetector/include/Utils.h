#pragma once

#define DELAY(cycles)                                                          \
  do {                                                                         \
    for (volatile uint32_t i = 0; i < cycles; i++)                             \
      asm("nop");                                                              \
  } while (false)
