#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include "FrameConfig.h"
#include "ProtocolFrame.h"

extern void Heap_Use(Protocol_Info_T* pi);

extern void HeartBeat_P_Handle(Protocol_Info_T* pi);
extern void Run_P_Handle(Protocol_Info_T* pi); 

extern void Cmd_Run(Protocol_Info_T* pi); 
extern void Cmd_R_Speed(Protocol_Info_T* pi); 
extern void Cmd_W_Para(Protocol_Info_T* pi); 
extern void Cmd_R_Para(Protocol_Info_T* pi); 
extern void Cmd_R_Pwr(Protocol_Info_T* pi); 
extern void Cmd_R_State(Protocol_Info_T* pi); 
extern void Cmd_R_Sn(Protocol_Info_T* pi);
#endif

