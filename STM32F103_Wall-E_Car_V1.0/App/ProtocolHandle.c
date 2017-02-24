#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h" 
#include "MotorControl.h"
#include "RudderControl.h" 
//#include <iar_dlmalloc.h>


void Heap_Use(Protocol_Info_T* pi){
  //__iar_dlmalloc_stats();
	printf("被注释\r\n");
}
 
void HeartBeat_P_Handle(Protocol_Info_T* pi){
	HeartBeat_P_T* hp = pi->ParameterList;
	printf("收到心跳:%d\r\n", hp->para1); 
//	Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,'C');
}

void Run_Protocol_Handle(Protocol_Info_T* pi){
	Run_P_T* rp = pi->ParameterList;
	uint16_t l_speed, r_speed;
	MOTOR_DIR l_dir, r_dir;
	printf("\r\n执行运动指令\r\n");
	motor_L->Exec_Protocol = motor_R->Exec_Protocol = *pi;
	Heart_Flag = 1;//心跳置位
	//速度获取
	l_speed = rp->para2 << 8 | rp->para3;
	r_speed = rp->para5 << 8 | rp->para6;
	if(l_speed > 0 && l_speed < 15)//速度最低15
		l_speed = 15;
	if(r_speed > 0 && r_speed < 15)
		r_speed = 15;
	//方向获取
	l_dir = (MOTOR_DIR)rp->para1;
	r_dir = (MOTOR_DIR)rp->para4;
	
	//重复指令忽略
	if(!(motor_L->DirCMD == l_dir && motor_L->SpeedCMD == l_speed))
		motor_L->Motor_Run(motor_L, l_dir, l_speed);
	if(!(motor_R->DirCMD == r_dir && motor_R->SpeedCMD == r_speed))
		motor_R->Motor_Run(motor_R, r_dir, r_speed);
	
	printf("左轮方向：%d, 速度：%d\t右轮方向：%d, 速度：%d\r\n", rp->para1, rp->para2 << 8 | rp->para3, rp->para4, rp->para5 << 8 | rp->para6 );
}

void Rudder_Protocol_Handle(Protocol_Info_T* pi){
	Rudder_P_T* rp = pi->ParameterList;
	RudderX->Angle_CMD = rp->X_Angle;
	RudderY->Angle_CMD = rp->Y_Angle;
	printf("舵机控制：%d, %d\r\n", (uint8_t)rp->X_Angle, (uint8_t)rp->Y_Angle);
}

 
