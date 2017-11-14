#include "Protocol.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h" 
#include <stdio.h> 
#include <string.h>
#include "ComBuff.h"
#include "TaskTimeManager.h"

void Ack_Send(Protocol_Info_T* pi){
	Ptcl_Ack_T Ptcl_Ack = {pi->Serial};
	Protocol_Send(PTCL_ACK, &Ptcl_Ack, sizeof(Ptcl_Ack_T));
}

void Send(uint8_t* Data, uint8_t Len){
//	HAL_UART_Transmit(&huart4, Data, Len, 100);
//	Uart1_DMA_Sender.Write(&Uart1_DMA_Sender, Data, Len);
}

void Protocol_Init(){ 
	Protocol_Desc_T pdt;
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Ptcl_Ack_T);
	pdt.ModuleAction = PTCL_ACK; 
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(OP_Ack_T);
	pdt.ModuleAction = OP_ACK; 
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);

//------------------------------------------------	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Super_Ack_T);
	pdt.ModuleAction = SUPER_ACK;
	pdt.Ack = Ack_Send;
	pdt.Handle = Super_Ack_Handle;
	Protocol_Register(&pdt,RECEIVE);

}

