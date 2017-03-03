#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include "FrameConfig.h"
#include "ProtocolFrame.h"

extern void HeartBeat_P_Handle(Protocol_Info_T* pi);
extern void Run_P_Handle(Protocol_Info_T* pi); 

extern void Cmd_P_Handle(Protocol_Info_T* pi);
extern void Cmd_A_P_Handle(Protocol_Info_T* pi);
extern void Cmd_C_P_Handle(Protocol_Info_T* pi);
#endif

