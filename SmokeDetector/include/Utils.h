#pragma once

#include "USART.h"

#define WAIT_UNTIL(condition)                                                  \
  do {                                                                         \
    asm("nop");                                                                \
  } while (!(condition))

typedef LPUART_1 DebugUART;

template <typename T> void DebugPrint(T data) { DebugUART::write(data); }

static void DebugPrintHexRaw(int number) {
  if (number >= 0x10) {
    DebugPrintHexRaw(number >> 4);
  }

  if (number % 16 < 10) {
    DebugUART::write(static_cast<uint8_t>('0' + (number % 16)));
  } else {
    DebugUART::write(static_cast<uint8_t>('A' + ((number % 16) - 10)));
  }
}

static void DebugPrintHex(int number, bool prefix = true) {
  if (prefix) {
    DebugUART::write(static_cast<uint8_t>('0'));
    DebugUART::write(static_cast<uint8_t>('x'));
  }

  if (number == 0) {
    DebugUART::write(static_cast<uint8_t>('0'));
    return;
  }

  DebugPrintHexRaw(number);
}

static void DebugPrintDecRaw(int number) {
  if (number >= 10) {
    DebugPrintDecRaw(number / 10);
  }

  DebugUART::write(static_cast<uint8_t>('0' + (number % 10)));
}

static void DebugPrintDec(int number) {
  if (number == 0) {
    DebugUART::write(static_cast<uint8_t>('0'));
    return;
  }

  DebugPrintDecRaw(number);
}
