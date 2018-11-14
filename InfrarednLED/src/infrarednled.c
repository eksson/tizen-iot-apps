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
#include <tizen.h>
#include <service_app.h>

#include "log.h"

#include "infrarednled.h"
#include "things_resource.h"

bool service_app_create(void *data)
{
	FN_CALL;

	app_data *ad = data;

	ad->getter_timer = ecore_timer_add(10.0f, _control_sensor_cb, ad);

	if (!ad->getter_timer) {
		ERR("Failed to add getter timer");
		return false;
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
