#pragma once

#include "GPIO.h"
#include "RingBuffer.h"
#include "Tick.h"
#include "USART.h"
#include "Utils.h"

const bool ZIGBEE_VERBOSE_LOGGING = true;

const uint16_t ZIGBEE_CMD_AF_DATA_CONFIRM = 0x4480;
const uint16_t ZIGBEE_CMD_AF_DATA_REQUEST = 0x2401;
const uint16_t ZIGBEE_CMD_AF_REGISTER = 0x2400;
const uint16_t ZIGBEE_CMD_APP_CNF_BDB_COMMISSIONING_NOTIFICATION = 0x4F80;
const uint16_t ZIGBEE_CMD_APP_CNF_BDB_SET_CHANNEL = 0x2F08;
const uint16_t ZIGBEE_CMD_APP_CNF_BDB_START_COMMISSIONING = 0x2F05;
const uint16_t ZIGBEE_CMD_SYS_OSAL_NV_WRITE = 0x2109;
const uint16_t ZIGBEE_CMD_SYS_RESET_IND = 0x4180;
const uint16_t ZIGBEE_CMD_ZB_PERMIT_JOINING_REQUEST = 0x2608;
const uint16_t ZIGBEE_CMD_ZDO_MGMT_PERMIT_JOIN_RSP = 0x45B6;
const uint16_t ZIGBEE_CMD_ZDO_NODE_DESC_RSP = 0x4582;
const uint16_t ZIGBEE_CMD_ZDO_STARTUP_FROM_APP = 0x2540;
const uint16_t ZIGBEE_CMD_ZDO_STATE_CHANGE_IND = 0x45C0;

const uint16_t ZIGBEE_CMD_SYNC_REPLY_MASK = 0x4000;
const uint16_t ZIGBEE_CMD_INVALID = 0xFFFF;

const uint8_t ZIGBEE_ROLE_COORDINATOR = 0x00;
const uint8_t ZIGBEE_ROLE_ROUTER = 0x01;
const uint8_t ZIGBEE_ROLE_END_DEVICE = 0x02;

const uint8_t ZIGBEE_COMMISSIONING_MODE_INITIALIZATION = 0x00;
const uint8_t ZIGBEE_COMMISSIONING_MODE_TOUCH_LINK = 0x01;
const uint8_t ZIGBEE_COMMISSIONING_MODE_NETWORK_STEERING = 0x02;
const uint8_t ZIGBEE_COMMISSIONING_MODE_NETWORK_FORMATION = 0x04;
const uint8_t ZIGBEE_COMMISSIONING_MODE_NETWORK_FINDING_AND_BINDING = 0x08;

const uint8_t ZIGBEE_ZDO_STATE_HOLD = 0x00;
const uint8_t ZIGBEE_ZDO_STATE_INIT = 0x01;
const uint8_t ZIGBEE_ZDO_STATE_NWK_DISC = 0x02;
const uint8_t ZIGBEE_ZDO_STATE_NWK_JOINING = 0x03;
const uint8_t ZIGBEE_ZDO_STATE_REJOINING = 0x04;
const uint8_t ZIGBEE_ZDO_STATE_END_DEVICE_UNAUTH = 0x05;
const uint8_t ZIGBEE_ZDO_STATE_END_DEVICE = 0x06;
const uint8_t ZIGBEE_ZDO_STATE_ROUTER = 0x07;
const uint8_t ZIGBEE_ZDO_STATE_COORD_STARTING = 0x08;
const uint8_t ZIGBEE_ZDO_STATE_ZB_COORD = 0x09;
const uint8_t ZIGBEE_ZDO_STATE_NWK_ORPHAN = 0x0A;

const uint8_t ZIGBEE_STATUS_SUCCESS = 0x00;
const uint8_t ZIGBEE_STATUS_NO_ROUTE = 0xCD;
const uint8_t ZIGBEE_STATUS_TIMEOUT = 0xFF;

const uint16_t ZIGBEE_SYNC_COMMANDS[] = {
    ZIGBEE_CMD_AF_DATA_REQUEST,
    ZIGBEE_CMD_AF_REGISTER,
    ZIGBEE_CMD_APP_CNF_BDB_SET_CHANNEL,
    ZIGBEE_CMD_APP_CNF_BDB_START_COMMISSIONING,
    ZIGBEE_CMD_SYS_OSAL_NV_WRITE,
    ZIGBEE_CMD_ZB_PERMIT_JOINING_REQUEST,
    ZIGBEE_CMD_ZDO_STARTUP_FROM_APP};

const uint32_t ZIGBEE_TIMEOUT_SYNC = 5000;
const uint32_t ZIGBEE_TIMEOUT_RESET = 5000;
const uint32_t ZIGBEE_TIMEOUT_DATA_CONFIRM = 1000;

template <typename USART, typename ResetPin> class Zigbee {
private:
  RingBuffer<uint8_t, 128> command_;

  uint16_t pendingSyncCommand_;
  uint8_t lastSyncCommandStatus_;
  int pendingDataConfirmTransId_ = -1;
  uint8_t lastDataConfirmStatus_;

  int commandDataLength() { return command_[1]; }
  int commandTotalLength() { return 1 + 1 + 2 + commandDataLength() + 1; }

public:
  bool isPowered = false;
  uint8_t zdoState = 0;

  Zigbee() {}

  void init() {
    USART::init();

    ResetPin::GPIO::init();
    ResetPin::setMode(GPIO_MODE_OUTPUT);
  }

  uint8_t calculateFCS() {
    uint8_t fcs = 0;
    for (int i = 1; i < commandTotalLength() - 1; i++) {
      fcs ^= command_[i];
    }
    return fcs;
  }

  char *humanReadableZDOState(uint8_t state) {
    switch (state) {
    case ZIGBEE_ZDO_STATE_HOLD:
      return "HOLD";
    case ZIGBEE_ZDO_STATE_INIT:
      return "INIT";
    case ZIGBEE_ZDO_STATE_NWK_DISC:
      return "NWK_DISC";
    case ZIGBEE_ZDO_STATE_NWK_JOINING:
      return "NWK_JOINING";
    case ZIGBEE_ZDO_STATE_REJOINING:
      return "REJOINING";
    case ZIGBEE_ZDO_STATE_END_DEVICE_UNAUTH:
      return "END_DEVICE_UNAUTH";
    case ZIGBEE_ZDO_STATE_END_DEVICE:
      return "END_DEVICE";
    case ZIGBEE_ZDO_STATE_ROUTER:
      return "ROUTER";
    case ZIGBEE_ZDO_STATE_COORD_STARTING:
      return "COORD_STARTING";
    case ZIGBEE_ZDO_STATE_ZB_COORD:
      return "ZB_COORD";
    case ZIGBEE_ZDO_STATE_NWK_ORPHAN:
      return "NWK_ORPHAN";
    default:
      return "!!!UNKNOWN!!!";
    }
  }

  void processCommand() {
    // command_[1]: Payload length.
    // command_[2-3]: Command.
    // command_[4-]: Payload.

    if (calculateFCS() != command_[command_.size() - 1]) {
      command_.clear();
      return;
    }

    uint16_t cmd = (command_[2] << 8) | command_[3];

    if constexpr (ZIGBEE_VERBOSE_LOGGING) {
      DebugPrint("[Zigbee] Received command ");
      DebugPrintHex(cmd);
      DebugPrint(":");
      for (int i = 4; i < 4 + command_[1]; i++) {
        DebugPrint(" ");
        DebugPrintHex(command_[i], false);
      }
      DebugPrint(".\n");
    }

    for (int i = 0;
         i < sizeof(ZIGBEE_SYNC_COMMANDS) / sizeof(ZIGBEE_SYNC_COMMANDS[0]);
         i++) {
      uint16_t syncCommand = ZIGBEE_SYNC_COMMANDS[i];
      if (cmd == (syncCommand | ZIGBEE_CMD_SYNC_REPLY_MASK)) {
        if (pendingSyncCommand_ == syncCommand) {
          pendingSyncCommand_ = ZIGBEE_CMD_INVALID;
          lastSyncCommandStatus_ = command_[4];
        }
        command_.clear();
        return;
      }
    }

    switch (cmd) {
    case ZIGBEE_CMD_SYS_RESET_IND:
      isPowered = true;
      break;
    case ZIGBEE_CMD_ZDO_STATE_CHANGE_IND:
      zdoState = command_[4];
      DebugPrint("[Zigbee] ZDO state changed to ");
      DebugPrint(humanReadableZDOState(zdoState));
      DebugPrint(".\n");
      break;
    case ZIGBEE_CMD_AF_DATA_CONFIRM:
      if (command_[6] == pendingDataConfirmTransId_) {
        lastDataConfirmStatus_ = command_[4];
        pendingDataConfirmTransId_ = -1;
      }
      break;
    case ZIGBEE_CMD_ZDO_NODE_DESC_RSP:
    case ZIGBEE_CMD_ZDO_MGMT_PERMIT_JOIN_RSP:
    case ZIGBEE_CMD_APP_CNF_BDB_COMMISSIONING_NOTIFICATION:
      // Things we don't care about
      break;
    default:
      DebugPrint("[Zigbee] !!!! Unrecognized command ");
      DebugPrintHex(cmd);
      DebugPrint(". \n");
      break;
    }

    command_.clear();
  }

  void process() {
    int c;
    while ((c = USART::read()) != -1) {
      uint8_t byte = static_cast<uint8_t>(c);

      if (command_.empty()) {
        // We're not in the middle of a command
        if (byte != 0xFE) {
          continue;
        }

        command_.push(byte);
      } else {
        // We're in the middle of a command
        command_.push(byte);

        if (command_.size() >= 2 && command_.size() == commandTotalLength()) {
          processCommand();
        }
      }
    }
  }

  uint8_t sendSyncCommand(uint16_t command, uint8_t dataSize, uint8_t *data,
                          bool wait = true) {
    Tick::delay(10);

    if (wait) {
      pendingSyncCommand_ = command;
    }

    USART::write(static_cast<uint8_t>(0xFE));
    USART::write(static_cast<uint8_t>(dataSize));
    USART::write(static_cast<uint8_t>((command & 0xFF00) >> 8));
    USART::write(static_cast<uint8_t>((command & 0x00FF)));

    uint8_t FCS = dataSize ^ ((command & 0xFF00) >> 8) ^ (command & 0x00FF);

    for (uint8_t i = 0; i < dataSize; i++) {
      FCS ^= data[i];
      USART::write(data[i]);
    }

    USART::write(FCS);

    if (wait) {
      uint32_t start = Tick::value;
      while (!Tick::hasElapsedSince(start, ZIGBEE_TIMEOUT_SYNC)) {
        process();
        if (pendingSyncCommand_ == ZIGBEE_CMD_INVALID) {
          return lastSyncCommandStatus_;
        }
      }
      return ZIGBEE_STATUS_TIMEOUT;
    } else {
      return ZIGBEE_STATUS_SUCCESS;
    }
  }

  uint8_t resetSettings() {
    uint8_t data[] = {0x03, 0x00, 0x00, 0x01, 0x03};
    uint8_t status;
    if ((status = sendSyncCommand(ZIGBEE_CMD_SYS_OSAL_NV_WRITE, sizeof(data),
                                  data)) != ZIGBEE_STATUS_SUCCESS) {
      DebugPrintHex(status);
      return status;
    }

    return reset();
  }

  uint8_t setRole(uint8_t role) {
    uint8_t data[] = {0x87, 0x00, 0x00, 0x01, role};
    return sendSyncCommand(ZIGBEE_CMD_SYS_OSAL_NV_WRITE, sizeof(data), data);
  }

  uint8_t setChannelMask(bool primary, uint32_t channelMask) {
    uint8_t data[] = {primary ? 0x01 : 0x00, (channelMask & 0x000000FF) >> 0,
                      (channelMask & 0x0000FF00) >> 8,
                      (channelMask & 0x00FF0000) >> 16,
                      (channelMask & 0xFF000000) >> 24};
    return sendSyncCommand(ZIGBEE_CMD_APP_CNF_BDB_SET_CHANNEL, sizeof(data),
                           data);
  }

  uint8_t startCommissioning(uint8_t commissioningMode) {
    uint8_t data[] = {commissioningMode};
    return sendSyncCommand(ZIGBEE_CMD_APP_CNF_BDB_START_COMMISSIONING,
                           sizeof(data), data);
  }

  uint8_t registerEndpoint(uint8_t endpoint, uint16_t appProfId,
                           uint16_t appDeviceId, uint8_t appDevVer,
                           uint8_t numInClusters, uint16_t *inClusters,
                           uint8_t numOutClusters, uint16_t *outClusters) {
    uint8_t data[0x49];
    uint8_t *ptr = data;

    *(ptr++) = endpoint;
    *(ptr++) = ((appProfId & 0x00FF) >> 0);
    *(ptr++) = ((appProfId & 0xFF00) >> 8);
    *(ptr++) = ((appDeviceId & 0x00FF) >> 0);
    *(ptr++) = ((appDeviceId & 0xFF00) >> 8);
    *(ptr++) = appDeviceId;
    *(ptr++) = 0;

    *(ptr++) = numInClusters;
    for (uint8_t i = 0; i < numInClusters; i++) {
      *(ptr++) = ((inClusters[i] & 0x00FF) >> 0);
      *(ptr++) = ((inClusters[i] & 0xFF00) >> 8);
    }

    *(ptr++) = numOutClusters;
    for (uint8_t i = 0; i < numOutClusters; i++) {
      *(ptr++) = ((outClusters[i] & 0x00FF) >> 0);
      *(ptr++) = ((outClusters[i] & 0xFF00) >> 8);
    }

    return sendSyncCommand(ZIGBEE_CMD_AF_REGISTER, ptr - data, data);
  }

  uint8_t dataRequest(uint16_t dstAddr, uint8_t dstEndpoint,
                      uint8_t srcEndpoint, uint16_t clusterId, uint8_t transId,
                      uint8_t options, uint8_t radius, uint8_t dataLen,
                      uint8_t *msgData) {
    uint8_t data[0x8A];
    uint8_t *ptr = data;

    *(ptr++) = ((dstAddr & 0x00FF) >> 0);
    *(ptr++) = ((dstAddr & 0xFF00) >> 8);
    *(ptr++) = dstEndpoint;
    *(ptr++) = srcEndpoint;
    *(ptr++) = ((clusterId & 0x00FF) >> 0);
    *(ptr++) = ((clusterId & 0xFF00) >> 8);
    *(ptr++) = transId;
    *(ptr++) = options;
    *(ptr++) = radius;
    *(ptr++) = dataLen;

    for (uint8_t i = 0; i < dataLen; i++) {
      *(ptr++) = msgData[i];
    }

    pendingDataConfirmTransId_ = transId;

    uint8_t result =
        sendSyncCommand(ZIGBEE_CMD_AF_DATA_REQUEST, ptr - data, data);
    if (result != ZIGBEE_STATUS_SUCCESS) {
      return result;
    }

    uint32_t start = Tick::value;
    while (!Tick::hasElapsedSince(start, ZIGBEE_TIMEOUT_DATA_CONFIRM)) {
      process();
      if (pendingDataConfirmTransId_ == -1) {
        return lastDataConfirmStatus_;
      }
    }

    return ZIGBEE_STATUS_TIMEOUT;
  }

  uint8_t permitJoiningRequest(uint16_t dest, uint8_t timeout) {
    uint8_t data[] = {dest & 0x00FF, (dest & 0xFF00) >> 8, timeout};
    return sendSyncCommand(ZIGBEE_CMD_ZB_PERMIT_JOINING_REQUEST, sizeof(data),
                           data);
  }

  uint8_t startup(uint16_t delay = 0) {
    uint8_t data[] = {delay & 0x00FF, (delay & 0xFF00) >> 8};
    return sendSyncCommand(ZIGBEE_CMD_ZDO_STARTUP_FROM_APP, sizeof(data), data);
  }

  uint8_t reset() {
    ResetPin::clear();
    Tick::delay(100);
    ResetPin::set();

    isPowered = false;
    zdoState = ZIGBEE_ZDO_STATE_HOLD;

    uint32_t start = Tick::value;
    while (!Tick::hasElapsedSince(start, ZIGBEE_TIMEOUT_RESET)) {
      process();
      if (isPowered) {
        return ZIGBEE_STATUS_SUCCESS;
      }
    }
    return ZIGBEE_STATUS_TIMEOUT;
  }
};
