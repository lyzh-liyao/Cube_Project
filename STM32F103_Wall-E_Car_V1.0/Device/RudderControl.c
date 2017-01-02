#include "RudderControl.h"
#include "OLED.h"
#include "tim.h"
#include "string.h"
#include "Log.h"
#include "MPU6050.h"
RUDDER_T  _RudderX = {0};
RUDDER_T  _RudderY = {0};
RUDDER_T* RudderX = &_RudderX;
RUDDER_T* RudderY = &_RudderY;
/****************************************************
	函数名:	setRudderAngle
	功能:	设置舵机角度
	返回值:	
	作者:	liyao 2015年11月20日17:14:40
****************************************************/
void _setRudderAngle(RUDDER_T* rudder,float angle){
	uint16_t PWM = angle * rudder->Angle_Code + RUDDER_MIN_WIDTH;
	rudder->Angle_Cur = angle;
	//printf("Angle_Code:%f,angle:%d,PWM:%d,counter:%d\r\n",rudder->Angle_Code,angle,PWM,TIM_GetCounter(rudder->TIMx));
	
	 __HAL_TIM_SET_COMPARE(rudder->TIMx, rudder->TIM_Channel, PWM);
	//TIM_SelectOutputTrigger(rudder->TIMx, TIM_TRGOSource_Reset);
	//TIM_SetCounter(rudder->TIMx, rudder->TIMx->ARR);
}



/****************************************************
	函数名:	Rudder_Init
	功能:	初始化舵机配置参数
	返回值:	
	作者:	liyao 2015年11月20日17:14:37
****************************************************/
void Rudder_Init(void){
	_RudderX.TIMx = &htim8;
	_RudderX.TIM_Channel = TIM_CHANNEL_1;
	_RudderX.TIM_Period_Pulse = RUDDER_MAX_WIDTH;//0°：0.5ms----180°：2.5ms
	_RudderX.Angle_Code = (RUDDER_MAX_WIDTH - RUDDER_MIN_WIDTH)*1.0/RUDDER_MAX_ANGLE;//2500-500/180°
	_RudderX.setRudderAngle = _setRudderAngle;
	
	_RudderY.TIMx = &htim8;
	_RudderY.TIM_Channel = TIM_CHANNEL_2;
	_RudderY.TIM_Period_Pulse = RUDDER_MAX_WIDTH;//0°：0.5ms----180°：2.5ms
	_RudderY.Angle_Code = (RUDDER_MAX_WIDTH - RUDDER_MIN_WIDTH)*1.0/RUDDER_MAX_ANGLE;
	_RudderY.setRudderAngle = _setRudderAngle;
	
	TIM_OC_InitTypeDef sConfigOC;
	sConfigOC.OCMode = TIM_OCMODE_PWM1; 
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	
  sConfigOC.Pulse = _RudderX.TIM_Period_Pulse;
  if (HAL_TIM_PWM_ConfigChannel(_RudderX.TIMx, &sConfigOC, _RudderX.TIM_Channel) != HAL_OK)
  {
    Error_Handler();
  }

	sConfigOC.Pulse = _RudderY.TIM_Period_Pulse;
  if (HAL_TIM_PWM_ConfigChannel(_RudderY.TIMx, &sConfigOC, _RudderY.TIM_Channel) != HAL_OK)
  {
    Error_Handler();
  }	
	HAL_TIM_PWM_Start(_RudderX.TIMx, _RudderX.TIM_Channel);
	HAL_TIM_PWM_Start(_RudderY.TIMx, _RudderY.TIM_Channel);
	
	 __HAL_TIM_SET_COMPARE(_RudderX.TIMx, _RudderX.TIM_Channel, 800);
	
	_RudderX.Angle_CMD = 90;
	_RudderY.Angle_CMD = 90;
	_RudderX.setRudderAngle(&_RudderX, 90);
	_RudderY.setRudderAngle(&_RudderY, 90);
}

//------------------------------------------------------

uint8_t Rudder_Angle = 0,Rudder_Turn = 0,Rudder_Pause = 0;
#ifdef __MPU6050_H_	
//void Rudder_Run(void){ 
	
	
	/*if(Ultrasonic->Check_Res == BARRIER)
		return;
	if(MPU6050->IsReady)
		{
			res = MPU6050->Get_MPU6050(MPU6050);
			if(res.Z >= 0 && res.Z <= 180){
				RudderX->setRudderAngle(RudderX,res.Z);
			}
			if(res.X >= 0 && res.X <= 180){
				RudderY->setRudderAngle(RudderY,res.X);
			}
			printf("%d\t%d\r\n",res.X,res.Z);
			
			
		}*/
	
	/*RudderX->setRudderAngle(RudderX,Rudder_Angle);
	RudderY->setRudderAngle(RudderY,Rudder_Angle);
	if(Rudder_Angle == 180)
		Rudder_Turn = 1;
	else if(Rudder_Angle == 0)
		Rudder_Turn = 0;
	if(Rudder_Turn)
		Rudder_Angle--;
	else 
		Rudder_Angle++;*/
	//RudderX->setRudderAngle(RudderX,180);
//}
#endif
void Rudder_Run(void){ 
//	if(Ultrasonic->Check_Res == BARRIER)
//		return;
//	RudderX->setRudderAngle(RudderX,Rudder_Angle);
//	RudderY->setRudderAngle(RudderY,Rudder_Angle);
	float x,y;
	x = RudderX->Angle_CMD - MPU6050->Z;
	y = RudderY->Angle_CMD - MPU6050->X;
	if(x > RUDDER_MAX_ANGLE)
		x = RUDDER_MAX_ANGLE;
	else if(x < 0)
		x = 0;
	if(y > (RUDDER_MAX_ANGLE - 20))
		y = (RUDDER_MAX_ANGLE - 20);
	else if(y < 0)
		y = 0;
	RudderX->setRudderAngle(RudderX, x);
	RudderY->setRudderAngle(RudderY, y);
	
	
//	if(Rudder_Angle == 180)
//		Rudder_Turn = 1;
//	else if(Rudder_Angle == 0)
//		Rudder_Turn = 0;
//	if(Rudder_Turn)
//		Rudder_Angle--;
//	else 
//		Rudder_Angle++;
	
	
	//printf("Rudder:%d%c\r\n",Rudder_Angle,(char)127);
	OLED_Buff = MALLOC(100);
	memset(OLED_Buff,0, 100);
	sprintf(OLED_Buff, "Rudder:%d,%d,%c\r\n",(uint8_t)x,(uint8_t)y,(char)127);
	OLED_Clear(3,4);
	OLED_ShowString(0,2,(uint8_t*)OLED_Buff);
	FREE(OLED_Buff);
	//RudderX->setRudderAngle(RudderX,180);
}

