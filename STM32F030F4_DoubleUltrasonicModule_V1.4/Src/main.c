/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#include "stm32f0xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "TaskTimeManager.h"
#include "ComBuff.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h"
#include "Protocol.h"
#include "UltrasonicDriver.h"
#include "ChipTools.h"
#include "InfraredDriver.h"


#define CHIPIDADDR 0x1FFFF7AC
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

int fputc(int ch,FILE *f)
{
//	#ifndef PRINT_ERR
//		Queue_Put(Uart_Tx_Queue, &ch); 
//	#else
	UART_TX_GPIO_Enable();
//	uint8_t c = (uint8_t)ch;
//	RobSend(&c, 1);
	while(HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
	UART_TX_GPIO_Disable();
	//Uart1_DMA_Sender.Write(&Uart1_DMA_Sender,(uint8_t*)&ch, 1);
	//while(HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
//	#endif
	return(ch);	   
}


Ultrasonic_T Ultrasonic = {0};
void SN_Report(void){
	for(int i = 0; i < 12; i++)
		Ultrasonic.Sn_Code += *(uint8_t*)(CHIPIDADDR+i);
	Protocol_Send(SN_REPORT, &Ultrasonic.Sn_Code, 2);
}
uint8_t UartData1[20] = {0};

	
void LED_TEST(void){ 
  static uint8_t seq = 0;
	//printf("LED_TEST:%d\r\n", seq++);  
	#ifdef INFRARED_RECV
	Infrared_Data_t rdata;
	if(InfraredRecvData(Infrared_r, &rdata) == 1){
		if((uint8_t)(rdata.ID+rdata.CMD+rdata.Data) == rdata.CheckSum)
			printf("%d %d %d %d\n", rdata.ID, rdata.CMD, rdata.Data, rdata.CheckSum);
	}
//	if(Queue_Pop(Infrared_r->InfQueue, &data) == 0){
//		for(int i=0; i < 16; i++){
////			printf("%d\r\n", Infrared->tdata[Infrared->tindex++]);
//		}
//		printf("Data:%d\r\n", data[0]);
//		
//	}
	
	#endif
#ifdef 	INFRARED_SEND
//	Infrared_Data_t sdata;
//	sdata.ID = seq++;
//	sdata.CMD = 2;
//	sdata.Data = 3;
//	sdata.CheckSum = (uint8_t)(sdata.ID+sdata.CMD+sdata.Data);
//	InfraredSendData(Infrared_s, &sdata);
	#endif
}





void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//ProtocolResolver_1->Protocol_Put(ProtocolResolver_1,UartData,20);
	HAL_UART_Receive_IT(&huart1, UartData1, 20);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
#ifdef INFRARED_RECV
	INF_R_GPIO_EXTI_Callback(GPIO_Pin);
#endif
}
	
int flag = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	ULT_TIM_PeriodElapsedCallback(htim);
#ifdef INFRARED_RECV
	INF_R_TIM_PeriodElapsedCallback(htim);
#endif	
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	#ifdef INFRARED_SEND
		INF_S_TIM_PWM_PulseFinishedCallback(htim);
	#endif
}
extern void UltrasonicRun(void);
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
//	EXTI_IT_DISABLE();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_TIM1_Init();
  MX_TIM14_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
	LL_SYSTICK_EnableIT();
	TaskTime_Init();
  ProtocolFrame_Init();
  Log_Init();
  ComBuff_Init();  
	ChipTools_Init();
	
	
	DelayMS(100);
 
	Ultrasonic_Init(&Ultrasonic);
	Infrared_s_Init();
	Infrared_r_Init();
	UART_TX_GPIO_Disable();
	SN_Report(); //上报ID
		
	/*-----------测试调试任务-----------------*/
  TaskTime_Add(TaskID++, TimeCycle(0,10), LED_TEST, Real_Mode); 
	/*-----------持续传输任务-----------------*/
//  TaskTime_Add(TaskID++, TimeCycle(0,30), SenderKeepTransmit, Count_Mode);
	/*-----------协议解析任务-----------------*/
//  TaskTime_Add(TaskID++, TimeCycle(0,30), PaddingProtocol, Real_Mode);
	/*-----------协议执行任务-----------------*/
//	TaskTime_Add(TaskID++, TimeCycle(0,30), FetchProtocols, Real_Mode);  
	/*-----------超声波测距任务-----------------*/
//	TaskTime_Add(TaskID++, TimeCycle(0,500), UltrasonicRun, Real_Mode);  
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    TaskTime_Run();  
//		LED_TEST();
//		SenderKeepTransmit();
//		PaddingProtocol();
//		FetchProtocols();
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);

  LL_RCC_HSI14_Enable();

   /* Wait till HSI14 is ready */
  while(LL_RCC_HSI14_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI14_SetCalibTrimming(16);

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);

  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(48000000);

  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

  LL_SetSystemCoreClock(48000000);

  LL_RCC_HSI14_EnableADCControl();

  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
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
