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
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "TaskTimeManager.h"
#include "ComBuff.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h"
#include "MotorDriver.h"
#include "MotorHeadDriver.h"
#include "MotorWingDriver.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
SYSTEM_MODE System_Mode = Self_Inspection;
int8_t Heart_Flag = -1;
int8_t Self_Inspection_TaskID = -1;
//---------心跳检查---------------
void Heart_Check(void){
	if(Heart_Flag == 0){ 
    printf("心跳终止\r\n");
	}else if(Heart_Flag == 1){
		 
		Heart_Flag = 0;
	}
}

uint8_t m_data = 0; 
uint8_t addr = 0; 
void LED_TEST(void){
  static uint8_t seq = 0;
//  uint8_t data[100] = "123456\r\n";
//  Uart1_DMA_Sender.Write(&Uart1_DMA_Sender, data, 8);
//  memset(data, 0, 8);  
  //PRINT_HEAP();
  printf("LED_TEST:%d\r\n", seq++);  
}

 
void Test(void){   
  static uint8_t f = 0;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, (GPIO_PinState)f); 
  f = ~f;
  TIM4->CCR3 = 1800;  
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
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();

  /* USER CODE BEGIN 2 */
  TaskTime_Init();
  ProtocolFrame_Init();
  Log_Init();
  ComBuff_Init(); 
  
  
  MotorInt();
  #ifndef PRINT_ERR 
		#ifndef BUILD_BIN
//		DelayS(2);
		#endif
	#else
		printf("main done\r\n");
	#endif
	printf("V%c.%c.%c",MAJORVERSION,MINORVERSION,BATEVERSION);
	ReportSnVersion_Protocol();  
  TaskTime_Add(TaskID++, TimeCycle(5, 0), LED_TEST, Count_Mode);
  TaskTime_Add(TaskID++, TimeCycle(0,30), SenderKeepTransmit, Count_Mode);
  TaskTime_Add(TaskID++, TimeCycle(0,30), PaddingProtocol, Count_Mode);
	TaskTime_Add(TaskID++, TimeCycle(0,30), FetchProtocols, Count_Mode); 
//	TaskTime_Add(TaskID++, TimeCycle(1,0), Test, Count_Mode); 
	//-----------自检-----------------------------
  Self_Inspection_TaskID = TaskTime_Add(TaskID++, TimeCycle(1,0), Board_Self_Inspection, Count_Mode); 
  //------------------------扫描电机限位情况----------------------------
	TaskTime_Add(TaskID++, TimeCycle(0,30), ScanMotorLimit, Count_Mode);
#ifndef PRINT_ERR	
  //------------------------上报协议----------------------------
	//TaskTime_Add(TaskID++, TimeCycle(0,200), UpLoadState, Count_Mode); 
  //------------------------检查心跳----------------------------
	//TaskTime_Add(TaskID++, TimeCycle(0,500), Heart_Check, Count_Mode); 
#endif
  //------------------------头部PID----------------------------
	TaskTime_Add(TaskID++, TimeCycle(0,100), HeadPidRun, Count_Mode);  
  //------------------------电流数据----------------------------
	//TaskTime_Add(TaskID++, TimeCycle(0,100), ReadCurrent, Count_Mode);  
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    TaskTime_Run(); 

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
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
