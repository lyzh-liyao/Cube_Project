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
	pdt.ProtocolSize = 1;
	pdt.ModuleAction = HEAP_USE; 
	pdt.Handle = Heap_Use;
	Protocol_Register(&pdt,RECEIVE);
  
  
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(RUN_PROTOCOL_T);
	pdt.ModuleAction = RUN_PROTOCOL;
	pdt.Handle = Run_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(ANGLE_PROTOCOL_T);
	pdt.ModuleAction = ANGLE_PROTOCOL;
	pdt.Handle = Angle_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(HEAD_PROTOCOL_T);
	pdt.ModuleAction = HEAD_PROTOCOL;
	pdt.Handle = Head_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(WING_PROTOCOL_T);
	pdt.ModuleAction = WING_PROTOCOL;
	pdt.Handle = Wing_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(PDRCORRECT_PROTOCOL_T);
	pdt.ModuleAction = PDRCORRECT_PROTOCOL;
	pdt.Handle = PDR_Correnct_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);  
  
//  memset(&pdt, 0, sizeof(Protocol_Desc_T));
//	pdt.ProtocolSize = sizeof(ODOMETERSCRAM_PROTOCOL_T);
//	pdt.ModuleAction = ODOMETERSCRAM_PROTOCOL;
//	pdt.Handle = PDR_Correnct_Protocol_Handle;
//	Protocol_Register(&pdt,RECEIVE);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(COMPENSATE_PROTOCOL_T);
	pdt.ModuleAction = COMPENSATE_PROTOCOL;
	pdt.Handle = Compensate_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(GETSNVERSION_PROTOCOL_T);
	pdt.ModuleAction = GETSNVERSION_PROTOCOL;
	pdt.Handle = GetSnVersion_Protocol_Handle;
	Protocol_Register(&pdt,RECEIVE);  
  
  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(STATE_PROTOCOL_T);
	pdt.ModuleAction = STATE_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(ASK_PROTOCOL_T);
	pdt.ModuleAction = ASK_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(ODOMETER_PROTOCOL_T);
	pdt.ModuleAction = ODOMETER_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(DEADRECKONING_PROTOCOL_T);
	pdt.ModuleAction = DEADRECKONING_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(RESPOND_PROTOCOL_T);
	pdt.ModuleAction = RESPOND_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);   
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(SELFINS_PROTOCOL_T);
	pdt.ModuleAction = SELFINS_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);  
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(SENDSNVERSION_PROTOCOL_T);
	pdt.ModuleAction = SENDSNVERSION_PROTOCOL;
	pdt.Send = Send;
	Protocol_Register(&pdt,SEND);  
}

