#include <Arduino.h>

#include "drivers/owl_gpio.h"

void task_led_blink(void *arg) {
  Serial.println("[INFO]: task_led_blink start...");
  for (;;) {
    gpio_led_blink();
  }
  Serial.println("[WARNING]: task_led_blink exit");
  vTaskDelete(NULL);
}

void tasks_init() {
  xTaskCreate(task_led_blink, "task_led_blink", 1024 * 2, NULL, 0, NULL);
}