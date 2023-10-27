#include "drivers/owl_ble.h"

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "owl_printer.h"

#define OWL_BLE_NAME "OWL-Printer-MINI"
#define OWL_BLE_SERVICE_UUID "4e3eeda0-83bf-45d7-8707-769d6348411c"
#define OWL_BLE_PACKET_INPUT_UUID "4e3eedb1-83bf-45d7-8707-769d6348411c"
#define OWL_BLE_STATUS_REPORT_UUID "4e3eedb2-83bf-45d7-8707-769d6348411c"

static int ble_connected = 0;

/**
 * 解析BLE收到的数据包.
 *
 * @param raw, 原始数据
 * @param len, 原始数据长度
 * @param packet, 解析之后的结果
 *
 * @return 包类型
 */
static int ble_parse_packet(uint8_t *raw, size_t len, OwlPacket **packet) {
  OwlPacket *p;
  p = (OwlPacket *)raw;
  *packet = p;

  // 检查是否为测试包
  if (len == PKT_TEST_LEN) {
    return PKT_TYPE_TEST;
  }

  // 检查包头
  if (p->magic[0] != 'O' || p->magic[1] != 'W' || p->magic[2] != 'L') {
    *packet = nullptr;
    return PKT_TYPE_UNKNOWN;
  }

  // 解析命令包
  if (len == PKT_CMD_LEN) {
    switch (p->type) {
      case PKT_TYPE_START:
      case PKT_TYPE_END:
        return p->type;
      default:
        *packet == nullptr;
        return PKT_TYPE_UNKNOWN;
    }
  }

  // 解析数据包
  if (len == PKT_DATA_LEN && p->type == PKT_TYPE_DATA) {
    return PKT_TYPE_DATA;
  }

  // 其他情况
  *packet = nullptr;
  return PKT_TYPE_UNKNOWN;
}

class OwlServerCallback : public BLEServerCallbacks {
 public:
  void onConnect(BLEServer *server) {
    ble_connected = 1;
    Serial.println("[INFO]: new ble connection");
  }

  void onDisconnect(BLEServer *server) {
    ble_connected = 0;
    Serial.println("[INFO]: destroy ble connection");
    server->startAdvertising();
  }
};

class OwlCharacterCallback : public BLECharacteristicCallbacks {
 public:
  void onRead(BLECharacteristic *c) {
    if(c->getUUID().equals(BLEUUID::fromString(OWL_BLE_STATUS_REPORT_UUID))) {
      float status[3] = {0};
      status[0] = printer_get_temp();
      status[1] = printer_get_volt();
      status[2] = printer_get_lack_paper();

      c->setValue((uint8_t *)status, sizeof(status));
      Serial.println("[INFO]: client read printer status");
    }
  }

  /**
   * 处理客户端发送的数据包
   */
  void onWrite(BLECharacteristic *c) {
    if (!c->getUUID().equals(BLEUUID(OWL_BLE_PACKET_INPUT_UUID))) {
      Serial.println("[INFO]: write unsupported characteristic");
      return;
    }

    size_t packet_len = c->getLength();
    uint8_t *raw_packet = c->getData();
    Serial.printf("[INFO]: raw packet len: %d\n[INFO]: raw packet data: ",
                  packet_len);
    for (int i = 0; i < packet_len; i++) {
      Serial.printf("0x%x%c", raw_packet[i], i == packet_len - 1 ? '\n' : ' ');
    }

    OwlPacket *packet = nullptr;

    int type = ble_parse_packet(raw_packet, packet_len, &packet);
    if (type == PKT_TYPE_UNKNOWN) {
      Serial.println("[WARNING]: UNKNOWN packet received");
      return;
    }

    switch (type) {
      case PKT_TYPE_START:
        Serial.println("[INFO]: START packet received");
        break;
      case PKT_TYPE_END:
        Serial.println("[INFO]: END packet received");
        break;
      case PKT_TYPE_DATA:
        Serial.print("[INFO]: DATA packet received: ");
        for (int i = 0; i < 48; i++) {
          Serial.printf("0x%x%c", packet->data[i],
                        i == PKT_DATA_LEN - 1 ? '\n' : ' ');
        }
        break;
      case PKT_TYPE_TEST:
        Serial.printf("[INFO]: TEST packet, tdata=0x%x\n", packet->tdata);
    }

    // 将数据包发送到printer
    printer_accept_packet(type, packet);
  }
};

void ble_init() {
  BLEDevice::init(OWL_BLE_NAME);

  // 创建server和service
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new OwlServerCallback());

  BLEService *service = server->createService(OWL_BLE_SERVICE_UUID);

  // 创建charateristic
  BLECharacteristic *packet_input = service->createCharacteristic(
      OWL_BLE_PACKET_INPUT_UUID, BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic *status_report = service->createCharacteristic(
      OWL_BLE_STATUS_REPORT_UUID,
      BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
  status_report->addDescriptor(new BLE2902());

  OwlCharacterCallback *cb = new OwlCharacterCallback();
  packet_input->setCallbacks(cb);
  status_report->setCallbacks(cb);

  // 开启service和server
  service->start();
  server->getAdvertising()->addServiceUUID(OWL_BLE_SERVICE_UUID);
  server->startAdvertising();
}

void ble_report_status() {}

int ble_get_connected() { return ble_connected; }