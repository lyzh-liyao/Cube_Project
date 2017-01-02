#ifndef __MOTORCONTROL_H__
#define __MOTORCONTROL_H__ 
#include "stm32f1xx_hal.h"
#include "ProtocolFrame.h"
#include "PID.h"

#define MAX_ENC	100	//单位之间内编码器最大变化量
#define MAX_PWM (1000-1) //最大pwm
#define ENC_MM 1
#define INIT_ENC 0x7FFF
#define RING_ENC 3000
#define PWM_BASE 110

typedef enum{ MOTOR_L, MOTOR_R }MOTOR_ID;	//左右电机ID
typedef enum{ MOTOR_STOP = 0x00 ,MOTOR_UP = 0x01, MOTOR_DOWN = 0x02, MOTOR_BRAKE = 0x03 }MOTOR_DIR;//电机运动方向
//指令类型:空指令，直线运动，指定运动，角度运动，弧线运动
typedef enum{ NONE, STRAIGHT, CUSTOM, ANGLE, ARC}CMD_TYPE;

typedef struct _Motor_T Motor_T;
struct _Motor_T{
	MOTOR_ID Id;
	TIM_HandleTypeDef* PWM_TIMx;	//PWM输出定时器
	TIM_HandleTypeDef* ENC_TIMx;	//编码器捕获定时器
	uint8_t TIM_Channel;		//PWM定时器通道
	GPIO_TypeDef* GPIOx;		//编码器GPIO组
	uint16_t INA_GPIO_Pinx;	//编码器A项IO Pin
	uint16_t INB_GPIO_Pinx;	//编码器B项IO Pin
	uint8_t Overflow_Flag;	//编码溢出标志位（定时器计数中断）
	Protocol_Info_T 	Exec_Protocol;//正在执行的协议
	CMD_TYPE Cmd_Type;			//指令类型   直线，原地转，弧线运动
	
	float Speed_Enc;				//最大允许速度分成0-100等分   *（0-100）=真实速度指令
	PID_T PID_Speed;				//速度环
	int16_t PID_Speed_Res;	//PID速度环计算结果
	PID_T PID_Location;			//位置环
	int16_t PID_location_Res;//PID位置环计算结果
	uint16_t Encoder_Cur;		//当前编码器值
	uint16_t Encoder_Last;	//上次编码器值
	int32_t Encoder_Diff;		//目标编码值与当前编码值的差
	int32_t Location_Diff;	//左右电机编码值的差
	
	int16_t Speed;					//即时速度
	int16_t SpeedCMD;				//命令速度
	//uint16_t Encoder_Begin;
	uint16_t Encoder_CMD;
	//uint16_t Encoder_CMD_Diff;
	float Enc_Angle_Code;
	int16_t SpeedBAK;				//用于暂停运动时恢复运动速度
	int16_t TIM_PWM;				//设置到TIM寄存器的值
	MOTOR_DIR Dir;					//电机当前转向（编码器动态值）
	MOTOR_DIR DirCMD;				//命令方向
	MOTOR_DIR DirBAK;				//用于暂停运动时恢复运动方向
	uint8_t IsOpposite_DIR;			//是否反向
	uint8_t IsOpposite_ENC;			//是否反向

	
	void (*Motor_Run)(Motor_T* motor, MOTOR_DIR dir, uint16_t speed);
	void (*Motor_Custom)(Motor_T* motor, MOTOR_DIR dir, uint16_t speed, int16_t Encoder);
	uint16_t (*Motor_Get_Encoder)(Motor_T* motor);
	uint16_t (*Motor_Set_Pwm)(Motor_T* motor, uint16_t Pwm);
	
};

extern Motor_T *motor_L, *motor_R;
extern void Motor_Init(void); 
extern void Motor_GPIO_Configuration(void);
extern void Motor_TIM_Configuration(void);
extern void Motor_PID(void);
extern void ReportState(void);

#endif

