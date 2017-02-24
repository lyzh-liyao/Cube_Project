#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h"
#include "StepMotor.h"
 
void HeartBeat_P_Handle(Protocol_Info_T* pi){ 
	HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
	printf("收到心跳:%d\r\n", Heartbeat_P->para1); 
}

//按指定距离转动
void Cmd_P_Handle(Protocol_Info_T* pi){ 
	CMD_PROTOCOL_T* Cmd_P = pi->ParameterList;
	printf("收到指令:\r\n");
	uint16_t integer  = Cmd_P->para2 << 8 | Cmd_P->para3;//整数
	uint16_t decimals = Cmd_P->para4 << 8 | Cmd_P->para5;//小数
	float offset = decimals;
	while(1){
		offset = offset / 10.0;
		if((int16_t)offset % 10 == 0)
			break;
	}
	offset = integer + offset;
	printf("方向:%d, 运动偏移量：%f,速度：%d\r\n",Cmd_P->para1, offset, Cmd_P->para6);
	stepMotor.Run_Offset(Cmd_P->para1, offset, Cmd_P->para6);
}
//按指定角度转动
void Cmd_A_P_Handle(Protocol_Info_T* pi){ 
	CMD_A_PROTOCOL_T* Cmd_P = pi->ParameterList;
	printf("收到指令:\r\n");
	uint16_t integer  = Cmd_P->para2 << 8 | Cmd_P->para3;//整数
	uint16_t decimals = Cmd_P->para4 << 8 | Cmd_P->para5;//小数
	float offset = decimals;
	while(1){
		offset = offset / 10.0;
		if((int16_t)offset % 10 == 0)
			break;
	}
	offset = integer + offset;
	printf("运动角度：%f,速度：%d\r\n", offset, Cmd_P->para6);
	stepMotor.Run_Angle(Cmd_P->para1, offset, Cmd_P->para6);
}
