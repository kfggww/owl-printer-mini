#ifndef OWL_PRINTER_H
#define OWL_PRINTER_H

#include <Arduino.h>

#include "drivers/owl_adc.h"
#include "drivers/owl_ble.h"
#include "drivers/owl_gpio.h"
#include "drivers/owl_phead.h"

void printer_init();

void printer_run();

void printer_accept_packet(uint8_t type, OwlPacket *packet);

void printer_report_status();

#endif