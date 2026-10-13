#include "stm32f4xx_hal.h"
#include "app/motor_test.h"

int main(void)
{
    HAL_Init();

    if (0U == motorTestInit())
    {
        while (1)
        {
        }
    }
    while (1)
    {
        motorTestRun();
    }
}