#include <Arduino.h>
#include <SPI.h>

#include "owl_configs.h"

#define SPI_CLK_FRQ 1000000
#define TW_LATCH_TIME_US 1

SPIClass hspi(HSPI);
SPISettings spi_settings(SPI_CLK_FRQ, MSBFIRST, SPI_MODE0);

void spi_init() {
  // 初始化SPI总线
  hspi.begin(PIN_SPI_SCK, -1, PIN_SPI_SDA, -1);
  hspi.setFrequency(SPI_CLK_FRQ);

  // 初始化打印头锁存引脚
  pinMode(PIN_LATCH, OUTPUT);
  digitalWrite(PIN_LATCH, HIGH);
}

void spi_send(void *data, int size) {
  hspi.beginTransaction(spi_settings);
  hspi.transfer(data, size);
  hspi.endTransaction();

  digitalWrite(PIN_LATCH, LOW);
  delayMicroseconds(TW_LATCH_TIME_US);
  digitalWrite(PIN_LATCH, HIGH);
}