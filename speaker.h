#include "stm32f4xx_hal.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
int init_speaker(void);
void zumbido(int ms);
