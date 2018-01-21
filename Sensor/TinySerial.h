#pragma once

#include <stddef.h>
#include <stdint.h>

extern volatile size_t count;

extern "C" void __vector_2(void)
    __attribute__((signal, used, externally_visible));

extern "C" void __vector_PCINT0_FALLING(void)
    __attribute__((signal, used, externally_visible));


const long TINY_SERIAL_BAUD = 115200;
const long TINY_SERIAL_TX_DELAY = 56;
const long TINY_SERIAL_RX_DELAY_INITIAL = 30;
const long TINY_SERIAL_RX_DELAY = 49;
const long TINY_SERIAL_RX_DELAY_LAST = 30;

class TinySerial {
private:
  const static size_t BUFFER_SIZE = 64;

  int txPin_;
  int rxPin_;

  int txPinMask_;
  int rxPinMask_;

  long delay_;
  long receiveInitialDelay_;
  long receiveLastDelay_;

  uint8_t head_ = 0, tail_ = 0;
  uint8_t buffer_[BUFFER_SIZE];

  void writeRaw(uint8_t byte);

  static TinySerial *listeningSerial_;
  static int listeningTxPinMask_;
  static int listeningRxPinMask_;

public:
  TinySerial(int txPin, int rxPin);

  void begin();

  void write(uint8_t byte);
  void write(uint8_t *buf, size_t len);
  void write(const char *str);

  size_t available();

  int read();

  void flush();

  friend void __vector_2();
  friend void __vector_PCINT0_FALLING();
};
