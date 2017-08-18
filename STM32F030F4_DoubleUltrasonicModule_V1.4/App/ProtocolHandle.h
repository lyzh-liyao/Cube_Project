#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include "FrameConfig.h"
#include "ProtocolFrame.h"

extern void Allot_ID_Handle(Protocol_Info_T* pi); 
extern void OP_Ctrl_Handle(Protocol_Info_T* pi); 
extern void Main_Answer_Handle(Protocol_Info_T* pi); 
extern void HeartBeat_Handle(Protocol_Info_T* pi);
#endif

