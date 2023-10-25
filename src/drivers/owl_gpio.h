#ifndef OWL_GPIO_H
#define OWL_GPIO_H

#define LED_ALWAYS_ON_MODE 0
#define LED_SLOWLY_BLINK_MODE 1
#define LED_FAST_BLINK_MODE 2

void owl_gpio_init();

int gpio_check_lack_paper();

void gpio_led_blink();

void gpio_led_set_mode(int mode);

#endif