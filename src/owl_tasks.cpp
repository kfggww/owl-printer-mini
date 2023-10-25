#include <Arduino.h>

#include "drivers/owl_adc.h"
#include "drivers/owl_gpio.h"
#include "owl_printer.h"

void task_led_blink(void *arg) {
  Serial.println("[INFO]: task_led_blink start...");
  for (;;) {
    gpio_led_blink();
  }
  Serial.println("[WARNING]: task_led_blink exit");
  vTaskDelete(NULL);
}

void task_print_status(void *arg) {
  Serial.println("[INFO]: task_print_status start...");
  for (;;) {
    float temp = adc_read_temp();
    float volt = adc_read_volt();
    int lack = gpio_read_lack_paper();
    printer_set_status(temp, volt, lack);

    Serial.printf("[INFO]: temp=%.2f, volt=%.2f, lack_paper=%d\n", temp, volt,
                  lack);

    gpio_notify_printer_paper_ready(lack);

    delay(1000);
  }
  Serial.println("[WARNING]: task_print_status exit");
  vTaskDelete(NULL);
}

void task_printer_run(void *arg) {
  Serial.println("[INFO]: task_printer_run start...");
  for (;;) {
    printer_run();
  }
  Serial.println("[WARNING]: task_printer_run exit");
  vTaskDelete(NULL);
}

void tasks_init() {
  xTaskCreate(task_led_blink, "task_led_blink", 1024 * 2, NULL, 0, NULL);
  xTaskCreate(task_print_status, "task_print_status", 1024 * 2, NULL, 0, NULL);
  xTaskCreate(task_printer_run, "task_printer_run", 1024 * 4, NULL, 0, NULL);
}