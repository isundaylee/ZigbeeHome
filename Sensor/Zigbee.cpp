#include "Zigbee.h"

const int Zigbee::INPUT_FLUSH_PREDELAY = 50;
const int Zigbee::WAKEUP_DELAY = 200;

Zigbee::Zigbee(int txPin, int rxPin) : serial_(rxPin, txPin) {}

void Zigbee::query(char *query, uint8_t *out, size_t responseSize) {
  this->wakeUp();
  this->flushSerialInput();

  serial_.write((uint8_t)0xFE);
  serial_.write(query);
  serial_.write((uint8_t)0xFF);

  // We account for the 0xFB prefix here. We also consume it before returning.
  this->waitForBytes(1 + responseSize);
  serial_.read();

  for (size_t i = 0; i < responseSize; i++) {
    out[i] = serial_.read();
  }
}

void Zigbee::waitForBytes(size_t count) {
  while (serial_.available() < count)
    delay(1);
}

void Zigbee::flushSerialInput() {
  delay(INPUT_FLUSH_PREDELAY);

  for (size_t i = serial_.available(); i > 0; i--) {
    serial_.read();
  }
}

void Zigbee::wakeUp() {
  this->flushSerialInput();

  serial_.write((uint8_t)0xFE);
  serial_.write((uint8_t)0xFF);

  for (size_t t = 0; t < WAKEUP_DELAY; t++) {
    delay(1);

    if (serial_.available() == 2) {
      this->flushSerialInput();
      return;
    }
  }

  this->wakeUp();
}

void Zigbee::begin(int baud) { serial_.begin(baud); }

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

void Zigbee::broadcast(char *buf) { this->broadcast(buf, strlen(buf)); }

void Zigbee::getMacAddress(uint8_t *out) {
  this->query("\x01\x06", out, 8);
}
