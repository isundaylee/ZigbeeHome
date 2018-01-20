#pragma once

#include "stdint.h"

class TinySerial {
private:
  int txPin_;
  int rxPin_;

  int txPinMask_;
  int rxPinMask_;

  long baud_;
  long delay_;

  void writeRaw(uint8_t byte);

public:
  TinySerial(int txPin, int rxPin);

  void begin(long baud);
  void write(uint8_t byte);
};
