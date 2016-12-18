#include "Protocol.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h" 
#include <stdio.h> 
#include <string.h>
#include "ComBuff.h"
 
 
 
void Send(uint8_t* data, uint8_t len){
	//Uart1_DMA_Sender.Write(&Uart1_DMA_Sender, data, len);
}
 

void Protocol_Init(){ 
	Protocol_Desc_T pdt;

	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(State_P_T);
	pdt.ModuleAction = REPORT_STATE; 
	pdt.Send = Send;
	pdt.Check = NULL;
	pdt.Handle = NULL;
	Protocol_Register(&pdt,SEND);
	
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = 1;
	pdt.ModuleAction = HEAP_USE; 
	pdt.Handle = Heap_Use;
	Protocol_Register(&pdt,RECEIVE);
  
  
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(HeartBeat_P_T);
	pdt.ModuleAction = HEART_BEAT; 
	pdt.Send = Send;
	pdt.Check = NULL;
	pdt.Handle = HeartBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Run_Protocol_T);
	pdt.ModuleAction = RUN_PROTOCOL;   
	pdt.Handle = Run_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);
  
}

