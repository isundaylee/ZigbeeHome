#include "Tick.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include <util/delay.h>

static_assert(F_CPU == 8000000,
              "Needs to adjust Tick for non-8Mhz clock frequency.");

/* static */ tick_t Tick::value = 0;

ISR(TIMER1_COMPA_vect) { Tick::value++; }

/* static */ void Tick::reset() { value = 0; }

/* static */ void Tick::begin() {
  TCCR1 |= _BV(CTC1);
  TCCR1 |= _BV(CS13) | _BV(CS12) | _BV(CS10);
  OCR1C = 1;
  TIMSK |= _BV(OCIE1A);
}

/* static */ tick_t Tick::since(tick_t t) { return value - t; }
