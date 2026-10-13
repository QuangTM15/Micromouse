
#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

/* ============================================================
 * ROBOT CONFIGURATION
 *
 * Hardware-dependent parameters must be calibrated
 * before operating the physical robot.
 * ============================================================ */

/* ============================================================
 * MOTOR DIRECTION
 * ============================================================ */

/* 0: Normal direction
 * 1: Inverted direction
 *
 * Actual direction must be verified on hardware.
 */
#define MOTOR_LEFT_INVERTED 0U
#define MOTOR_RIGHT_INVERTED 0U

/* ============================================================
 * ENCODER DIRECTION
 * ============================================================ */

/* 0: Normal counting direction
 * 1: Inverted counting direction
 *
 * Actual direction must be verified on hardware.
 */
#define ENCODER_LEFT_INVERTED 0U
#define ENCODER_RIGHT_INVERTED 0U

/* ============================================================
 * ROBOT GEOMETRY
 * ============================================================ */

/* Wheel diameter in millimeters */
#define ROBOT_WHEEL_DIAMETER_MM 0.0f

/* Distance between wheel contact centers in millimeters */
#define ROBOT_WHEEL_BASE_MM 0.0f

/* Encoder counts per wheel revolution
 * Include gearbox ratio and quadrature counting mode.
 */
#define ROBOT_ENCODER_CPR 0U

/* ============================================================
 * MOTOR CONTROL
 * ============================================================ */

/* PWM frequency in Hz */
#define MOTOR_PWM_FREQUENCY_HZ 20000U

/* Duty cycle range (permille: 0 to 1000) */
#define MOTOR_PWM_MAX_DUTY 1000U

/* ============================================================
 * IMU CONFIGURATION
 * ============================================================ */

/* IMU axis orientation must be calibrated after mounting.
 *
 * 0: Normal axis
 * 1: Inverted axis
 */
#define IMU_GYRO_X_INVERTED 0U
#define IMU_GYRO_Y_INVERTED 0U
#define IMU_GYRO_Z_INVERTED 0U

#define IMU_ACCEL_X_INVERTED 0U
#define IMU_ACCEL_Y_INVERTED 0U
#define IMU_ACCEL_Z_INVERTED 0U

/* ============================================================
 * MAZE CONFIGURATION
 * ============================================================ */

/* Standard Micromouse maze */
#define MAZE_WIDTH_CELLS 16U
#define MAZE_HEIGHT_CELLS 16U

/* Nominal cell size in millimeters */
#define MAZE_CELL_SIZE_MM 180.0f

#endif /* ROBOT_CONFIG_H */
