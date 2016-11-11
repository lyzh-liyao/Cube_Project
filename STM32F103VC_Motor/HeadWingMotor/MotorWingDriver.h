#ifndef __MOTOR_WING_DRIVER_H__
#define __MOTOR_WING_DRIVER_H__
#include "MotorDriver.h" 
#include "ProtocolFrame.h"
#include "tim.h"
#define WING_MAX_PWM 1500			//翅膀常规、最大运动速度
//#define WING_SLOWLY_PWM 120		//翅膀下降反向最大PWM
#define WING_BASE_PWM 500			//翅膀向上基础PWM

#define WING_INIT_PWM 1000			//翅膀初始化速度
#define WING_KEEP_PWM 150			//翅膀保持速度
#define WING_DOWN_PWM 800			//翅膀下降时的瞬时PWM

#define WING_MAX_ANGLE 110			//翅膀最大角度
#define WING_MIN_ANGLE 0				//翅膀最小角度

#define WING_LIMIT_TRUE 0				
#define WING_LIMIT_FALSE 1 			

#define WING_ENC_MAX 50 				//设计码数
#define WING_DIFFENC 5					//允许码数误差
#define WING_ELE_MAX 5000				//最大允许电流
#define WING_ELE_MIN 50					//最小允许电流

#define W_TIME_OUT_US (5 * 1000 * 1000) //自检超时时间 
  
typedef enum {DIR_WING_U, DIR_WING_D, DIR_WING_CUSTOM} DIR_WING;//翅膀运动方向定义
typedef enum { MOTOR_W_STOP, MOTOR_W_KEEP, MOTOR_W_RUN}RUN_STATE;		//翅膀电机运动状态
typedef enum	{SELF_W_ERROR, SELF_W_DONE}SELF_STATE_W;
//liyao 2015年9月6日13:39:57
#define GET_ENCODER_WING_L 		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) //左翅编码器
#define GET_ENCODER_WING_R  	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)	//右翅编码器
#define GET_LIMIT_WING_L 		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)	//左翅限位器
#define GET_LIMIT_WING_R  	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)	//右翅限位器
/****************************************************
	结构体名:	MOTOR_WING_PARM
	功能: 翅膀电机实体
	作者:	liyao 2015年9月14日12:24:31
****************************************************/
typedef struct _MOTOR_WING_PARM MOTOR_WING_PARM ;
struct _MOTOR_WING_PARM						//里面的参数根据需要，不一定都用到
{
	MOTOR_ID	MotorID;			//电机ID 1-2
	Protocol_Info_T 	Exec_Protocol;//正在执行的协议
	
	int16_t	  Speed;				//当前速度
	int16_t 	SpeedCMD;		//此处存有上位机发来未执行的速度	0-100		复位为0xff，在启动电机前先要稳定DIR一段时间
	DIR_WING		MotorDir;			//正在执行的方向
	DIR_WING		MotorDirCMD;		//命令方向
	uint32_t	Encoder;			//初始数据0x7fffffff，再此基础上做加减，在每次上传后恢复初始值
	uint32_t	EncoderLast;			//初始数据0x7fffffff，再此基础上做加减，在每次上传后恢复初始值
	int32_t  EncoderSUM;	//限位后的累计值;
	
	int16_t EncoderMAX;	//编码器最大值
	int16_t EncoderCMD;	//目标编码器值
	int16_t Diff_Encoder;
	
	uint16_t  Current;				//当前电流
	uint16_t Electricity;//AD采集电流信息
	uint16_t 	ElectricityLast;		//最后一次AD采集电流信息
	uint8_t  Down_Flag;//向下运动标志位
	
	TIM_TypeDef* TIMx;		//对应PWM定时器
	GPIO_TypeDef* GPIOx;	//对应PWM	IO组
	uint16_t GPIO_Pin;		//对应PWM	IO口
	
	SELF_STATE_W	SelfState;
	uint8_t LimitState;	//限位情况
	RUN_STATE RunState;	//是否运行中
	float Angle_Code;	//1°角度对应编码器值的相对量
	uint8_t Speed_Code;//1速度对应的PWM
	uint16_t Angle_Cur;	//当前角度
	uint16_t Angle_CMD;	//当前角度
	uint16_t  Protect_Time;//保护计数
	
	uint16_t  PWM_Base;//最小能动pwm
	float Gain_PWM;//PWM增益
	
	void (*MotorWingInit)(MOTOR_WING_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
	void (*ExecuteWingMotor)(MOTOR_WING_PARM* motor);
	void (*StopWingMotor)(MOTOR_WING_PARM* motor);
	void (*KeepWingMotor)(MOTOR_WING_PARM* motor);
	void (*LimitTrigger)(MOTOR_WING_PARM* motor);
	void (*MotorProtectCheck)(MOTOR_WING_PARM* motor);
	void (*SetWingMotorParam)(MOTOR_WING_PARM* motor, uint16_t speed,int16_t angle);
	void (*ExecSpeedPWM)(TIM_TypeDef* TIMx, int16_t PWM);
	
};  



extern MOTOR_WING_PARM Motor_WL;	//左翅
extern MOTOR_WING_PARM Motor_WR;	//右翅


extern void MotorWingInit(MOTOR_WING_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
extern void SetWingMotorParam(MOTOR_WING_PARM* motor, uint16_t speed,int16_t angle);
extern void Wing_Self_Inspection(void);
//static void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM);
extern void	ExecuteWingMotor(MOTOR_WING_PARM* motor);
extern void StopWingMotor(MOTOR_WING_PARM* motor);
extern void ScanWingMotorLimit(void);
extern void KeepWingMotor(MOTOR_WING_PARM* motor);
extern void BacklashWingMotor(MOTOR_WING_PARM* motor);
#endif

