#ifndef OWL_GPIO_H
#define OWL_GPIO_H

#include <Arduino.h>

#define LED_ALWAYS_ON_MODE 0
#define LED_SLOWLY_BLINK_MODE 1
#define LED_FAST_BLINK_MODE 2

extern SemaphoreHandle_t sem_paper_ready;

void owl_gpio_init();

int gpio_read_lack_paper();

void gpio_notify_printer_paper_ready(int lack);

void gpio_led_blink();

void gpio_led_set_mode(int mode);

#endif