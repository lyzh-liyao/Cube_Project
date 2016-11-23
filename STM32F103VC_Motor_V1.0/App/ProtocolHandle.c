#include "ProtocolHandle.h" 
#include "ComBuff.h" 
//#include <iar_dlmalloc.h>
#include "MotorWingDriver.h"
#include "MotorHeadDriver.h"
#include <stdlib.h>
#include <string.h>

void Heap_Use(Protocol_Info_T* pi){
//  __iar_dlmalloc_stats();
}
 
//###################################协议校验函数区###################################

int8_t self_check(Protocol_Info_T* pi){
	if(System_Mode == Normal)
		return 1;
	else 
		return -1;
}

uint8_t CheckPower(void);

void ask_send(uint8_t seq){
  ASK_PROTOCOL_T ask_p = {0};
  ask_p.para1 = seq;
  Protocol_Send(ASK_PROTOCOL ,&ask_p, sizeof(ASK_PROTOCOL_T));
}
/****************************************************
	函数名:	angle_protocol_check
	功能:		双轮角度控制电机协议校验
	参数:		ANGLE_PROTOCOL_T协议实体指针
	返回值:	
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
int8_t angle_protocol_check(Protocol_Info_T* pi){
	ANGLE_PROTOCOL_T* ap = pi->ParameterList;
	if(!(ap->para16 <=1))//是否新指令校验
		return -1;
	return 0;
}

/****************************************************
	函数名:	wing_protocol_check
	功能:		翅膀电机协议校验
	参数:		WING_PROTOCOL_T协议实体指针
	返回值:	-1：左翅速度参数有误 -2：右翅速度参数有误 -3：是否新指令参数有误
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
int8_t wing_protocol_check(Protocol_Info_T* pi){
	WING_PROTOCOL_T* wp = pi->ParameterList;
	if(!(wp->para2 >=0 && wp->para2 <=100))//左翅速度校验
		return -1;
	if(!(wp->para4 >=0 && wp->para4 <=100))//右翅速度校验
		return -2;
	if(!(wp->para6 >= 0 && wp->para6 <=1))//是否新指令校验
		return -3;
	return 0;
}
/****************************************************
	函数名:	Run_Protocol_Handle
	功能:		双轮控制命令协议函数
	作者:		liyao 2015年10月16日15:21:46
****************************************************/
void Run_Protocol_Handle(Protocol_Info_T* pi){
	RUN_PROTOCOL_T* rp = pi->ParameterList;
	#ifdef PRINT_ERR 
			printf("收到运动指令\r\n");
	#endif
	#ifndef PRINT_ERR 
	ask_send(rp->para7);
	#endif
	if(rp->para8 == 1){ 
		if(CheckPower() == 0) return;
		//处理左右轮运动
	}
}

/****************************************************
	函数名:	Wing_Protocol_Handle
	功能:		翅膀控制命令协议函数
	作者:		liyao 2015年10月16日15:21:46
****************************************************/
void Wing_Protocol_Handle(Protocol_Info_T* pi){
	WING_PROTOCOL_T* wp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(wp->para5);
	#endif
	
	if(CheckPower() == 0) return;
	if(wp->para6 == 1){
		#ifdef PRINT_ERR 
			printf("收到翅膀指令\r\n");
		#endif
		//处理翅膀运动 
		Motor_WL.Exec_Protocol = *pi;
		Motor_WR.Exec_Protocol = *pi;
		if(wp->para1 > (WING_MAX_ANGLE - 10))
			wp->para1 = (WING_MAX_ANGLE - 10);
		if(wp->para3 > (WING_MAX_ANGLE - 10))
			wp->para3 = (WING_MAX_ANGLE - 10);
		Motor_WL.SetWingMotorParam(&Motor_WL, wp->para2, wp->para1);
		Motor_WR.SetWingMotorParam(&Motor_WR, wp->para4, wp->para3);	
		Motor_WL.ExecuteWingMotor(&Motor_WL);
		Motor_WR.ExecuteWingMotor(&Motor_WR);
	}
	
}

/****************************************************
	函数名:	Head_Protocol_Handle
	功能:		头部控制命令协议函数
	作者:		liyao 2015年10月16日15:21:46
****************************************************/
void Head_Protocol_Handle(Protocol_Info_T* pi){
	HEAD_PROTOCOL_T* hp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(hp->para3);
	#endif
	if(hp->para4 == 1){
		#ifdef PRINT_ERR 
			printf("收到头部指令\r\n");
		#endif 
		if(CheckPower() == 0) return;
		Motor_H.Exec_Protocol = *pi;
		if(abs((int)Motor_H.Angle_Cur - hp->para1) < 5 && hp->para2 != 0  && Motor_H.RunState == MOTOR_H_STOP)
			return;
		//处理头部运动
		Motor_H.SetHeadMotorParam(&Motor_H, hp->para2, hp->para1);
		Motor_H.ExecuteHeadMotor(&Motor_H);
	}
}

/****************************************************
	函数名:	Head_Protocol_Handle
	功能:		双轮指定角度命令协议函数
	作者:		liyao 2015年10月16日15:21:46
****************************************************/
void Angle_Protocol_Handle(Protocol_Info_T* pi){
	ANGLE_PROTOCOL_T* ap =  pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(ap->para16);
	#endif
	if(ap->para16 == 1){
		if(CheckPower() == 0) return;
	}
}

/****************************************************
	函数名:	PDR_Correnct_Protocol_Handle
	功能:		航位校验命令协议函数
	作者:		liyao 2015年10月20日15:26:37
****************************************************/
void PDR_Correnct_Protocol_Handle(Protocol_Info_T* pi){
	PDRCORRECT_PROTOCOL_T* pcp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(pcp->para14);
	#endif
}

/****************************************************
	函数名:	OdometerScram_Protocol_Handle
	功能:		双轮根据里程转指定角度
	作者:		liyao 2015年12月24日16:28:42
****************************************************/
void OdometerScram_Protocol_Handle(Protocol_Info_T* pi){
	ODOMETERSCRAM_T* osp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(osp->para16);
	#endif

}

/****************************************************
	函数名:	Compensate_Protocol_Handle
	功能:		驱动轮补偿处理
	作者:		liyao 2016年1月9日 19:20:22
****************************************************/
void Compensate_Protocol_Handle(Protocol_Info_T* pi){
	COMPENSATE_PROTOCOL_T* cp =  pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(cp->para42);
	#endif

}


/****************************************************
	函数名:	ReportSnVersion_Protocol
	功能:		上报程序版本及芯片ChipID
	作者:		2016年8月23日11:46:40
****************************************************/
void ReportSnVersion_Protocol(){
	SENDSNVERSION_PROTOCOL_T sendSnVersion_protocol = {0}; 
	uint8_t verSN[32] = {'E','0','1','M','O','T','0','4','V',MAJORVERSION,MINORVERSION,BATEVERSION,VDATE};
	
  verSN[20] = *((uint8_t*)(CHIPIDADDR + 11));
	verSN[21] = *((uint8_t*)(CHIPIDADDR + 10));
	verSN[22] = *((uint8_t*)(CHIPIDADDR + 9));
	verSN[23] = *((uint8_t*)(CHIPIDADDR + 8));
	verSN[24] = *((uint8_t*)(CHIPIDADDR + 7));
	verSN[25] = *((uint8_t*)(CHIPIDADDR + 6));
	verSN[26] = *((uint8_t*)(CHIPIDADDR + 5));
	verSN[27] = *((uint8_t*)(CHIPIDADDR + 4));
	verSN[28] = *((uint8_t*)(CHIPIDADDR + 3)); 
	verSN[29] = *((uint8_t*)(CHIPIDADDR + 2));
	verSN[30] = *((uint8_t*)(CHIPIDADDR + 1));
	verSN[31] = *((uint8_t*)(CHIPIDADDR + 0)); 
	
	memcpy(&sendSnVersion_protocol, verSN, sizeof(uint8_t) * 32);
  Protocol_Send(SENDSNVERSION_PROTOCOL ,&sendSnVersion_protocol, sizeof(SENDSNVERSION_PROTOCOL_T));
}


/****************************************************
	函数名:	GetSnVersion_Protocol_Handle
	功能:		获取程序版本及芯片ChipID
	作者:		liyao 2016年5月11日09:34:02
****************************************************/
void GetSnVersion_Protocol_Handle(Protocol_Info_T* pi){
	GETSNVERSION_PROTOCOL_T* gp =  pi->ParameterList; 
	#ifndef PRINT_ERR 
	ask_send(gp->para3);
	#endif
	if(gp->para1 == 0x01){ 
		ReportSnVersion_Protocol();
	}else if(gp->para1 == 0x02){
		NVIC_SystemReset();
	}
} 


/****************************************
函数名:	UpLoadState
备注: 上传状态机器人当前状态，每500ms开始以(1字节/5ms)的频率发送一组数据
****************************************/
int32_t FloatTime = 0;
void UpLoadState(void)
{
	uint8_t nomalError=0;
	uint8_t crashError=0;
	//检查供电是否异常 异常则停止运动
	if(CheckPower() == 0){  
		crashError |= (1<<3);
		crashError |= (1<<4); 
	};
	//Protocol_Info_T protocol_info = Create_Protocol_Info(sizeof(STATE_PROTOCOL_T),STATE_PROTOCOL,NULL,NULL);
	STATE_PROTOCOL_T state_protocol = {0}; 
//	state_protocol.para1 = Motor1.motorDir;//编码器1方向（左轮） 
//  state_protocol.para2 = Motor1.motorSpeed;//左轮增量            
//  state_protocol.para3 = Motor2.motorDir;   //编码器2方向（右轮） 
//  state_protocol.para4 = Motor2.motorSpeed;//右轮增量    
 
	state_protocol.para5 = (int)Motor_H.Angle_Cur;    //头部角度 	    
  state_protocol.para6 = Motor_WL.Angle_Cur;        //左翅角度 
	state_protocol.para7 = Motor_WR.Angle_Cur; 		   //右翅角度
//	state_protocol.para8 = Motor1.Current>>4;
//	state_protocol.para9 = Motor2.Current>>4;
	state_protocol.para10 = Motor_H.Current>>4;
	state_protocol.para11 = Motor_WL.Current>>4;
  state_protocol.para12 = 0;   //序号 
//	if(Motor1.CurrentStrategy ==OBSTACLE_CURRENT){
//		crashError |= 0x01;
//	}
//	if(Motor2.CurrentStrategy ==OBSTACLE_CURRENT){
//		crashError |= 0x01<<1;
//	}
	//---para13---start---
//	nomalError |=	(Motor1.runningflag<<1);//左轮是否运动
//	nomalError |=	(Motor2.runningflag<<2);//右轮是否运动
	if(Motor_H.RunState == MOTOR_H_RUN)//头部是否运动
		nomalError |= 1 << 3; 
	if(Motor_WL.RunState == MOTOR_W_RUN)//左翅是否运动
		nomalError |= 1 << 4;     
	if(Motor_WR.RunState == MOTOR_W_RUN)//右翅是否运动
		nomalError |= 1 << 5;  
//	if(MOTOR_FLOAT_A == 0){//左轮是否悬空
//		nomalError |=	(1<<6);
//		if(FloatTime == 0){
//			FloatTime = microsecond; 
//		}else if(Elapse_Us(microsecond, FloatTime) > 10*1000*1000){
//			ScramFlag = 1; 
//		}
//	}
//	if(MOTOR_FLOAT_B == 0){//右轮是否悬空
//		nomalError |=	(1<<7);
//		if(FloatTime == 0){
//			FloatTime = microsecond; 
//		}else if(Elapse_Us(microsecond, FloatTime) > 10*1000*1000){
//			ScramFlag = 1; 
//		}
//	}
//	if(MOTOR_FLOAT_A == 1 && MOTOR_FLOAT_B == 1) FloatTime = 0;
//	
  state_protocol.para13 = nomalError;   //错误信息按位 置1  
	//---para13---end--- 
  state_protocol.para14 = crashError;	//错误信息按位 置1  
	Protocol_Send(STATE_PROTOCOL ,&state_protocol, sizeof(STATE_PROTOCOL_T));

//	protocol_info.len = sizeof(ODOMETER_PROTOCOL_T);
//	protocol_info.type = ODOMETER_PROTOCOL;
	ODOMETER_PROTOCOL_T odometer_protocol = {0}; 
//	odometer_protocol.para1 = Motor1.encoder>>24;//左轮里程
//  odometer_protocol.para2 = Motor1.encoder>>16;//左轮里程        
//  odometer_protocol.para3 = Motor1.encoder>>8;   //左轮里程
//  odometer_protocol.para4 = Motor1.encoder;//左轮里程 
//  odometer_protocol.para5 = Motor2.encoder>>24;		//右轮里程           
//  odometer_protocol.para6 = Motor2.encoder>>16;   //右轮里程           
//  odometer_protocol.para7 = Motor2.encoder>>8;		//右轮里程           
//  odometer_protocol.para8 = Motor2.encoder; 	//右轮里程          
  odometer_protocol.para9 = 0; 	//保留           
  odometer_protocol.para10 = 0;	 //保留    
	Protocol_Send(ODOMETER_PROTOCOL ,&odometer_protocol, sizeof(ODOMETER_PROTOCOL_T));

	//20151020-tc-modify
	DEADRECKONING_PROTOCOL_T deadreckoning_protocol = {0}; 
  deadreckoning_protocol.para14 = 0;	 //保留   
	Protocol_Send(DEADRECKONING_PROTOCOL ,&deadreckoning_protocol, sizeof(DEADRECKONING_PROTOCOL_T));

}
 


 
