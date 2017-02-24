#include "Protocol.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h" 
#include <stdio.h> 
#include <string.h>
#include "ComBuff.h"
 
void Ack_Send(Protocol_Info_T* pi){
	uint8_t data[2] = {0};
	data[0] = pi->Serial;
	data[1] = pi->Action;
	Protocol_Send(ANSWER, data, 2); 
}

void Send(uint8_t* data, uint8_t len){
	Uart2_DMA_Sender.Write(&Uart2_DMA_Sender, data, len);
}
 

void Protocol_Init(){ 
	Protocol_Desc_T pdt;

	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = 2;
	pdt.ModuleAction = ANSWER;
  pdt.Send = Send;
	Protocol_Register(&pdt,SEND);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_PROTOCOL; 
	pdt.Handle = Cmd_P_Handle;
	//pdt.Ack = Ack_Send;
	Protocol_Register(&pdt,RECEIVE);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(CMD_A_PROTOCOL_T);
	pdt.ModuleAction = CMD_A_PROTOCOL; 
	pdt.Handle = Cmd_A_P_Handle;
	//pdt.Ack = Ack_Send;
	Protocol_Register(&pdt,RECEIVE);
}

