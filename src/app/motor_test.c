
#include "motor_test.h"
#include "drivers/motor.h"
#include "config/board_config.h"
#include "stm32f4xx_hal.h"

#define MOTOR_TEST_DUTY 200
#define MOTOR_TEST_RUN_MS 1000U
#define MOTOR_TEST_PAUSE_MS 500U
#define MOTOR_TEST_POLL_MS 10U
#define MOTOR_TEST_DEBOUNCE 30U

typedef struct
{
    int16_t left;
    int16_t right;
} motor_test_step_t;

static const motor_test_step_t test_steps[] =
    {
        {MOTOR_TEST_DUTY, 0},
        {-MOTOR_TEST_DUTY, 0},
        {0, MOTOR_TEST_DUTY},
        {0, -MOTOR_TEST_DUTY}};

static void motorTestButtonsInit(void);
static uint8_t motorTestButtonPressed(uint16_t pin);
static uint8_t motorTestWait(uint32_t duration);
static uint8_t motorTestWaitRelease(uint16_t pin);

uint8_t motorTestInit(void)
{
    uint8_t result = 0U;
    motorTestButtonsInit();
    result = motorInit();
    return result;
}

void motorTestRun(void)
{
    uint8_t completed = 1U;
    uint8_t step = 0U;
    motorDisable();
    /* Wait for BT1 to be pressed. */
    while (0U == motorTestButtonPressed(BUTTON_1_PIN))
    {
        HAL_Delay(MOTOR_TEST_POLL_MS);
    }
    /* Wait for BT1 release. */
    completed = motorTestWaitRelease(BUTTON_1_PIN);
    if (0U != completed)
    {
        completed = motorEnable();
    }
    if (0U != completed)
    {
        /* Allow DRV8833 to complete its wake-up sequence. */
        while (0U == motorIsReady())
        {
            if (0U != motorTestButtonPressed(BUTTON_2_PIN))
            {
                completed = 0U;
                break;
            }
            HAL_Delay(MOTOR_TEST_POLL_MS);
        }
    }
    for (step = 0U;
         (step < (sizeof(test_steps) / sizeof(test_steps[0]))) &&
         (0U != completed);
         step++)
    {
        motorStop();
        completed = motorTestWait(MOTOR_TEST_PAUSE_MS);
        if (0U != completed)
        {
            motorSetPWM(test_steps[step].left, test_steps[step].right);
            completed = motorTestWait(MOTOR_TEST_RUN_MS);
        }
    }
    motorDisable();
    /* Wait until both buttons are released. */
    while ((0U != motorTestButtonPressed(BUTTON_1_PIN)) ||
           (0U != motorTestButtonPressed(BUTTON_2_PIN)))
    {
        HAL_Delay(MOTOR_TEST_POLL_MS);
    }
}

static void motorTestButtonsInit(void)
{
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpio.Pin = BUTTON_1_PIN | BUTTON_2_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUTTON_1_PORT, &gpio);
}

static uint8_t motorTestButtonPressed(uint16_t pin)
{
    uint8_t pressed = 0U;
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(BUTTON_1_PORT, pin))
    {
        HAL_Delay(MOTOR_TEST_DEBOUNCE);
        if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(BUTTON_1_PORT, pin))
        {
            pressed = 1U;
        }
    }
    return pressed;
}

static uint8_t motorTestWaitRelease(uint16_t pin)
{
    uint8_t completed = 1U;
    while (0U != motorTestButtonPressed(pin))
    {
        if (0U != motorTestButtonPressed(BUTTON_2_PIN))
        {
            completed = 0U;
            break;
        }
        HAL_Delay(MOTOR_TEST_POLL_MS);
    }
    return completed;
}

static uint8_t motorTestWait(uint32_t duration)
{
    uint8_t completed = 1U;
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < duration)
    {
        if (0U != motorTestButtonPressed(BUTTON_2_PIN))
        {
            motorDisable();
            completed = 0U;
            break;
        }
        HAL_Delay(MOTOR_TEST_POLL_MS);
    }
    return completed;
}
