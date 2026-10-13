
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32f4xx_hal.h"

/* ============================================================
 * MOTOR DRIVER - DRV8833
 * ============================================================ */

/* Driver enable (nSLEEP) */
#define MOTOR_SLEEP_PORT GPIOA
#define MOTOR_SLEEP_PIN GPIO_PIN_0

/* Left motor - M3 */
#define MOTOR_LEFT_IN1_PORT GPIOA
#define MOTOR_LEFT_IN1_PIN GPIO_PIN_1
#define MOTOR_LEFT_IN2_PORT GPIOA
#define MOTOR_LEFT_IN2_PIN GPIO_PIN_2

/* Right motor - M4 */
#define MOTOR_RIGHT_IN1_PORT GPIOA
#define MOTOR_RIGHT_IN1_PIN GPIO_PIN_3
#define MOTOR_RIGHT_IN2_PORT GPIOA
#define MOTOR_RIGHT_IN2_PIN GPIO_PIN_4

/* Motor PWM configuration
 * PA1 -> TIM2_CH2
 * PA2 -> TIM2_CH3
 * PA3 -> TIM2_CH4
 * PA4 -> GPIO (no hardware PWM)
 */
#define MOTOR_PWM_TIMER TIM2

#define MOTOR_LEFT_IN1_CHANNEL TIM_CHANNEL_2
#define MOTOR_LEFT_IN2_CHANNEL TIM_CHANNEL_3
#define MOTOR_RIGHT_PWM_CHANNEL TIM_CHANNEL_4

/* ============================================================
 * WHEEL ENCODERS
 * ============================================================ */

/* Left encoder - M3
 * PB4 -> TIM3_CH1
 * PB5 -> TIM3_CH2
 */
#define ENCODER_LEFT_TIMER TIM3
#define ENCODER_LEFT_A_PORT GPIOB
#define ENCODER_LEFT_A_PIN GPIO_PIN_4
#define ENCODER_LEFT_B_PORT GPIOB
#define ENCODER_LEFT_B_PIN GPIO_PIN_5

/* Right encoder - M4
 * PA8 -> TIM1_CH1 (C2 on schematic)
 * PA9 -> TIM1_CH2 (C1 on schematic)
 */
#define ENCODER_RIGHT_TIMER TIM1
#define ENCODER_RIGHT_A_PORT GPIOA
#define ENCODER_RIGHT_A_PIN GPIO_PIN_8
#define ENCODER_RIGHT_B_PORT GPIOA
#define ENCODER_RIGHT_B_PIN GPIO_PIN_9

/* ============================================================
 * I2C BUS - ICM-20602 AND VL53L0X
 * ============================================================ */

/* I2C1:
 * PB8 -> SCL
 * PB9 -> SDA
 */
#define SENSOR_I2C_INSTANCE I2C1

#define SENSOR_I2C_SCL_PORT GPIOB
#define SENSOR_I2C_SCL_PIN GPIO_PIN_8
#define SENSOR_I2C_SDA_PORT GPIOB
#define SENSOR_I2C_SDA_PIN GPIO_PIN_9

/* ICM-20602 - I2C mode */
#define IMU_CS_PORT GPIOA
#define IMU_CS_PIN GPIO_PIN_6

/* 7-bit I2C address (SA0 connected to 3.3V) */
#define IMU_I2C_ADDRESS 0x69U

/* ============================================================
 * VL53L0X - XSHUT CONTROL
 * ============================================================ */

/* Sensor U3 - XSHUT1 */
#define TOF_1_XSHUT_PORT GPIOB
#define TOF_1_XSHUT_PIN GPIO_PIN_12

/* Sensor U6 - XSHUT2 */
#define TOF_2_XSHUT_PORT GPIOB
#define TOF_2_XSHUT_PIN GPIO_PIN_13

/* Sensor U2 - XSHUT3 */
#define TOF_3_XSHUT_PORT GPIOB
#define TOF_3_XSHUT_PIN GPIO_PIN_14

/* Sensor U5 - XSHUT4 */
#define TOF_4_XSHUT_PORT GPIOA
#define TOF_4_XSHUT_PIN GPIO_PIN_11

/* Sensor U4 - XSHUT5 */
#define TOF_5_XSHUT_PORT GPIOA
#define TOF_5_XSHUT_PIN GPIO_PIN_10

#define TOF_SENSOR_COUNT 5U

/* Default 7-bit address before reassignment */
#define TOF_DEFAULT_I2C_ADDRESS 0x29U

/* ============================================================
 * USER BUTTONS
 * ============================================================ */

#define BUTTON_1_PORT GPIOB
#define BUTTON_1_PIN GPIO_PIN_6

#define BUTTON_2_PORT GPIOB
#define BUTTON_2_PIN GPIO_PIN_7

#endif /* BOARD_CONFIG_H */
