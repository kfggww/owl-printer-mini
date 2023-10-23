#include "owl_heater.h"

#include <Arduino.h>

#include "owl_configs.h"

#define STB_HEAT_TIME_US 2000
#define STB_COOLING_TIME_US 200

static uint8_t heater_stb_pins[N_STBS] = {
    PIN_STB1, PIN_STB2, PIN_STB3, PIN_STB4, PIN_STB5, PIN_STB6,
};

/**
 * 初始化加热头.
 */
void heater_init() {
  // 加热电源开关, 默认关闭
  pinMode(PIN_HEAT_POWER_EN, OUTPUT);
  digitalWrite(PIN_HEAT_POWER_EN, LOW);

  // 打印通道, 默认关闭
  for (int i = 0; i < N_STBS; i++) {
    pinMode(heater_stb_pins[i], OUTPUT);
    digitalWrite(heater_stb_pins[i], LOW);
  }
}

/**
 * 运行加热通道, 加热一段时间后冷却.
 *
 * @param id, 通道id, 取值区间为[0, N_STBS)
 */
void heater_run_stb(int id) {
  if (id < 0 || id >= N_STBS) {
    Serial.println("ERROR: stb id out of range");
    return;
  }

  digitalWrite(heater_stb_pins[id], HIGH);
  delayMicroseconds(STB_HEAT_TIME_US);
  digitalWrite(heater_stb_pins[id], LOW);
  delayMicroseconds(STB_COOLING_TIME_US);
}

/**
 * 关闭全部通道.
 */
void heater_stop_stbs() {
  for (int i = 0; i < N_STBS; i++) {
    digitalWrite(heater_stb_pins[i], LOW);
  }
}

void heater_power_on() { digitalWrite(PIN_HEAT_POWER_EN, HIGH); }

void heater_power_off() { digitalWrite(PIN_HEAT_POWER_EN, LOW); }