#include "Zigbee.h"

#include <avr/io.h>

#include <util/delay.h>

#include <string.h>

const int Zigbee::WAKEUP_MESSAGE_LENGTH = 10;
const int Zigbee::WAKEUP_WAIT_TIME = 20;
const int Zigbee::QUERY_DELAY = 200;
const int Zigbee::MESSAGE_GAP_DELAY = 10;

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

void Zigbee::flushSerialInput() { serial_.flush(); }

void Zigbee::wakeUp() {
  this->flushSerialInput();

  for (size_t i = 0; i < WAKEUP_MESSAGE_LENGTH; i++) {
    serial_.write((uint8_t)0x00);
  }

  if (this->waitForBytes(2, WAKEUP_WAIT_TIME)) {
    _delay_ms(MESSAGE_GAP_DELAY);

    this->flushSerialInput();
  } else {
    this->wakeUp();
  }
}

void Zigbee::begin() { serial_.begin(); }

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
