
#include <peripheral_io.h>

#include "log.h"

#include "infrarednled.h"
#include "things_resource.h"

#define INFRARED_MOTION_SENSOR_PIN_NUMBER	21
#define LED_2PIN_PIN_NUMBER					20


typedef enum {
	LED_LIGHT_OFF = 0,
	LED_LIGHT_ON,
} led_light_on_off;

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

	INFO("Sensor Detected value : %d", value);

	if (value)
		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_ON);
	else
		ret = led_2pin_write(LED_2PIN_PIN_NUMBER, LED_LIGHT_OFF);

	INFO("LED result : %d", ret);

	return ECORE_CALLBACK_RENEW;							//=EINA_TRUE
}

void things_close_all(void)
{
	peripheral_gpio_h sensor_h;

	for (int pin_num = 1; pin_num <= GPIO_PIN_MAX; pin_num++) {
		peripheral_gpio_open(pin_num, &sensor_h);
		if (sensor_h) peripheral_gpio_close(sensor_h);
		INFO("GPIO[%d] is closing...", pin_num);
	}
}
