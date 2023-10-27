#include "owl_printer.h"

#define NBYTES_PER_LINE 48
#define MAX_NLINES 100

struct PrinterDes {
  PrinterState state;
  uint8_t buffer[MAX_NLINES][NBYTES_PER_LINE];
  uint8_t r_index;
  uint8_t w_index;
  uint8_t buf_size;
  SemaphoreHandle_t lock;

  float temp;
  float volt;
  int lack_paper;
};

static PrinterDes pdes;

static void pdes_init() {
  memset(&pdes, 0, sizeof(pdes));
  pdes.state = PState_Ready;
  pdes.lock = xSemaphoreCreateMutex();
}

void printer_init() {
  adc_init();
  owl_gpio_init();
  ble_init();
  phead_init();
  pdes_init();
}

/**
 * 打印任务, 等待有数据可以打印.
 */
void printer_run() {
  if (xSemaphoreTake(pdes.lock, 20) == pdTRUE) {
    // 检查当前状态
    if (pdes.state != PState_Working) {
      xSemaphoreGive(pdes.lock);
      return;
    }

    Serial.println("[INFO]: printer working...");

    // 打印全部数据
    uint8_t counter = 0;
    while (counter < pdes.buf_size) {
      // 等待纸张就绪
      while (pdes.lack_paper) {
        gpio_led_set_mode(LED_FAST_BLINK_MODE);
        pdes.state = PState_Pause;
        xSemaphoreTake(sem_paper_ready, portMAX_DELAY);
        pdes.state = PState_Working;
        gpio_led_set_mode(LED_SLOWLY_BLINK_MODE);
      }
      uint8_t *data = pdes.buffer[pdes.r_index];
      phead_draw_line(data, NBYTES_PER_LINE);
      counter++;
      pdes.r_index = (pdes.r_index + 1) % MAX_NLINES;
    }

    Serial.println("[INFO]: printer working done!");

    // 更新状态, 并释放锁
    pdes.state = PState_Ready;
    pdes.buf_size = 0;
    gpio_led_set_mode(LED_ALWAYS_ON_MODE);
    xSemaphoreGive(pdes.lock);
  }
  delay(100);
}

/**
 * 接收BLE发送过来的数据包. 更新打印机状态, 以及待打印数据.
 *
 * @param type, 包类型
 * @param packet, 包内容
 */
void printer_accept_packet(uint8_t type, OwlPacket *packet) {
  // 处在working状态, 丢弃当前包
  if (pdes.state == PState_Working) {
    Serial.printf(
        "[WARNING]: drop packet of type (0x%x), because printer is working\n",
        type);
    return;
  }

  // 尝试3次
  int taken = 0;
  for (int i = 0; i < 3 && taken == 0; i++) {
    if (xSemaphoreTake(pdes.lock, 20) == pdTRUE) {
      // 当前状态, 当前写指针位置
      taken = 1;
      PrinterState current_state = pdes.state;
      uint8_t w_index = pdes.w_index;

      // 根据当前状态以及包类型, 进行状态转移
      switch (current_state) {
        case PState_Ready:
          if (type == PKT_TYPE_START) {
            pdes.state = PState_Waitting;
          } else if (type == PKT_TYPE_TEST) {
            memset(pdes.buffer[w_index], packet->tdata, NBYTES_PER_LINE);
            pdes.buf_size++;
            pdes.w_index = (w_index + 1) % MAX_NLINES;
            pdes.state = PState_Working;
          }
          gpio_led_set_mode(LED_SLOWLY_BLINK_MODE);
          break;
        case PState_Waitting:
          if (type == PKT_TYPE_END) {
            pdes.state = PState_Working;
          } else if (type == PKT_TYPE_DATA) {
            memcpy(pdes.buffer[w_index], packet->data, NBYTES_PER_LINE);
            pdes.buf_size++;
            pdes.w_index = (w_index + 1) % MAX_NLINES;
          }
          break;
      }

      // 处理溢出问题, 发送数据溢出, 将导致打印数据丢失.
      if (pdes.buf_size > MAX_NLINES) {
        Serial.println("[WARNING]: printer buffer overflow!");
        pdes.buf_size = pdes.w_index - pdes.r_index;
      }

      xSemaphoreGive(pdes.lock);
    }
  }

  if (taken == 0) {
    Serial.printf(
        "[WARNING]: drop packet of type (0x%x), because lock is not "
        "available\n",
        type);
  }
}

float printer_get_temp() { return pdes.temp; }

float printer_get_volt() { return pdes.volt; }

float printer_get_lack_paper() { return pdes.lack_paper; }

void printer_set_status(float temp, float volt, int lack) {
  pdes.temp = temp;
  pdes.volt = volt;
  pdes.lack_paper = lack;
}

PrinterState printer_get_state() { return pdes.state; }