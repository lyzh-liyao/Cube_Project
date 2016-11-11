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
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) & HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);//22V24V供电检查
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
	 
	Motor_H.MotorHeadInit(&Motor_H, ID_MOTOR_H, TIM4, GPIOD, GPIO_PIN_11);
	Motor_WL.MotorWingInit(&Motor_WL, ID_MOTOR_LC, TIM4, GPIOB, GPIO_PIN_10);
	Motor_WR.MotorWingInit(&Motor_WR, ID_MOTOR_RC, TIM4, GPIOB, GPIO_PIN_9);
}
 























