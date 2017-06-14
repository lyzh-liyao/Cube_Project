#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h"

 
void HeartBeat_P_Handle(Protocol_Info_T* pi){ 
	HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
	printf("ÊÕµ½ÐÄÌø:%d\r\n", Heartbeat_P->para1); 
}
 
 
