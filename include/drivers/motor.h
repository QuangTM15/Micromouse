
#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#define MOTOR_DUTY_MAX 1000U

#define MOTOR_OUTPUT_LOW 0U
#define MOTOR_OUTPUT_HIGH 1U
#define MOTOR_OUTPUT_PWM 2U

#define MOTOR_LEFT 0U
#define MOTOR_RIGHT 1U

typedef struct
{
    uint8_t in1_mode;
    uint8_t in2_mode;
    uint16_t in1_duty;
    uint16_t in2_duty;
} motor_output_t;

/* Output calculation. */
uint8_t motorComputeOutput(uint8_t motor, int16_t command, uint8_t inverted, motor_output_t *output);
uint8_t motorComputeBrake(motor_output_t *output);

/* Hardware driver. */
uint8_t motorInit(void);
uint8_t motorEnable(void);
uint8_t motorIsReady(void);

void motorSetPWM(int16_t left, int16_t right);
void motorSetLeftPWM(int16_t command);
void motorSetRightPWM(int16_t command);

void motorStop(void);
void motorBrake(void);
void motorDisable(void);

#endif /* MOTOR_H */
