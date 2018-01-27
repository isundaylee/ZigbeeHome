#include "ZClient.h"

#include <avr/io.h>

#include <util/delay.h>

#include <string.h>

/* static */ uint8_t ZClient::sendBuffer_[ZClient::SEND_BUFFER_SIZE];

ZClient::ZClient(int txPin, int rxPin, device_t deviceType)
    : bee(txPin, rxPin), deviceType(deviceType) {}

bool ZClient::checkReadyStatus() {
  ready = (bee.getNetworkState() == Zigbee::NETWORK_UP);

  return ready;
}

void ZClient::waitUntilReady() {
  while (!this->checkReadyStatus()) {
    _delay_ms(100);
  }

  ready = true;
}

void ZClient::begin() {
  bee.begin();

  this->waitUntilReady();

  while (!bee.getMacAddress(macAddress)) {
    ready = false;
    this->waitUntilReady();
  }
}

uint8_t *ZClient::encodeByte(uint8_t byte, uint8_t *ptr) {
  if (byte == BEGIN) {
    *(ptr++) = ESC;
    *(ptr++) = ESC_BEGIN;
  } else if (byte == END) {
    *(ptr++) = ESC;
    *(ptr++) = ESC_END;
  } else if (byte == ESC) {
    *(ptr++) = ESC;
    *(ptr++) = ESC_ESC;
  } else {
    *(ptr++) = byte;
  }

  return ptr;
}

uint8_t *ZClient::encodeBytes(uint8_t *buf, size_t len, uint8_t *ptr) {
  for (size_t i = 0; i < len; i++) {
    ptr = this->encodeByte(buf[i], ptr);
  }

  return ptr;
}

uint8_t *ZClient::encodeValue(bool value, uint8_t *ptr) {
  ptr = this->encodeByte(BOOL, ptr);
  ptr = this->encodeByte(value ? 1 : 0, ptr);

  return ptr;
}

uint8_t *ZClient::encodeValue(const char *str, uint8_t *ptr) {
  ptr = this->encodeByte(STRING, ptr);
  ptr = this->encodeByte(strlen(str), ptr);
  return this->encodeBytes((uint8_t *)str, strlen(str), ptr);
}

uint8_t *ZClient::encodeValue(uint8_t byte, uint8_t *ptr) {
  ptr = this->encodeByte(UINT8, ptr);
  ptr = this->encodeByte(byte, ptr);

  return ptr;
}

uint8_t *ZClient::encodeValue(uint16_t number, uint8_t *ptr) {
  ptr = this->encodeByte(UINT16, ptr);
  ptr = this->encodeByte((number & 0xFF00) >> 8, ptr);
  ptr = this->encodeByte(number & 0x00FF, ptr);

  return ptr;
}
