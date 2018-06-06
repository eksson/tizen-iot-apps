/*
 * dcmoter_l298n.h
 *
 *  Created on: May 16, 2018
 *      Author: samsung
 */

#ifndef __DCMOTOR_L298N_H__
#define __DCMOTOR_L298N_H__


/* Default channel numbers of PCA9685 with enable pins of L298N */

#define DCMOTOR_SPEED_MAX		1000

/**
 * @brief Enumeration for motor id.
 */
typedef enum {
	DCMOTOR_1,
	DCMOTOR_2,
	DCMOTOR_MAX
} dcmotor_id_e;

typedef enum {
	DCMOTOR_NONE,
	DCMOTOR_STOP,
	DCMOTOR_FORWARD,
	DCMOTOR_BACKWARD,
} dcmotor_state_e;

typedef struct _dcmotor_driver {
	unsigned int pin_1;
	unsigned int pin_2;
	dcmotor_state_e dcmt_state;
	peripheral_gpio_h pin1_h;
	peripheral_gpio_h pin2_h;
} dcmotor_driver_h;

/**
 * @param[in] id The motor id
 * @param[in] pin1 The first pin number to control motor
 * @param[in] pin2 The second pin number to control motor
 * @param[in] en_ch The enable channnel number to control PWM signal
 *
 * @return 0 on success, otherwise a negative error value
 * @before resource_set_motor_driver_L298N_speed() : Optional
 */
int dcmotor_L298N_driver_init(dcmotor_id_e dm_id, unsigned int pin1, unsigned int pin2);

/**
 * @param[in] id The motor id
 * @param[in] speed The speed to control motor, 0 to stop motor,
 * positive value to rotate clockwise and higher value to rotate more fast
 * negative value to rotate couterclockwise and lower value to rotate more fast
 * @return 0 on success, otherwise a negative error value
 * @before resource_set_motor_driver_L298N_speed() : Optional
 */
int dcmotor_L298N_run_set(dcmotor_id_e dm_id, int speed);


void dcmotor_L298N_close(dcmotor_id_e dm_id);

void dcmotor_L298N_close_all(void);

#endif /* __DCMOTOR_L298N_H__ */
