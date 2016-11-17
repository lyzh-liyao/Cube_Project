/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

/* USER CODE BEGIN 0 */
#include "MotorWingDriver.h"
#include "MotorHeadDriver.h"
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line0 interrupt.
*/
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel4 global interrupt.
*/
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel5 global interrupt.
*/
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
* @brief This function handles EXTI line[9:5] interrupts.
*/
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
* @brief This function handles EXTI line[15:10] interrupts.
*/
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  switch(GPIO_Pin){
    case H_ENC_A_PIN:
         if(GET_ENC_HEAD_A == GET_ENC_HEAD_B){
              Motor_H.Angle_Enc_Cur ++;
              Motor_H.MotorDir = DIR_MOTOR_HEAD_R;
          }else{
              Motor_H.Angle_Enc_Cur --;
              Motor_H.MotorDir = DIR_MOTOR_HEAD_L;
              if(Motor_H.Angle_Enc_Cur < 0)
                Motor_H.Angle_Enc_Cur = 0;
          }
            
          if(Motor_H.Angle_Enc_Cur > Motor_H.Head_Mid_Enc_L)
            Motor_H.location = LOCATION_RIGHT;
          else if(Motor_H.Angle_Enc_Cur < Motor_H.Head_Mid_Enc_R)
            Motor_H.location = LOCATION_LEFT;
          else
            Motor_H.location = LOCATION_MIDDLE;
           
          #ifdef PRINT_ERR 
            //printf("cur:%d,%f.2,A%d,%d,%d\r\n",Motor_H.Angle_Enc_Cur,Motor_H.Angle_Cur,GET_ENC_HEAD_A,GET_LIMIT_HEAD_L,GET_LIMIT_HEAD_R);
          #endif 
          if(System_Mode == Normal){
                Motor_H.Angle_Cur = Motor_H.Angle_Enc_Cur / Motor_H.Angle_Enc_Code;
          }
         break;
    case H_ENC_B_PIN:
         if(GET_ENC_HEAD_A == GET_ENC_HEAD_B){
              Motor_H.Angle_Enc_Cur --;
              Motor_H.MotorDir = DIR_MOTOR_HEAD_L;
              if(Motor_H.Angle_Enc_Cur < 0)
                Motor_H.Angle_Enc_Cur = 0;
          }else{
              Motor_H.Angle_Enc_Cur ++;
              Motor_H.MotorDir = DIR_MOTOR_HEAD_R;
          } 
          
          if(Motor_H.Angle_Enc_Cur > Motor_H.Head_Mid_Enc_L)
            Motor_H.location = LOCATION_RIGHT;
          else if(Motor_H.Angle_Enc_Cur < Motor_H.Head_Mid_Enc_R)
            Motor_H.location = LOCATION_LEFT;
          else
            Motor_H.location = LOCATION_MIDDLE;
          
           
          #ifdef PRINT_ERR 
            //printf("cur:%d,%f.2,B%d,%d,%d\r\n",Motor_H.Angle_Enc_Cur,Motor_H.Angle_Cur,GET_ENC_HEAD_B,GET_LIMIT_HEAD_L,GET_LIMIT_HEAD_R);
          #endif
       
          if(System_Mode == Normal){
              Motor_H.Angle_Cur = Motor_H.Angle_Enc_Cur / Motor_H.Angle_Enc_Code;
          }
         break;
    case W_ENC_L_PIN:
         Motor_WL.Encoder--; //目标值递减 
         if(Motor_WL.MotorDirCMD == DIR_WING_U){
           Motor_WL.EncoderSUM++; 
         }else if(Motor_WL.MotorDirCMD == DIR_WING_D){ 
           Motor_WL.EncoderSUM--;
         }
         
         if(Motor_WL.EncoderSUM < 0)Motor_WL.EncoderSUM = 0;
         if(Motor_WL.Encoder < 3 && Motor_WL.SelfState == SELF_W_DONE)
           Motor_WL.ExecSpeedPWM(&Motor_WL, 500); 
         //当前角度计算
         if(Motor_WL.Angle_Code != 0){
           Motor_WL.Angle_Cur = (uint16_t)(Motor_WL.EncoderSUM / Motor_WL.Angle_Code);
         }
         //判断目标到达
         if(Motor_WL.Encoder <= 0){
           Motor_WL.Encoder = 0;
           Motor_WL.KeepWingMotor(&Motor_WL);//位置保持
         } 
//      printf("左翅编码触发\r\n");
      break;
    case W_ENC_R_PIN:
         Motor_WR.Encoder--;//任务值递减
         //翅膀定位值
         if(Motor_WR.MotorDir == DIR_WING_U)
           Motor_WR.EncoderSUM++;
         else if(Motor_WR.MotorDir == DIR_WING_D)
           Motor_WR.EncoderSUM--;
         if(Motor_WR.EncoderSUM < 0) Motor_WR.EncoderSUM = 0;
         
         if(Motor_WR.Encoder < 3 && Motor_WR.SelfState == SELF_W_DONE)
           Motor_WR.ExecSpeedPWM(&Motor_WR, 500);
         /*Motor_WR.EncoderSUM++;
         printf("Renc:%d\r\n",Motor_WR.EncoderSUM);*/
         if(Motor_WR.EncoderSUM < 0)Motor_WR.EncoderSUM = 0;
         //当前角度计算	
         if(Motor_WR.Angle_Code != 0)
         Motor_WR.Angle_Cur = (uint16_t)(Motor_WR.EncoderSUM / Motor_WR.Angle_Code);
         //判断目标到达
         if(Motor_WR.Encoder <= 0){
           Motor_WR.Encoder = 0;
           Motor_WR.KeepWingMotor(&Motor_WR);
         }
//      printf("右翅编码触发\r\n");
      break;
  } 
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
