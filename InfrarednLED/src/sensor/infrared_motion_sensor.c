/*
 * infrared_motion_sensor.c
 *
 *  Created on: Nov 12, 2018
 *      Author: eson
 */



#include <peripheral_io.h>

#include "log.h"

#define GPIO_PIN_MAX	40

static peripheral_gpio_h gpio_h[GPIO_PIN_MAX + 1] = { NULL, };

void infrared_motion_sensor_close(int pin_num)
{
	INFO("Infrared Motion Sensor is finishing...");
	peripheral_gpio_close(gpio_h[pin_num]);
	gpio_h[pin_num] = NULL;
}

int infrared_motion_sensor_read(int pin_num, uint32_t *out_value)
{
FN_CALL;
	int ret;
	if (gpio_h[pin_num] == NULL) {
		ret = peripheral_gpio_open(pin_num, &gpio_h[pin_num]);
		if (ret != PERIPHERAL_ERROR_NONE) {
			ERR("peripheral_gpio_open() failed!![%d]", ret);
			return ret;
		}
	}

	ret = peripheral_gpio_set_direction(gpio_h[pin_num], PERIPHERAL_GPIO_DIRECTION_IN);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_set_dreection() failed!![%d]", ret);
		return ret;
	}
	ret = peripheral_gpio_read(gpio_h[pin_num], out_value);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_read() failed!![%d]", ret);
		return ret;
	}
	return PERIPHERAL_ERROR_NONE;
}
