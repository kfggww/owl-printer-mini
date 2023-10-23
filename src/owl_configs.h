#ifndef OWL_CONFIGS_H
#define OWL_CONFIGS_H

/* 步进电机控制 */
#define PIN_MOTOR_AP 23
#define PIN_MOTOR_BP 21
#define PIN_MOTOR_AN 22
#define PIN_MOTOR_BN 19

/* 打印头数据接收 */
#define PIN_SPI_SCK 15
#define PIN_SPI_SDA 13
/* 打印头数据接收锁存 */
#define PIN_LATCH 12

/* 打印头通道控制 */
#define PIN_STB1 14
#define PIN_STB2 27
#define PIN_STB3 26
#define PIN_STB4 25
#define PIN_STB5 33
#define PIN_STB6 32

/* 打印头加热电源使能 */
#define PIN_HEAT_POWER_EN 17

/* 电池电压检测 */
#define PIN_BATTERY_ADC 34

/* 打印头温度检测 */
#define PIN_TEMP_ADC 36

/* 缺纸检测 */
#define PIN_LACK_PAPER 35

#endif