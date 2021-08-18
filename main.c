/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Proyecto de manejo del joystick de la tarjeta de aplicaciones 
	*					 (MAB). Se emplea el RTOS v2 para la realización del proyecto, y se
	*					 utilizan los hilos para realizar la gestión de los rebotes del joystick.
	*					 Se activa la interrupción del flanco de bajada para que al producirse 
	*					 la pulsación se desactive la interrupción de subida y se active la de 
	*					 bajada evitando asi que se puedan filtrar espurios de los rebotes del 
	*					 pulsador.
	*					 Se configura el reloj del sistema para que trabaje a una frecuencia 
	*					 de 180 MHz utilizando como fuente de reloj el PLL con el HSI.
	*					 Se controla el número de pulsaciones mediante una variable por
	*					 cada pulsación del joystick. Se utiliza la USART para enviar un mensaje
	*					 al terminal indicando la pulsación que se ha realizado.
	*					 Se configuran los pines de tx y rx de la USART en el RTE_Device.h 
	*					 que son:
	*							-Pin de tx: PD8
	*							-Pin de rx: PD9
	
	*					 Se utiliza el speaker de la tarjeta de aplicacones para emitir un 
	*					 zumbido cada vez que se realiza una pulsación. El speaker se va a utilizar
	*					 a traves del Timer 1 para crear la señal PWM a traves del canal 1.
	*					 Por lo que el speaker de la tarjeta de aplicaciones (P26) se conecta:
	*					 		
	*					 Canal 1 del Timer 1: PE9
	*
	*					 Se conecta el joystick a los siguientes pines:
	*						
	*					 PIN UP-> 	 PF2	
	*					 PIN DOWN->	 PF3	
	*					 PIN CENTER->PF14	
	*					 PIN LEFT->	 PF3
	*					 PIN RIGHT-> PF10
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
#include "USART.h"
#include "joystick.h"
#include "speaker.h"
#include "Delay.h" 	
#include "Watchdog.h"

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


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(int fallo);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/*Inicialización del IWDG*/
	if (init_Watchdog() != 0)
			Error_Handler(4);
	
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  if (HAL_Init() != HAL_OK)
		Error_Handler(0);

  /* Inicialización y configuración del reloj a 180 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();
	
	/*Inicialización del joystick*/
	Init_GPIO();
		if (init_speaker() != HAL_OK)
		Error_Handler(3);
		
	/*Inicializaciñon del Delay*/
	Init_Delay(180,4);
	
	/* Inicialización de la USART a traves de la función init_USART de la libreria USART
	*	 y habilitación de la transmisión
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
	*/
	if (init_USART() != 0)
		Error_Handler(2);

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
  * @brief  Función de configuración del reloj del sistema en el que se utiliza
	*					como fuente de reloj del sistema el PLL con el HSI como fuente de 
	*					reloj. Se configura una frecuencia del sistema de 180 MHz.
  *         Se configuran los paramteros de la siguiente manera: 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 8
  *            PLL_N                          = 180
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{ 
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  /** Se configura el HSI como fuente de reloj del PLL y se configuran
	* 	los parametros del PLL para ajusta la frecuencia a 180 MHz con una
	* 	frecuencia del HSI de 16 MHZ (por defecto).
	* 	SYSCLK =[(16MHz(frecuencia HSI)/8(PLLM))*180 (PLLN)]/2 (PLLP) = 180 MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se activa el modo de Over Drive para poder alcanzar los 180 MHz
	* 	como frecuencia del sistema
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se selecciona el PLL como fuente de reloj del sistema y se configuran los parametros
	*		para configurar el HCLK, PCLK1 y PCLK2. La frecuencia máxima del HCLK es 180 MHZ, la 
	*		frecuencia máxima del PCLK1 es de 45 MHZ y la frecuencia máxima del PCLK2 es de 90 MHz
	*		HCLK = SYSCK/AHB = 180 MHz / 1 = 180 MHz
	*		PCLK1 = HCLK/APB1 = 180 MHz / 4 = 45 MHZ
	*		PCLK2 = HCLK/APB2 = 180 MHz / 2 = 90 MHZ
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler(1);
  }
}




/**
  * @brief  Función que se realiza cuando se ha producido algun error.
	* @param  fallo: Variable que recoge el error producido
  * @retval None
  */
static void Error_Handler(int fallo)
{	
	char buf[100];

	if(fallo == 0)
	/* Mensaje si se ha producido un error en la inicializacón de la librería HAL*/
	printf(buf,"\r Se ha producido un error al inicializar la librería HAL\n");
	else if (fallo == 1)
	/* Mensaje si se ha producido un error en la inicializacón del reloj del sistema*/
	printf(buf,"\r Se ha producido un error al inicializar el reloj del sistema\n");
		else if(fallo == 2)
	/* Mensaje si se ha producido un error en la inicializacón de la USART*/
	printf(buf,"\r Se ha producido un error al inicializar la USART\n");
	else if (fallo == 3)
		/* Mensaje si se ha producido un error en la inicializacón del Timer del speaker*/
		printf(buf,"\r Se ha producido un error al inicializar el Timer 1\n");
	else if (fallo == 4)
		/* Mensaje si se ha producido un error en la inicialización del Watchdog*/
		printf(buf,"\r Se ha producido un error al inicializar el Watchdog\n");

	while(1)
  {
  }
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
