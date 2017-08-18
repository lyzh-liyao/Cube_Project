#include "Protocol.h"
#include "ProtocolFrame.h"
#include "ProtocolHandle.h" 
#include <stdio.h> 
#include <string.h>
#include "ComBuff.h"
#include "TaskTimeManager.h"
#include "UltrasonicDriver.h"

extern Ultrasonic_T Ultrasonic;
void Ack_Send(Protocol_Info_T* pi){
	uint8_t* ID = pi->ParameterList;
	if(ID[0] != Ultrasonic.ID && ID[0] != 0xFF)
		return;
	uint8_t data[3] = {0};
	data[0] = Ultrasonic.ID;
	data[1] = pi->Serial;
	data[2] = pi->Action;
	Protocol_Send(ANSWER, data, 3); 
}

//抢占总线发送数据
void RobSend(uint8_t* data, uint8_t len){
	ContendBegin:
	while(SYNC_READ() == SYNC_BUSY){
		DelayUS(rand()%500);
		
//		SYNC_WRITE_BUSY();
//		while(1);
		//DelayUS((uint8_t)Ultrasonic.Sn_Code+200);
	} 
	SYNC_WRITE_BUSY();
	DelayUS(rand()%100);
	SYNC_WRITE_FREE();
	DelayUS(1);
	if(SYNC_READ() == SYNC_BUSY)
		goto ContendBegin;
	SYNC_WRITE_BUSY();
	UART_TX_GPIO_Enable();
	DelayUS(500);
	HAL_UART_Transmit(&huart1, data, len, 100);
	UART_TX_GPIO_Disable();
	SYNC_WRITE_FREE();
}


void Protocol_Init(){ 
	Protocol_Desc_T pdt;
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = 2;
	pdt.ModuleAction = SN_REPORT; 
	pdt.Send = RobSend;
	Protocol_Register(&pdt,SEND);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Distance_T);
	pdt.ModuleAction = DISTANCE_REPORT; 
	pdt.Send = RobSend;
	Protocol_Register(&pdt,SEND);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = 3;
	pdt.ModuleAction = ANSWER;
  pdt.Send = RobSend;
	Protocol_Register(&pdt,SEND);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = 3;
	pdt.ModuleAction = OP_ANSWER;
  pdt.Send = RobSend;
	Protocol_Register(&pdt,SEND);
	

//------------------------------------------------
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Allot_ID_T);
	pdt.ModuleAction = ALLOT_ID;
	pdt.Ack = Ack_Send;
	pdt.Handle = Allot_ID_Handle;
	Protocol_Register(&pdt,RECEIVE);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(OP_Ctrl_T);
	pdt.ModuleAction = OP_CTRL; 
	pdt.Ack = Ack_Send;
	pdt.Handle = OP_Ctrl_Handle;
	Protocol_Register(&pdt,RECEIVE);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Main_Answer_T);
	pdt.ModuleAction = MAIN_ANSWER;
	pdt.Handle = Main_Answer_Handle;
	Protocol_Register(&pdt,RECEIVE);
	
	memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(Main_Heart_T);
	pdt.ModuleAction = MAIN_HEART;
	pdt.Ack = Ack_Send;
	pdt.Handle = HeartBeat_Handle;
	Protocol_Register(&pdt,RECEIVE);

}

