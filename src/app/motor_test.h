
#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include <stdint.h>

/* Initialize buttons and the motor driver. */
uint8_t motorTestInit(void);

/* Run the motor hardware test once. */
void motorTestRun(void);

#endif /* MOTOR_TEST_H */
