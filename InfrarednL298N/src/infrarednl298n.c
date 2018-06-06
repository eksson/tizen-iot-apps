/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <Ecore.h>
#include <infrarednl298n.h>
#include <tizen.h>
#include <service_app.h>

#include "log.h"

#include "things_resource.h"

int dcmt_1_speed = 0;
int dcmt_2_speed = 0;

Eina_Bool _control_sensor_cb(void *data)
{
	FN_CALL;

	uint32_t value = -1;
	int ret = -1;

	app_data *ad = (app_data *)data;

	if (!ad->getter_timer) {
		ERR("Cannot read a getter_timer");
		return ECORE_CALLBACK_CANCEL;						//=EINA_FALSE
	}

	ret = infrared_motion_sensor_read(INFRARED_MOTION_SENSOR_PIN_NUMBER, &value);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("infrared_motion_sensor_read() failed!![%d]", ret);
		service_app_exit();
	}

	INFO("Sensor Detected value : %d", value);

	if (value) {
		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_ON);

		dcmt_1_speed += DCMOTOR_SPEED_STEP;
		if (dcmt_1_speed > DCMOTOR_SPEED_MAX)
			dcmt_1_speed = ((int)(DCMOTOR_SPEED_MAX / DCMOTOR_SPEED_STEP)) * DCMOTOR_SPEED_STEP;

		dcmt_2_speed += DCMOTOR_SPEED_STEP;
		if (dcmt_2_speed > DCMOTOR_SPEED_MAX)
			dcmt_2_speed = ((int)(DCMOTOR_SPEED_MAX / DCMOTOR_SPEED_STEP)) * DCMOTOR_SPEED_STEP;
	} else {
		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_OFF);

		dcmt_1_speed -= DCMOTOR_SPEED_STEP;
		if (dcmt_1_speed > 0)
			dcmt_1_speed = 0;

		dcmt_2_speed -= DCMOTOR_SPEED_STEP;
		if (dcmt_2_speed < 0)
			dcmt_2_speed = 0;
	}

	dcmotor_L298N_speed_set(DCMOTOR_1, dcmt_1_speed);
	dcmotor_L298N_speed_set(DCMOTOR_2, dcmt_2_speed);

	INFO("LED result : %d", ret);

	INFO("motor speed : DCMOTOR_1[%d], DCMOTOR_2[%d]...", dcmt_1_speed, dcmt_2_speed);

	return ECORE_CALLBACK_RENEW;							//=EINA_TRUE
}

bool service_app_create(void *data)
{
	FN_CALL;

	app_data *ad = (app_data *)data;

	int ret = -1;

	ad->getter_timer = ecore_timer_add(1.0f, _control_sensor_cb, ad);

	if (!ad->getter_timer) {
		ERR("Failed to add getter timer");
		return false;
	}

	ret = dcmotor_L298N_driver_init(DCMOTOR_1, DCMOTOR1_PIN1, DCMOTOR1_PIN2, DCMOTOR1_EN_CH);
	if (ret) {
		ERR("dcmotor_L298N_dirver_init(DCMOTOR_1) %d : ", ret);
		service_app_exit();
	}

	ret = dcmotor_L298N_driver_init(DCMOTOR_2, DCMOTOR2_PIN1, DCMOTOR2_PIN2, DCMOTOR2_EN_CH);
	if (ret) {
		ERR("dcmotor_L298N_driver_init(DCMOTOR_2) %d : ", ret);
		service_app_exit();
	}

	return true;
}

void service_app_terminate(void *data)
{
	app_data *ad = (app_data *)data;

	if (ad->getter_timer) {
		ecore_timer_del(ad->getter_timer);
	}

	things_resource_close_all();

	free(ad);
}

void service_app_control(app_control_h app_control, void *data)
{
    /* APP_CONTROL */
	dcmotor_L298N_speed_set(DCMOTOR_1, 0);
	dcmotor_L298N_speed_set(DCMOTOR_2, 0);
//	resource_set_servo_motor_value(0, 450);
}

int main(int argc, char* argv[])
{
	app_data *ad = NULL;
	int ret = 0;
	service_app_lifecycle_callback_s event_callback;


	ad = calloc(1, sizeof(app_data));

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	ret = service_app_main(argc, argv, &event_callback, ad);

	return ret;
}
