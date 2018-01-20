#pragma once

#include <stddef.h>
#include <stdint.h>

extern volatile size_t count;

extern "C" void __vector_2(void)
    __attribute__((signal, used, externally_visible));

class TinySerial {
private:
  const static size_t BUFFER_SIZE = 64;

  int txPin_;
  int rxPin_;

  int txPinMask_;
  int rxPinMask_;

  long baud_;
  long delay_;
  long receiveInitialDelay_;
  long receiveLastDelay_;

  uint8_t head_ = 0, tail_ = 0;
  uint8_t buffer_[BUFFER_SIZE];

  void writeRaw(uint8_t byte);

  static size_t serialCount_;
  static TinySerial *allSerials_[8];

public:
  TinySerial(int txPin, int rxPin);

  void begin(long baud);

  void write(uint8_t byte);
  void write(uint8_t *buf, size_t len);
  void write(const char *str);

  size_t available();

  int read();

  friend void __vector_2();
};
