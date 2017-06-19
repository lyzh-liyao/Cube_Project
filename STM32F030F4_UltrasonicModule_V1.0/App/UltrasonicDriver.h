#ifndef _ULTRASONICDRIVER_H_
#define _ULTRASONICDRIVER_H_
#include "Log.h"
#include "tim.h"
#include "adc.h"
#include "dma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


typedef enum { ULT_NONE, ULT_SEND, ULT_SEND_FINISH, ULT_RECEIVE, ULT_RECEIVE_FINISH, ULT_DMA_FINISH }ULT_STATE;
typedef enum { ULT_OK, ULT_BUSY, ULT_TIMEOUT,ULT_ERROR }ULT_RESULT;
typedef struct _Ultrasonic_T Ultrasonic_T;
struct _Ultrasonic_T{
	uint8_t Wave;
	volatile ULT_STATE Ult_State;
	uint8_t* Wave_Adc_Buff;
};

extern ULT_RESULT Ultrasonic_Init(Ultrasonic_T* Ultrasonic);  
extern ULT_RESULT Ultrasonic_Send(Ultrasonic_T*);
extern int8_t getTempSensor(void);

static Ultrasonic_T* _Ultrasonic;
#define WAVE_ADC_BUFF 400
#define ULT_WAVE_COUNT 16
#define ULT_ADC	hadc
#define ULT_WAVE_TIM htim16
#define ULT_CAL_TIM  htim17
#define ULT_CYCLE_TIM htim1

#define ULT_CAL_TIM_START() __HAL_TIM_SET_COUNTER(&ULT_CAL_TIM, 0);HAL_TIM_Base_Start(&ULT_CAL_TIM) //开始计时
#define ULT_CAL_TIM_STOP()  HAL_TIM_Base_Stop(&ULT_CAL_TIM) 	  //停止计时
#define ULT_CAL_TIM_CNT()   __HAL_TIM_GET_COUNTER(&ULT_CAL_TIM)	//获取计时数


//清空DMA缓冲区
//打开ADC定时器触发器
//开启ADC采集
#define ULT_DMA_START(hadc)			memset(_Ultrasonic->Wave_Adc_Buff, 0, WAVE_ADC_BUFF);\
																HAL_TIM_Base_Start(&ULT_CYCLE_TIM);\
																HAL_ADC_Start_DMA(hadc, (uint32_t*)_Ultrasonic->Wave_Adc_Buff, WAVE_ADC_BUFF)	
//停止ADC采集
//停止ADC定时触发器
#define ULT_DMA_STOP(hadc)			HAL_ADC_Stop_DMA(hadc); \
																HAL_TIM_Base_Stop(&ULT_CYCLE_TIM)	
//uint8_t SendUltrasonic(uint8_t WaveCount){
//	
//}


#endif


