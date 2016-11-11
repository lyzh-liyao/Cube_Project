#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include "FrameConfig.h"
#include "ProtocolFrame.h"

extern void Heap_Use(Protocol_Info_T* pi);

extern int8_t run_protocol_check  (Protocol_Info_T* pi);
extern int8_t angle_protocol_check(Protocol_Info_T* pi);
extern int8_t head_protocol_check (Protocol_Info_T* pi);
extern int8_t wing_protocol_check (Protocol_Info_T* pi);

extern int8_t self_check(Protocol_Info_T* pi);
extern void Run_Protocol_Handle(Protocol_Info_T* pi);
extern void Wing_Protocol_Handle(Protocol_Info_T* pi);
extern void Head_Protocol_Handle(Protocol_Info_T* pi);
extern void Angle_Protocol_Handle(Protocol_Info_T* pi);
extern void PDR_Correnct_Protocol_Handle(Protocol_Info_T* pi);
extern void OdometerScram_Protocol_Handle(Protocol_Info_T* pi);
extern void Compensate_Protocol_Handle(Protocol_Info_T* pi);
extern void GetSnVersion_Protocol_Handle(Protocol_Info_T* pi);
extern void ReportSnVersion_Protocol(void);
extern void DealUsartCMD(void);
extern void UpLoadState(void);
#endif

