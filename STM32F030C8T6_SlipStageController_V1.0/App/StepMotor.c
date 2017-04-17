#include "StepMotor.h"
#include "stdio.h"
#include "string.h"
#include "tim.h"
#include "stm32_hal_legacy.h"
#include "Log.h"
#include "TaskTimeManager.h"  
#include "combuff.h"

StepMotor stepMotor;

uint8_t CheckLimit(uint8_t dir){
	if(HAL_GPIO_ReadPin(L_LIMIT_GPIO_Port, L_LIMIT_Pin) == GPIO_PIN_RESET){
		if(dir == DIR_LEFT)
			return 0;
	}else if(HAL_GPIO_ReadPin(R_LIMIT_GPIO_Port, R_LIMIT_Pin) == GPIO_PIN_RESET){
		if(dir == DIR_RIGHT)
			return 0;
	}
	return 1;
}

static void _Run_Offset(uint8_t dir, float Offset, uint8_t Speed){
	if(stepMotor.Motor_State == RUNNING_M){
		Log.error("电机正在运动，指令放弃\r\n");
		return;
	}
	if(CheckLimit(dir) == 0){
		printf("限位禁止运行\r\n");
		return;
	}
	
	stepMotor.Dir_Cmd = dir;
	if(dir == DIR_RIGHT){
		SET_RIGHT_DIR();
	}else if(dir == DIR_LEFT){
		SET_LEFT_DIR();
	}
	uint16_t s = 48000000 / htim3.Init.Prescaler / (Speed * stepMotor.SingleMillimeter_Setp);//计算tim的autoreload值
	__HAL_TIM_SetAutoreload(&htim3, s);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, s / 2);
	
	stepMotor.Location_Cmd_Speed = Speed;
	stepMotor.Location_Cur_Step = 0;
	stepMotor.Location_Cmd_Step = (uint32_t)(Offset / stepMotor.SingleStep_D);//计算目标偏移量所需步数
	stepMotor.Motor_State = RUNNING_M;
	if(stepMotor.Location_Cmd_Step > stepMotor.Max_Step)//最大步数限制
		stepMotor.Location_Cmd_Step = stepMotor.Max_Step;
	//printf("s;%d,offset:%f,SingleStep_D:%f,stepMotor.Location_Cmd_Step:%d\r\n", s,Offset,stepMotor.SingleStep_D,stepMotor.Location_Cmd_Step );
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
}

static void _Run_Angle(uint8_t dir, float Angle, uint8_t Speed){
	if(stepMotor.Motor_State == RUNNING_M){
		Log.error("电机正在运动，指令放弃\r\n");
		return;
	}
	if(CheckLimit(dir) == 0){
		printf("限位禁止运行\r\n");
		return;
	}
	
	
	stepMotor.Dir_Cmd = dir;
	if(dir == DIR_RIGHT){
		SET_RIGHT_DIR();
	}else if(dir == DIR_LEFT){
		SET_LEFT_DIR();
	}
	
	uint16_t s = 48000000 / htim3.Init.Prescaler / (Speed * stepMotor.SingleMillimeter_Setp);//计算tim的autoreload值
	__HAL_TIM_SetAutoreload(&htim3, s);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, s / 2);
	
	stepMotor.Location_Cmd_Speed = Speed;
	stepMotor.Location_Cur_Step = 0;
	stepMotor.Location_Cmd_Step = Angle / stepMotor.SingleStep_A;//计算目标角度所需步数
	stepMotor.Motor_State = RUNNING_M;
	if(stepMotor.Location_Cmd_Step > stepMotor.Max_Step)//最大步数限制
		stepMotor.Location_Cmd_Step = stepMotor.Max_Step;
	printf("s;%d,stepMotor.Location_Cmd_Step:%d\r\n,", s,stepMotor.Location_Cmd_Step );
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
}

void _Stop(void){
	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_1);
	stepMotor.Location_Cmd_Speed = 0;
	stepMotor.Location_Cur_Step = 0;
	stepMotor.Location_Cmd_Step = 0;
	stepMotor.Motor_State = FINISH_M;
	Uart1_DMA_Sender.Write(&Uart1_DMA_Sender, "-----OK\r\n", 9); 
	printf("-----OK\r\n绝对定位：%d\r\n", stepMotor.Absolute_Step/stepMotor.SingleMillimeter_Setp);
}

void StepMotor_Init(void){
	memset(&stepMotor, 0, sizeof(StepMotor));
	stepMotor.SingleStep_D = PERIMETER / (STEPMOTOR_STEP * MICROSTEP * 1.0);
	stepMotor.SingleStep_A = 360 / (STEPMOTOR_STEP * MICROSTEP * 1.0);
	stepMotor.SingleMillimeter_Setp = 1 / stepMotor.SingleStep_D;
	stepMotor.Max_Step = MAX_SIZE * stepMotor.SingleMillimeter_Setp;
	stepMotor.Run_Angle = _Run_Angle;
	stepMotor.Run_Offset = _Run_Offset;
	stepMotor.Stop = _Stop;
	stepMotor.Absolute_Step = 0;
	
	printf("stepMotor.SingleStep_D;%f\r\n,", stepMotor.SingleStep_D);
	printf("stepMotor.SingleStep_A;%f\r\n,", stepMotor.SingleStep_A);
	printf("stepMotor.SingleMillimeter_Setp;%d\r\n,", stepMotor.SingleMillimeter_Setp); 
	stepMotor.Run_Offset(DIR_LEFT, 10000.0f,100);
}

//---------------------------------------------------------------------
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if(stepMotor.Dir_Cmd == DIR_RIGHT)
		stepMotor.Absolute_Step++;
	else if(stepMotor.Dir_Cmd == DIR_LEFT)
		stepMotor.Absolute_Step--;
	if(stepMotor.Location_Cur_Step++ == stepMotor.Location_Cmd_Step){ 
		stepMotor.Stop();
		//printf("运动完成\r\n");
	}
}

//-------------------------------------------------------------------------
uint32_t Jiffies_Last = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	return;
	if(Jiffies_Last == 0){
		Jiffies_Last = jiffies;
	}else{
		uint32_t Jiffies_Diff;
		if(jiffies < Jiffies_Last)
			Jiffies_Diff = 0xffffffff - Jiffies_Last + jiffies;
		else
			Jiffies_Diff = jiffies - Jiffies_Last;
		
		Jiffies_Last = jiffies;
		if(Jiffies_Diff < 500)
			return;
	}
	if(GPIO_Pin == L_LIMIT_Pin){
		stepMotor.Absolute_Step = 0;
		if(stepMotor.Dir_Cmd == DIR_LEFT)
			stepMotor.Stop();
		printf("左\r\n");
	}else if(GPIO_Pin == M_LIMIT_Pin){
		printf("中\r\n");
	}else if(GPIO_Pin == R_LIMIT_Pin){
		if(stepMotor.Dir_Cmd == DIR_RIGHT)
			stepMotor.Stop();
		printf("右\r\n");
	}
}

void Scan_Limit(void){
	uint16_t GPIO_Pin = 0;
	if(HAL_GPIO_ReadPin(L_LIMIT_GPIO_Port, L_LIMIT_Pin) == GPIO_PIN_RESET)
		GPIO_Pin = L_LIMIT_Pin;
	else if(HAL_GPIO_ReadPin(R_LIMIT_GPIO_Port, R_LIMIT_Pin) == GPIO_PIN_RESET)
		GPIO_Pin = R_LIMIT_Pin;
	else if(HAL_GPIO_ReadPin(M_LIMIT_GPIO_Port, M_LIMIT_Pin) == GPIO_PIN_RESET)
		GPIO_Pin = M_LIMIT_Pin;
	if(GPIO_Pin == 0)
		return;
	
	if(GPIO_Pin == L_LIMIT_Pin){
		stepMotor.Absolute_Step = 0;
		if(stepMotor.Dir_Cmd == DIR_LEFT)
			stepMotor.Stop();
		printf("左\r\n");
	}else if(GPIO_Pin == M_LIMIT_Pin){
		printf("中\r\n");
	}else if(GPIO_Pin == R_LIMIT_Pin){
		if(stepMotor.Dir_Cmd == DIR_RIGHT)
			stepMotor.Stop();
		printf("右\r\n");
	}
}

























