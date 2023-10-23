#include <Arduino.h>

#include "owl_configs.h"

void adc_init() { analogReadResolution(12); }

float adc_read_temp() {
  const float B = 3950;
  const float R25 = 30;
  const float T25 = 273.15 + 25;

  float tx = 0.0;

  uint32_t v = analogReadMilliVolts(PIN_TEMP_ADC);
  float rx = 10.0 * v / (3300.0 - v);
  tx = B * T25 / (T25 * log(rx / R25) + B);

  return tx - 273.15;
}

float adc_read_volt() {
  return analogReadMilliVolts(PIN_BATTERY_ADC) / 1000.0 * 2;
}