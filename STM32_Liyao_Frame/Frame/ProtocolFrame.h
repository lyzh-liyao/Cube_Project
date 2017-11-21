#ifndef __PROTOCOLFRAME_H__
#define __PROTOCOLFRAME_H__
#include "FrameConfig.h"
//#include "Protocol.h" 
#include "LOG.h"
#include "Queue.h"
/*-----字符转义-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/
#define PROTOCOL_HEAD 0xFD
#define PROTOCOL_TAIL 0xF8

#define SRC_MODULE_Pos 16
#define TARGET_MODULE_Pos 8
#define ACTION_MODULE_Pos 0
#define u8 uint8_t 
#define HandleAdd(FunName)   extern void FunName(Protocol_Info_T* pi)
#define CUSTOM_MODULE  0xEE				//发送时指定模块
#define BROADCAST_MODULE 0xFF
//将特征包转换成广播特征
#define TO_BROADCAST_MODULE_ACTION(MODULE_ACTION) (MODULE_ACTION | 0x0000FF00)
//组合成uint32_t类型的特征包
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) ((SrcModule << SRC_MODULE_Pos)|(TargetModule << TARGET_MODULE_Pos)|(Action << ACTION_MODULE_Pos))
//从特征包提取信息
#define PULL_SRC_MODULE(val)					((val  >> SRC_MODULE_Pos) & 0xFF)
#define PULL_TARGET_MODULE(val)				((val >> TARGET_MODULE_Pos) & 0xFF)
#define PULL_ACTION_MODULE(val)				((val >> ACTION_MODULE_Pos) & 0xFF)
//变长协议
#define ELONGATE_SIZE 0xFF 
#define UART_BUFFSIZE 100
/*协议类型  发送   接收  转发*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
typedef enum{OK_P, LEN_ERR_P, DATALEN_ERR_P,CHECKSUM_ERR_P, TAIL_ERR_P, EQUALS_ERR_P}PROTOCOL_Status;
//---------------------------框架区------------------------------------- 
typedef struct _PROTOCOL_INFO_T Protocol_Info_T ; 
typedef struct _PROTOCOL_DESC_T Protocol_Desc_T ; 
typedef struct _PROTOCOL_RESOLVER_T Protocol_Resolver_T;

///****************************************************
//	结构体名:	Module_Action_T
//	功能: 协议模块和指令码
//	作者：liyao 2017年7月19日15:49:04
//****************************************************/
//typedef struct {
//	MODULE Src;
//	MODULE Target;
//	uint8_t Action;
//}Module_Action_T;

/****************************************************
	结构体名:	Protocol_Desc_T
	功能: 协议描述
	作者：liyao 2017年7月19日14:46:20
****************************************************/
struct _PROTOCOL_DESC_T{ 
	uint32_t ModuleAction;
	uint8_t ProtocolSize;	//参数结构体大小
	uint8_t	Serial; 			//历史协议编号(流水号)
	PROTOCOL_TYPE ProtocolType;	//协议类型
	void (*Send)(uint8_t* Data, uint8_t Len);//发送回调函数(发送协议需实现)
	void (*Transpond)(uint8_t* Data, uint8_t Len);//转发回调函数(转发协议需实现)
	void (*Handle)(Protocol_Info_T*);							//处理回调函数(接收函数需实现)
	int8_t (*Check)(void*);												//协议参数检查函数(所有协议可实现)
	void (*Ack)(Protocol_Info_T*);								//应答函数(所有协议可实现)
	
};
/****************************************************
	结构体名:	Protocol_Info_T
	功能: 协议信息描述
	作者：liyao 2017年7月19日14:42:51
****************************************************/
struct _PROTOCOL_INFO_T{
	uint8_t Head;		//帧头
	uint8_t Action;//指令码
	uint8_t  SrcModule;//源模块
	uint8_t	TargetModule;//目标模块
	uint8_t Serial;	//序号
	uint8_t DataLen;//数据长度
	void* ParameterList;//参数
	uint8_t CheckSum;//校验和 
	uint8_t Tail;		//帧尾 
	uint8_t AllLen;		//总长度
	Protocol_Desc_T* ProtocolDesc;//协议描述
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
	uint8_t*  ParaData;
	//uint8_t  Index;
	int8_t 	 Is_FE;
	uint16_t CheckSum;
	uint8_t  Recv_State;
	int8_t (*Protocol_Put)(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
	void (*Fetch_Protocol)(Protocol_Resolver_T* pr);
};



//###################################声明区################################### 
#if PROTOCOL_RESOLVER_1 || PROTOCOL_RESOLVER_IT_1
	extern Protocol_Resolver_T *ProtocolResolver_1; 
#endif
#if PROTOCOL_RESOLVER_2 || PROTOCOL_RESOLVER_IT_2
	extern Protocol_Resolver_T *ProtocolResolver_2; 
#endif
#if PROTOCOL_RESOLVER_3 || PROTOCOL_RESOLVER_IT_3
	extern Protocol_Resolver_T *ProtocolResolver_3; 
#endif
#if PROTOCOL_RESOLVER_4 || PROTOCOL_RESOLVER_IT_4
	extern Protocol_Resolver_T *ProtocolResolver_4; 
#endif
#if PROTOCOL_RESOLVER_5 || PROTOCOL_RESOLVER_IT_5
	extern Protocol_Resolver_T *ProtocolResolver_5; 
#endif


#define ProtocolFrame_IRQHandler() do{if(PaddingProtocol()) return;}while(0)
extern void ProtocolFrame_Init(void);
//extern Protocol_Info_T Create_Protocol_Info(int8_t len,SEND_ACTION type,void (*handle)(Protocol_Info_T*),int8_t (*check)(void*)); 
//int8_t Send_To_Uart(Protocol_Info_T* protocol_info);
//int8_t Send_To_Buff(Protocol_Info_T* protocol_info); 
extern void Protocol_Send(uint32_t ModuleAction,void* Data,uint8_t Len);
extern void Protocol_Send_Transpond(Protocol_Info_T* pi);
extern int8_t PaddingProtocol(void);
extern void FetchProtocols(void);
extern int8_t Protocol_Register(Protocol_Desc_T* Desc_T, PROTOCOL_TYPE Protocol_Type); 
#endif
