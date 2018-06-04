/*
 * LED_2pin.h
 *
 *  Created on: May 15, 2018
 *      Author: samsung
 */

#ifndef __LED_2PIN_H__
#define __LED_2PIN_H__

typedef enum {
	LED_LIGHT_OFF = 0,
	LED_LIGHT_ON,
} led_light_on_off;

extern int led_2pin_write(int pin_num, int write_value);

extern void led_2pin_close(int pin_num);

#endif /* __LED_2PIN_H__ */
