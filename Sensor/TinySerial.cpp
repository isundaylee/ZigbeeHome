#include "TinySerial.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>
#include <string.h>

volatile size_t count = 0;

/* static */ size_t TinySerial::serialCount_ = 0;
/* static */ TinySerial *TinySerial::allSerials_[8];

TinySerial::TinySerial(int txPin, int rxPin) : txPin_(txPin), rxPin_(rxPin) {
  txPinMask_ = 1 << txPin_;
  rxPinMask_ = 1 << rxPin_;

  allSerials_[serialCount_++] = this;
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
    receiveInitialDelay_ = 130;
    receiveLastDelay_ = 110;
  } else if (F_CPU == 8000000 && baud == 115200) {
    delay_ = 14;
    receiveInitialDelay_ = 11;
    receiveLastDelay_ = 7;
  } else {
    delay_ = (F_CPU / baud - 4) / 4;
    receiveInitialDelay_ = delay_ * 2 / 3;
    receiveLastDelay_ = delay_ * 2 / 3;
  }

  cli();

  DDRB |= txPinMask_;
  DDRB &= ~(rxPinMask_);
  PORTB |= (txPinMask_ | rxPinMask_);

  GIMSK |= (1 << PCIE);
  PCMSK = rxPinMask_;

  sei();
}

size_t TinySerial::available() {
  if (tail_ >= head_) {
    return tail_ - head_;
  } else {
    return (BUFFER_SIZE - tail_ + head_);
  }
}

ISR(PCINT0_vect) {
  // PORTB &= ~8;

  TinySerial *serial = TinySerial::allSerials_[0];

  // FIXME: Currently only first TinySerial can receive.
  if ((PINB & serial->rxPinMask_) != 0) {
    PORTB |= 8;
    return;
  }

  _delay_loop_2(serial->receiveInitialDelay_);

  uint8_t byte = 0;
  for (uint8_t bit = 1; bit != 128; bit <<= 1) {
    if ((PINB & serial->rxPinMask_) != 0) {
      byte += bit;
    }

    _delay_loop_2(serial->delay_);
  }

  if ((PINB & serial->rxPinMask_) != 0) {
    byte += 128;
  }
  _delay_loop_2(serial->receiveLastDelay_);

  GIFR &= ~PCIE;

  if (serial->tail_ !=
      (serial->head_ - 1 + TinySerial::BUFFER_SIZE) % TinySerial::BUFFER_SIZE) {
    serial->buffer_[serial->tail_] = byte;
    serial->tail_ = (serial->tail_ + 1) % TinySerial::BUFFER_SIZE;
  }

  // PORTB |= 8;
}

void TinySerial::write(uint8_t byte) {
  cli();
  this->writeRaw(byte);
  sei();
}

void TinySerial::write(uint8_t *buf, size_t len) {
  cli();
  for (size_t i = 0; i < len; i++) {
    this->writeRaw(buf[i]);
  }
  sei();
}

void TinySerial::write(const char *str) {
  this->write((uint8_t *)str, strlen(str));
}

int TinySerial::read() {
  if (tail_ == head_) {
    return -1;
  }

  int dequeud = buffer_[head_];
  head_ = (head_ + 1) % BUFFER_SIZE;

  return dequeud;
}
