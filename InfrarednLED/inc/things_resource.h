

#ifndef __THINGS_RESOURCE_H__
#define __THINGS_RESOURCE_H__

#include <peripheral_io.h>
#include <Ecore.h>

#include "resource/infrared_motion_sensor.h"
#include "resource/led_2pin.h"


#define GPIO_PIN_MAX	40


extern Eina_Bool _control_sensor_cb(void *data);

extern void things_close_all(void);

#endif /* __THINGS_RESOURCE_H__ */
