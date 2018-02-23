#pragma once

#include "USART.h"

#define DELAY(cycles)                                                          \
  do {                                                                         \
    for (volatile uint32_t i = 0; i < cycles; i++)                             \
      asm("nop");                                                              \
  } while (false)

typedef LPUART_1 DebugUART;

template <typename T> void DebugPrint(T data) { DebugUART::write(data); }
