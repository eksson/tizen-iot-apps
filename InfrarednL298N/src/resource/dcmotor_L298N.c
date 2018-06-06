/*
 * dcmoter_l298n.c
 *
 *  Created on: May 16, 2018
 *      Author: samsung
 */


#include <peripheral_io.h>

#include "log.h"
#include "resource/dcmotor_L298N.h"


static dcmotor_driver_h dcmd_h[DCMOTOR_MAX] = {
	{0, 0, DCMOTOR_NONE, NULL, NULL},
};


/* see Principle section in http://wiki.sunfounder.cc/index.php?title=Motor_Driver_Module-L298N */

static int __dcmotor_stop(dcmotor_id_e dcmt_id)
{
	int ret = PERIPHERAL_ERROR_NONE;
	int dcmt_v1 = 0;
	int dcmt_v2 = 0;

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_NONE) {
		ERR("the dcmotor[%d] is not initialized - state(%d)",
			dcmt_id, dcmd_h[dcmt_id].dcmt_state);
		return -1;
	}

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_STOP) {
		INFO("the dcmotor[%d] is already stopped", dcmt_id);
		return 0;
	}

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_FORWARD) {
		dcmt_v1 = 0;
		dcmt_v2 = 0;
	} else if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_BACKWARD) {
		dcmt_v1 = 1;
		dcmt_v2 = 1;
	}

	/* Brake DC motor */
	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin1_h, dcmt_v1);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("Failed to set value[%d] on the dcmotor[%d] pin 1", dcmt_v1, dcmt_id);
		return -1;
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin2_h, dcmt_v2);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("Failed to set value[%d] on the dcmotor[%d] pin 2", dcmt_v2, dcmt_id);
		return -1;
	}

	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	return 0;
}

static int __dcmotor_close(dcmotor_id_e dcmt_id)
{
	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_NONE) {
		ERR("the dcmotor[%d] is not initialized - state(%d)",
			dcmt_id, dcmd_h[dcmt_id].dcmt_state);
		return -1;
	}

	if (dcmd_h[dcmt_id].dcmt_state > DCMOTOR_STOP)
		__dcmotor_stop(dcmt_id);

	if (dcmd_h[dcmt_id].pin1_h) {
		peripheral_gpio_close(dcmd_h[dcmt_id].pin1_h);
		dcmd_h[dcmt_id].pin1_h = NULL;
	}

	if (dcmd_h[dcmt_id].pin2_h) {
		peripheral_gpio_close(dcmd_h[dcmt_id].pin2_h);
		dcmd_h[dcmt_id].pin2_h = NULL;
	}

	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	return 0;
}

void dcmotor_L298N_close(dcmotor_id_e dcmt_id)
{
	__dcmotor_close(dcmt_id);
}

void dcmotor_L298N_close_all(void)
{
	for (int i = DCMOTOR_1; i < DCMOTOR_MAX; i++)
		__dcmotor_close(i);
}

int dcmotor_L298N_driver_init(dcmotor_id_e dcmt_id,	unsigned int pin1, unsigned int pin2)
{
	int ret;

	if (dcmd_h[dcmt_id].dcmt_state > DCMOTOR_NONE) {
		ERR("cannot initialize a dcmotor[%d] in this state[%d]", dcmt_id, dcmd_h[dcmt_id].dcmt_state);
		return -1;
	}

	dcmd_h[dcmt_id].pin_1 = pin1;
	dcmd_h[dcmt_id].pin_2 = pin2;
	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	ret = peripheral_gpio_open(dcmd_h[dcmt_id].pin_1, &dcmd_h[dcmt_id].pin1_h);
	if (ret == PERIPHERAL_ERROR_NONE) {
		peripheral_gpio_set_direction(dcmd_h[dcmt_id].pin1_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	} else {
		ERR("failed to open a dcmotor[%d] gpio pin1[%u]", dcmt_id, dcmd_h[dcmt_id].pin_1);
		goto ERROR;
	}

	ret = peripheral_gpio_open(dcmd_h[dcmt_id].pin_2, &dcmd_h[dcmt_id].pin2_h);
	if (ret == PERIPHERAL_ERROR_NONE) {
		peripheral_gpio_set_direction(dcmd_h[dcmt_id].pin2_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	} else {
		ERR("failed to open a dcmotor[%d] gpio pin2[%u]", dcmt_id, dcmd_h[dcmt_id].pin_2);
		goto ERROR;
	}

	dcmd_h[dcmt_id].pin_1 = pin1;
	dcmd_h[dcmt_id].pin_2 = pin2;
	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_STOP;

	return 0;

ERROR:
	dcmd_h[dcmt_id].pin_1 = 0;
	dcmd_h[dcmt_id].pin_2 = 0;
	dcmd_h[dcmt_id].dcmt_state = DCMOTOR_NONE;
	dcmd_h[dcmt_id].pin1_h = NULL;
	dcmd_h[dcmt_id].pin2_h = NULL;

	return -1;
}

int dcmotor_L298N_run_set(dcmotor_id_e dcmt_id, int dcmt_run)
{
	int ret = 0;
	int dcmt_v1 = 0;
	int dcmt_v2 = 0;

	if (dcmd_h[dcmt_id].dcmt_state == DCMOTOR_NONE) {
		ERR("a dcmoter[%d] is not connected!", dcmt_id);
		return -1;
	}

	if (dcmt_run == DCMOTOR_STOP) {		//stop a dcmotor
		ret = __dcmotor_stop(dcmt_id);
		if (ret) {
			ERR("failed to stop a dcmotor[%d]", dcmt_id);
			return -1;
		}
		return 0; /* done */
	}

	switch (dcmt_run) {
	case DCMOTOR_FORWARD:
		dcmt_v1 = 1;
		dcmt_v2 = 0;
		break;
	case DCMOTOR_BACKWARD:
		dcmt_v1 = 0;
		dcmt_v2 = 1;
		break;
	}

	if (dcmd_h[dcmt_id].dcmt_state != dcmt_run) {
		ret = __dcmotor_stop(dcmt_id);
		if (ret) {
			ERR("failed to stop a dcmotor[%d]", dcmt_id);
			return -1;
		}
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin1_h, dcmt_v1);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("failed to set value[%d] on the dcmotor[%d] pin 1", dcmt_v1, dcmt_id);
		return -1;
	}

	ret = peripheral_gpio_write(dcmd_h[dcmt_id].pin2_h, dcmt_v2);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("failed to set value[%d] on the dcmotor[%d] pin 2", dcmt_v2, dcmt_id);
		return -1;
	}

	dcmd_h[dcmt_id].dcmt_state = dcmt_run;

	return 0;
}
