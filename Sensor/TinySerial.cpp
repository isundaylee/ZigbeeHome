#include "TinySerial.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

TinySerial::TinySerial(int txPin, int rxPin) : txPin_(txPin), rxPin_(rxPin) {
  txPinMask_ = 1 << txPin_;
  rxPinMask_ = 1 << rxPin_;
}

void TinySerial::writeRaw(uint8_t byte) {
  PORTB &= ~txPinMask_;
  _delay_loop_2(delay_);

  for (uint8_t mask = 1; mask != 0; mask <<= 1) {
    if ((byte & mask) == 0) {
      PORTB &= ~txPinMask_;
      _delay_loop_2(delay_);
    } else {
      PORTB |= txPinMask_;
      _delay_loop_2(delay_);
    }
  }

  PORTB |= txPinMask_;
  _delay_loop_2(delay_);
}

void TinySerial::begin(long baud) {
  baud_ = baud;

  // We have these hand-tuned settings hard-coded for common baud rates.
  // For other cases, we use the formula in the last else clause.
  if (F_CPU == 8000000 && baud == 9600) {
    delay_ = 216;
  } else if (F_CPU == 8000000 && baud == 115200) {
    delay_ = 15;
  } else {
    delay_ = (F_CPU / baud - 4) / 4;
  }

  DDRB |= txPinMask_;
  PORTB |= txPinMask_;
}

void TinySerial::write(uint8_t byte) {
  cli();
  this->writeRaw(byte);
  sei();
}
