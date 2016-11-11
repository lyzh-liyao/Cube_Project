#ifndef __MOTOR_HEAD_DRIVER_H__
#define __MOTOR_HEAD_DRIVER_H__  
#include "ProtocolFrame.h"
#include "PID.h"
#include "MotorDriver.h"

#define HEAD_MAX_PWM 2399			//头部速度最大PWM
//#define HEAD_BASE_PWM 200			//头部速度基础PWM

#define HEAD_INIT_PWM 2000

#define HEAD_MAX_ANGLE 240			//头部最大角度
#define HEAD_MIN_ANGLE 0				//头部最小角度

#define HEAD_LIMIT_TRUE 0
#define HEAD_LIMIT_FALSE 1

#define DIR_MOTOR_HEAD_L		0		//头部左转
#define DIR_MOTOR_HEAD_R		1		//头部右转
#define DIR_MOTOR_HEAD_NONE	3		//头部无动作

#define MID_OFFSET	0	//码盘中点偏差

#define PID_41PROPORTION 5   //4.1码盘速度比例  
#define PID_45PROPORTION 10   	 //4.5码盘速度比例  

#define HEAD_45MID_ENC 128						//4.5码盘中点码数
#define HEAD_45MID_ENC_L 126					//4.5码盘左侧中点码数
#define HEAD_45MID_ENC_R 128					//4.5码盘右侧中点码数
#define HEAD_41MID_ENC 259						//4.1码盘中点码数
#define HEAD_41MID_ENC_L 259					//4.1码盘左侧中点码数
#define HEAD_41MID_ENC_R 259					//4.1码盘右侧中点码数

#define HEAD_41ENC_MAX 517				//4.1码盘设计码数
#define HEAD_45ENC_MAX 256				//4.5码盘设计码数
#define HEAD_DIFFENC	20				//允许码数误差
#define HEAD_ELE_MAX 5000				//最大允许电流
#define HEAD_ELE_MIN 50					//最小允许电流

#define H_TIME_OUT_US (10 * 1000 * 1000) //自检超时时间

#define GET_LIMIT_HEAD_L 			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)	//左头限位器
#define GET_LIMIT_HEAD_R  			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)	//右头限位器
#define GET_ENC_HEAD_A 			HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7)	//头部A项编码器
#define GET_ENC_HEAD_B 			HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6)	//头部B项编码器
typedef enum	{MOTOR_H_STOP, MOTOR_H_RUN }RUN_STATE_H;		//头部电机运动状态
typedef enum	{SELF_H_ERROR, SELF_H_RUNNING, SELF_H_DONE}SELF_STATE_H;
typedef enum	{LOCATION_UNKNOWN, LOCATION_MIDDLE, LOCATION_LEFT, LOCATION_RIGHT}LOCATION_H;
/****************************************************
	结构体名:	MOTOR_HEAD_PARM
	功能: 头部电机实体
	作者:	liyao 2015年9月14日12:24:31
****************************************************/
typedef struct _MOTOR_HEAD_PARM MOTOR_HEAD_PARM ;
struct _MOTOR_HEAD_PARM			//里面的参数根据需要，不一定都用到
{
	MOTOR_ID	motorID;				//电机ID 1-2
	Protocol_Info_T 	Exec_Protocol;//正在执行的协议
	uint8_t   BoardVersion;		//码盘版本
	int16_t	  Speed;					//当前速度
	int16_t 	SpeedCMD;				//此处存有上位机发来未执行的速度	0-100 
	uint8_t		MotorDir;				//电机当前方向
	uint8_t		MotorDirCMD;		//此处存有上位机发来的方向指令，1和2
	int16_t 	PWM_Out;				//输出的PWM
	uint16_t  Current;				//当前电流
	uint16_t  CurrentLast;		//上次电流
	uint16_t 	Electricity;		//AD采集电流信息
	uint16_t 	ElectricityLast;//最后一次AD采集电流信息
	uint8_t 	FuseSensor;			//保险丝状态
	
	TIM_TypeDef* 	TIMx;		//对应PWM定时器
	GPIO_TypeDef* GPIOx;	//对应PWM	IO组
	uint16_t GPIO_PIN;		//对应PWM	IO口
	
	PID_T PID_H_Speed;
	//PID_T PID_H_Location;
	int16_t Last_Encoder;
	int16_t Diff_Encoder;
	int16_t Diff_Angle_Enc;
	
	int8_t				Self_TaskID;
	int8_t 			  LimitFlag;
	RUN_STATE_H   RunState;				//是否运行中
	SELF_STATE_H	SelfState;
	float 				Angle_Enc_Code;	//1°角度对应电位器值的相对量
	int16_t    		Encoder_Max;	//最大角度
	int16_t    		Encoder_Min;	//最小角度
	uint8_t				AngleCMD;				//目标角度
	int16_t				Angle_Enc_CMD;	//目标角度对应的编码器值
	float					Angle_Cur;			//当前角度
	int16_t				Angle_Enc_Cur;	//当前角度对应的AD值
	int16_t  			Protect_Time;//保护计数
	int16_t 			ProtectCheckEnc;
	
	LOCATION_H location;//所在位置 <120°还是>120° 
	uint16_t 			Head_Enc_Max;//最大码数
	float					PID_Proportion;//码盘速度比例  
	uint16_t			Head_Mid_Enc_L;//左中点
	uint16_t			Head_Mid_Enc_R;//右中点
	
	
	void (*MotorHeadInit)(MOTOR_HEAD_PARM* motor,MOTOR_ID motor_id,TIM_TypeDef* TIMx,GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);	//初始化函数
	void (*ExecuteHeadMotor)(MOTOR_HEAD_PARM* motor);
	void (*StopHeadMotor)(MOTOR_HEAD_PARM* motor);
	void (*MotorProtectCheck)(MOTOR_HEAD_PARM* motor);
	void (*SetHeadMotorParam)(MOTOR_HEAD_PARM* motor, uint16_t speed,uint16_t angle);
	void (*ExecSpeedPWM)(TIM_TypeDef* TIMx, int16_t PWM);
};



extern MOTOR_HEAD_PARM Motor_H;	//头部电机


extern void MotorHeadInit(MOTOR_HEAD_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_PIN);
extern void SetHeadMotorParam(MOTOR_HEAD_PARM* motor, uint16_t speed, uint16_t angle);
extern void Head_Self_Inspection(void);
extern void HeadPidRun(void);
extern void ExecuteHeadMotor(MOTOR_HEAD_PARM* motor);
extern void StopHeadMotor(MOTOR_HEAD_PARM* motor);
extern void ScanHeadMotorLimit(void);
extern int8_t Head_RunLeft(void);
#endif
 
