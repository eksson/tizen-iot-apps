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

Eina_Bool _control_sensor_cb(void *data)
{
	FN_CALL;

	uint32_t sensor_value = -1;
	int ret = -1;

	app_data *ad = (app_data *)data;

	if (!ad->getter_timer) {
		ERR("Cannot read a getter_timer");
		return ECORE_CALLBACK_CANCEL;						//=EINA_FALSE
	}

	ret = infrared_motion_sensor_read(INFRARED_MOTION_SENSOR_PIN_NUMBER, &sensor_value);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("infrared_motion_sensor_read() failed!![%d]", ret);
		dcmotor_L298N_run_set(DCMOTOR_1, DCMOTOR_STOP);
		dcmotor_L298N_run_set(DCMOTOR_2, DCMOTOR_STOP);

		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_OFF);
	} else if (sensor_value) {
		dcmotor_L298N_run_set(DCMOTOR_1, DCMOTOR_FORWARD);
		dcmotor_L298N_run_set(DCMOTOR_2, DCMOTOR_FORWARD);
		INFO("dcmotors' status : DCMOTOR_1 run forward, DCMOTOR_2 run forward...");

		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_ON);
	} else {
		dcmotor_L298N_run_set(DCMOTOR_1, DCMOTOR_BACKWARD);
		dcmotor_L298N_run_set(DCMOTOR_2, DCMOTOR_BACKWARD);
		INFO("dcmotors' status : DCMOTOR_1 run backward, DCMOTOR_2 run forward...");

		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_OFF);
	}

	INFO("Sensor Detected value : %d", sensor_value);

	INFO("LED result : %d", ret);

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

	ret = dcmotor_L298N_driver_init(DCMOTOR_1, DCMOTOR1_PIN1, DCMOTOR1_PIN2);
	if (ret) {
		ERR("dcmotor_L298N_dirver_init(DCMOTOR_1) %d : ", ret);
		service_app_exit();
	}

	ret = dcmotor_L298N_driver_init(DCMOTOR_2, DCMOTOR2_PIN1, DCMOTOR2_PIN2);
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

	free(ad);
}

void service_app_control(app_control_h app_control, void *data)
{
    /* APP_CONTROL */
	dcmotor_L298N_run_set(DCMOTOR_1, DCMOTOR_STOP);
	dcmotor_L298N_run_set(DCMOTOR_2, DCMOTOR_STOP);
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
