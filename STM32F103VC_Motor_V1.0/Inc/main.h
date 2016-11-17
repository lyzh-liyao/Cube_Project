/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal.h"
#include "stdint.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define H_DIR_Pin GPIO_PIN_9
#define H_DIR_GPIO_Port GPIOD
#define W_DIR_L_Pin GPIO_PIN_10
#define W_DIR_L_GPIO_Port GPIOD
#define W_DIR_R_Pin GPIO_PIN_11
#define W_DIR_R_GPIO_Port GPIOD
#define W_PWM_R_Pin GPIO_PIN_12
#define W_PWM_R_GPIO_Port GPIOD
#define W_PWM_L_Pin GPIO_PIN_13
#define W_PWM_L_GPIO_Port GPIOD
#define H_PWM_Pin GPIO_PIN_14
#define H_PWM_GPIO_Port GPIOD
#define W_IO_L_Pin GPIO_PIN_15
#define W_IO_L_GPIO_Port GPIOD
#define W_ENC_L_Pin GPIO_PIN_9
#define W_ENC_L_GPIO_Port GPIOC
#define W_IO_R_Pin GPIO_PIN_8
#define W_IO_R_GPIO_Port GPIOA
#define W_ENC_R_Pin GPIO_PIN_11
#define W_ENC_R_GPIO_Port GPIOA
#define H_IO_R_Pin GPIO_PIN_12
#define H_IO_R_GPIO_Port GPIOA
#define H_IO_L_Pin GPIO_PIN_15
#define H_IO_L_GPIO_Port GPIOA
#define H_ENC_A_Pin GPIO_PIN_12
#define H_ENC_A_GPIO_Port GPIOC
#define H_ENC_B_Pin GPIO_PIN_0
#define H_ENC_B_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
typedef enum{Self_Inspection, Normal}SYSTEM_MODE;
extern SYSTEM_MODE System_Mode;

#define ADC_SAMPLING_NUM 50	
#define ADC_CHANNEL_NUM 5

#define CHIPIDADDR 0x1FFFF7AC
#define MAJORVERSION '4'
#define MINORVERSION '4'
#define BATEVERSION	 '8'
#define VDATE '2','0','1','6','0','9','1','2'

extern int8_t Self_Inspection_TaskID;
/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
