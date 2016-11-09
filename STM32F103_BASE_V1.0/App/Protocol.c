#include "Protocol.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h" 
#include <stdio.h> 
#include <string.h>
#include "ComBuff.h"
 
 
 
void Send(uint8_t* data, uint8_t len){
	Uart1_DMA_Sender.Write(&Uart1_DMA_Sender, data, len);
}
 

void Protocol_Init(){ 
	Protocol_Desc_T pdt;

	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(STATE_PROTOCOL_T);
	pdt.ModuleAction = REPORT_STATE; 
	pdt.Send = Send;
	pdt.Check = NULL;
	pdt.Handle = NULL;
	Protocol_Register(&pdt,SEND);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = HEART_BEAT; 
	pdt.Send = Send;
	pdt.Check = NULL;
	pdt.Handle = HeartBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_RUN;
	pdt.Handle = Cmd_Run;
	Protocol_Register(&pdt,RECEIVE);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_R_SPEED;
	pdt.Handle = Cmd_R_Speed;
	Protocol_Register(&pdt,RECEIVE);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_W_PARA;
	pdt.Handle = Cmd_W_Para;
	Protocol_Register(&pdt,RECEIVE);  

  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_R_PARA;
	pdt.Handle = Cmd_R_Para;
	Protocol_Register(&pdt,RECEIVE); 
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_R_PWR;
	pdt.Handle = Cmd_R_Pwr;
	Protocol_Register(&pdt,RECEIVE); 
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_R_STATE;
	pdt.Handle = Cmd_R_State;
	Protocol_Register(&pdt,RECEIVE); 
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_R_SN;
	pdt.Handle = Cmd_R_Sn;
	Protocol_Register(&pdt,RECEIVE); 
  
}

