#include <Arduino.h>

#include "drivers/owl_adc.h"
#include "drivers/owl_ble.h"
#include "drivers/owl_gpio.h"
#include "drivers/owl_motor.h"
#include "drivers/owl_phead.h"

static uint8_t buffer[120][48];

void test_init() {
  Serial.begin(115200);
  adc_init();
  gpio_init();
  motor_init();
  phead_init();
  ble_init();
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

/**
 * 打印6大行数据, 每行宽度20, 每个小行打印前需要重置data.
 */
void test_phead_with_reset() {
  uint8_t data[48];
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 20; j++) {
      memset(data, 0, 48);
      memset(data + 8 * i, 0xf0, 48 - 8 * i);
      phead_draw_line(data, 48);
    }
  }
}

/**
 * 初始化一次data之后, 循环打印20行.
 *
 * @bug 1) 通过spi发送到打印头之后, 原始数据会被修改;
 *      2) 使用这种方式调用, 后续的打印也会失败;
 */
void test_phead_without_reset() {
  uint8_t data[48];
  memset(data, 0xf0, 48);
  for (int j = 0; j < 20; j++) {
    phead_draw_line(data, 48);
  }
}

/**
 * 使用预先分配的buffer作为数据源, 驱动打印头.
 */
void test_phead_with_buffer() {
  // 初始化buffer
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 20; j++) {
      memset(buffer[i * 20 + j] + 8 * i, 0xf0, 48 - 8 * i);
    }
  }

  // 开始打印
  for (int k = 0; k < 120; k++) {
    phead_draw_line(buffer[k], 48);
  }
}

void test_run() {
  delay(1000);
  Serial.println("test_run begin...");

  test_adc_and_gpio();
  // test_motor();
  test_phead_with_reset();
  // test_phead_without_reset();
  test_phead_with_buffer();

  Serial.println("test_run end...");
  for (;;) {
    delay(1000);
  }
}