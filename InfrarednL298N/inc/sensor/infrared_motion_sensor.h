
#ifndef __INFRARED_MOTION_SENSOR_H__
#define __INFRARED_MOTION_SENSOR_H__

/**
 * @brief Reads the value of gpio connected infrared motion sensor(HC-SR501).
 * @param[in] pin_num The number of the gpio pin connected to the infrared motion sensor
 * @param[out] out_value The value of the gpio (zero or non-zero)
 * @return 0 on success, otherwise a negative error value
 * @see If the gpio pin is not open, creates gpio handle before reading the value of gpio.
 */

extern int infrared_motion_sensor_read(int pin_num, uint32_t *out_value);

extern void infrared_motion_sensor_close(int pin_num);

#endif /* __INFRARED_MOTION_SENSOR_H__ */
