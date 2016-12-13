#ifndef __PROTOCOLFRAME_H__
#define __PROTOCOLFRAME_H__
#include "FrameConfig.h"
#include "Protocol.h" 
#include "LOG.h"
#include "Queue.h"
/*-----字符转义-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/

#define PROTOCOL_SINGLE_BUFF 100
 
/*协议类型  发送   接收  转发*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
   
//---------------------------框架区------------------------------------- 
typedef struct _PROTOCOL_INFO_T Protocol_Info_T ; 
typedef struct _PROTOCOL_DESC_T Protocol_Desc_T ; 
typedef struct _PROTOCOL_RESOLVER_T Protocol_Resolver_T;
/****************************************************
	结构体名:	Protocol_Desc_T
	功能: 协议描述
	作者：liyao 2015年9月8日14:10:51
****************************************************/
struct _PROTOCOL_DESC_T{ 
	MODULE_ACTION ModuleAction;
	MODULE TargetModule;
	MODULE SrcModule;
	uint8_t 		ProtocolSize;	//参数结构体大小
	uint8_t		Serial; 
	void (*Send)(uint8_t* Data, uint8_t Len);
	void (*Handle)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};

/****************************************************
	结构体名:	Protocol_Info_T
	功能: 协议信息描述
	作者：liyao 2015年9月8日14:10:51
****************************************************/
struct _PROTOCOL_INFO_T{
	uint8_t Head;		//帧头
	uint8_t Standby1;//预留
	uint8_t Plen;		//帧长
	uint8_t Module; //目标模块
	uint8_t Serial;	//序号
	uint8_t Action;	//指令码 
	void* ParameterList;//参数
	uint8_t CheckSum;//校验和 
	uint8_t Tail;		//帧尾 
	uint8_t ParaLen;	//参数长度
	uint8_t AllLen;		//总长度
	Protocol_Desc_T* ProtocolDesc;//协议描述
	void (*Handle)(Protocol_Info_T*);
	void (*Ack)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};


/****************************************************
	结构体名:	Protocol_Resolver_T
	功能: 协议解析器
	作者：liyao 2016年9月7日17:55:45
****************************************************/
struct _PROTOCOL_RESOLVER_T{ 
	Queue_Head_T  *Protocol_Queue; 
	Protocol_Info_T pi;   
	uint8_t  Cnt; 				//参数个数计数
	uint8_t  ParaData[PROTOCOL_SINGLE_BUFF];
	uint8_t  Index;
	int8_t 	 Is_FE;
	uint16_t CheckSum;
	uint8_t  Recv_State;
	int8_t (*Protocol_Put)(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
	void (*Fetch_Protocol)(Protocol_Resolver_T* pr);
};



//###################################声明区################################### 
#if PROTOCOL_RESOLVER_1
	extern Protocol_Resolver_T *ProtocolResolver_1; 
#endif
#if PROTOCOL_RESOLVER_2
	extern Protocol_Resolver_T *ProtocolResolver_2; 
#endif
#if PROTOCOL_RESOLVER_3
	extern Protocol_Resolver_T *ProtocolResolver_3; 
#endif
#if PROTOCOL_RESOLVER_4
	extern Protocol_Resolver_T *ProtocolResolver_4; 
#endif

extern void ProtocolFrame_Init(void);
//extern Protocol_Info_T Create_Protocol_Info(int8_t len,SEND_ACTION type,void (*handle)(Protocol_Info_T*),int8_t (*check)(void*)); 
//int8_t Send_To_Uart(Protocol_Info_T* protocol_info);
//int8_t Send_To_Buff(Protocol_Info_T* protocol_info);
extern void Protocol_Send(MODULE_ACTION Module_Action,void* Data,uint8_t Len);
extern void Protocol_Send_Transpond(Protocol_Info_T* pi);
extern void FetchProtocols(void);
uint16_t char_special(uint8_t num);
extern int8_t Protocol_Register(Protocol_Desc_T* Desc_T,PROTOCOL_TYPE Protocol_Type);
#endif
