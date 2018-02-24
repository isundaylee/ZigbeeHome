#pragma once

#include "USART.h"

typedef LPUART_1 DebugUART;

template <typename T> void DebugPrint(T data) { DebugUART::write(data); }

void DebugPrintHexRaw(int number) {
  if (number >= 0x10) {
    DebugPrintHexRaw(number >> 4);
  }

  if (number % 16 < 10) {
    DebugUART::write(static_cast<uint8_t>('0' + (number % 16)));
  } else {
    DebugUART::write(static_cast<uint8_t>('A' + ((number % 16) - 10)));
  }
}

void DebugPrintHex(int number) {
  DebugUART::write(static_cast<uint8_t>('0'));
  DebugUART::write(static_cast<uint8_t>('x'));

  if (number == 0) {
    DebugUART::write(static_cast<uint8_t>('0'));
    return;
  }

  DebugPrintHexRaw(number);
}
