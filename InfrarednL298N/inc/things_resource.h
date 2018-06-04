

#ifndef __THINGS_RESOURCE_H__
#define __THINGS_RESOURCE_H__

#include <peripheral_io.h>

#include "resource/infrared_motion_sensor.h"
#include "resource/led_2pin.h"
#include "resource/dcmotor_L298N.h"


#define LED_2PIN_PIN_NUMBER					13
#define INFRARED_MOTION_SENSOR_PIN_NUMBER	21

#define GPIO_PIN_MAX		40
#define MOTOR_SPEED_STEP	10


extern void things_close_all(void);


#endif /* __THINGS_RESOURCE_H__ */
