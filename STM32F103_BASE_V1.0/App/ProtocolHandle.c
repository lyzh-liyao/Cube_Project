#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h" 
#include <iar_dlmalloc.h>

void Heap_Use(Protocol_Info_T* pi){
  __iar_dlmalloc_stats();
}
 
void HeartBeat_P_Handle(Protocol_Info_T* pi){
	HeartBeat_P_T* hp = pi->ParameterList;
	printf("ÊÕµ½ÐÄÌø:%d\r\n", hp->para1); 
//	Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,'C');
}
 


 
