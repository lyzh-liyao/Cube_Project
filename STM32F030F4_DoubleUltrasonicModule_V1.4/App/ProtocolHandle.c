#include "ProtocolHandle.h"
#include "ComBuff.h"
#include "Protocol.h"
#include "UltrasonicDriver.h"
#include "TaskTimeManager.h"

extern Ultrasonic_T Ultrasonic;

void OP_Answer_Send(uint8_t Action, uint8_t State){
	uint8_t data[3] = {0};
	data[0] = Ultrasonic.ID;
	data[1] = Action;
	data[2] = State;
	Protocol_Send(OP_ANSWER, data, 3);
}
void UltrasonicRun(void){
	ULT_RESULT res = Ultrasonic.Send(&Ultrasonic);
//	uint8_t temp = Ultrasonic.Wave_Adc_Buff[0][ADC_TEMP_CHANNEL];
//	temp = (1430 - (temp*3300/255 ))/40  + 25;
//	printf("温度:%d\r\n", temp);
}


void Allot_ID_Handle(Protocol_Info_T* pi){
	Allot_ID_T* Allot_ID = pi->ParameterList;
//	printf("%X %X %X %X\r\n",Allot_ID->SN_H,Allot_ID->SN_L,Ultrasonic.Sn_Code & 0xFF,(Ultrasonic.Sn_Code >> 8));
	if(Ultrasonic.ID == 0 && Allot_ID->SN_H == (Ultrasonic.Sn_Code >> 8) && Allot_ID->SN_L == (Ultrasonic.Sn_Code & 0xFF)){
		Ultrasonic.ID = Allot_ID->ID;
		OP_Answer_Send(pi->Action, OP_ACK_OK);
		printf("ID设置:%d\r\n", Allot_ID->ID);
	}
}

void OP_Ctrl_Handle(Protocol_Info_T* pi){
	OP_Ctrl_T* OP_Ctrl = pi->ParameterList;
	if(OP_Ctrl->ID != Ultrasonic.ID && OP_Ctrl->ID != 0xFF)
		return;
	switch(OP_Ctrl->Cmd){
		case OPEN_ULT:
			printf("指令OPEN_ULT:%d\r\n", OP_Ctrl->Data_H << 8 | OP_Ctrl->Data_L);
			if(Ultrasonic.TaskID == -1)
				Ultrasonic.TaskID = TaskTime_Add(TaskID++, TimeCycle(0,(OP_Ctrl->Data_H << 8 | OP_Ctrl->Data_L)), UltrasonicRun, Real_Mode);
			break;
		case CLOSE_ULT:
			printf("指令CLOSE_ULT\r\n");
			if(Ultrasonic.TaskID != -1){
				TaskTime_Remove(Ultrasonic.TaskID);
				Ultrasonic.TaskID = -1;
			}
			break;
		case CHANGE_INTERVAL:
			printf("指令CHANGE_INTERVAL\r\n");
			break;
	}
	uint8_t data[3] = {0};
	data[0] = Ultrasonic.ID;
	data[1] = pi->Action;
	data[2] = OP_ACK_OK;
	Protocol_Send(OP_ANSWER, data, 3);
}
void Main_Answer_Handle(Protocol_Info_T* pi){
	Main_Answer_T* Main_Answer = pi->ParameterList;
	if(Main_Answer->ID != Ultrasonic.ID && Main_Answer->ID != 0xFF)
		return;
	printf("上位机应答：seq：%d，action：%d\r\n", Main_Answer->Seq, Main_Answer->Action);
}

void HeartBeat_Handle(Protocol_Info_T* pi){ 
	Main_Heart_T* Main_Heart = pi->ParameterList;
	printf("收到心跳:%d\r\n", Main_Heart->Seq); 
}

