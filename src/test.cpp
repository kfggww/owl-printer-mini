#include <Arduino.h>

#include "drivers/owl_adc.h"
#include "drivers/owl_gpio.h"
#include "drivers/owl_motor.h"
#include "drivers/owl_phead.h"

void test_init() {
  Serial.begin(115200);
  adc_init();
  gpio_init();
  motor_init();
  phead_init();
}

void test_adc_and_gpio() {
  for (int i = 0; i < 3; i++) {
    float temp = adc_read_temp();
    float volt = adc_read_volt();
    int lack_paper = gpio_check_lack_paper();
    Serial.printf("{temp: %.2f, volt: %.2f, lack_paper: %d}\n", temp, volt,
                  lack_paper);
    delay(1000);
  }
}

void test_motor() {
  for (int i = 0; i < 3; i++) {
    motor_run(100);
    motor_reverse_run(50);
    delay(1000);
  }
}

void test_phead() {
  uint8_t data[48];
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 20; j++) {
      memset(data, 0, 48);
      memset(data + 8 * i, 0x55, 48 - 8 * i);
      phead_draw_line(data, 48);
    }
  }
}

void test_run() {
  delay(1000);
  Serial.println("test_run begin...");

  test_adc_and_gpio();
  test_motor();
  test_phead();

  Serial.println("test_run end...");
  for (;;) {
    delay(1000);
  }
}