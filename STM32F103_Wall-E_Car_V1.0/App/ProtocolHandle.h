#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include "FrameConfig.h"
#include "ProtocolFrame.h"

extern void Heap_Use(Protocol_Info_T* pi);

extern void HeartBeat_P_Handle(Protocol_Info_T* pi); 

extern void Run_Protocol_Handle(Protocol_Info_T* pi);
extern void Rudder_Protocol_Handle(Protocol_Info_T* pi);
#endif

