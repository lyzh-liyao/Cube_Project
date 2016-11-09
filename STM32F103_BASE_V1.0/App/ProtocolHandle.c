#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h"

void Heap_Use(Protocol_Info_T* pi){
  __iar_dlmalloc_stats();
}
/*

  	     = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x01),
  	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x02),
  CMD_W_PARA	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x03),
  CMD_R_PARA	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x04),
  CMD_R_PWR	   = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x05),
  CMD_R_STATE	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x06),
  CMD_R_SN	   = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x07),

*/
void HeartBeat_P_Handle(Protocol_Info_T* pi){
//	HEARTBEAT_PROTOCOL_T hp = pi->protocol.HeartBeat_P;
	 
	//Protocol_Send_Transpond(pi);
	HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
	printf("收到心跳:%d\r\n", Heartbeat_P->para1);
	
//	Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,'C');
}

void Run_P_Handle(Protocol_Info_T* pi){
	//RUN_PROTOCOL_T rp = pi->protocol.Run_P;
//	printf("收到运动指令\r\n");
}


void Cmd_Run(Protocol_Info_T* pi){
  printf("Cmd_Run\r\n");
}

void Cmd_R_Speed(Protocol_Info_T* pi){
  printf("Cmd_R_Speed\r\n");
}

void Cmd_W_Para(Protocol_Info_T* pi){
  printf("Cmd_W_Para\r\n");
}

void Cmd_R_Para(Protocol_Info_T* pi){
  printf("Cmd_R_Para\r\n");
}

void Cmd_R_Pwr(Protocol_Info_T* pi){
  printf("Cmd_R_Pwr\r\n");
}

void Cmd_R_State(Protocol_Info_T* pi){
  printf("Cmd_R_State\r\n");
}

void Cmd_R_Sn(Protocol_Info_T* pi){
  printf("Cmd_R_Sn\r\n");
}


 
