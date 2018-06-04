/*
 * led_2pin.c
 *
 *  Created on: Jun 4, 2018
 *      Author: samsung
 */


#include <peripheral_io.h>

#include "log.h"

#define GPIO_PIN_MAX	40


static peripheral_gpio_h gpio_h[GPIO_PIN_MAX + 1] = { NULL, };

void led_2pin_close(int pin_num)
{
	INFO("LED_2pin is finishing...");
	peripheral_gpio_close(gpio_h[pin_num]);
	gpio_h[pin_num] = NULL;
}

int led_2pin_write(int pin_num, int write_value)
{
	FN_CALL;

	int ret;

	if (gpio_h[pin_num] == NULL) {
		ret = peripheral_gpio_open(pin_num, &gpio_h[pin_num]);
		if (ret != PERIPHERAL_ERROR_NONE) {
			ERR("peripheral_gpio_open() failed!![%d]", ret);
			return ret;
		}

		ret = peripheral_gpio_set_direction(gpio_h[pin_num],
				PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
		if (ret != PERIPHERAL_ERROR_NONE) {
			ERR("peripheral_gpio_set_dreection() failed!![%d]", ret);
			return ret;
		}
	}

	ret = peripheral_gpio_write(gpio_h[pin_num], write_value);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_write() failed!![%d]", ret);
		return ret;
	}

	INFO("LED Light : %s", write_value ? "OFF" : "ON");

	return PERIPHERAL_ERROR_NONE;
}
