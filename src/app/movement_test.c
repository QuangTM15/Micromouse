
#include "movement_test.h"
#include "drivers/motor.h"
#include "stm32f4xx_hal.h"

#define MOVEMENT_FORWARD_PWM 250
#define MOVEMENT_TURN_PWM 250

#define MOVEMENT_START_DELAY 2000U
#define MOVEMENT_FORWARD_MS 800U
#define MOVEMENT_TURN_MS 400U
#define MOVEMENT_PAUSE_MS 500U

typedef struct
{
    int16_t left_pwm;
    int16_t right_pwm;
    uint32_t duration_ms;
} movement_step_t;

static const movement_step_t movement_steps[] =
    {
        {MOVEMENT_FORWARD_PWM, MOVEMENT_FORWARD_PWM, MOVEMENT_FORWARD_MS},
        {-MOVEMENT_TURN_PWM, MOVEMENT_TURN_PWM, MOVEMENT_TURN_MS},
        {MOVEMENT_FORWARD_PWM, MOVEMENT_FORWARD_PWM, MOVEMENT_FORWARD_MS},
        {MOVEMENT_TURN_PWM, -MOVEMENT_TURN_PWM, MOVEMENT_TURN_MS},
        {MOVEMENT_FORWARD_PWM, MOVEMENT_FORWARD_PWM, MOVEMENT_FORWARD_MS}};

uint8_t movementTestInit(void)
{
    return motorInit();
}

void movementTestRun(void)
{
    uint32_t step = 0U;

    motorDisable();

    HAL_Delay(MOVEMENT_START_DELAY);

    if (0U != motorEnable())
    {
        while (0U == motorIsReady())
        {
            HAL_Delay(1U);
        }

        for (step = 0U; step < (sizeof(movement_steps) / sizeof(movement_steps[0])); step++)
        {
            motorStop();
            HAL_Delay(MOVEMENT_PAUSE_MS);

            motorSetPWM(movement_steps[step].left_pwm,
                        movement_steps[step].right_pwm);

            HAL_Delay(movement_steps[step].duration_ms);
        }
    }

    motorDisable();
}
