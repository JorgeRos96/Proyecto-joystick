#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"  

#define SIGLEFT    0x001
#define SIGDOWN    0x002
#define SIGRIGHT   0x004
#define SIGUP 		 0x008
#define SIGCENTER  0x010
#define SIGBAJADAL 0X020
#define SIGBAJADAR 0X040
#define SIGBAJADAU 0X080
#define SIGBAJADAD 0X100
#define SIGBAJADAC 0X200

int contUp = 0;
int contDown = 0;
int contRight = 0;
int contLeft = 0;
int contCenter = 0;

__NO_RETURN static void rebotes (void *arg); 
osThreadId_t tid_rebotes;    

#define APP_MAIN_STK_SZ (1024U)
static uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};



/**
  * @brief Hilo main donde se crea el hilo responsable de la gestión de los rebotes
	* @param arg
  * @retval None
  */
__NO_RETURN void app_main (void *arg) {
	
	 tid_rebotes = osThreadNew (rebotes, NULL, NULL);
	
	 osThreadExit();
}
/**
  * @brief Hilo de gestión de los rebotes donde se realiza las acciones corespondientes en cada pulsación.
	*				 En este caso se realiza el aumento del contador de pulsación.
	* @param arg
  * @retval None
  */
static __NO_RETURN void rebotes (void *arg) {
	uint32_t flag;
		  GPIO_InitTypeDef GPIO_InitStruct = {0};

  while (1) {
		
		flag = osThreadFlagsWait (0x3FF, osFlagsWaitAny, osWaitForever);

		
		//Subida
		if(flag == SIGLEFT){
	osDelay(10);
	GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	osThreadFlagsClear(SIGLEFT);
		}
		if(flag == SIGRIGHT){
	osDelay(10);
	GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);	
  osThreadFlagsClear(SIGRIGHT);			
		}
		if(flag == SIGDOWN){
	osDelay(10);	
	GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);	
	osThreadFlagsClear(SIGDOWN);
		}
		if(flag == SIGUP){
	osDelay(10);
	GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	osThreadFlagsClear(SIGUP);
		}
		if(flag == SIGCENTER){
	osDelay(10);
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	osThreadFlagsClear(SIGCENTER);
		}
		
		//BAJADAS
		if(flag == SIGBAJADAL){
		
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		osThreadFlagsClear(SIGBAJADAL);
			contLeft++;
			}
		if(flag == SIGBAJADAR){
			
	GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	osThreadFlagsClear(SIGBAJADAR);
			contRight++;
		}
		if(flag == SIGBAJADAU){
			
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
			osThreadFlagsClear(SIGBAJADAU);
			contUp++;
				}
		if(flag == SIGBAJADAD){
			
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
			osThreadFlagsClear(SIGBAJADAD);
			contDown++;
			}
		if(flag == SIGBAJADAC){
			
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);		
	osThreadFlagsClear(SIGBAJADAC);
		contCenter++;
			}
  }
}