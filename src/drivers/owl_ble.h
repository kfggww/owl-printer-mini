#ifndef OWL_BLE_H
#define OWL_BLE_H

#include <Arduino.h>

/*包长度*/
#define PKT_TEST_LEN 1
#define PKT_CMD_LEN 4
#define PKT_DATA_LEN 52

/*包类型*/
#define PKT_TYPE_UNKNOWN 0x00
#define PKT_TYPE_START 0x01
#define PKT_TYPE_DATA 0x02
#define PKT_TYPE_END 0xFF
#define PKT_TYPE_TEST 0x03

/*包格式*/
union OwlPacket {
  struct {
    uint8_t magic[3];
    uint8_t type;
    uint8_t data[48];
  };
  uint8_t tdata;
};

void ble_init();
void ble_report_status();
int ble_get_connected();

#endif