/**
  ******************************************************************************
  * @file    Templates/Src/speaker.c
  * @author  MCD Application Team
  * @brief   Fichero de inicialización del speaker de la tarjeta de aplicaciones
	*					
	*					 El speaker se va a utilizar a traves del Timer 1 para crear la señal 
	*					 PWM a traves del canal 1. Por lo que el speaker de la tarjeta de 
	*					 aplicaciones (P26) se conecta:
	*		
	*					 Canal 1 del Timer 1: PE9
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */

#include "speaker.h"
#include "Delay.h"

TIM_HandleTypeDef htim1;

void zumbido(int ms);
int init_speaker(void);

/**
  * @brief Inicialización y configuración del Timer 1 con una frecuencia de 2.68 kHz
	* 			 Como la frecuencia del sistema es de 180 MHz y el Timer 1 tiene como
	*				 fuente de reloj el APB1 de 90 MHz, se establece un counter de 33600 
	*				 para tener la frecuencia de 2.68 kHZ.
	*				
	*					f = 90*10^6/(33600) = 2.68 kHz 
  * @param None
  * @retval Devuelve 0 si se ha realizado correctamente la inicialización del Timer o 2
	*					en caso de error.
  */
 int init_speaker(void)
{

  
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	/*Configuración del Timer 1*/
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 33600;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
		return -1;
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    return -1;

  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
  }
	
	/*Configuración del canal 1 del Timer 1 para generación de señal PWM*/
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 16800;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
		return -1;
  }
  
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
		return -1;
  }
  HAL_TIM_MspPostInit(&htim1);
	
	return 0;
}

/**
  * @brief Función para habilitación del speaker y emitir un zumbido de duración programable 
	* @param ms: Variable que indica la duración del zumbido
  * @retval None
  */
void zumbido(int ms){
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		Delay_ms(ms);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}