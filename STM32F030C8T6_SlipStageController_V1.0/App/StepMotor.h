#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H
#include "stm32f0xx_hal.h"
#include "main.h"

#define STEPMOTOR_STEP 200	//步进电机360°步数
#define MICROSTEP 32       	//细分控制器细分参数
#define PERIMETER 39.94  			//同步轮转动一圈皮带运动距离 单位mm
#define MAX_SIZE 850

#define DIR_RIGHT 1
#define DIR_LEFT 0
#define SET_RIGHT_DIR() HAL_GPIO_WritePin(GPIO_DIR_GPIO_Port, GPIO_DIR_Pin, GPIO_PIN_SET)
#define SET_LEFT_DIR() HAL_GPIO_WritePin(GPIO_DIR_GPIO_Port, GPIO_DIR_Pin, GPIO_PIN_RESET)

typedef enum{NONE_M, RUNNING_M, FINISH_M}MOTOR_STATE;
typedef struct _StepMotor StepMotor;
struct _StepMotor{
	uint8_t MicroStep;
	//float Location_Cur_Offset;//本次指令当前已经运动步数
	float Location_Cmd_Offset;//指定运动距离
	uint32_t Location_Cur_Step;//本次指令当前已经运动步数
	uint32_t Location_Cmd_Step;//指定运动步数
	uint32_t Max_Step;				//滑台最大运行距离
	uint32_t Absolute_Step;	  //全局绝对步数
	
	uint8_t Dir_Cmd;		//运动方向
	uint8_t Location_Cmd_Speed;//指定运动速度
	float SingleStep_D;//单步距离 mm
	float SingleStep_A;//单步角度 mm
	uint16_t SingleMillimeter_Setp;//运动1毫米所需的步数
	MOTOR_STATE Motor_State;//运动状态
	
	void (*Run_Offset)(uint8_t dir, float Offset, uint8_t Speed);
	void (*Run_Angle)(uint8_t dir,float Angle, uint8_t Speed);
	void (*Stop)(void);
};


extern StepMotor stepMotor;

extern void StepMotor_Init(void);
extern void Scan_Limit(void);
#endif

