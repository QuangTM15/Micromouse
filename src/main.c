
#include "stm32f4xx_hal.h"
#include "app/movement_test.h"

int main(void)
{
    HAL_Init();
    if (0U == movementTestInit())
    {
        while (1)
        {
        }
    }
    movementTestRun();
    while (1)
    {
        HAL_Delay(100U);
    }
}
