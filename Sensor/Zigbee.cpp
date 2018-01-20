#include "Zigbee.h"

#include <util/delay.h>

#include <string.h>

const int Zigbee::INPUT_FLUSH_PREDELAY = 50;
const int Zigbee::WAKEUP_DELAY = 200;
const int Zigbee::QUERY_DELAY = 200;

Zigbee::Zigbee(int txPin, int rxPin) : serial_(txPin, rxPin) {}

bool Zigbee::query(const char *query, uint8_t *out, size_t responseSize) {
  this->wakeUp();
  this->flushSerialInput();

  serial_.write((uint8_t)0xFE);
  serial_.write(query);
  serial_.write((uint8_t)0xFF);

  // We account for the 0xFB prefix here. We also consume it before returning.
  if (this->waitForBytes(1 + responseSize, QUERY_DELAY)) {
    serial_.read();

    for (size_t i = 0; i < responseSize; i++) {
      out[i] = serial_.read();
    }

    return true;
  } else {
    return false;
  }
}

bool Zigbee::waitForBytes(size_t count, size_t maxWaitTime) {
  for (size_t i = 0; i < maxWaitTime; i++) {
    if ((size_t)serial_.available() >= count) {
      return true;
    }

    _delay_ms(1);
  }

  return false;
}

void Zigbee::flushSerialInput() {
  _delay_ms(INPUT_FLUSH_PREDELAY);

  for (size_t i = serial_.available(); i > 0; i--) {
    serial_.read();
  }
}

void Zigbee::wakeUp() {
  this->flushSerialInput();

  serial_.write((uint8_t)0xFE);
  serial_.write((uint8_t)0xFF);

  if (this->waitForBytes(2, WAKEUP_DELAY)) {
    this->flushSerialInput();
  } else {
    this->wakeUp();
  }
}

void Zigbee::begin(long baud) {
  serial_.begin(baud);

  // for (uint8_t i = 0; 1; i++) {
  //   uint8_t buf[64];
  //   for (int j=0; j<64; j++) {
  //     buf[j] = i;
  //   }
  //   this->broadcast(buf, 64);
  //   _delay_ms(500);
  // }
}

void Zigbee::broadcast(uint8_t *buf, size_t len) {
  this->wakeUp();

  size_t totalLen = 2 + len;

  serial_.write((uint8_t)0xFC);
  serial_.write((uint8_t)totalLen);
  serial_.write((uint8_t)0x01);
  serial_.write((uint8_t)0x01);

  for (size_t i = 0; i < len; i++) {
    serial_.write((uint8_t)buf[i]);
  }
}

void Zigbee::broadcast(const char *buf) {
  this->broadcast((uint8_t *)buf, strlen(buf));
}

void Zigbee::getMacAddress(uint8_t *out) { this->query("\x01\x06", out, 8); }
