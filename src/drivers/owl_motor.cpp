#include <Arduino.h>

#include "owl_configs.h"

#define STEP_NCYCLES 8           // 步进电机运行一周的步数
#define N_POLES 4                // 步进电机磁极数
#define MOTOR_HOLD_TIME_US 4000  // 磁极通电保持时间

/* 步进电机磁极引脚表 */
static uint8_t motor_pole_table[N_POLES] = {
    PIN_MOTOR_AP,
    PIN_MOTOR_BP,
    PIN_MOTOR_AN,
    PIN_MOTOR_BN,
};

/**
 * 步进电机控制表, 分别表示 {A+, B+, A-, B-}.
 *
 * 递增遍历, 步进电机正传;
 * 递减遍历, 步进电机反转
 */
static uint8_t motor_step_table[STEP_NCYCLES][4] = {
    {1, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 0},
    {0, 1, 1, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0},
};

/* 步进电机当前位置 */
static uint8_t motor_pos;

/**
 * 步进电机初始化.
 */
void motor_init() {
  motor_pos = 0;
  for (int i = 0; i < N_POLES; i++) {
    pinMode(motor_pole_table[i], OUTPUT);
    digitalWrite(motor_pole_table[i], LOW);
  }
}

/**
 * 步进电机正转.
 *
 * @param nstep, 步数
 */
void motor_run(int nstep) {
  while (nstep--) {
    for (int i = 0; i < N_POLES; i++) {
      digitalWrite(motor_pole_table[i], motor_step_table[motor_pos][i]);
    }
    motor_pos = (motor_pos + 1) % STEP_NCYCLES;
    delayMicroseconds(MOTOR_HOLD_TIME_US);
  }
}

/**
 * 步进电机反转.
 *
 * @param nstep, 步数
 */
void motor_reverse_run(int nstep) {
  while (nstep--) {
    for (int i = 0; i < N_POLES; i++) {
      digitalWrite(motor_pole_table[i], motor_step_table[motor_pos][i]);
    }
    if (motor_pos == 0)
      motor_pos = STEP_NCYCLES - 1;
    else
      motor_pos -= 1;

    delayMicroseconds(MOTOR_HOLD_TIME_US);
  }
}

/**
 * 步进电机停止.
 */
void motor_stop() {
  for (int i = 0; i < N_POLES; i++) {
    digitalWrite(motor_pole_table[i], LOW);
  }
}