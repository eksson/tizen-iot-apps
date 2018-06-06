/*
 * dcmoter_l298n.c
 *
 *  Created on: May 16, 2018
 *      Author: samsung
 */


#include <peripheral_io.h>

#include "log.h"
#include "resource/dcmotor_L298N.h"


typedef enum {
	DCMOTOR_NONE,
//	DCMOTOR_CONFIGURED,
	DCMOTOR_STOP,
	DCMOTOR_FORWARD,
	DCMOTOR_BACKWARD,
} dcmotor_state_e;

typedef struct _dcmotor_driver {
	unsigned int pin_1;
	unsigned int pin_2;
	unsigned int en_ch;
	dcmotor_state_e dcmt_state;
	peripheral_gpio_h pin1_h;
	peripheral_gpio_h pin2_h;
	peripheral_gpio_h pin3_h;
} dcmotor_driver_h;

static dcmotor_driver_h dcmd_h[DCMOTOR_MAX] = {
	{0, 0, 0, DCMOTOR_NONE, NULL, NULL, NULL},
};


/* see Principle section in http://wiki.sunfounder.cc/index.php?title=Motor_Driver_Module-L298N */

static int __dcmotor_stop(dcmotor_id_e dcmt_id)
{
	int ret = PERIPHERAL_ERROR_NONE;
	int motor1_v = 0;
	int motor2_v = 0;

	if (dcmd_h[dcmt_id].dcmt_state <= DCMOTOR_NONE) {
//		_E("motor[%d] are not initialized - state(%d)",
//			id, dcmd_h[id].motor_state);
		return -1;
	}

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_STOP) {
//		_D("motor[%d] is already stopped", id);
		return 0;
	}

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_FORWARD) {
		motor1_v = 0;
		motor2_v = 0;
	} else if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_BACKWARD) {
		motor1_v = 1;
		motor2_v = 1;
	}

	/* Brake DC motor */
	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin1_h, motor1_v);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("Failed to set value[%d] Motor[%d] pin 1", motor1_v, id);
		return -1;
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin2_h, motor2_v);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("Failed to set value[%d] Motor[%d] pin 2", motor2_v, id);
		return -1;
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin3_h, 0);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("Failed to set value[%d] Motor[%d] pin 2", motor2_v, id);
		return -1;
	}

	/* set stop DC motor */
	// need to stop motor or not?, it may stop motor to free running
//	resource_pca9685_set_value_to_channel(dcmd_h[id].en_ch, 0, 0);

	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	return 0;
}


static int __fini_motor_by_id(dcmotor_id_e dcmt_id)
{
//	retv_if(id == MOTOR_ID_MAX, -1);

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_NONE)
		return 0;

	if (dcmd_h[dcmt_id].dcmt_state > DCMOTOR_STOP)
		__dcmotor_stop(dcmt_id);

//	resource_pca9685_fini(dcmd_h[id].en_ch);

	if (dcmd_h[dcmt_id].pin1_h) {
		peripheral_gpio_close(dcmd_h[dcmt_id].pin1_h);
		dcmd_h[dcmt_id].pin1_h = NULL;
	}

	if (dcmd_h[dcmt_id].pin2_h) {
		peripheral_gpio_close(dcmd_h[dcmt_id].pin2_h);
		dcmd_h[dcmt_id].pin2_h = NULL;
	}

	if (dcmd_h[dcmt_id].pin3_h) {
		peripheral_gpio_close(dcmd_h[dcmt_id].pin3_h);
		dcmd_h[dcmt_id].pin3_h = NULL;
	}

	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	return 0;
}



void dcmotor_L298N_close(dcmotor_id_e dcmt_id)
{
	__fini_motor_by_id(dcmt_id);
}

void dcmotor_L298N_close_all(void)
{
	int i;
	for (i = DCMOTOR_1; i < DCMOTOR_MAX; i++)
		__fini_motor_by_id(i);
}

int dcmotor_L298N_driver_init(dcmotor_id_e dcmt_id,
	unsigned int pin1, unsigned int pin2, unsigned en_ch)
{
	int ret;

	if (dcmd_h[dcmt_id].dcmt_state > DCMOTOR_NONE) {
//		_E("cannot set configuration motor[%d] in this state[%d]",
//			id, dcmd_h[id].motor_state);
		return -1;
	}

	dcmd_h[dcmt_id].pin_1 = pin1;
	dcmd_h[dcmt_id].pin_2 = pin2;
	dcmd_h[dcmt_id].en_ch = en_ch;
	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	ret = peripheral_gpio_open(dcmd_h[dcmt_id].pin_1, &dcmd_h[dcmt_id].pin1_h);
	if (ret == PERIPHERAL_ERROR_NONE) {
		peripheral_gpio_set_direction(dcmd_h[dcmt_id].pin1_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	} else {
	//		_E("failed to open Motor[%d] gpio pin1[%u]", id, dcmd_h[id].pin_1);
		goto ERROR;
	}

	ret = peripheral_gpio_open(dcmd_h[dcmt_id].pin_2, &dcmd_h[dcmt_id].pin2_h);
	if (ret == PERIPHERAL_ERROR_NONE) {
		peripheral_gpio_set_direction(dcmd_h[dcmt_id].pin2_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	} else {
//		_E("failed to open Motor[%d] gpio pin2[%u]", id, dcmd_h[id].pin_2);
		goto ERROR;
	}

	ret = peripheral_gpio_open(dcmd_h[dcmt_id].en_ch, &dcmd_h[dcmt_id].pin3_h);
	if (ret == PERIPHERAL_ERROR_NONE) {
		peripheral_gpio_set_direction(dcmd_h[dcmt_id].pin3_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	} else {
//		_E("failed to open Motor[%d] gpio pin2[%u]", id, dcmd_h[id].pin_2);
		goto ERROR;
	}

	dcmd_h[dcmt_id].pin_1 = pin1;
	dcmd_h[dcmt_id].pin_2 = pin2;
	dcmd_h[dcmt_id].en_ch = en_ch;
	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	return 0;

ERROR:
	dcmd_h[dcmt_id].pin_1 = 0;
	dcmd_h[dcmt_id].pin_2 = 0;
	dcmd_h[dcmt_id].en_ch = 0;
	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_NONE;
	dcmd_h[dcmt_id].pin1_h = NULL;
	dcmd_h[dcmt_id].pin2_h = NULL;
	dcmd_h[dcmt_id].pin3_h = NULL;

	return -1;
}

int dcmotor_L298N_speed_set(dcmotor_id_e dcmt_id, int speed)
{
	int ret = 0;
	const int dcmt_value_max = DCMOTOR_SPEED_MAX;
	int dcmt_value = 0;
	int dcmt_direction = DCMOTOR_NONE;
	int dcmt_v1 = 0;
	int dcmt_v2 = 0;

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_NONE) {
//		ret = __init_motor_by_id(dcmt_id);
//		if (ret) {
//			_E("failed to __init_motor_by_id()");
//			return -1;
//		}
		return -1;
	}

	if (speed == 0) {
		/* brake and stop */
		ret = __dcmotor_stop(dcmt_id);
		if (ret) {
//			_E("failed to stop motor[%d]", id);
			return -1;
		}
		return 0; /* done */
	}

	if (speed >= 0) dcmt_value = speed;
	else dcmt_value = speed * -1;

	if (dcmt_value > dcmt_value_max) {
		dcmt_value = dcmt_value_max;
//		_D("max speed is %d", value_max);
	}
//	_D("set speed %d", value);


	if (speed > 0)
		dcmt_direction = DCMOTOR_FORWARD; /* will be set forward */
	else
		dcmt_direction = DCMOTOR_BACKWARD; /* will be set backward */

	if (dcmd_h[dcmt_id].dcmt_state == dcmt_direction)
		goto SET_SPEED;
	else {
		/* brake and stop */
		ret = __dcmotor_stop(dcmt_id);
		if (ret) {
//			_E("failed to stop motor[%d]", dcmt_id);
			return -1;
		}
	}

	switch (dcmt_direction) {
	case DCMOTOR_FORWARD:
		dcmt_v1 = 1;
		dcmt_v2 = 0;
		break;
	case DCMOTOR_BACKWARD:
		dcmt_v1 = 0;
		dcmt_v2 = 1;
		break;
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin1_h, dcmt_v1);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("failed to set value[%d] Motor[%d] pin 1", motor_v_1, id);
		return -1;
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin2_h, dcmt_v2);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("failed to set value[%d] Motor[%d] pin 2", motor_v_2, id);
		return -1;
	}

SET_SPEED:
	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin3_h, dcmt_value);
	if (ret != PERIPHERAL_ERROR_NONE) {
//		_E("failed to set value[%d] Motor[%d] pin 2", motor_v_2, id);
		return -1;
	}

	dcmd_h[dcmt_id].dcmt_state = dcmt_direction;

	return 0;
}
