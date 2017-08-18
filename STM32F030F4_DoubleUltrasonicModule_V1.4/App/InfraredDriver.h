#ifndef _INFRAREDDRIVER__H_
#define _INFRAREDDRIVER__H_ 
#include "stm32f0xx_hal.h"
#include "Queue.h"

#define INFRARED_RECV
#define INFRARED_SEND

typedef struct _Infrared_Data_t Infrared_Data_t;
struct _Infrared_Data_t{
	uint8_t ID;
	uint8_t CMD;
	uint8_t Data;
	uint8_t CheckSum;
};

#ifdef INFRARED_SEND
#define PWM_START 50
#define PWM_STOP PWM_START
#define PWM_BIT		15
#define PWM_HOP		15   //300-400us
#define INF_SET		0
#define INF_RESET 631
#define INF_START INF_RESET
#define INF_STOP  INF_SET

#define INF_CAL_TIM  htim14
#define INF_CHANNEL	 TIM_CHANNEL_1
typedef enum{InfFree_S, InfStart_S, InfHop_S, InfSData_S, InfCheck_S, InfSStop_S }Inf_SState;

struct _Infrared_s_t{
	uint8_t BuffIndex;
	Infrared_Data_t BuffData;
	uint8_t Data;
	uint8_t Index;
	int8_t PWM_Cnt;
	volatile Inf_SState InfState;

};
typedef struct _Infrared_s_t Infrared_s_t  ;
extern Infrared_s_t* Infrared_s;
extern void Infrared_s_Init(void);
extern int InfraredSendData(Infrared_s_t* Inf, Infrared_Data_t* Data);
extern void INF_S_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
#endif

#ifdef INFRARED_RECV
#define INF_DATA_Pin 				GPIO_PIN_4
#define INF_DATA_GPIO_Port 	GPIOA
#define INF_DELAY_TIM 			htim3
#define INF_START_DELAY 		1200  //us
#define INF_DATA_DELAY	 		600    //us
#define INF_DATA_TIMEOUT 		1200 //us

#define INF_TIMER_START() 			__HAL_TIM_SET_AUTORELOAD(&INF_DELAY_TIM, 0xFFFF);__HAL_TIM_SET_COUNTER(&INF_DELAY_TIM, 0);HAL_TIM_Base_Start_IT(&INF_DELAY_TIM)
#define INF_TIMER_START_CD(cnt) __HAL_TIM_SET_AUTORELOAD(&INF_DELAY_TIM, cnt);__HAL_TIM_SET_COUNTER(&INF_DELAY_TIM, 0);HAL_TIM_Base_Start_IT(&INF_DELAY_TIM)
#define INF_TIMER_STOP()				HAL_TIM_Base_Stop_IT(&INF_DELAY_TIM)
#define INF_TIMER_GETCOUNT()		__HAL_TIM_GET_COUNTER(&INF_DELAY_TIM)

typedef enum{InfNULL_R, InfReady_R, InfStart_R, InfRcvData_R, InfWaitHop_R,InfFullData_R}Inf_RState;
struct _Infrared_r_t{
	Infrared_Data_t InfData;
	uint8_t InfIndex;
	int8_t Recv_Cnt;
	uint16_t Recv_Data;
	volatile Inf_RState InfState;
};
typedef struct _Infrared_r_t Infrared_r_t;

extern Infrared_r_t* Infrared_r;
extern void Infrared_r_Init(void);
extern int InfraredRecvData(Infrared_r_t* Inf_r, Infrared_Data_t* Data);
extern void INF_R_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
extern void INF_R_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif



#define INF_SELF_CMD 0x01
#define CHAR_CMD 0xF1
#define CHAR_ACK 0xF0


extern void InfraredSendSelf(void);

#endif
