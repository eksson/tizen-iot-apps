/*
 * dcmotor_L298N.c
 *
 *  Created on: Jun 4, 2018
 *      Author: samsung
 */


#include <peripheral_io.h>

#include "log.h"
#include "resource/dcmotor_L298N.h"


typedef enum {
	MOTOR_STATE_NONE,
	MOTOR_STATE_CONFIGURED,
	MOTOR_STATE_STOP,
	MOTOR_STATE_FORWARD,
	MOTOR_STATE_BACKWARD,
} motor_state_e;

typedef struct __motor_driver_s {
	unsigned int pin_1;
	unsigned int pin_2;
	unsigned int en_ch;
	motor_state_e motor_state;
	peripheral_gpio_h pin1_h;
	peripheral_gpio_h pin2_h;
	peripheral_gpio_h pin3_h;
} motor_driver_s;

static motor_driver_s g_md_h[MOTOR_ID_MAX] = {
	{0, 0, 0, MOTOR_STATE_NONE, NULL, NULL},
};


/* see Principle section in http://wiki.sunfounder.cc/index.php?title=Motor_Driver_Module-L298N */

static int __dcmotor_stop(motor_id_e id)
{
	int ret = PERIPHERAL_ERROR_NONE;
	int motor1_v = 0;
	int motor2_v = 0;

	if (g_md_h[id].motor_state <= MOTOR_STATE_CONFIGURED) {
//		_E("motor[%d] are not initialized - state(%d)",
//			id, g_md_h[id].motor_state);
		return -1;
	}

	if (g_md_h[id].motor_state == MOTOR_STATE_STOP) {
//		_D("motor[%d] is already stopped", id);
		return 0;
	}

	if (g_md_h[id].motor_state == MOTOR_STATE_FORWARD) {
		motor1_v = 0;
		motor2_v = 0;
	} else if (g_md_h[id].motor_state == MOTOR_STATE_BACKWARD) {
		motor1_v = 1;
		motor2_v = 1;
	}

	/* Brake DC motor */
	ret = peripheral_gpio_write(g_md_h[id].pin1_h, motor1_v);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("Failed to set value[%d] Motor[%d] pin 1", motor1_v, id);
		return -1;
	}

	ret = peripheral_gpio_write(g_md_h[id].pin2_h, motor2_v);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("Failed to set value[%d] Motor[%d] pin 2", motor2_v, id);
		return -1;
	}

	/* set stop DC motor */
	// need to stop motor or not?, it may stop motor to free running
//	resource_pca9685_set_value_to_channel(g_md_h[id].en_ch, 0, 0);

	g_md_h[id].motor_state = MOTOR_STATE_STOP;

	return 0;
}

static int __set_default_configuration_by_id(motor_id_e id)
{
	unsigned int pin_1, pin_2, en_ch;

	switch (id) {
	case MOTOR_ID_1:
		pin_1 = DEFAULT_MOTOR1_PIN1;
		pin_2 = DEFAULT_MOTOR1_PIN2;
		en_ch = DEFAULT_MOTOR1_EN_CH;
	break;
	case MOTOR_ID_2:
		pin_1 = DEFAULT_MOTOR2_PIN1;
		pin_2 = DEFAULT_MOTOR2_PIN2;
		en_ch = DEFAULT_MOTOR2_EN_CH;
	break;
	case MOTOR_ID_3:
		pin_1 = DEFAULT_MOTOR3_PIN1;
		pin_2 = DEFAULT_MOTOR3_PIN2;
		en_ch = DEFAULT_MOTOR3_EN_CH;
	break;
	case MOTOR_ID_4:
		pin_1 = DEFAULT_MOTOR4_PIN1;
		pin_2 = DEFAULT_MOTOR4_PIN2;
		en_ch = DEFAULT_MOTOR4_EN_CH;
	break;
	case MOTOR_ID_MAX:
	default:
//		_E("Unkwon ID[%d]", id);
		return -1;
	break;
	}

	g_md_h[id].pin_1 = pin_1;
	g_md_h[id].pin_2 = pin_2;
	g_md_h[id].en_ch = en_ch;
	g_md_h[id].motor_state = MOTOR_STATE_CONFIGURED;

	return 0;
}

static int __fini_motor_by_id(motor_id_e id)
{
//	retv_if(id == MOTOR_ID_MAX, -1);

	if (g_md_h[id].motor_state <= MOTOR_STATE_CONFIGURED)
		return 0;

	if (g_md_h[id].motor_state > MOTOR_STATE_STOP)
		__dcmotor_stop(id);

//	resource_pca9685_fini(g_md_h[id].en_ch);

	if (g_md_h[id].pin1_h) {
		peripheral_gpio_close(g_md_h[id].pin1_h);
		g_md_h[id].pin1_h = NULL;
	}

	if (g_md_h[id].pin2_h) {
		peripheral_gpio_close(g_md_h[id].pin2_h);
		g_md_h[id].pin2_h = NULL;
	}

	g_md_h[id].motor_state = MOTOR_STATE_CONFIGURED;

	return 0;
}

static int __init_motor_by_id(motor_id_e id)
{
	int ret = 0;

//	retv_if(id == MOTOR_ID_MAX, -1);

	if (g_md_h[id].motor_state == MOTOR_STATE_NONE)
		__set_default_configuration_by_id(id);

//	ret = resource_pca9685_init(g_md_h[id].en_ch);
	if (ret) {
//		_E("failed to init PCA9685");
		return -1;
	}

	/* open pins for Motor */
	ret = peripheral_gpio_open(g_md_h[id].pin_1, &g_md_h[id].pin1_h);
	if (ret == PERIPHERAL_ERROR_NONE)
		peripheral_gpio_set_direction(g_md_h[id].pin1_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	else {
//		_E("failed to open Motor[%d] gpio pin1[%u]", id, g_md_h[id].pin_1);
		goto ERROR;
	}

	ret = peripheral_gpio_open(g_md_h[id].pin_2, &g_md_h[id].pin2_h);
	if (ret == PERIPHERAL_ERROR_NONE)
		peripheral_gpio_set_direction(g_md_h[id].pin2_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	else {
//		_E("failed to open Motor[%d] gpio pin2[%u]", id, g_md_h[id].pin_2);
		goto ERROR;
	}

	g_md_h[id].motor_state = MOTOR_STATE_STOP;

	return 0;

ERROR:
//	resource_pca9685_fini(g_md_h[id].en_ch);

	if (g_md_h[id].pin1_h) {
		peripheral_gpio_close(g_md_h[id].pin1_h);
		g_md_h[id].pin1_h = NULL;
	}

	if (g_md_h[id].pin2_h) {
		peripheral_gpio_close(g_md_h[id].pin2_h);
		g_md_h[id].pin2_h = NULL;
	}

	return -1;
}

void dcmotor_L298N_close(motor_id_e id)
{
	__fini_motor_by_id(id);
	return;
}

void dcmotor_L298N_close_all(void)
{
	int i;
	for (i = MOTOR_ID_1; i < MOTOR_ID_MAX; i++)
		__fini_motor_by_id(i);

	return;
}

int dcmotor_L298N_configuration_set(motor_id_e id,
	unsigned int pin1, unsigned int pin2, unsigned en_ch)
{

	if (g_md_h[id].motor_state > MOTOR_STATE_CONFIGURED) {
//		_E("cannot set configuration motor[%d] in this state[%d]",
//			id, g_md_h[id].motor_state);
		return -1;
	}

	g_md_h[id].pin_1 = pin1;
	g_md_h[id].pin_2 = pin2;
	g_md_h[id].en_ch = en_ch;
	g_md_h[id].motor_state = MOTOR_STATE_CONFIGURED;

	return 0;
}

int dcmotor_L298N_speed_set(motor_id_e id, int speed)
{
	int ret = 0;
	const int value_max = MOTOR_SPEED_MAX;
	int value = 0;
	int e_state = MOTOR_STATE_NONE;
	int motor_v_1 = 0;
	int motor_v_2 = 0;

	if (g_md_h[id].motor_state <= MOTOR_STATE_CONFIGURED) {
		ret = __init_motor_by_id(id);
		if (ret) {
//			_E("failed to __init_motor_by_id()");
			return -1;
		}
	}

	//value = abs(speed);
	if (speed >= 0) value = speed;
	else value = speed * -1;

	if (value > value_max) {
		value = value_max;
//		_D("max speed is %d", value_max);
	}
//	_D("set speed %d", value);

	if (speed == 0) {
		/* brake and stop */
		ret = __dcmotor_stop(id);
		if (ret) {
//			_E("failed to stop motor[%d]", id);
			return -1;
		}
		return 0; /* done */
	}

	if (speed > 0)
		e_state = MOTOR_STATE_FORWARD; /* will be set forward */
	else
		e_state = MOTOR_STATE_BACKWARD; /* will be set backward */

	if (g_md_h[id].motor_state == e_state)
		goto SET_SPEED;
	else {
		/* brake and stop */
		ret = __dcmotor_stop(id);
		if (ret) {
//			_E("failed to stop motor[%d]", id);
			return -1;
		}
	}

	switch (e_state) {
	case MOTOR_STATE_FORWARD:
		motor_v_1 = 1;
		motor_v_2 = 0;
		break;
	case MOTOR_STATE_BACKWARD:
		motor_v_1 = 0;
		motor_v_2 = 1;
		break;
	}
	ret = peripheral_gpio_write(g_md_h[id].pin1_h, motor_v_1);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("failed to set value[%d] Motor[%d] pin 1", motor_v_1, id);
		return -1;
	}

	ret = peripheral_gpio_write(g_md_h[id].pin2_h, motor_v_2);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("failed to set value[%d] Motor[%d] pin 2", motor_v_2, id);
		return -1;
	}

SET_SPEED:
//	ret = resource_pca9685_set_value_to_channel(g_md_h[id].en_ch, 0, value);
	if (ret) {
//		_E("failed to set speed - %d", speed);
		return -1;
	}

	g_md_h[id].motor_state = e_state;

	return 0;
}
