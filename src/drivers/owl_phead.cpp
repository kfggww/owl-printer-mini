#include <Arduino.h>

#include "owl_heater.h"
#include "owl_motor.h"
#include "owl_spi.h"

#define NBYTES_PER_LINE 48

void phead_init() {
  heater_init();
  motor_init();
  spi_init();
}

void phead_draw_line(void *data, int size) {
  if (size < 0 || size > NBYTES_PER_LINE) {
    Serial.println("ERROR: data size out of range");
    return;
  }

  heater_power_on();

  spi_send(data, size);
  motor_run(1);
  for (int i = 0; i < N_STBS; i++) {
    heater_run_stb(i);
  }
  motor_run(3);
  heater_stop_stbs();

  heater_power_off();
}