/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim3;

osThreadId vLEDHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

#define disfps		16
#define disbufNum	4

uint8_t	disBuf[disbufNum][12][7];

// 12 x 12 x 8bit, rgba2222
//uint8_t disIMG[12][12]={
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//{0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x30, 0x30, 0x30, 0x0c, 0x0c, 0x0c},
//};
uint8_t disIMG[12][12];
const uint16_t disMap[12] = { 0x0010, 0x0008, 0x0004, 0x0002, 0x0020, 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200 };

//uint8_t disIMG[12][12]={
//{0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30},
//{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
//{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
//{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
//{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
//{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
//{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
void vLEDTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t listNum = 0;
	listNum += 7;
	if (listNum >= (disbufNum * 12 * 7))
		listNum = 0;
	HAL_SPI_Transmit_DMA(&hspi1,(uint8_t *)disBuf+listNum,7);
}

void SetPiont(void)
{
	uint8_t listNum;
	uint8_t fpsNum;
	uint64_t tmp;
	uint8_t rgb;
	uint8_t i;

	for (fpsNum = 0; fpsNum < disbufNum;fpsNum++)
		for (listNum = 0; listNum < 12; listNum++)
		{
			tmp = 0;
			
			for(i=0;i<12;i++)
			{
				rgb = disIMG[11-i][listNum];
				
				tmp <<= 3;
				if(((rgb>>6) & 0x03) > fpsNum)		//red
					tmp |= 0x02;
				if(((rgb>>4) & 0x03) > fpsNum)		//green
				{
					if (listNum & 0x01)
						tmp |= 0x01;
					else
						tmp |= 0x04;
				}
				if(((rgb>>2) & 0x03) > fpsNum)		//blue
				{
					if (listNum & 0x01)
						tmp |= 0x04;
					else
						tmp |= 0x01;
				}
			}
			
			disBuf[fpsNum][listNum][0] = disMap[listNum] >> 8;
			disBuf[fpsNum][listNum][1] = disMap[listNum] & 0xff;
			disBuf[fpsNum][listNum][2] = (tmp>>32)&0xff;
			disBuf[fpsNum][listNum][3] = (tmp>>24)&0xff;
			disBuf[fpsNum][listNum][4] = (tmp>>16)&0xff;
			disBuf[fpsNum][listNum][5] = (tmp>>8 )&0xff;
			disBuf[fpsNum][listNum][6] = (tmp>>0 )&0xff;
		}
}

void TheMatrix(void)
{
	uint8_t i;
	for(i=0;i<12*4;i++)
	{
		disIMG[(i+0)%12][0] = 0x10;
		disIMG[(i+1)%12][0] = 0x10;
		disIMG[(i+2)%12][0] = 0x10;
		disIMG[(i+3)%12][0] = 0x20;
		disIMG[(i+4)%12][0] = 0x20;
		disIMG[(i+5)%12][0] = 0x30;
		disIMG[(i+6)%12][0] = 0x10;
		disIMG[(i+7)%12][0] = 0x00;
		disIMG[(i+8)%12][0] = 0x00;
		disIMG[(i+9)%12][0] = 0x00;
		disIMG[(i+10)%12][0] = 0x00;
		disIMG[(i+11)%12][0] = 0x00;
		disIMG[(i+12)%12][0] = 0x00;
		
		
		SetPiont();
		osDelay(60);
	}
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of vLED */
  osThreadDef(vLED, vLEDTask, osPriorityNormal, 0, 128);
  vLEDHandle = osThreadCreate(osThread(vLED), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi1.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi1);

}

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 15;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 71;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

}

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOD_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* vLEDTask function */
void vLEDTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
	HAL_TIM_Base_Start_IT(&htim3);
  /* Infinite loop */
  for(;;)
  {
		//osDelay(40);
		//SetPiont();
		TheMatrix();
  }
  /* USER CODE END 5 */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
