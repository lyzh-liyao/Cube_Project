 
#include "MotorDriver.h"
#include "MotorWingDriver.h" 
#include <stdio.h>
#include <string.h>
#include "TaskTimeManager.h"
static void LimitTrigger(MOTOR_WING_PARM* motor);
static void Motor_WProtectCheck(MOTOR_WING_PARM* motor);
static void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM);
/****************************************************
函数名:MotorWingInit
功能: 翅膀电机实体初始化
作者:	李尧 2015年9月14日13:44:23
****************************************************/	 
void MotorWingInit(MOTOR_WING_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin){
	memset(motor, 0,sizeof(MOTOR_WING_PARM)); 
	
	motor->TIMx = TIMx;
	motor->GPIOx = GPIOx;
	motor->GPIO_Pin = GPIO_Pin;
	motor->Angle_Code = 0.45;
	motor->Encoder = ENCODER_CON_RES;							//初始化编码器值
	motor->EncoderLast = ENCODER_CON_RES;							//初始化编码器值
	motor->LimitState = WING_LIMIT_FALSE;					//初始化限位为无
	motor->Speed_Code = (WING_MAX_PWM - WING_BASE_PWM) / 100;			//计算速度对应的PWM
	motor->SelfState = SELF_W_ERROR;
	motor->Gain_PWM = 1;
	motor->EncoderMAX = WING_ENC_MAX;
	
	motor->ExecSpeedPWM = ExecSpeedPWM;						//赋值PWM设置函数
	motor->ExecuteWingMotor = ExecuteWingMotor;		//赋值执行函数
	motor->StopWingMotor = StopWingMotor;					//赋值停止函数	
	motor->LimitTrigger = LimitTrigger;						//赋值限位触发函数
	motor->KeepWingMotor = KeepWingMotor;					//赋值保持函数	
	motor->MotorProtectCheck = Motor_WProtectCheck; //赋值电机保护函数
	motor->SetWingMotorParam = SetWingMotorParam;	//赋值设置参数函数
}

/****************************************************
函数名:_Motor_Bottom
功能: 翅膀电机归位
作者:	李尧 2015年9月14日13:44:23
****************************************************/	 
int8_t _Motor_Bottom(MOTOR_WING_PARM* motor_WL,MOTOR_WING_PARM* motor_WR, uint16_t speed){
	int8_t Finish_L = 0,Finish_R = 0;
	motor_WL->MotorDir = motor_WR->MotorDir = DIR_WING_D;
	HAL_GPIO_WritePin(motor_WL->GPIOx, motor_WL->GPIO_Pin, (GPIO_PinState)DIR_WING_D);
	HAL_GPIO_WritePin(motor_WR->GPIOx, motor_WR->GPIO_Pin, (GPIO_PinState)DIR_WING_D);
	motor_WL->ExecSpeedPWM(motor_WL->TIMx, speed);
	motor_WR->ExecSpeedPWM(motor_WR->TIMx, speed); 
	motor_WL->EncoderLast = motor_WL->Encoder;
	motor_WR->EncoderLast = motor_WR->Encoder;
	
	do 
	{ 
		DelayMS(300);
		motor_WL->Diff_Encoder = motor_WL->Encoder - motor_WL->EncoderLast;
		motor_WL->EncoderLast = motor_WL->Encoder;
		//左电机到达限位
		if( GET_LIMIT_WING_L == WING_LIMIT_TRUE && Finish_L == 0)
		{
		  motor_WL->StopWingMotor(motor_WL);
			LimitTrigger(motor_WL);
			Finish_L = 1;
			#ifdef PRINT_ERR 	        
		    printf("左翅膀下归位完成\r\n");
			#endif
		}
		
		
		 //右电机到达限位
		if(GET_LIMIT_WING_R == WING_LIMIT_TRUE && Finish_R == 0)
		{
			motor_WR->StopWingMotor(motor_WR);
			LimitTrigger(motor_WR);
			Finish_R = 1;
			#ifdef PRINT_ERR 
		    printf("右翅膀下归位完成\r\n");
			#endif
		}
		motor_WR->Diff_Encoder = motor_WR->Encoder - motor_WR->EncoderLast;
		motor_WR->EncoderLast = motor_WR->Encoder;
		
		if(motor_WL->Diff_Encoder == 0 && Finish_L == 0){
			motor_WL->StopWingMotor(motor_WL);
			Finish_L = -1;
			#ifdef PRINT_ERR 	        
		    printf("左翅膀下归位超时\r\n");
			#endif
		}
		if(motor_WR->Diff_Encoder == 0 && Finish_R == 0){
			motor_WR->StopWingMotor(motor_WR);
			Finish_R = -1;
			#ifdef PRINT_ERR 	        
		    printf("右翅膀下归位超时\r\n");
			#endif
		}
   }while( !(Finish_L && Finish_R));
	 motor_WL->Encoder = motor_WR->Encoder = 0;
	return Finish_L + Finish_R;
}
/****************************************************
函数名:_Motor_TryUP
功能: 尝试能够运动的最小PWM
作者:	李尧 2015年9月14日13:44:23
****************************************************/	 
void _Motor_TryUP(MOTOR_WING_PARM* motor_WL,MOTOR_WING_PARM* motor_WR){
	int8_t Finish_L = 0,Finish_R = 0;
	motor_WL->MotorDir = motor_WR->MotorDir = DIR_WING_U;
	HAL_GPIO_WritePin(motor_WL->GPIOx, motor_WL->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	HAL_GPIO_WritePin(motor_WR->GPIOx, motor_WR->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	
	do{
		motor_WL->ExecSpeedPWM(motor_WL->TIMx, motor_WL->PWM_Base);
		if(Finish_L == 0)
			motor_WL->PWM_Base += 10;
		
		if(motor_WL->EncoderSUM != 0 && Finish_L == 0){
			Finish_L = 1;
			#ifdef PRINT_ERR 
		    printf("左翅膀最小能动PWM:%d\r\n",motor_WL->PWM_Base);
			#endif
		}
		
		if(motor_WL->PWM_Base == WING_MAX_PWM){
			Finish_L = 1;
			motor_WL->PWM_Base = 0;
			#ifdef PRINT_ERR 
		    printf("测试左翅膀最小能动PWM失败\r\n");
			#endif
		}
		motor_WR->ExecSpeedPWM(motor_WR->TIMx, motor_WR->PWM_Base);
		if(Finish_R == 0)
			motor_WR->PWM_Base += 10;
		if(motor_WR->EncoderSUM != 0 && Finish_R == 0){
			Finish_R = 1;
			#ifdef PRINT_ERR 
		    printf("右翅膀最小能动PWM:%d\r\n",motor_WR->PWM_Base);
			#endif
		}
		
		if(motor_WR->PWM_Base == WING_MAX_PWM){
			Finish_R = 1;
			motor_WR->PWM_Base = 0;
			#ifdef PRINT_ERR 
		    printf("测试右翅膀最小能动PWM失败\r\n");
			#endif
		}
		DelayMS(25);
		//printf("%d,%d,右增益%f\r\n",motor_WL->PWM_Base,motor_WR->PWM_Base,motor_WR->Gain_PWM);
	
	}while(!(Finish_L && Finish_R));
	/*if(motor_WL->PWM_Base >0 && motor_WR->PWM_Base>0){
		if(motor_WL->PWM_Base > motor_WR->PWM_Base){
			motor_WL->Gain_PWM = (float)motor_WL->PWM_Base / motor_WR->PWM_Base;
			printf("左增益%f\r\n", motor_WR->Gain_PWM);
		}else{
			motor_WR->Gain_PWM = (float)motor_WR->PWM_Base / motor_WL->PWM_Base;
			printf("%d,%d,右增益%f\r\n",motor_WL->PWM_Base,motor_WR->PWM_Base,motor_WR->Gain_PWM);
		}
	}*/
	
}
/****************************************************
函数名:_Motor_Top
功能: 翅膀电机运动到顶端
作者:	李尧 2015年9月14日13:44:23
****************************************************/	 
int8_t _Motor_Top(MOTOR_WING_PARM* motor_WL,MOTOR_WING_PARM* motor_WR){
	int8_t Finish_L = 0,Finish_R = 0;
	motor_WL->EncoderSUM = motor_WR->EncoderSUM = 0;
	motor_WL->MotorDir = motor_WR->MotorDir = DIR_WING_U;
	HAL_GPIO_WritePin(motor_WL->GPIOx, motor_WL->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	HAL_GPIO_WritePin(motor_WR->GPIOx, motor_WR->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	motor_WL->ExecSpeedPWM(motor_WL->TIMx, WING_INIT_PWM);
	motor_WR->ExecSpeedPWM(motor_WR->TIMx, WING_INIT_PWM);
	motor_WL->EncoderLast = motor_WL->Encoder;
	motor_WR->EncoderLast = motor_WR->Encoder;
	do{
		DelayMS(300);
		motor_WL->Diff_Encoder = motor_WL->Encoder - motor_WL->EncoderLast;
		motor_WL->EncoderLast = motor_WL->Encoder;
		//左电机到达顶端
		if(motor_WL->Diff_Encoder == 0 && Finish_L == 0)
		{
			motor_WL->EncoderMAX = motor_WL->EncoderSUM ;
			motor_WL->Angle_Code = motor_WL->EncoderMAX * 1.0 / WING_MAX_ANGLE;//1°对应编码器值
			Finish_L = 1;
			#ifdef PRINT_ERR 	        
			printf("左翅膀上归位完成,Encoder_L:%d,Code:%f\r\n",motor_WL->EncoderSUM,motor_WL->Angle_Code);
			#endif
		}
		
		motor_WR->Diff_Encoder = motor_WR->Encoder - motor_WR->EncoderLast;
		motor_WR->EncoderLast = motor_WR->Encoder;
		//右电机到达顶端
		if( motor_WR->Diff_Encoder == 0 && Finish_R == 0)
		{
			motor_WR->EncoderMAX = motor_WR->EncoderSUM ;
			motor_WR->Angle_Code = motor_WR->EncoderMAX * 1.0 / WING_MAX_ANGLE;//1°对应编码器值
			Finish_R = 1;
			
			#ifdef PRINT_ERR 
		    printf("右翅膀上归位完成,Encoder_R:%d,Code:%f\r\n",motor_WR->EncoderSUM,motor_WR->Angle_Code);
			#endif
		}
		//读取记录翅膀电机运动电流
		ReadCurrent();
		motor_WL->ElectricityLast = motor_WR->ElectricityLast = motor_WL->Electricity;
	}while( !(Finish_L && Finish_R));
	return Finish_L + Finish_R;
}

/****************************************************
函数名:Wing_Self_Inspection
功能: 执行翅膀电机初始化
作者:	李尧 2015年9月14s日13:44:23
****************************************************/	 
void Wing_Self_Inspection(){ 
	MOTOR_WING_PARM* motor_WL = &Motor_WL,* motor_WR = &Motor_WR;
	#ifdef PRINT_ERR 
		printf("翅膀初始化开始\r\n");
	#endif
	//电机归位 
	if(_Motor_Bottom(motor_WL, motor_WR, WING_INIT_PWM) <= 0){
		#ifdef PRINT_ERR 
		printf("翅膀下归位超时\r\n");
		#endif
		
	}
	//_Motor_TryUP(motor_WL, motor_WR);
	//向上运行
	if(_Motor_Top(motor_WL, motor_WR) <= 0){
		#ifdef PRINT_ERR 
		printf("翅膀上运行超时\r\n");
		#endif
	}
	//电机回位
	if(_Motor_Bottom(motor_WL, motor_WR, 100) <= 0){ 
		#ifdef PRINT_ERR 
		printf("翅膀下回位超时\r\n");
		#endif
	}
	
	/*if(	motor_WL->EncoderMAX == 0	||
			motor_WL->Angle_Code == 0 ||
			motor_WR->EncoderMAX == 0	||
			motor_WR->Angle_Code == 0 
		){
			#ifdef PRINT_ERR 
			printf("自检参数异常\r\n");
			#endif
			return;
		}*/
	motor_WL->SelfState = motor_WR->SelfState = SELF_W_DONE;
	#ifdef PRINT_ERR 
		printf("翅膀初始化完成\r\n");
	#endif
}
	
/****************************************************
函数名:ExecSpeedPWM
功能: 设置输出PWM
****************************************************/	 
void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM){
	if(PWM > WING_MAX_PWM) PWM = WING_MAX_PWM;
	if(PWM < 0) PWM = 0;
	TIM_SetCompare1(TIMx, PWM);
}



/****************************************************
函数名:SetWingMotorParam
功能: 设置电机参数
参数:	电机实体，速度，方向，角度（0-90）
作者:	李尧 2015年9月14日13:44:23
****************************************************/
void SetWingMotorParam(MOTOR_WING_PARM* motor, uint16_t speed, int16_t angle){
	//if(motor->SelfState == SELF_W_ERROR) return;
	if(speed != 0)
		speed = speed * motor->Speed_Code + WING_BASE_PWM;
	
	if(angle > WING_MAX_ANGLE)  angle = WING_MAX_ANGLE;//目标角度合法性校验
	motor->Angle_CMD = angle;
	int16_t offset_angle = angle - motor->Angle_Cur;	//计算相对于当前位置的角度差
	if(offset_angle > 0) 
		motor->MotorDirCMD = DIR_WING_U;
	else
		motor->MotorDirCMD = DIR_WING_D;
	offset_angle = offset_angle < 0?-offset_angle:offset_angle;//取绝对值
	
	motor->EncoderCMD = (offset_angle * motor->Angle_Code);		 //计算角度偏差对应编码值
	//printf("cur角度：%d,角度enc：%d",motor->Angle_Cur,motor->EncoderCMD);
	motor->SpeedCMD = speed; 																	 //设置运动速度
	
}

/****************************************************
函数名:ExecuteWingMotor
功能: 执行电机参数
作者:	李尧 2015年9月14日13:44:23
****************************************************/	 
void ExecuteWingMotor(MOTOR_WING_PARM* motor){
	//执行电机方向
	if(motor->SpeedCMD == 0){
		motor->KeepWingMotor(motor);
		return;
	}
	motor->MotorDir = motor->MotorDirCMD;//标识当前方向
	motor->Speed = motor->SpeedCMD;			 //标识当前速度
	motor->Encoder = motor->EncoderCMD;	 //目标编码值取值
	motor->RunState = MOTOR_W_RUN; 				 //设置当前运动状态
	
	if(motor->Angle_CMD == 0){
		motor->MotorDir = motor->MotorDirCMD = DIR_WING_D;
		//motor->Encoder = motor->EncoderCMD = 800;
		motor->Down_Flag = 1;
	}
	/*if(motor->MotorDirCMD == DIR_WING_D){//向下运行译为自由落体并向上缓冲
		uint8_t para_speed = (motor->Speed - WING_BASE_PWM)/motor->Speed_Code;
		motor->Speed = WING_SLOWLY_PWM - (WING_SLOWLY_PWM / 100 * para_speed);  //缓冲pwm
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
		motor->Speed = motor->PWM_Base;
	}*/
	HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)motor->MotorDirCMD);
	motor->ExecSpeedPWM(motor->TIMx, motor->Speed);//执行电机速度
}

/****************************************************
函数名:StopWingMotor
功能: 电机停止
作者:	李尧 2015年9月14日13:44:23
****************************************************/
void StopWingMotor(MOTOR_WING_PARM* motor){
	if(motor->MotorDirCMD == DIR_WING_D)
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	else if(motor->MotorDirCMD == DIR_WING_U)
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_D); 
	motor->ExecSpeedPWM(motor->TIMx, 0);//设置电机速度为0
	motor->Encoder = 0;
}

/****************************************************
函数名:KeepWingMotor
功能: 电机停止
作者:	李尧 2015年9月14日13:44:23
****************************************************/
void KeepWingMotor(MOTOR_WING_PARM* motor){
	motor->RunState = MOTOR_W_KEEP;												//设置当前运动状态
	//motor->MotorDirCMD = DIR_WING_U;										//设置方向为向上
	//motor->Speed = motor->SpeedCMD = motor->PWM_Base*0.7;//WING_KEEP_SPEED * motor->Speed_Code; 	//设置保持速度
	
	/*if(motor->MotorDirCMD == DIR_WING_U){
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_D);
	}else if(motor->MotorDirCMD == DIR_WING_D){ 
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	}*/
	HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	motor->ExecSpeedPWM(motor->TIMx, WING_KEEP_PWM);				  //执行电机速度
	//respond_protocol_send(&motor->Exec_Protocol);
	#ifdef PRINT_ERR 
	//printf("KEEP保持\r\n");
	#endif
}

/****************************************************
函数名:LimitTrigger
功能: 电机限位触发
作者:	李尧 2015年10月14日22:26:22
****************************************************/
void LimitTrigger(MOTOR_WING_PARM* motor){
	motor->EncoderSUM = 0;								//初始化编码绝对定位
	motor->Protect_Time = 0;							//初始化保护计数
	motor->Angle_Cur = 0;									//初始化当前角度
	motor->LimitState = WING_LIMIT_TRUE;	//设置限位标志
}

/****************************************************
函数名:Motor_WProtectCheck
功能: 电机保护检查
作者:	李尧 2015年10月14日22:26:22
****************************************************/
void Motor_WProtectCheck(MOTOR_WING_PARM* motor){
	if(motor->RunState == MOTOR_W_STOP){		//运动状态为停止
		motor->Protect_Time = 0;
	}else{
		if(motor->Protect_Time++ == 2000){		//计数器累加到2000 * 30 = 60秒
			motor->Protect_Time = 0;					//初始化保护计数器
			motor->StopWingMotor(motor);			//电机停止
			#ifdef PRINT_ERR 
			printf("触发翅膀电机保护\r\n");
			#endif
		}
	}
}

/****************************************************
函数名:ScanWingMotorLimit
功能: 扫描翅膀限位情况
作者:	李尧 2015年9月14日13:44:23
****************************************************/
void ScanWingMotorLimit(void){
	
	//运动过程中指定时间内编码器没有变化，保护翅膀电机
	Motor_WL.MotorProtectCheck(&Motor_WL);
	Motor_WR.MotorProtectCheck(&Motor_WR);
	//翅膀向下时的瞬时加速
	if(Motor_WL.Down_Flag == 1){
		Motor_WL.Down_Flag ++;
		Motor_WL.ExecSpeedPWM(Motor_WL.TIMx, WING_DOWN_PWM); 
	}else if(Motor_WL.Down_Flag == 2){
		Motor_WL.Down_Flag = 0;
		Motor_WL.ExecSpeedPWM(Motor_WL.TIMx, 0);
	}
	
	if(Motor_WR.Down_Flag == 1){
		Motor_WR.Down_Flag ++;
		Motor_WR.ExecSpeedPWM(Motor_WR.TIMx, WING_DOWN_PWM);
	}else if(Motor_WR.Down_Flag == 2){
		Motor_WR.Down_Flag = 0;
		Motor_WR.ExecSpeedPWM(Motor_WR.TIMx, 0);
	}
	//左翅限位检测
	if(GET_LIMIT_WING_L == WING_LIMIT_TRUE){
		Motor_WL.LimitTrigger(&Motor_WL);
		if(Motor_WL.MotorDirCMD == DIR_WING_D){	//防止电机还没来得及向上就触发限位
			Motor_WL.StopWingMotor(&Motor_WL);//电机停止
			Motor_WL.RunState = MOTOR_W_STOP;					//设置运动状态
		}
		#ifdef PRINT_ERR 
				//printf("翅膀左限位\r\n");
		#endif
	}else{
		Motor_WL.LimitState = WING_LIMIT_FALSE; //设置限位标志
	}

	//右翅限位检测
	if(GET_LIMIT_WING_R == WING_LIMIT_TRUE){
		Motor_WR.LimitTrigger(&Motor_WR);
		if(Motor_WR.MotorDirCMD == DIR_WING_D){	//防止电机还没来得及向上就触发限位
			Motor_WR.StopWingMotor(&Motor_WR);//电机停止
			Motor_WR.RunState = MOTOR_W_STOP;					//设置运动状态
		}
		#ifdef PRINT_ERR 
			//printf("翅膀右限位\r\n");
		#endif
	}else{
		Motor_WR.LimitState = WING_LIMIT_FALSE; //设置限位标志
	}
}



















