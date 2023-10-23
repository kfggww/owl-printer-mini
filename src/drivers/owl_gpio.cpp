#include <Arduino.h>

#include "owl_configs.h"

/**
 * GPIO初始化.
 */
void gpio_init() { pinMode(PIN_LACK_PAPER, INPUT); }

/**
 * 缺纸检测.
 *
 * @return 0: 不缺纸; 1: 缺纸
 */
int gpio_check_lack_paper() { return digitalRead(PIN_LACK_PAPER); }