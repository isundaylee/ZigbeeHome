#include "Zigbee.h"

#include <avr/io.h>

#include <util/delay.h>

#include <string.h>

const int Zigbee::WAKEUP_MESSAGE_LENGTH = 10;
const int Zigbee::WAKEUP_WAIT_TIME_INITIAL = 200;
const int Zigbee::WAKEKE_WAIT_TIME_BACKOFF_FACTOR = 2;
const int Zigbee::WAKEUP_WAIT_TIME_MAX = 200;
const int Zigbee::WAKEUP_MAX_TRIES = 7;
const int Zigbee::QUERY_WAIT_TIME = 500;
const int Zigbee::MESSAGE_GAP_DELAY = 20;

Zigbee::Zigbee(int txPin, int rxPin) : serial_(txPin, rxPin), lastOutTime_(0) {}

void Zigbee::beforeMessage() {
  while (Tick::since(lastOutTime_) < MESSAGE_GAP_DELAY) {
    _delay_ms(1);
  }

  serial_.flush();
}

void Zigbee::afterMessage() { lastOutTime_ = Tick::value; }

bool Zigbee::query(const char *query, uint8_t *out, size_t responseSize) {
  Zigbee::beforeMessage();

  serial_.write((uint8_t)0xFE);
  serial_.write(query);
  serial_.write((uint8_t)0xFF);

  // We account for the 0xFB prefix here. We also consume it before returning.
  if (this->waitForBytes(1 + responseSize, QUERY_WAIT_TIME)) {
    serial_.read();

    for (size_t i = 0; i < responseSize; i++) {
      out[i] = serial_.read();
    }

    Zigbee::afterMessage();
    return true;
  } else {
    Zigbee::afterMessage();
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

bool Zigbee::wakeUp() {
  Zigbee::beforeMessage();

  size_t wait_time = WAKEUP_WAIT_TIME_INITIAL;
  for (size_t tries = 0; tries < WAKEUP_MAX_TRIES; tries++) {
    serial_.flush();

    for (size_t i = 0; i < WAKEUP_MESSAGE_LENGTH; i++) {
      serial_.write((uint8_t)0x00);
    }

    if (this->waitForBytes(2, wait_time)) {
      Zigbee::afterMessage();
      return true;
    }

    if (wait_time < WAKEUP_WAIT_TIME_MAX) {
      wait_time *= WAKEKE_WAIT_TIME_BACKOFF_FACTOR;
    }
  }

  return false;
}

void Zigbee::begin() { serial_.begin(); }

bool Zigbee::broadcast(uint8_t *buf, size_t len) {
  size_t totalLen = 2 + len;

  Zigbee::beforeMessage();

  serial_.write((uint8_t)0xFC);
  serial_.write((uint8_t)totalLen);
  serial_.write((uint8_t)0x01);
  serial_.write((uint8_t)0x01);

  for (size_t i = 0; i < len; i++) {
    serial_.write((uint8_t)buf[i]);
  }

  Zigbee::afterMessage();
  return true;
}

bool Zigbee::broadcast(const char *buf) {
  return this->broadcast((uint8_t *)buf, strlen(buf));
}

bool Zigbee::unicast(addr_t addr, uint8_t *buf, size_t len) {
  size_t totalLen = 4 + len;

  Zigbee::beforeMessage();

  serial_.write((uint8_t)0xFC);
  serial_.write((uint8_t)totalLen);
  serial_.write((uint8_t)0x03);
  serial_.write((uint8_t)0x01);
  serial_.write((uint8_t) ((addr & 0xFF00) >> 8));
  serial_.write((uint8_t) (addr & 0x00FF));

  for (size_t i = 0; i < len; i++) {
    serial_.write((uint8_t)buf[i]);
  }

  Zigbee::afterMessage();
  return true;
}

bool Zigbee::unicast(addr_t addr, const char *buf) {
  return this->unicast(addr, (uint8_t *)buf, strlen(buf));
}

bool Zigbee::getMacAddress(uint8_t *out) {
  if (!this->query("\x01\x06", out, 8)) {
    return false;
  } else {
    return true;
  }
}

int Zigbee::getNetworkState() {
  uint8_t buf[1];

  if (!this->query("\x01\x02", buf, 1)) {
    return ERROR;
  }

  return buf[0];
}
