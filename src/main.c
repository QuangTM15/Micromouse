#include "stm32f4xx_hal.h"
#include "drivers/motor.h"

int main(void)
{
    HAL_Init();

    if (0U == motorInit())
    {
        while (1)
        {
        }
    }
    HAL_Delay(2000U);
    if (0U == motorEnable())
    {
        while (1)
        {
        }
    }
    while (0U == motorIsReady())
    {
        HAL_Delay(1U);
    }
    motorSetPWM(500, 500);
    while (1)
    {
        HAL_Delay(100U);
    }
}