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

int motor_id_1_speed = 0;
int motor_id_2_speed = 0;

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

	if (!value) {
		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_ON);

		motor_id_1_speed += MOTOR_SPEED_STEP;
		if (motor_id_1_speed > MOTOR_SPEED_MAX)
			motor_id_1_speed = ((int)(MOTOR_SPEED_MAX / MOTOR_SPEED_STEP)) * MOTOR_SPEED_STEP;

		motor_id_2_speed += MOTOR_SPEED_STEP;
		if (motor_id_2_speed > MOTOR_SPEED_MAX)
			motor_id_2_speed = ((int)(MOTOR_SPEED_MAX / MOTOR_SPEED_STEP)) * MOTOR_SPEED_STEP;

		dcmotor_L298N_speed_set(MOTOR_ID_1, motor_id_1_speed);
		dcmotor_L298N_speed_set(MOTOR_ID_2, motor_id_2_speed);
	} else {
		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_OFF);

		motor_id_1_speed -= MOTOR_SPEED_STEP;
		if (motor_id_1_speed > 0)
			motor_id_1_speed = 0;

		motor_id_2_speed -= MOTOR_SPEED_STEP;
		if (motor_id_2_speed < 0)
			motor_id_2_speed = 0;

		dcmotor_L298N_speed_set(MOTOR_ID_1, motor_id_1_speed);
		dcmotor_L298N_speed_set(MOTOR_ID_2, motor_id_2_speed);
	}

	INFO("LED result : %d", ret);

	INFO("motor speed : MOTOR_ID_1[%d], MOTOR_ID_2[%d]...", motor_id_1_speed, motor_id_2_speed);

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

	ret = dcmotor_L298N_configuration_set(MOTOR_ID_1, 19, 16, 5);
	if (ret) {
		ERR("dcmotor_L298N_configuration_set(MOTOR_ID_1) %d : ", ret);
		service_app_exit();
	}

	ret = dcmotor_L298N_configuration_set(MOTOR_ID_2, 26, 20, 4);
	if (ret) {
		ERR("dcmotor_L298N_configuration_set(MOTOR_ID_2) %d : ", ret);
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

	things_close_all();

	free(ad);
}

void service_app_control(app_control_h app_control, void *data)
{
    /* APP_CONTROL */
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
