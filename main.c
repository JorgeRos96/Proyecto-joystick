/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Proyecto de manejo del joystick de la tarjeta de aplicaciones 
	*					 (MAB). Para ello se utiliza el CMSIS_OS2 para evitar los rebotes,
	*					 y se controla el numero de pulsaciones mediante una variable por
	*					 cada lado del joystick. 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "joystick.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

/* Private define ------------------------------------------------------------*/
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

/* Private variables ---------------------------------------------------------*/

extern osThreadId_t tid_rebotes;


int UP = 0;
int DOWN = 0;
int RIGHT = 0;
int LEFT = 0;
int CENTER = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();

  /* Add your application code here
     */
  MX_GPIO_Init();

#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize ();

 osThreadNew(app_main, NULL, &app_main_attr); 

  /* Start thread execution */
  osKernelStart();
#endif

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}



/**
  * @brief Función de callback de las lineas de interrupción. En este caso la pulsación del joystick.
	* @param htim: Timer que ha realizado la cuenta e invoca a la función de callback  
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if( GPIO_Pin == GPIO_PIN_2){
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);
		if(UP == 0)
		{
			UP = 1;
			osThreadFlagsSet (tid_rebotes, SIGUP);
		}	
		else {
		UP = 0;
		osThreadFlagsSet (tid_rebotes, SIGBAJADAU);
		}		
}
	if( GPIO_Pin == GPIO_PIN_3){
		HAL_NVIC_DisableIRQ(EXTI3_IRQn);
		if(DOWN == 0)
		{
			DOWN = 1;
			osThreadFlagsSet (tid_rebotes, SIGDOWN);
		}	
		else {
		DOWN = 0;
		osThreadFlagsSet (tid_rebotes, SIGBAJADAD);
		}		
}
	if( GPIO_Pin == GPIO_PIN_5){
	 HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		if(LEFT == 0)
		{
			LEFT = 1;
			osThreadFlagsSet (tid_rebotes, SIGLEFT);
		}	
		else {
		LEFT = 0;
		osThreadFlagsSet (tid_rebotes, SIGBAJADAL);
		}		
}
if( GPIO_Pin == GPIO_PIN_14){
	 HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		if(CENTER == 0)
		{
			CENTER = 1;
			osThreadFlagsSet (tid_rebotes, SIGCENTER);
		}	
		else {
		CENTER = 0;
		osThreadFlagsSet (tid_rebotes, SIGBAJADAC);
		}		
}
if( GPIO_Pin == GPIO_PIN_10){
	 HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		if(RIGHT == 0)
		{
			RIGHT = 1;
			osThreadFlagsSet (tid_rebotes, SIGRIGHT);
		}	
		else {
		RIGHT = 0;
		osThreadFlagsSet (tid_rebotes, SIGBAJADAR);
		}		
}
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
