#include "ProtocolHandle.h"
#include "ComBuff.h"
#include "Protocol.h"
#include "TaskTimeManager.h"

void Template_Recv_Handle(Protocol_Info_T* pi){
	Template_Recv_T* data = pi->ParameterList;
	data->para1 = data->para1;
}

