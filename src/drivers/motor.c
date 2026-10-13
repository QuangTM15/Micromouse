
#include "drivers/motor.h"
#include "config/board_config.h"
#include "config/robot_config.h"
#include "stm32f4xx_hal.h"
#include <stddef.h>

#define MOTOR_WAKEUP_TIME_MS 2U

static TIM_HandleTypeDef htim2;
static uint32_t pwm_ticks = 0U;
static uint32_t enable_tick = 0U;
static uint8_t motor_initialized = 0U;
static uint8_t motor_enabled = 0U;

static int16_t left_command = 0;
static int16_t right_command = 0;

static uint32_t motorGetTimerClock(void);
static uint32_t motorDutyToCompare(uint16_t duty);
static uint32_t motorModeToCompare(uint8_t mode, uint16_t duty);
static void motorApplyOutput(uint8_t motor, const motor_output_t *output);
static void motorSetCoastOutputs(void);
static void motorUpdateOutputs(void);

uint8_t motorComputeOutput(uint8_t motor, int16_t command, uint8_t inverted, motor_output_t *output)
{
    uint8_t result = 0U;
    int32_t value = (int32_t)command;
    uint16_t duty = 0U;

    if ((NULL != output) &&
        ((MOTOR_LEFT == motor) || (MOTOR_RIGHT == motor)) &&
        (inverted <= 1U))
    {
        result = 1U;

        if (0U != inverted)
        {
            value = -value;
        }

        if (value > (int32_t)MOTOR_DUTY_MAX)
        {
            value = (int32_t)MOTOR_DUTY_MAX;
        }
        else if (value < -(int32_t)MOTOR_DUTY_MAX)
        {
            value = -(int32_t)MOTOR_DUTY_MAX;
        }

        output->in1_mode = MOTOR_OUTPUT_LOW;
        output->in2_mode = MOTOR_OUTPUT_LOW;
        output->in1_duty = 0U;
        output->in2_duty = 0U;

        if (value > 0)
        {
            duty = (uint16_t)value;
            output->in1_mode = MOTOR_OUTPUT_PWM;
            output->in1_duty = duty;
        }
        else if (value < 0)
        {
            duty = (uint16_t)(-value);

            if (MOTOR_LEFT == motor)
            {
                output->in2_mode = MOTOR_OUTPUT_PWM;
                output->in2_duty = duty;
            }
            else
            {
                /* Right IN2 is GPIO-only. */
                output->in1_mode = MOTOR_OUTPUT_PWM;
                output->in2_mode = MOTOR_OUTPUT_HIGH;
                output->in1_duty = (uint16_t)(MOTOR_DUTY_MAX - duty);
            }
        }
    }

    return result;
}

uint8_t motorComputeBrake(motor_output_t *output)
{
    uint8_t result = 0U;

    if (NULL != output)
    {
        output->in1_mode = MOTOR_OUTPUT_HIGH;
        output->in2_mode = MOTOR_OUTPUT_HIGH;
        output->in1_duty = 0U;
        output->in2_duty = 0U;
        result = 1U;
    }

    return result;
}

static uint32_t motorGetTimerClock(void)
{
    RCC_ClkInitTypeDef clocks = {0};
    uint32_t latency = 0U;
    uint32_t clock = HAL_RCC_GetPCLK1Freq();

    HAL_RCC_GetClockConfig(&clocks, &latency);

    if (RCC_HCLK_DIV1 != clocks.APB1CLKDivider)
    {
        clock *= 2U;
    }

    return clock;
}

static uint32_t motorDutyToCompare(uint16_t duty)
{
    uint32_t compare = 0U;

    if (duty >= MOTOR_DUTY_MAX)
    {
        compare = pwm_ticks;
    }
    else
    {
        compare = (pwm_ticks * (uint32_t)duty) / MOTOR_DUTY_MAX;
    }

    return compare;
}

static uint32_t motorModeToCompare(uint8_t mode, uint16_t duty)
{
    uint32_t compare = 0U;

    if (MOTOR_OUTPUT_HIGH == mode)
    {
        compare = pwm_ticks;
    }
    else if (MOTOR_OUTPUT_PWM == mode)
    {
        compare = motorDutyToCompare(duty);
    }

    return compare;
}

static void motorSetCoastOutputs(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, MOTOR_LEFT_IN1_CHANNEL, 0U);
    __HAL_TIM_SET_COMPARE(&htim2, MOTOR_LEFT_IN2_CHANNEL, 0U);
    __HAL_TIM_SET_COMPARE(&htim2, MOTOR_RIGHT_PWM_CHANNEL, 0U);

    HAL_GPIO_WritePin(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN, GPIO_PIN_RESET);
}

static void motorApplyOutput(uint8_t motor, const motor_output_t *output)
{
    uint32_t compare1 = 0U;
    uint32_t compare2 = 0U;

    if (NULL != output)
    {
        compare1 = motorModeToCompare(output->in1_mode, output->in1_duty);
        compare2 = motorModeToCompare(output->in2_mode, output->in2_duty);

        if (MOTOR_LEFT == motor)
        {
            __HAL_TIM_SET_COMPARE(&htim2, MOTOR_LEFT_IN1_CHANNEL, compare1);
            __HAL_TIM_SET_COMPARE(&htim2, MOTOR_LEFT_IN2_CHANNEL, compare2);
        }
        else if (MOTOR_RIGHT == motor)
        {
            /*
             * Update GPIO before PWM. Outputs are first driven
             * to coast by motorUpdateOutputs().
             */
            HAL_GPIO_WritePin(
                MOTOR_RIGHT_IN2_PORT,
                MOTOR_RIGHT_IN2_PIN,
                (MOTOR_OUTPUT_HIGH == output->in2_mode) ? GPIO_PIN_SET : GPIO_PIN_RESET);

            __HAL_TIM_SET_COMPARE(&htim2, MOTOR_RIGHT_PWM_CHANNEL, compare1);
        }
    }
}

static void motorUpdateOutputs(void)
{
    motor_output_t left_output = {0};
    motor_output_t right_output = {0};

    if ((0U != motor_initialized) && (0U != motorIsReady()))
    {
        if ((0U != motorComputeOutput(MOTOR_LEFT, left_command, MOTOR_LEFT_INVERTED, &left_output)) &&
            (0U != motorComputeOutput(MOTOR_RIGHT, right_command, MOTOR_RIGHT_INVERTED, &right_output)))
        {
            motorSetCoastOutputs();

            motorApplyOutput(MOTOR_LEFT, &left_output);
            motorApplyOutput(MOTOR_RIGHT, &right_output);
        }
    }
}

uint8_t motorInit(void)
{
    uint8_t result = 0U;
    uint8_t valid = 1U;
    uint32_t timer_clock = 0U;

    GPIO_InitTypeDef gpio = {0};
    TIM_OC_InitTypeDef pwm = {0};

    motor_initialized = 0U;
    motor_enabled = 0U;
    left_command = 0;
    right_command = 0;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_GPIO_WritePin(MOTOR_SLEEP_PORT, MOTOR_SLEEP_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN, GPIO_PIN_RESET);

    gpio.Pin = MOTOR_SLEEP_PIN | MOTOR_RIGHT_IN2_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = MOTOR_LEFT_IN1_PIN | MOTOR_LEFT_IN2_PIN | MOTOR_RIGHT_IN1_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &gpio);

    timer_clock = motorGetTimerClock();

    if ((0U == MOTOR_PWM_FREQUENCY_HZ) ||
        (timer_clock < MOTOR_PWM_FREQUENCY_HZ))
    {
        valid = 0U;
    }

    if (0U != valid)
    {
        pwm_ticks = timer_clock / MOTOR_PWM_FREQUENCY_HZ;

        htim2.Instance = MOTOR_PWM_TIMER;
        htim2.Init.Prescaler = 0U;
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim2.Init.Period = pwm_ticks - 1U;
        htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        if (HAL_OK != HAL_TIM_PWM_Init(&htim2))
        {
            valid = 0U;
        }
    }

    if (0U != valid)
    {
        pwm.OCMode = TIM_OCMODE_PWM1;
        pwm.Pulse = 0U;
        pwm.OCPolarity = TIM_OCPOLARITY_HIGH;
        pwm.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_OK != HAL_TIM_PWM_ConfigChannel(&htim2, &pwm, MOTOR_LEFT_IN1_CHANNEL))
        {
            valid = 0U;
        }

        if ((0U != valid) &&
            (HAL_OK != HAL_TIM_PWM_ConfigChannel(&htim2, &pwm, MOTOR_LEFT_IN2_CHANNEL)))
        {
            valid = 0U;
        }

        if ((0U != valid) &&
            (HAL_OK != HAL_TIM_PWM_ConfigChannel(&htim2, &pwm, MOTOR_RIGHT_PWM_CHANNEL)))
        {
            valid = 0U;
        }
    }

    if (0U != valid)
    {
        if (HAL_OK != HAL_TIM_PWM_Start(&htim2, MOTOR_LEFT_IN1_CHANNEL))
        {
            valid = 0U;
        }

        if ((0U != valid) &&
            (HAL_OK != HAL_TIM_PWM_Start(&htim2, MOTOR_LEFT_IN2_CHANNEL)))
        {
            valid = 0U;
        }

        if ((0U != valid) &&
            (HAL_OK != HAL_TIM_PWM_Start(&htim2, MOTOR_RIGHT_PWM_CHANNEL)))
        {
            valid = 0U;
        }
    }

    if (0U != valid)
    {
        motorSetCoastOutputs();
        motor_initialized = 1U;
        result = 1U;
    }
    else
    {
        HAL_GPIO_WritePin(MOTOR_SLEEP_PORT, MOTOR_SLEEP_PIN, GPIO_PIN_RESET);
    }

    return result;
}

uint8_t motorEnable(void)
{
    uint8_t result = 0U;

    if (0U != motor_initialized)
    {
        if (0U == motor_enabled)
        {
            motorSetCoastOutputs();
            left_command = 0;
            right_command = 0;

            enable_tick = HAL_GetTick();
            motor_enabled = 1U;

            HAL_GPIO_WritePin(MOTOR_SLEEP_PORT, MOTOR_SLEEP_PIN, GPIO_PIN_SET);
        }

        result = 1U;
    }

    return result;
}

uint8_t motorIsReady(void)
{
    uint8_t result = 0U;

    if ((0U != motor_initialized) && (0U != motor_enabled))
    {
        if ((HAL_GetTick() - enable_tick) >= MOTOR_WAKEUP_TIME_MS)
        {
            result = 1U;
        }
    }

    return result;
}

void motorSetPWM(int16_t left, int16_t right)
{
    if (0U != motorIsReady())
    {
        left_command = left;
        right_command = right;
        motorUpdateOutputs();
    }
}

void motorSetLeftPWM(int16_t command)
{
    if (0U != motorIsReady())
    {
        left_command = command;
        motorUpdateOutputs();
    }
}

void motorSetRightPWM(int16_t command)
{
    if (0U != motorIsReady())
    {
        right_command = command;
        motorUpdateOutputs();
    }
}

void motorStop(void)
{
    if (0U != motor_initialized)
    {
        left_command = 0;
        right_command = 0;
        motorSetCoastOutputs();
    }
}

void motorBrake(void)
{
    motor_output_t brake = {0};
    if ((0U != motorIsReady()) &&
        (0U != motorComputeBrake(&brake)))
    {
        left_command = 0;
        right_command = 0;
        motorSetCoastOutputs();
        motorApplyOutput(MOTOR_LEFT, &brake);
        motorApplyOutput(MOTOR_RIGHT, &brake);
    }
}

void motorDisable(void)
{
    if (0U != motor_initialized)
    {
        motorSetCoastOutputs();
    }
    left_command = 0;
    right_command = 0;
    motor_enabled = 0U;
    HAL_GPIO_WritePin(MOTOR_SLEEP_PORT, MOTOR_SLEEP_PIN, GPIO_PIN_RESET);
}
