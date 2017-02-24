/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
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
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "TaskTimeManager.h"
#include "ProtocolFrame.h"
#include "ComBuff.h" 
#include "MotorControl.h"
#include "oled.h"
#include "RudderControl.h"  
#include "UltrasonicControl.h"
#include "MPU6050.h"
#include "bmp180.h"
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

void SetPWM(TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t Pwm){
  TIM_OC_InitTypeDef sConfigOC;
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = Pwm;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, Channel) != HAL_OK)
  {
    Error_Handler();
  }
	 
	HAL_TIM_PWM_Start(htim, Channel); 
}

/****************************************************
        函数名: PaddingProtocol
        功能:   从串口缓冲区中读取数据到协议解析器
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
void PaddingProtocol(void){
	#define BUFFSIZE 100
	int8_t cnt = 0;
	uint8_t data[BUFFSIZE] = {0};  
	#if PROTOCOL_RESOLVER_1 && UART1_DMA_RECEIVER
    if((cnt = Uart1_DMA_Receiver.ReadTo(&Uart1_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
      ProtocolResolver_1->Protocol_Put(ProtocolResolver_1,data,cnt);
  #endif
	#if PROTOCOL_RESOLVER_2 && UART2_DMA_RECEIVER
		if((cnt = Uart2_DMA_Receiver.Read(&Uart2_DMA_Receiver,data,BUFFSIZE))>0){
			for(uint8_t i = 0; i < cnt; i++)
				Ultrasonic_Recv(Ultrasonic, data[i]);
		}
	#endif
	#if PROTOCOL_RESOLVER_3 && UART3_DMA_RECEIVER
		if((cnt = Uart3_DMA_Receiver.ReadTo(&Uart3_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_3->Protocol_Put(ProtocolResolver_3,data,cnt);
	#endif
	#if PROTOCOL_RESOLVER_4 && UART4_DMA_RECEIVER
		while(Uart4_DMA_Receiver.ReadByte(&Uart4_DMA_Receiver,data)==0)
			MPU6050->Put_Byte(MPU6050,data[0]);
	#endif
}


void LED_TEST(void){
  static uint8_t seq = 0; 
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, (GPIO_PinState)(seq%2));
	//HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, (GPIO_PinState)seq%2);
	seq++; 
	OLED_Buff = MALLOC(100);
	memset(OLED_Buff,0, 100);
	sprintf(OLED_Buff, "LED_TEST:%d\r\n",seq);
	OLED_Clear(1,2);
	OLED_ShowString(0,0,(uint8_t*)OLED_Buff);
  printf("%s",OLED_Buff);  
	FREE(OLED_Buff);
	//RudderX->setRudderAngle(RudderX, seq%100+40);
	uint8_t chipid = 0;
	uint8_t chipid_reg = 0xD0;
	uint8_t res = 0; 
//	if(HAL_OK == (res = HAL_I2C_Mem_Read(&hi2c1, 0xee, chipid_reg, I2C_MEMADD_SIZE_8BIT, &chipid, 1, 1000))){
//			printf("chipid:%x\r\n", chipid);
//	}else{
//		printf("%d\r\n", res);
//	}
	
//	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c1, 0xee, &chipid_reg, 1, 100)){
//		printf("请求发送成功\r\n");
//		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c1, 0xee, &chipid, 1, 100)){
//			printf("chipid:%x\r\n", chipid);
//		}
//	}
//		short ut = bmp180_get_ut(); 
//		DelayMS(50);
//		short up = bmp180_get_up(); 
//		short gut = bmp180_get_temperature(ut);
//		short gup = bmp180_get_pressure(up);
//		printf("%d, %d, %d, %d \r\n", ut, up, gut, gup);


				 uint8_t BMP180_ID = BMP180_ReadOneByte(0xd0);      //读取ID地址
         printf("BMP180_ID:0x%x \r\n",BMP180_ID);
         long UT = Get_BMP180UT();           
         long UP = Get_BMP180UP();                                
         Convert_UncompensatedToTrue(UT,UP); 
				 
	
}

void Fetch_MPU6050(void){ 
	MPU6050->Get_MPU6050(MPU6050);
	//printf("X:%d,Y:%d,Z:%d\r\n",MPU6050->X,MPU6050->Y,MPU6050->Z);
	OLED_Buff = MALLOC(100);
	memset(OLED_Buff,0, 100);
	sprintf(OLED_Buff, "X:%d,Y:%d,Z:%d\r\n",(uint8_t)MPU6050->X,(uint8_t)MPU6050->Y,(uint8_t)MPU6050->Z);
	OLED_Clear(7,8);
	OLED_ShowString(0,6,(uint8_t*)OLED_Buff);
	//printf("%s",OLED_Buff);  
	FREE(OLED_Buff);
}

uint8_t Heart_Flag = 2;
void Heart_Beat(void){
	if(Heart_Flag == 0){
		Heart_Flag = 3;
		motor_L->Motor_Run(motor_L, MOTOR_BRAKE, 0);
		motor_R->Motor_Run(motor_R, MOTOR_BRAKE, 0);
		printf("心跳中断 终止运动\r\n");
	}else if(Heart_Flag == 1){
		Heart_Flag = 0;
	}else if(Heart_Flag == 2){
		
	}
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	struct bmp180_t bmp180 = {0};
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  MX_TIM8_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();

  /* USER CODE BEGIN 2 */
//	__HAL_AFIO_REMAP_SWJ_NONJTRST();
  TaskTime_Init();
  ProtocolFrame_Init();
  Log_Init();
  ComBuff_Init(); 
	
	
	Motor_Init();				//驱动电机初始化  
	OLED_Init();				//OLED液晶初始化
	Rudder_Init(); 			//舵机初始化
	MPU6050_Init();			//MPU6050初始化
//	bmp180_init(&bmp180);
Read_CalibrationData();
  TaskTime_Add(TaskID++, TimeCycle(1, 0), LED_TEST, Count_Mode);
  TaskTime_Add(TaskID++, TimeCycle(0,30), SenderKeepTransmit, Count_Mode);
  TaskTime_Add(TaskID++, TimeCycle(0,30), PaddingProtocol, Count_Mode);
	TaskTime_Add(TaskID++, TimeCycle(0,30), FetchProtocols, Count_Mode); 
	//------------上报运动状态----------------------
	TaskTime_Add(TaskID++, TimeCycle(0,400), ReportState, Real_Mode); 
	//------------电机PID控制----------------------
	TaskTime_Add(TaskID++, TimeCycle(0,75), Motor_PID, Real_Mode);
	//------------舵机----------------------
	TaskTime_Add(TaskID++, TimeCycle(0, 10), Rudder_Run, Real_Mode);	
	//------------超声波----------------------
	TaskTime_Add(TaskID++, TimeCycle(0, 50), Ultrasonic_Run, Real_Mode);
	//------------处理MPU6050数据----------------------
	TaskTime_Add(TaskID++, TimeCycle(0, 10), Fetch_MPU6050, Real_Mode);  
	//------------心跳检查----------------------
	TaskTime_Add(TaskID++, TimeCycle(0, 500), Heart_Beat, Real_Mode);  
	 
//	motor_L->Motor_Run(motor_L, MOTOR_UP, 20);
//	motor_R->Motor_Run(motor_R, MOTOR_UP, 20);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {//*
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
    Log.error("Error_Handler");
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
