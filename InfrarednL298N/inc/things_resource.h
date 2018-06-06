

#ifndef __THINGS_RESOURCE_H__
#define __THINGS_RESOURCE_H__

#include <peripheral_io.h>

#include "resource/infrared_motion_sensor.h"
#include "resource/led_2pin.h"
#include "resource/dcmotor_L298N.h"

#define GPIO_PIN_MAX		40

#define LED_2PIN_PIN_NUMBER					20
#define INFRARED_MOTION_SENSOR_PIN_NUMBER	21

/* Default GPIO pins of raspberry pi 3 */
#define DCMOTOR1_PIN1 27
#define DCMOTOR1_PIN2 22

#define DCMOTOR2_PIN1 6
#define DCMOTOR2_PIN2 5

#define DCMOTOR_SPEED_STEP	10


extern void things_resource_close_all(void);


#endif /* __THINGS_RESOURCE_H__ */
