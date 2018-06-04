/*
 * dcmoter_l298n.h
 *
 *  Created on: May 16, 2018
 *      Author: samsung
 */

#ifndef __DCMOTOR_L298N_H__
#define __DCMOTOR_L298N_H__


/* Default GPIO pins of raspberry pi 3 connected with IN pins of L298N */
#define DEFAULT_MOTOR1_PIN1 26
#define DEFAULT_MOTOR1_PIN2 20

#define DEFAULT_MOTOR2_PIN1 19
#define DEFAULT_MOTOR2_PIN2 16

#define DEFAULT_MOTOR3_PIN1 6
#define DEFAULT_MOTOR3_PIN2 12

#define DEFAULT_MOTOR4_PIN1 22
#define DEFAULT_MOTOR4_PIN2 23

/* Default channel numbers of PCA9685 with enable pins of L298N */
#define DEFAULT_MOTOR1_EN_CH 1
#define DEFAULT_MOTOR2_EN_CH 2
#define DEFAULT_MOTOR3_EN_CH 3
#define DEFAULT_MOTOR4_EN_CH 4

#define MOTOR_SPEED_MAX		4095

/**
 * @brief Enumeration for motor id.
 */
typedef enum {
	MOTOR_ID_1,
	MOTOR_ID_2,
	MOTOR_ID_3,
	MOTOR_ID_4,
	MOTOR_ID_MAX
} motor_id_e;

/**
 * @param[in] id The motor id
 * @param[in] pin1 The first pin number to control motor
 * @param[in] pin2 The second pin number to control motor
 * @param[in] en_ch The enable channnel number to control PWM signal
 *
 * @return 0 on success, otherwise a negative error value
 * @before resource_set_motor_driver_L298N_speed() : Optional
 */
int dcmotor_L298N_configuration_set(motor_id_e id,
	unsigned int pin1, unsigned int pin2, unsigned en_ch);

/**
 * @param[in] id The motor id
 * @param[in] speed The speed to control motor, 0 to stop motor,
 * positive value to rotate clockwise and higher value to rotate more fast
 * negative value to rotate couterclockwise and lower value to rotate more fast
 * @return 0 on success, otherwise a negative error value
 * @before resource_set_motor_driver_L298N_speed() : Optional
 */
int dcmotor_L298N_speed_set(motor_id_e id, int speed);


void dcmotor_L298N_close(motor_id_e id);

void dcmotor_L298N_close_all(void);

#endif /* __DCMOTOR_L298N_H__ */
