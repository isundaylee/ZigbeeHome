#include "TinySerial.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>
#include <string.h>

volatile size_t count = 0;

/* static */ TinySerial *TinySerial::listeningSerial_ = NULL;
/* static */ int TinySerial::listeningTxPinMask_ = 0;
/* static */ int TinySerial::listeningRxPinMask_ = 0;

TinySerial::TinySerial(int txPin, int rxPin) : txPin_(txPin), rxPin_(rxPin) {
  txPinMask_ = 1 << txPin_;
  rxPinMask_ = 1 << rxPin_;
}

void TinySerial::writeRaw(uint8_t byte) {
  PORTB &= ~txPinMask_;
  __builtin_avr_delay_cycles(TINY_SERIAL_TX_DELAY);

  for (uint8_t mask = 1; mask != 0; mask <<= 1) {
    if ((byte & mask) == 0) {
      PORTB &= ~txPinMask_;
      __builtin_avr_delay_cycles(TINY_SERIAL_TX_DELAY);
    } else {
      PORTB |= txPinMask_;
      __builtin_avr_delay_cycles(TINY_SERIAL_TX_DELAY);
    }
  }

  PORTB |= txPinMask_;
  __builtin_avr_delay_cycles(TINY_SERIAL_TX_DELAY);
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
    delay_ = 20;
    receiveInitialDelay_ = 11;
    receiveLastDelay_ = 7;
  } else {
    delay_ = (F_CPU / baud - 4) / 4;
    receiveInitialDelay_ = delay_ * 2 / 3;
    receiveLastDelay_ = delay_ * 2 / 3;
  }

  TinySerial::listeningSerial_ = this;
  TinySerial::listeningTxPinMask_ = txPinMask_;
  TinySerial::listeningRxPinMask_ = rxPinMask_;

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

extern "C" void tiny_serial_handle_interrupt(void) {}

ISR(PCINT0_vect, ISR_NAKED) {
  asm("push r24\t\n"
      "push r25\t\n");

  asm("push r15\t\n"
      "push r16\t\n"
      "in r16, %0\t\n"
      "in r15, %1\t\n"
      "and r16, %2\t\n"
      "brne notzero\t\n"
      "out %1, r15\t\n"
      "pop r16\t\n"
      "pop r15\t\n"
      "pop r25\t\n"
      "pop r24\t\n"
      "rjmp __vector_PCINT0_FALLING\t\n"
      "notzero:\t\n"
      "out %1, r15\t\n"
      "pop r16\t\n"
      "pop r15\t\n"
      "pop r25\t\n"
      "pop r24\t\n"
      "reti\t\n"
      :
      : "i"(_SFR_IO_ADDR(PINB)), "i"(_SFR_IO_ADDR(SREG)),
        "r"(TinySerial::listeningRxPinMask_));
}

ISR(__vector_PCINT0_FALLING) {
  TinySerial *serial = TinySerial::listeningSerial_;

  __builtin_avr_delay_cycles(TINY_SERIAL_RX_DELAY_INITIAL);

  uint8_t byte = 0;
  for (uint8_t bit = 1; bit != 128; bit <<= 1) {
    uint8_t not_bit = ~bit;

    if ((PINB & TinySerial::listeningRxPinMask_) != 0) {
      byte |= bit;
    } else {
      byte &= not_bit;
    }

    __builtin_avr_delay_cycles(TINY_SERIAL_RX_DELAY);
  }

  if ((PINB & TinySerial::listeningRxPinMask_) != 0) {
    byte |= 128;
  } else {
    byte &= ~128;
  }
  __builtin_avr_delay_cycles(TINY_SERIAL_RX_DELAY_LAST);

  GIFR &= ~PCIE;

  if (serial->tail_ !=
      (serial->head_ - 1 + TinySerial::BUFFER_SIZE) % TinySerial::BUFFER_SIZE) {
    serial->buffer_[serial->tail_] = byte;
    serial->tail_ = (serial->tail_ + 1) % TinySerial::BUFFER_SIZE;
  }
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
