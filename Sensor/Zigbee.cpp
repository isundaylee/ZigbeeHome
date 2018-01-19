#include "Zigbee.h"

const int Zigbee::WAKEUP_DELAY = 200;

Zigbee::Zigbee(int txPin, int rxPin) : serial_(rxPin, txPin) {}

void Zigbee::flushSerialInput() {
  for (size_t i = 0; i < serial_.available(); i++) {
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

void Zigbee::broadcast(uint8_t *buf) { this->broadcast(buf, strlen(buf)); }
