#include "Protocol.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h" 
#include <stdio.h> 
#include <string.h>
#include "ComBuff.h"
#include "TaskTimeManager.h"

void Ack_Send(Protocol_Info_T* pi){
	
}

void Send(uint8_t* Data, uint8_t Len){
	
}

void Protocol_Init(){ 
	Protocol_Desc_T pdt;
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Template_Send_T);
	pdt.ModuleAction = SEND_TYPE; 
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);

//------------------------------------------------	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Template_Recv_T);
	pdt.ModuleAction = RECV_TYPE;
	pdt.Ack = Ack_Send;
	pdt.Handle = Template_Recv_Handle;
	Protocol_Register(&pdt,RECEIVE);

}

