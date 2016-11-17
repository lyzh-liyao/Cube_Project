#include <stdio.h>
#include <string.h>  
#include "MotorHeadDriver.h" 
#include "TaskTimeManager.h"
#include "tim.h"
static void Motor_HProtectCheck(MOTOR_HEAD_PARM* motor);
static void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM);
/****************************************************
函数名:MotorHeadInit
功能: 头部电机实体初始化
作者:	李尧 2015年9月14日13:44:23
****************************************************/	 
void MotorHeadInit(MOTOR_HEAD_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_PIN){
	memset(motor, 0,sizeof(MOTOR_HEAD_PARM)); 
	motor->TIMx = TIMx;
	motor->GPIOx = GPIOx;
	motor->GPIO_PIN = GPIO_PIN;
	//motor->Angle_Enc_Code = HEAD_ENC_MAX * 1.0 / HEAD_MAX_ANGLE;
	//motor->Encoder_Max = HEAD_ENC_MAX; 
	motor->Encoder_Min = MID_OFFSET; 
	motor->SelfState = SELF_H_ERROR;  
	
	Motor_H.RunState = MOTOR_H_STOP;							//赋值头部初始状态
	motor->ExecSpeedPWM = ExecSpeedPWM;						//赋值PWM设置函数
	motor->ExecuteHeadMotor = ExecuteHeadMotor;		//赋值执行函数
	motor->SetHeadMotorParam = SetHeadMotorParam;	//赋值参数设置函数
	motor->StopHeadMotor = StopHeadMotor;					//赋值停止函数
	motor->MotorProtectCheck = Motor_HProtectCheck; //赋值电机保护函数
	
	PID_Init(&motor->PID_H_Speed, 40, 1, 0, 1, 2400);
}

void ErrorDefault(void){
	if(Motor_H.BoardVersion == 41){
			Motor_H.Head_Enc_Max = HEAD_41ENC_MAX; 
			Motor_H.PID_Proportion = PID_41PROPORTION;
		}else if(Motor_H.BoardVersion == 45){
			Motor_H.Head_Enc_Max = HEAD_45ENC_MAX;
			Motor_H.PID_Proportion = PID_45PROPORTION;
		} 
    Motor_H.Angle_Enc_Code = Motor_H.Head_Enc_Max * 1.0 / HEAD_MAX_ANGLE;
    Motor_H.Encoder_Max = Motor_H.Head_Enc_Max; 
}

/****************************************************
函数名:ExecSpeedPWM
功能: 设置输出PWM
作者:	李尧 2015年10月14日22:26:22
****************************************************/	 
void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM){
	if(PWM > HEAD_MAX_PWM) PWM = HEAD_MAX_PWM;
	if(PWM < 0) PWM = 0;
	//TIM_SetCompare1(TIMx, PWM);
  TIMx->CCR3 = PWM;
}

/****************************************************
函数名:ExecuteHeadMotor
功能: 执行电机参数
作者:	李尧 2015年10月14日22:26:22
****************************************************/	 
void ExecuteHeadMotor(MOTOR_HEAD_PARM* motor){
//	if(motor->Diff_Angle_Enc < (motor->Angle_Enc_Code * 5)){ 
//		motor->StopHeadMotor(motor);			//电机停止
//		return;
//	}
	motor->RunState = MOTOR_H_RUN;
	HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_PIN, (GPIO_PinState)motor->MotorDirCMD);
	motor->Speed = motor->SpeedCMD;
	motor->PWM_Out = 0;
	motor->Last_Encoder = -1;
	motor->PID_H_Speed.PID_Reset(&motor->PID_H_Speed);
}

/****************************************************
函数名:SetHeadMotorParam
功能: 设置电机参数
作者:	李尧 2015年10月14日22:26:22
****************************************************/
 void SetHeadMotorParam(MOTOR_HEAD_PARM* motor, uint16_t speed,uint16_t angle){
	if(motor->SelfState == SELF_H_DONE){//自检模式不进行角度校验，uint类型不用校验小于0
		angle = angle > HEAD_MAX_ANGLE?HEAD_MAX_ANGLE:angle;
	} 
	
	int16_t Diff_Angle = (int16_t)(motor->Angle_Cur - angle);
	if(Diff_Angle < 0) Diff_Angle = -Diff_Angle;
	//速度限制
	if(Diff_Angle < 15 && speed > 40){
		speed = 40;
	}
	
	motor->SpeedCMD = (int16_t)(speed / motor->PID_Proportion);//speed / PID_PROPORTION;
	
	if(speed > 0 && speed < motor->PID_Proportion)//非0最小速度
		motor->SpeedCMD = 1;
	motor->AngleCMD = angle;
	if(angle == 120 &&  Motor_H.BoardVersion == 45){//中点不通过计算直接定值
		if(motor->location == LOCATION_RIGHT)
			motor->Angle_Enc_CMD = Motor_H.Head_Mid_Enc_R;
		else if(motor->location == LOCATION_LEFT)
			motor->Angle_Enc_CMD = Motor_H.Head_Mid_Enc_L;
	} else{ 
		motor->Angle_Enc_CMD = (int16_t)(angle * motor->Angle_Enc_Code);
	}
	motor->Diff_Angle_Enc = motor->Angle_Enc_CMD - motor->Angle_Enc_Cur;
 
	if(motor->Diff_Angle_Enc < 0)
		motor->Diff_Angle_Enc = -motor->Diff_Angle_Enc;
	if(motor->Angle_Cur > motor->AngleCMD){
		Motor_H.MotorDirCMD = DIR_MOTOR_HEAD_L;
	}else if(motor->Angle_Cur < motor->AngleCMD){
		Motor_H.MotorDirCMD = DIR_MOTOR_HEAD_R;
	}
}

/****************************************************
函数名:StopHeadMotor
功能: 电机停止
作者:	李尧 2015年10月14日22:26:22
****************************************************/
void StopHeadMotor(MOTOR_HEAD_PARM* motor){
	motor->ExecSpeedPWM(motor->TIMx, 0);
	motor->PID_H_Speed.PID_Reset(&motor->PID_H_Speed);
	motor->SpeedCMD = 0;
	motor->Last_Encoder = -1;
	if(motor->MotorDirCMD == DIR_MOTOR_HEAD_L || motor->MotorDirCMD == DIR_MOTOR_HEAD_R)
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_PIN, (GPIO_PinState)!motor->MotorDirCMD);
	motor->MotorDirCMD = DIR_MOTOR_HEAD_NONE;
	motor->RunState = MOTOR_H_STOP;
}

/****************************************************
函数名:MotorProtectCheck
功能: 电机保护检查
作者:	李尧 2015年10月14日22:26:22
****************************************************/ 
void Motor_HProtectCheck(MOTOR_HEAD_PARM* motor){
	if(motor->RunState == MOTOR_H_STOP){		//运动状态为停止
		motor->ProtectCheckEnc = motor->Protect_Time = 0;
	}else{
		if(motor->Protect_Time++ == 30){		//计数器累加到100
			motor->Protect_Time = 0;					//初始化保护计数器 
			if(motor->ProtectCheckEnc == motor->Angle_Enc_Cur){
				motor->StopHeadMotor(motor);			//电机停止
				#ifdef PRINT_ERR 
				printf("触发头部电机保护\r\n");
				#endif
			}
			motor->ProtectCheckEnc = motor->Angle_Enc_Cur;
		}
	}
}
/****************************************************
函数名:HeadPidRun
功能: 通过pid算法控制PWM
作者:	李尧 2015年10月14日22:26:22
****************************************************/
void HeadPidRun(void){
	if(Motor_H.RunState == MOTOR_H_RUN && Motor_H.SpeedCMD == 0) 
		Motor_H.StopHeadMotor(&Motor_H);
	if(Motor_H.RunState == MOTOR_H_STOP)
		return; 
	if(Motor_H.Last_Encoder == -1){
		Motor_H.Last_Encoder = Motor_H.Angle_Enc_Cur;
		return;
	}
	//单位时间内编码器数量
	Motor_H.Diff_Encoder = Motor_H.Angle_Enc_Cur - Motor_H.Last_Encoder;
	if(Motor_H.Diff_Encoder < 0)
		Motor_H.Diff_Encoder = -Motor_H.Diff_Encoder;
	Motor_H.Last_Encoder = Motor_H.Angle_Enc_Cur;
	
	//角度差值计算
	int16_t Diff_Angle_Enc =  Motor_H.Angle_Enc_CMD - Motor_H.Angle_Enc_Cur;
//	
//	//检查是否达到目标位置
//	if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_L && Diff_Angle_Enc >=0){
//		Motor_H.StopHeadMotor(&Motor_H);			//电机停止
//		return;
//	}else if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_R && Diff_Angle_Enc <=MID_OFFSET){
//		Motor_H.StopHeadMotor(&Motor_H);			//电机停止
//		return;
//	}
	#ifdef PRINT_ERR 
		//printf("目标ENC:%d,当前ENC:%d\r\n", Motor_H.Angle_Enc_CMD, Motor_H.Angle_Enc_Cur);
	#endif
	//没有达到目标位置
	if(Diff_Angle_Enc < 0)
		Diff_Angle_Enc = -Diff_Angle_Enc; 
	
//速度为100时 提前70个码开始线性减速比较合适  故从速度*0.7个码开始减速
	if(Diff_Angle_Enc < (Motor_H.SpeedCMD*3) )
			Motor_H.Speed = (int16_t)(Motor_H.SpeedCMD * (Diff_Angle_Enc/(Motor_H.SpeedCMD*3.0)));
	
	if(Motor_H.Speed < 1){ 
		Motor_H.Speed = 1; 
	}
		
	//pid计算执行
	PID_T* pid_s = &Motor_H.PID_H_Speed; 
	pid_s->PID_Calculate(pid_s,Motor_H.Diff_Encoder,Motor_H.Speed);

	Motor_H.PWM_Out += pid_s->Res;
	if(Motor_H.PWM_Out > HEAD_MAX_PWM)
		Motor_H.PWM_Out = HEAD_MAX_PWM;
	ExecSpeedPWM(Motor_H.TIMx, Motor_H.PWM_Out);
}
/****************************************************
函数名:ScanHeadMotorLimit
功能: 扫描头部限位情况
作者:	李尧 2015年10月14日22:26:22
****************************************************/
void ScanHeadMotorLimit(void){
	if(System_Mode == Normal){
		Motor_H.MotorProtectCheck(&Motor_H);
	}  
	int16_t Diff_Angle_Enc =  Motor_H.Angle_Enc_CMD - Motor_H.Angle_Enc_Cur; 
	if(System_Mode == Normal && GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE && GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE){
		if(Motor_H.BoardVersion == 45){
			Motor_H.Angle_Enc_Cur = HEAD_45MID_ENC;                 //liyao20160906 
			;
		}else if(Motor_H.BoardVersion == 41){
			Motor_H.Angle_Enc_Cur = Motor_H.Encoder_Max/2;                  //liyao20160906
		} 
	}
	//检查是否达到目标位置
	if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_L && Diff_Angle_Enc >=0){
		Motor_H.StopHeadMotor(&Motor_H);			//电机停止
		#ifdef PRINT_ERR 
			printf("cur:%d,%f.2,B%d,%d,%d\r\n",Motor_H.Angle_Enc_Cur,Motor_H.Angle_Cur,GET_ENC_HEAD_B,GET_LIMIT_HEAD_L,GET_LIMIT_HEAD_R);
		#endif
		return;
	}else if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_R && Diff_Angle_Enc <=MID_OFFSET){
		Motor_H.StopHeadMotor(&Motor_H);			//电机停止
		#ifdef PRINT_ERR 
			printf("cur:%d,%f.2,B%d,%d,%d\r\n",Motor_H.Angle_Enc_Cur,Motor_H.Angle_Cur,GET_ENC_HEAD_B,GET_LIMIT_HEAD_L,GET_LIMIT_HEAD_R);
		#endif
		return;
	}  
	if(GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE && Motor_H.Angle_Enc_Cur < Motor_H.Encoder_Max*0.2){ //读左限位器为0
		Motor_H.LimitFlag |= (1<<1);//限位置1
		Motor_H.Angle_Enc_Cur = Motor_H.Encoder_Min;
		if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_L){//方向向左运行
		Motor_H.StopHeadMotor(&Motor_H);//电机停止
		#ifdef PRINT_ERR 
			printf("头部左限位\r\n");
		#endif
		}
	}else{
		Motor_H.LimitFlag &= ~(1<<1);//限位置0
	}
	
	if(GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE && Motor_H.Angle_Enc_Cur > Motor_H.Encoder_Max*0.8){	//读右限位器为0
		Motor_H.LimitFlag |= (1<<0);//限位置1
		Motor_H.Angle_Enc_Cur = Motor_H.Encoder_Max;
		if(Motor_H.MotorDirCMD == DIR_MOTOR_HEAD_R ){//方向向右运行
		Motor_H.StopHeadMotor(&Motor_H);
		#ifdef PRINT_ERR 
			printf("头部右限位\r\n");
		#endif
		}
	}else{
		Motor_H.LimitFlag &= ~(1<<0);//限位置0
	}
  
  
  
//  if(GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE){
//			printf("头部左限位\r\n"); 
//  }
//  if(GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE){
//			printf("头部右限位\r\n"); 
//  }
}
/****************************************************
函数名:Head_RunLeft
功能: 头部旋转到左归位
****************************************************/	
int8_t Head_RunLeft(){
	MOTOR_HEAD_PARM* motor_H = &Motor_H;
	uint16_t timeout = 0;
//电机左归位
	HAL_GPIO_WritePin(motor_H->GPIOx, motor_H->GPIO_PIN, (GPIO_PinState)DIR_MOTOR_HEAD_L);
	ExecSpeedPWM(motor_H->TIMx, HEAD_INIT_PWM);
	motor_H->Last_Encoder = motor_H->Angle_Enc_Cur = 1000;
	DelayMS(500);
	while(GET_LIMIT_HEAD_L == HEAD_LIMIT_FALSE ){
		DelayMS(10);
		if(GET_LIMIT_HEAD_R == HEAD_LIMIT_TRUE)
			DelayMS(1000);
		if(timeout++ == 100){
			timeout = 0;
			motor_H->Diff_Encoder = motor_H->Angle_Enc_Cur - motor_H->Last_Encoder;
			motor_H->Last_Encoder = motor_H->Angle_Enc_Cur;
			if(GET_LIMIT_HEAD_L == HEAD_LIMIT_FALSE &&  motor_H->Diff_Encoder == 0){//放弃自检
				motor_H->StopHeadMotor(motor_H);
				#ifdef PRINT_ERR  
				printf("左超时\r\n");
				#endif
				return -1;
			}
		}
	} //等待到达左限位
	motor_H->Angle_Enc_Cur = 0;
	ExecSpeedPWM(motor_H->TIMx, 0);
	return 0;
}
/****************************************************
函数名:Head_Self_Inspection
功能: 执行头部电机初始化
****************************************************/	 
void Head_Self_Inspection(){
	MOTOR_HEAD_PARM* motor_H = &Motor_H;
	uint16_t timeout = 0;
	#ifdef PRINT_ERR  
		printf("头部初始化开始\r\n");
	#endif
	if(Head_RunLeft() == -1)
		return; 
	DelayMS(500);
	#ifdef PRINT_ERR
		printf("头部左归位完成\r\n");
	#endif
	//电机右归位运行
	HAL_GPIO_WritePin(motor_H->GPIOx, motor_H->GPIO_PIN, (GPIO_PinState)DIR_MOTOR_HEAD_R);
	ExecSpeedPWM(motor_H->TIMx, HEAD_INIT_PWM);
	motor_H->Last_Encoder = motor_H->Angle_Enc_Cur = 0;
	DelayMS(500);
	while(GET_LIMIT_HEAD_R == HEAD_LIMIT_FALSE ){
		DelayMS(10);
		if(GET_LIMIT_HEAD_L == HEAD_LIMIT_TRUE)
			DelayMS(1000);
		if(timeout++ == 100){
			timeout = 0;
			motor_H->Diff_Encoder = motor_H->Angle_Enc_Cur - motor_H->Last_Encoder;
			motor_H->Last_Encoder = motor_H->Angle_Enc_Cur;
			if(GET_LIMIT_HEAD_R == HEAD_LIMIT_FALSE && motor_H->Diff_Encoder == 0){//放弃自检
				motor_H->StopHeadMotor(motor_H);
				#ifdef PRINT_ERR  
				printf("右超时\r\n");
				#endif
				return;
			}
		}
		//读取记录头部电机运动电流
		ReadCurrent();
		motor_H->ElectricityLast = motor_H->Electricity;
	}//等待到达右限位0
	#ifdef PRINT_ERR 
		printf("头部右归位完成\r\n");
	#endif
	motor_H->Encoder_Min = MID_OFFSET;
	motor_H->Encoder_Max = motor_H->Angle_Enc_Cur;//motor_H->Angle_Enc_Cur;	//记录最大码数
	if(motor_H->Encoder_Max > 300){
		motor_H->BoardVersion = 41;
		motor_H->Head_Enc_Max = HEAD_41ENC_MAX;
		motor_H->Head_Mid_Enc_L = HEAD_41MID_ENC_L;
		motor_H->Head_Mid_Enc_R = HEAD_41MID_ENC_R;
		motor_H->PID_Proportion = PID_41PROPORTION;
	}else{
		motor_H->BoardVersion = 45;
		motor_H->Head_Enc_Max = HEAD_45ENC_MAX;
		motor_H->Head_Mid_Enc_L = HEAD_45MID_ENC_L;
		motor_H->Head_Mid_Enc_R = HEAD_45MID_ENC_R;
		motor_H->PID_Proportion = PID_45PROPORTION;
	}
	//计算1°对应的编码器值
	motor_H->Angle_Enc_Code = motor_H->Encoder_Max * 1.0 / HEAD_MAX_ANGLE;
	motor_H->StopHeadMotor(motor_H);
	#ifdef PRINT_ERR 
		printf("初始化完毕Enc_Max:%d-----Enc_Min:%d-----Angle_Code:%f\r\n",
		motor_H->Encoder_Max,motor_H->Encoder_Min,motor_H->Angle_Enc_Code);
	#endif
		if(
		 motor_H->Angle_Enc_Cur == 0  || 
		 motor_H->Angle_Enc_Code == 0 ||
		 motor_H->Encoder_Max == 0 
	){
		#ifdef PRINT_ERR 
		printf("自检失败return -1 %f %d %f %d \r\n",motor_H->Angle_Cur,motor_H->Angle_Enc_Cur,
		motor_H->Angle_Enc_Code,motor_H->Encoder_Max);
		#endif
		ErrorDefault();
		return;
	}
	ExecSpeedPWM(motor_H->TIMx, 0);
	DelayMS(500);
	motor_H->SelfState = SELF_H_DONE; 
	motor_H->Angle_Cur = Motor_H.Angle_Enc_Cur / Motor_H.Angle_Enc_Code;
	motor_H->SetHeadMotorParam(motor_H, 50, 120);
	motor_H->ExecuteHeadMotor(motor_H);
}



















