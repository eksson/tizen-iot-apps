

#ifndef __THINGS_RESOURCE_H__
#define __THINGS_RESOURCE_H__

#include <peripheral_io.h>
#include <Ecore.h>

#include <sensor/infrared_motion_sensor.h>
#include <sensor/led_2pin_sensor.h>


#define GPIO_PIN_MAX	40


extern Eina_Bool _control_sensor_cb(void *data);

extern void things_close_all(void);

#endif /* __THINGS_RESOURCE_H__ */
