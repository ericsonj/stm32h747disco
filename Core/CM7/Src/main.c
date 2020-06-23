/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_HwSemaphoreCoreSync/CM7/Src/main.c
  * @author  MCD Application Team
  * @brief   This is the main program for Cortex-M7 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#ifdef USE_LCD
#include "lcd_trace.h"
#endif
#include "App.h"
#include <stdlib.h>
#include "LcdLog.h"
#include "sd_diskio_dma.h"
#include "ff_gen_drv.h"
#include "SDFatFs.h"

/* Private typedef -----------------------------------------------------------*/
//typedef enum {
//  APPLICATION_IDLE = 0,
//  APPLICATION_RUNNING,
//  APPLICATION_SD_UNPLUGGED,
//  APPLICATION_STATUS_CHANGED,
//}FS_FileOperationsTypeDef;
/* Private define ------------------------------------------------------------*/
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#define semtstSTACK_SIZE configMINIMAL_STACK_SIZE
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osSemaphoreId osSemaphore;
uint8_t pData[512];
char SDPath[4];
//static uint8_t isInitialized = 0;
FS_FileOperationsTypeDef Appli_statetmp = APPLICATION_IDLE;
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
//ALIGN_32BYTES(uint8_t rtext[96]);
uint8_t workBuffer[_MAX_SS];

/* Private functions ---------------------------------------------------------*/
#if SIMPLE_TASK
/* Private function prototypes -----------------------------------------------*/
static void CORE1_SemaphoreCoreSync(void const *argument);
#endif
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void BSP_Config(void);
//static void SD_Config(void);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  int32_t timeout;

  MPU_Config();

  /* System Init, System clock, voltage scaling and L1-Cache configuration are done by CPU1 (Cortex-M7) 
     in the meantime Domain D2 is put in STOP mode(Cortex-M4 in deep-sleep)
  */
  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
    Error_Handler();
  }

  /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
  */
  HAL_Init();

  /* Configure the system clock to 400 MHz */
  SystemClock_Config();  

  /* When system initialization is finished, Cortex-M7 will release (wakeup) Cortex-M4  by means of 
     HSEM notification. Cortex-M4 release could be also ensured by any Domain D2 wakeup source (SEV,EXTI..).
  */
  
  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();

  /*Take HSEM */
  HAL_HSEM_FastTake(HSEM_ID_0);   
  /*Release HSEM in order to notify the CPU2(CM4)*/     
  HAL_HSEM_Release(HSEM_ID_0,0);

  /* wait until CPU2 wakes up from stop mode */
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
    Error_Handler();
  }

   /* Add Cortex-M7 user application code here */ 
  /* Configure LED1 */

  BSP_Config();

//  SDFatFs_Init();
//  SD_Config();
//  BSP_QSPI_Init();

#if 0
  QSPI_Info flashInfo;
  BSP_QSPI_GetInfo(&flashInfo);

  uint32_t WRITE_READ_ADDR = 0x1000;

  BSP_QSPI_Read(pData, WRITE_READ_ADDR, sizeof(pData));
  memset(pData, 0x45, sizeof(pData));
  BSP_QSPI_Erase_Block(WRITE_READ_ADDR);
  BSP_QSPI_Write(pData, WRITE_READ_ADDR, sizeof(pData));
#endif

  APP_init();

#if SIMPLE_TASK
  /* Create the Thread that toggle LED1 */
  osThreadDef(CORE1_Thread, CORE1_SemaphoreCoreSync, osPriorityNormal, 0, semtstSTACK_SIZE);
  osThreadCreate(osThread(CORE1_Thread), (void *) osSemaphore);
#endif

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  for (;;);

}

#if SIMPLE_TASK
/**
  * @brief  Semaphore Test.
  * @param  argument: Not used
  * @retval None
  */
static void CORE1_SemaphoreCoreSync(void const *argument)
{
  for(;;)
  {
    /*Take Hw Semaphore 0*/
//    HAL_HSEM_FastTake(HSEM_ID_0);
	 LCDLog_RLog(0, "Test %s", "LCDLog");
	 LCDLog_RLog(1, "Test %s", "1");
	 LCDLog_RLog(2, "Test %s", "2");
	 LCDLog_RLog(3, "Test %s", "3");
	 osDelay(500);
    /*Release Hw Semaphore 0 in order to notify the CPU2(CM4)*/
//    HAL_HSEM_Release(HSEM_ID_0,0);
  }
}
#endif


/**
  * @brief  BSP Configuration
  * @param  None
  * @retval None
  */
static void BSP_Config(void) {
#ifdef USE_LCD

	/* Initialize the LCD */
	BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);

	GUI_SetFuncDriver(&LCD_Driver);
	GUI_Clear(GUI_COLOR_DARKGRAY);

	GUI_SetTextColor(GUI_COLOR_WHITE);
	GUI_SetFont(&Font20);
	GUI_SetBackColor(GUI_COLOR_DARKGRAY);
//	GUI_DisplayChar(20, 20, 'A');



//	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

	/* Initialize LCD Log module */
//	UTIL_LCD_TRACE_Init();

	/* Show Header and Footer texts */
//	UTIL_LCD_TRACE_SetHeader((uint8_t*) "Webserver Application");
//	UTIL_LCD_TRACE_SetFooter((uint8_t*) "STM32H747I-DISCO board");

	LCDLog_RLog(0, "State: Ethernet Initialization ...");
#endif

	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	Appli_state = APPLICATION_STATUS_CHANGED;
}

/**
  * @brief  Init the SDIO/SDMMC device
  * @param  none
  * @retval None
  */
#if 0
static void SD_Initialize(void)
{
  if (isInitialized == 0)
  {
    BSP_SD_Init(0);
    BSP_SD_DetectITConfig(0);

    if(BSP_SD_IsDetected(0))
    {
      isInitialized = 1;
    }
  }
}

/**
 * @brief  Compares two buffers.
 * @param  pBuffer1, pBuffer2: buffers to be compared.
 * @param  BufferLength: buffer's length
 * @retval 1: pBuffer identical to pBuffer1
 *         0: pBuffer differs from pBuffer1
 */
static uint8_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2,
		uint32_t BufferLength) {
	while (BufferLength--) {
		if (*pBuffer1 != *pBuffer2) {
			return 1;
		}

		pBuffer1++;
		pBuffer2++;
	}
	return 0;
}

static void FS_FileOperations(void) {
	FRESULT res; /* FatFs function common result code */
	uint32_t byteswritten, bytesread; /* File write/read counts */
	uint8_t wtext[] =
			"[STM32H747_Eval/CORE_CM7]:This is STM32 working with FatFs + DMA"; /* File write buffer */

	/* Register the file system object to the FatFs module */
	if (f_mount(&SDFatFs, (TCHAR const*) SDPath, 0) == FR_OK) {
		/* Create and Open a new text file object with write access */
		if (f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE)
				== FR_OK) {
			/* Write data to the text file */
			res = f_write(&MyFile, wtext, sizeof(wtext), (void*) &byteswritten);

			if ((byteswritten > 0) && (res == FR_OK)) {
				/* Close the open text file */
				f_close(&MyFile);

				/* Open the text file object with read access */
				if (f_open(&MyFile, "STM32.TXT", FA_READ) == FR_OK) {
					/* Read data from the text file */
					res = f_read(&MyFile, rtext, sizeof(rtext),
							(void*) &bytesread);

					if ((bytesread > 0) && (res == FR_OK)) {
						/* Close the open text file */
						f_close(&MyFile);

						/* Compare read data with the expected data */
						if (Buffercmp(rtext, wtext, byteswritten) == 0) {
							/* Success of the demo: no error occurrence */
							BSP_LED_On(LED_GREEN);
							return;
						}
					}
				}
			}
		}
	}
	/* Error */
	Error_Handler();
}


void SD_Config(void) {

	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_RED);

	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {

		/*##-2- Init the SD Card #################################################*/

		SD_Initialize();

		/* Create FAT volume */

		if (BSP_SD_IsDetected(0)) {
			FRESULT res;
			res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));
			if (res != FR_OK) {
				Error_Handler();
			}
			Appli_state = APPLICATION_RUNNING;
		}

		/* Infinite loop */
		while (1) {
			/* Mass Storage Application State Machine */
			switch (Appli_state) {
			case APPLICATION_RUNNING:
				BSP_LED_Off(LED_RED);
				SD_Initialize();
				FS_FileOperations();
				Appli_state = APPLICATION_IDLE;
				return;

			case APPLICATION_IDLE:
				break;

			case APPLICATION_SD_UNPLUGGED:
				if (isInitialized == 1) {
					isInitialized = 0;
					Error_Handler();
				}
				Appli_state = APPLICATION_IDLE;
				break;

			case APPLICATION_STATUS_CHANGED:
				if (BSP_SD_IsDetected(0)) {
					Appli_state = APPLICATION_RUNNING;
				} else {
					Appli_state = APPLICATION_SD_UNPLUGGED;
					f_mount(NULL, (TCHAR const*) "", 0);
				}
				break;
			default:
				break;
			}
		}
	}
}
#endif

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  
  /* Enable D2 domain SRAM3 Clock (0x30040000 AXI)*/
  __HAL_RCC_D2SRAM3_CLK_ENABLE();
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
  /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)       
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
  
          To do this please uncomment the following code 
  */

  __HAL_RCC_CSI_ENABLE() ;
  
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;
  
  HAL_EnableCompensationCell();

}

/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as Device not cacheable
     for ETH DMA descriptors */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as Normal Non Cacheable
     for LwIP RAM heap which contains the Tx buffers */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30044000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

#if 0

#ifdef USE_LCD
  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif
#endif

//  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
//  MPU_InitStruct.BaseAddress = 0x30020000;
//  MPU_InitStruct.Size = MPU_REGION_SIZE_512B;
//  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
//  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
//  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
//  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
//  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
//  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL2;
//  MPU_InitStruct.SubRegionDisable = 0x00;
//  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
//
//  HAL_MPU_ConfigRegion(&MPU_InitStruct);


  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
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
