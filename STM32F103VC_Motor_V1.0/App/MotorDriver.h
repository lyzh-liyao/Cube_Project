#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H 
#include "stm32f1xx_hal.h"

#define ADC_SAMPLING_NUM 50	
#define ADC_CHANNEL_NUM 5

#define ADC_H_CHANNEL 0 //ADC_H	 PB1
#define ADC_W_CHANNEL 1	//ADC_W PB0
#define ADC_H_Angle_CHANNEL 2	//ADC_Angle_H PA5
#define ADC_R_CHANNEL 3	//ADC_R PA1
#define ADC_L_CHANNEL 4	//ADC_L PA0

/*******************************************
	自检重试次数
*******************************************/
#define RETRYCOUNT 5

typedef enum {ID_MOTOR_L = 1,ID_MOTOR_R,ID_MOTOR_H,ID_MOTOR_LC,ID_MOTOR_RC}MOTOR_ID;
#define ENCODER_CON_RES 0x7fffffff

extern void MotorInt(void);
extern void ScanMotorLimit(void); 
extern void ReadCurrent(void);
extern void Board_Self_Inspection(void);
extern uint8_t CheckPower(void);
extern uint8_t PowerFlag;
#endif  
