#include "drivers/owl_gpio.h"

#include <Arduino.h>

#include "owl_configs.h"
#include "owl_printer.h"

#define SLOWLY_BLINK_INTERVAL_MS 800
#define FAST_BLINK_INTERVAL_MS 200

SemaphoreHandle_t sem_paper_ready;

void ARDUINO_ISR_ATTR gpio_btn_isr(void) {
  int lack = digitalRead(PIN_LACK_PAPER);
  PrinterState state = printer_get_state();

  if (lack == 0 && printer_get_pause()) {
    printer_set_pause(0);
    xSemaphoreGive(sem_paper_ready);
    Serial.println("[INFO]: paper ready confirmed");
  }
}

/**
 * GPIO初始化.
 */
void owl_gpio_init() {
  pinMode(PIN_LACK_PAPER, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  sem_paper_ready = xSemaphoreCreateBinary();
  attachInterrupt(PIN_BTN, gpio_btn_isr, FALLING);
}

/**
 * 缺纸检测.
 *
 * @return 0: 不缺纸; 1: 缺纸
 */
int gpio_read_lack_paper() { return digitalRead(PIN_LACK_PAPER); }

static int volatile led_mode = LED_ALWAYS_ON_MODE;

/**
 * LED闪烁控制.
 */
void gpio_led_blink() {
  switch (led_mode) {
    case LED_ALWAYS_ON_MODE:
      digitalWrite(PIN_LED, LOW);
      delay(10);
      break;
    case LED_SLOWLY_BLINK_MODE:
      digitalWrite(PIN_LED, LOW);
      delay(SLOWLY_BLINK_INTERVAL_MS);
      digitalWrite(PIN_LED, HIGH);
      delay(SLOWLY_BLINK_INTERVAL_MS);
      break;
    case LED_FAST_BLINK_MODE:
      digitalWrite(PIN_LED, LOW);
      delay(FAST_BLINK_INTERVAL_MS);
      digitalWrite(PIN_LED, HIGH);
      delay(FAST_BLINK_INTERVAL_MS);
      break;
  }
}

void gpio_led_set_mode(int mode) { led_mode = mode; }