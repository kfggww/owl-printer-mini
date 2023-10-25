#include "drivers/owl_gpio.h"

#include <Arduino.h>

#include "owl_configs.h"

#define SLOWLY_BLINK_INTERVAL_MS 800
#define FAST_BLINK_INTERVAL_MS 200

/**
 * GPIO初始化.
 */
void owl_gpio_init() {
  pinMode(PIN_LACK_PAPER, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
}

/**
 * 缺纸检测.
 *
 * @return 0: 不缺纸; 1: 缺纸
 */
int gpio_check_lack_paper() { return digitalRead(PIN_LACK_PAPER); }

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

void gpio_led_set_mode(int mode) {
  led_mode = mode;
}