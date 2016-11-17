//###########################################################################
//
// FILE:    MotorDrive.c
//
// TITLE:   电机驱动器驱动程序
//
// ASSUMPTIONS:
//
//
// DESCRIPTION:
//         
//
//###########################################################################
#include "stm32f1xx_hal.h"
#include "main.h"  
#include "MotorWingDriver.h"
#include "MotorHeadDriver.h" 
#include <stdio.h>
#include <string.h>
#include "MotorDriver.h"
#include "TaskTimeManager.h"
#include "main.h"

#define HEADSPEEDMAX 100
#define WINGSPEEDMAX 100
 
MOTOR_HEAD_PARM Motor_H;	//头部电机
MOTOR_WING_PARM Motor_WL;	//左翅
MOTOR_WING_PARM Motor_WR;	//右翅

void MotorInt(void);
void ScanMotorLimit(void); 

uint8_t PowerFlag = 0; 
uint8_t CheckPower(void){//高电平正常 低电平异常
	//printf("%d,%d\r\n",GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_13) , GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_14));
//	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) & HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);//22V24V供电检查
  return 1;
}

void ScanMotorLimit(void){
	ScanHeadMotorLimit();
	ScanWingMotorLimit();
}

/****************************************************
函数名:MotorInt
备注: 电机初始化
****************************************************/
void MotorInt(void)	// 初始化电机参数 
{
	
	Motor_H.MotorHeadInit = MotorHeadInit;
	Motor_WL.MotorWingInit = MotorWingInit;
	Motor_WR.MotorWingInit = MotorWingInit;
	 
	Motor_H.MotorHeadInit(&Motor_H, ID_MOTOR_H, TIM4, GPIOD, GPIO_PIN_9);
	Motor_WL.MotorWingInit(&Motor_WL, ID_MOTOR_LC, TIM4, GPIOD, GPIO_PIN_11);
	Motor_WR.MotorWingInit(&Motor_WR, ID_MOTOR_RC, TIM4, GPIOD, GPIO_PIN_10);
  
  HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_3);
}

/****************************************************
函数名:ReadCurrent
备注: 读取电机电流
****************************************************/
uint32_t cal_ADC_value[ADC_CHANNEL_NUM];
volatile uint16_t ADC_valueBuf[ADC_SAMPLING_NUM][ADC_CHANNEL_NUM];
void ReadCurrent(void)
{	
  uint16_t i=0,j=0;	
	memset(cal_ADC_value,0,ADC_CHANNEL_NUM); 
	for(j = 0; j < ADC_CHANNEL_NUM; j++){
		for(i = 0; i < ADC_SAMPLING_NUM; i++)
			cal_ADC_value[j] += ADC_valueBuf[i][j];
		cal_ADC_value[j] /= ADC_SAMPLING_NUM;
		if(cal_ADC_value[i]>0x0fff)
			cal_ADC_value[i]=0x0fff;
	} 
	Motor_H.ElectricityLast = Motor_H.Electricity;
	Motor_WL.ElectricityLast = Motor_WL.Electricity;
	Motor_WR.ElectricityLast = Motor_WR.Electricity; 
	 
	Motor_H.Current = cal_ADC_value[ADC_H_CHANNEL];
	/*Motor1.Electricity = (cal_ADC_value[ADC_L_CHANNEL]*3300)/4096/25/0.02;
	Motor2.Electricity = (cal_ADC_value[ADC_R_CHANNEL]*3300)/4096/25/0.02;*/
	Motor_H.Current = cal_ADC_value[ADC_H_CHANNEL];
	Motor_WL.Current = Motor_WR.Current = cal_ADC_value[ADC_W_CHANNEL];
	Motor_H.Electricity = (uint16_t)((cal_ADC_value[ADC_H_CHANNEL]*3300)/4096/7.49/0.2);
	Motor_WL.Electricity = Motor_WR.Electricity =  (uint16_t)((cal_ADC_value[ADC_W_CHANNEL]*3300)/4096/7.49/0.2);
	//Motor_H.Angle_Enc_Cur = cal_ADC_value[ADC_H_Angle_CHANNEL] ;//Kalman_Filter(10 * cal_ADC_value[ADC_H_Angle_CHANNEL],&Motor_H.kalman);
	if(System_Mode != Self_Inspection){ 
		Motor_H.Angle_Cur = Motor_H.Angle_Enc_Cur / Motor_H.Angle_Enc_Code;
	} 
}

/****************************************************
函数名:Board_Self_Inspection
备注: 头部翅膀自检
****************************************************/
void Board_Self_Inspection(void){//自检
		uint8_t retryCount = 0; 
BeginSelfInspection:
	while(CheckPower() == 0){//供电检查
		printf("等待动力电\r\n");
		return;
	} 
	PowerFlag = 0;
	Wing_Self_Inspection(); //翅膀自检 
	if((Motor_WL.EncoderMAX < (WING_ENC_MAX - WING_DIFFENC) || Motor_WL.EncoderMAX > (WING_ENC_MAX + WING_DIFFENC))//判断左翅膀码数
		
	|| (Motor_WR.EncoderMAX < (WING_ENC_MAX - WING_DIFFENC) || Motor_WR.EncoderMAX > (WING_ENC_MAX + WING_DIFFENC)))//判断右翅膀码数
		Wing_Self_Inspection(); //翅膀再次自检
	Head_Self_Inspection();	//头部自检 
	#ifdef PRINT_ERR
		printf("自检结束\r\n");
	#endif
	if(PowerFlag != 0){//中途被断电
		if(retryCount++ == RETRYCOUNT){ 
			MotorInt();
			while(CheckPower() == 0);//等待上电
			Head_RunLeft();					 //头部左转到限位
			Motor_H.SetHeadMotorParam(&Motor_H, 50, 120);//头部回归中位
			Motor_H.ExecuteHeadMotor(&Motor_H);
			#ifdef PRINT_ERR
				printf("goto JumpSelfInspection\r\n");
			#endif
			goto JumpSelfInspection;
		}
		goto BeginSelfInspection;
	} 
JumpSelfInspection:	{
	SELFINS_PROTOCOL_T Selfins_Protocol = {0};  
	if(Motor_WL.ElectricityLast > WING_ELE_MAX || Motor_WL.ElectricityLast < WING_ELE_MIN)//判断翅膀电流是否异常
		Selfins_Protocol.para1 |= 1 << 0;
	if(Motor_H.ElectricityLast > HEAD_ELE_MAX || Motor_H.ElectricityLast < HEAD_ELE_MIN)//判断头部电流是否异常
		Selfins_Protocol.para1 |= 1 << 1;
	if(Motor_WL.EncoderMAX < (WING_ENC_MAX - WING_DIFFENC) || Motor_WL.EncoderMAX > (WING_ENC_MAX + WING_DIFFENC))//判断左翅膀码数
		Selfins_Protocol.para1 |= 1 << 2;
	if(Motor_WR.EncoderMAX < (WING_ENC_MAX - WING_DIFFENC) || Motor_WR.EncoderMAX > (WING_ENC_MAX + WING_DIFFENC))//判断右翅膀码数
		Selfins_Protocol.para1 |= 1 << 3;
	if(Motor_H.Encoder_Max < (Motor_H.Head_Enc_Max - HEAD_DIFFENC) || Motor_H.Encoder_Max > (Motor_H.Head_Enc_Max + HEAD_DIFFENC))//判断头部码数
		Selfins_Protocol.para1 |= 1 << 4;
	Selfins_Protocol.para3 = Motor_WL.ElectricityLast >> 8;	//双翅电流
	Selfins_Protocol.para4 = Motor_WL.ElectricityLast;
	Selfins_Protocol.para5 = Motor_H.ElectricityLast >> 8;		//头部电流
	Selfins_Protocol.para6 = Motor_H.ElectricityLast;				
	Selfins_Protocol.para7 = Motor_WL.EncoderMAX;						//左翅码数
	Selfins_Protocol.para8 = Motor_WR.EncoderMAX;
	Selfins_Protocol.para9 = Motor_H.Encoder_Max >> 8;				//右翅码数
	Selfins_Protocol.para10 = Motor_H.Encoder_Max;
	#ifndef PRINT_ERR
	Selfins_Protocol_Send(&pit); //上报自检信息
  Protocol_Send(SELFINS_PROTOCOL, &Selfins_Protocol, sizeof(SELFINS_PROTOCOL_T));
	#endif 
	System_Mode = Normal;
	TaskTime_Remove(Self_Inspection_TaskID); 
}
} 


















