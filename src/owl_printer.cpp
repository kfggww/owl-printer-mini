#include "owl_printer.h"

#define NBYTES_PER_LINE 48
#define MAX_NLINES 400

struct PrinterDes {
  PrinterState state;
  uint8_t buffer[MAX_NLINES][NBYTES_PER_LINE];
  uint8_t r_index;
  uint8_t w_index;
  uint8_t buf_size;
  SemaphoreHandle_t lock;
  SemaphoreHandle_t sem_data_ready;
  uint8_t pause;

  float temp;
  float volt;
  int lack_paper;
};

static PrinterDes pdes;

static void pdes_init() {
  memset(&pdes, 0, sizeof(pdes));
  pdes.state = PState_Ready;
  pdes.lock = xSemaphoreCreateMutex();
  pdes.sem_data_ready = xSemaphoreCreateBinary();
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
    uint8_t nlines = 100;
    nlines = nlines <= pdes.buf_size ? nlines : pdes.buf_size;
    uint8_t r_index = pdes.r_index;

    pdes.buf_size -= nlines;
    pdes.r_index = (r_index + nlines) % MAX_NLINES;

    xSemaphoreGive(pdes.lock);

    // 开始打印数据
    while (nlines > 0) {
      // 等待纸张就绪
      while (pdes.lack_paper) {
        pdes.pause = 1;
        gpio_led_set_mode(LED_FAST_BLINK_MODE);
        xSemaphoreTake(sem_paper_ready, portMAX_DELAY);
      }
      gpio_led_set_mode(LED_SLOWLY_BLINK_MODE);
      uint8_t *data = pdes.buffer[r_index];
      phead_draw_line(data, NBYTES_PER_LINE);
      nlines -= 1;
      r_index = (r_index + 1) % MAX_NLINES;
    }

    gpio_led_set_mode(LED_ALWAYS_ON_MODE);
  }
  // 等待数据就绪
  xSemaphoreTake(pdes.sem_data_ready, 200);
}

/**
 * 接收BLE发送过来的数据包. 更新打印机状态, 以及待打印数据.
 *
 * @param type, 包类型
 * @param packet, 包内容
 */
void printer_accept_packet(uint8_t type, OwlPacket *packet) {
  int taken = 0;
  if (xSemaphoreTake(pdes.lock, 600) == pdTRUE) {
    // 当前状态, 当前写指针位置
    taken = 1;
    uint8_t w_index = pdes.w_index;

    // 根据当前状态以及包类型, 进行状态转移
    switch (type) {
      case PKT_TYPE_TEST:
        memset(pdes.buffer[w_index], packet->tdata, NBYTES_PER_LINE);
        pdes.buf_size++;
        pdes.w_index = (w_index + 1) % MAX_NLINES;
        if (pdes.buf_size >= 100) xSemaphoreGive(pdes.sem_data_ready);
        break;
      case PKT_TYPE_DATA:
        memcpy(pdes.buffer[w_index], packet->data, NBYTES_PER_LINE);
        pdes.buf_size++;
        pdes.w_index = (w_index + 1) % MAX_NLINES;
        if (pdes.buf_size >= 100) xSemaphoreGive(pdes.sem_data_ready);
        break;
    }

    // 处理溢出问题, 发送数据溢出, 将导致打印数据丢失.
    if (pdes.buf_size > MAX_NLINES) {
      Serial.println("[WARNING]: printer buffer overflow!");
      pdes.buf_size = pdes.w_index - pdes.r_index;
    }

    xSemaphoreGive(pdes.lock);
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

void printer_set_pause(uint8_t pause) { pdes.pause = pause; }

uint8_t printer_get_pause() { return pdes.pause; }
