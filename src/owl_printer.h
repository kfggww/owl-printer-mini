#ifndef OWL_PRINTER_H
#define OWL_PRINTER_H

#include <Arduino.h>

#include "drivers/owl_adc.h"
#include "drivers/owl_ble.h"
#include "drivers/owl_gpio.h"
#include "drivers/owl_phead.h"

/* 打印机的状态 */
enum PrinterState {
  PState_Ready,
  PState_Waitting,
  PState_Working,
  PState_Pause,  // 缺纸暂停
};

void printer_init();

void printer_run();

void printer_accept_packet(uint8_t type, OwlPacket *packet);

float printer_get_temp();

float printer_get_volt();

float printer_get_lack_paper();

void printer_set_status(float temp, float volt, int lack);

PrinterState printer_get_state();

#endif