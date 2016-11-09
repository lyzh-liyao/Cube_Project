#include "ProtocolFrame.h" 
#include "Protocol.h"  
#include "LOG.h" 
#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PRINT_ERR
//###################################对外变量区###################################
//Protocol_Info_T protocol_send_infos[SEND_PROTOCOL_NUM] = {0};//发送协议栈
//Protocol_Info_T recv_protocol_infos[RECV_PROTOCOL_NUM] = {0};//接收协议栈 
//static uint8_t _Recv_Protocol_Arr[RECV_PROTOCOL_NUM] = {0};//已经接收到协议的索引  
//Protocol_Desc_T Send_Desc_P[SEND_PROTOCOL_NUM];//发送协议栈
//Protocol_Desc_T Recv_Desc_P[RECV_PROTOCOL_NUM];//接收协议栈 
//Protocol_Desc_T Transpond_Desc_P[TRANSPOND_PROTOCOL_NUM];//接收协议栈

List_Head_T* Send_Desc_P = NULL;
List_Head_T* Recv_Desc_P = NULL;
List_Head_T* Transpond_Desc_P = NULL;
//###################################对内变量区################################### 
 


#if UART1_PROTOCOL_RESOLVER
	#define UART1_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART1_Resolver; 
	Protocol_Resolver_T* UART1_Resolver = &_UART1_Resolver;
#endif
#if UART2_PROTOCOL_RESOLVER
	#define UART2_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART2_Resolver;
	Protocol_Resolver_T* UART2_Resolver = &_UART2_Resolver;
#endif
#if UART3_PROTOCOL_RESOLVER
	#define UART3_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART3_Resolver; 
	Protocol_Resolver_T* UART3_Resolver = &_UART3_Resolver;
#endif
#if UART4_PROTOCOL_RESOLVER
	#define UART4_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART4_Resolver;
	Protocol_Resolver_T* UART4_Resolver = &_UART4_Resolver;
#endif
//-----------------------------------------------------  
//###################################对内函数区###################################
/****************************************************
	函数名:	clean_recv_buf
	功能:		清除协议栈正在写入的协议
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
void _clean_recv_buf(Protocol_Resolver_T* pr){
	memset(&pr->pi, 0, sizeof(Protocol_Info_T)); 
	pr->Recv_State = 0;
	pr->Cnt = 0;
	pr->Index = 0;
	pr->Is_FE = 0;
	pr->CheckSum = 0;
}

/****************************************************
	函数名:	_Fetch_Protocol
	功能:		提取并执行已经缓存的协议
	作者:		liyao 2016年9月8日10:54:34
****************************************************/
void _Fetch_Protocol(Protocol_Resolver_T* pr){
	Protocol_Info_T pi;
	while(Queue_Link_Get(pr->Protocol_Queue,&pi) == 0){ 
		if(pi.Check != NULL){
			if(pi.Check(&pi) < 0){
				Log.error("协议校验不通过\r\n");
				break;
			}
		}
		if(pi.Handle != NULL){
			pi.Handle(&pi);
		}else{
			Log.error("收到协议但是无处理函数\r\n");
		} 
		FREE(pi.ParameterList);
	}
}
 
/****************************************************
	函数名:	Protocol_Put
	功能:		接收协议数据并解析封装
	参数:		协议数据
	注意: 	通过protocol_flag标志位标示是否解析出新的协议
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len){
	uint8_t i, data; 
	uint16_t src_board_action;
	List_Node_T* Cur_Node = Recv_Desc_P->Head;
	
	for(i = 0; i < len; i++){
		data = datas[i];
		if(pr->pi.Head == 0xFD && data == 0xFD){ //协议被切断抛弃
			_clean_recv_buf(pr);
			Log.error("协议中途出现0xFD\r\n");
			return -1;
		}
		if(data == 0xFE){//处理转义
			pr->Is_FE = 1;
			continue;
		}else if(pr->Is_FE){
			switch(data){
				case 0x7D: data = 0xFD;break;
				case 0x78: data = 0xF8;break;
				case 0x7E: data = 0xFE;break;
			} 
			pr->Is_FE = 0;
		}
			
		if(pr->Recv_State > 0 && pr->Recv_State < 7)//排除帧头帧尾计算校验和
			pr->CheckSum += data;
	//协议解析状态机
		switch(pr->Recv_State){
			case 0:	//处理帧头
						pr->pi.Head = data;
						pr->Recv_State++; 
						break;
			case 1:	//处理预留位
						pr->pi.Standby1 = data;
						pr->Recv_State++; 
						break;
			case 2: //处理帧长(从ID到数据位最后一个)
						pr->Recv_State++; 
						pr->pi.Plen = data;
						if(data < 4){
							_clean_recv_buf(pr);
							Log.error("处理帧长错误\r\n");
							return -2;
						}
						else
							pr->Cnt = pr->pi.ParaLen = data - 3;//计算结果为参数个数
							pr->pi.AllLen = data + 5;//计算结果为协议总长度包括FD、F8
						break;
			case 3: //处理目标板
						pr->pi.Module = data;
						pr->Recv_State++; 
						break;
			case 4: //处理编号
						pr->pi.Serial = data;
						pr->Recv_State++; 
						break;
			case 5: //处理指令码(ACTION)
						pr->pi.Action = data;
						pr->Recv_State++; 
						break;
			case 6: //处理参数 
						pr->ParaData[pr->Index++] = data;
						//((uint8_t *)(&pr->pi.ParameterList))[pr->index++] = data;  
						if(--pr->Cnt == 0)
							pr->Recv_State++;
						break;
			case 7: //处理校验和校验  
						pr->pi.CheckSum = data;
						/*校验和暂时关闭*/
						if(((uint8_t)pr->CheckSum & 0xff) != data){
              uint8_t mess[50] = {0};
              sprintf((char *)mess, "协议校验和错误:%X\r\n", pr->CheckSum);
							Log.error((char const*)mess);
							_clean_recv_buf(pr);
							return -3;
						}else{ 
							pr->Recv_State++; 
						} 
						break;
			case 8: //处理帧尾 帧类型和长度进行匹配 
						if(data != 0xF8){
							_clean_recv_buf(pr);
							Log.error("帧尾位置非0xF8错误\r\n");
							return -4;
						}
						pr->pi.Tail = data;
						src_board_action = pr->pi.Module << 8 | pr->pi.Action;

						while(Cur_Node != NULL){
							Protocol_Desc_T* pdt = Cur_Node->Data;
							if(	src_board_action ==  pdt->ModuleAction &&//目标板匹配,动作匹配 
									pr->pi.ParaLen == pdt->ProtocolSize)//帧长度匹配
							{
								pr->pi.ParameterList = MALLOC(pr->Index);
								MALLOC_CHECK(pr->pi.ParameterList, "_Protocol_Put");
								memcpy(pr->pi.ParameterList, pr->ParaData, pr->Index);
								pr->pi.Handle = pdt->Handle;
								pr->pi.Check = pdt->Check;
								pr->pi.ProtocolDesc = pdt;
								break;
							}
							Cur_Node = Cur_Node->Next;
						}
						 
						if(Cur_Node == NULL){//校验不通过 
							FREE(pr->pi.ParameterList);
							_clean_recv_buf(pr);
							Log.error("现有协议库无匹配当前协议\r\n");
							return -5;
						}else{
							Queue_Link_Put(pr->Protocol_Queue, &pr->pi, sizeof(Protocol_Info_T));//将协议信息放入协议缓冲队列  
              //UART1_Resolver->Fetch_Protocol(UART1_Resolver);
							_clean_recv_buf(pr); 
						}
						break;
		}
	}; 
	return 0;
}
/****************************************************
	函数名:	IsShift
	参数:		原字符
	功能: 	字符转义
****************************************************/
 uint16_t IsShift(uint8_t* Data){
  if(*Data == 0xFD || *Data == 0xF8 || *Data == 0xFE){//??
     switch(*Data){
        case 0xFD:return 0xFE<<8 | 0x7D;
        case 0xF8:return 0xFE<<8 | 0x78;
        case 0xFE:return 0xFE<<8 | 0x7E;
    }
  }else{
      return 0;
  }
  return 0;
}

//###################################对外函数区###################################

/****************************************************
	函数名:	getCheckSum_ByProtocolInfo
	功能:		根据协议信息获得校验和
	参数:		Protocol_Info_T协议描述信息
	返回值:	校验和结果
	注意：	只返回低8位
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
uint8_t getCheckSum_ByProtocolInfo(Protocol_Info_T* pi){
	uint8_t i, sum = 0; 
  sum += pi->Standby1;
  sum += pi->Plen;
  sum += pi->Module;
  sum += pi->Serial;
  sum += pi->Action;
  for(i = 0; i < pi->ParaLen; i++)
    sum+=((uint8_t*)pi->ParameterList)[i];  
  return sum;
}

/****************************************************
	函数名:	Protocol_Register
	功能:		协议注册
	作者:		liyao 2016年9月18日16:12:16  
****************************************************/
int8_t Protocol_Register(Protocol_Desc_T* Desc_T,PROTOCOL_TYPE Protocol_Type){
	List_Head_T* tmp_Desc_Head;
	switch((uint8_t)Protocol_Type){
		case (uint8_t)SEND: 
			tmp_Desc_Head = Send_Desc_P;
			break;
		case (uint8_t)RECEIVE: 
			tmp_Desc_Head = Recv_Desc_P;
			break;
		case (uint8_t)TRANSPOND: 
			tmp_Desc_Head = Transpond_Desc_P;
			break;  
	}
	
	Desc_T->SrcModule = (MODULE)(Desc_T->ModuleAction >> 12);
	Desc_T->TargetModule = (MODULE)(Desc_T->ModuleAction & 0x0f);
	List_Add(tmp_Desc_Head, Desc_T, sizeof(Protocol_Desc_T)); 
	return -1;
	
}

/****************************************************
	函数名:	Get_Protocol_Description
	功能:		获取协议描述信息
	作者:		liyao 2016年9月18日16:12:16  
****************************************************/
Protocol_Desc_T* Get_Protocol_Description(MODULE_ACTION ModuleAction,PROTOCOL_TYPE Protocol_Type){
	List_Head_T* tmp_Desc_Head;
	List_Node_T* Cur_Node = NULL;
	switch((uint8_t)Protocol_Type){
		case (uint8_t)SEND: 
			tmp_Desc_Head = Send_Desc_P;
			break;
		case (uint8_t)RECEIVE: 
			tmp_Desc_Head = Recv_Desc_P;
			break;
		case (uint8_t)TRANSPOND: 
			tmp_Desc_Head = Transpond_Desc_P;
			break;  
	}
	
	Cur_Node = tmp_Desc_Head->Head;
	while(Cur_Node != NULL){
		Protocol_Desc_T* pdt = Cur_Node->Data;
		if(ModuleAction == pdt->ModuleAction)
			return pdt;
		Cur_Node = Cur_Node->Next;
	} 
	Log.error("Get_Protocol_Description未找到描述结构体");
	return NULL;
	
}

/****************************************************
	函数名:	ProtocolFrame_Init
	功能:		初始化全部协议和相关校验、执行函数
	作者:		liyao 2015年9月8日14:10:51      
****************************************************/
void ProtocolFrame_Init(){ 
	Send_Desc_P = List_Init(0);
	Recv_Desc_P = List_Init(0);
	Transpond_Desc_P = List_Init(0);
	//协议列表初始化
	Protocol_Init();
	//发送数据队列初始化 
#if UART1_PROTOCOL_RESOLVER
//	UART1_Resolver->Protocol_Queue = Queue_Init( _UART1_Protocol_QueueBuf,sizeof(Protocol_Info_T), UART1_RPQUEUE_SIZE);
	UART1_Resolver->Protocol_Queue = Queue_Link_Init(UART1_RPQUEUE_SIZE); 
	UART1_Resolver->Protocol_Put = _Protocol_Put;
	UART1_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif  

#if UART2_PROTOCOL_RESOLVER
//	UART2_Resolver->Protocol_Queue = Queue_Init( _UART2_Protocol_QueueBuf,sizeof(Protocol_Info_T), UART2_RPQUEUE_SIZE);
	UART2_Resolver->Protocol_Queue = Queue_Link_Init(UART2_RPQUEUE_SIZE);
	UART2_Resolver->RPQueue_Size = UART2_RPQUEUE_SIZE; 
	UART2_Resolver->Protocol_Put = _Protocol_Put;
	UART2_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif  
	
#if UART3_PROTOCOL_RESOLVER
//	UART3_Resolver->Protocol_Queue = Queue_Init( _UART3_Protocol_QueueBuf,sizeof(Protocol_Info_T), UART3_RPQUEUE_SIZE);
	UART3_Resolver->Protocol_Queue = Queue_Link_Init(UART3_RPQUEUE_SIZE);
	UART3_Resolver->RPQueue_Size = UART3_RPQUEUE_SIZE; 
	UART3_Resolver->Protocol_Put = _Protocol_Put;
	UART3_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif 
	
#if UART4_PROTOCOL_RESOLVER
//	UART4_Resolver->Protocol_Queue = Queue_Init( _UART4_Protocol_QueueBuf,sizeof(Protocol_Info_T), UART4_RPQUEUE_SIZE);
	UART4_Resolver->Protocol_Queue = Queue_Link_Init(UART4_RPQUEUE_SIZE);
	UART4_Resolver->RPQueue_Size = UART4_RPQUEUE_SIZE; 
	UART4_Resolver->Protocol_Put = _Protocol_Put;
	UART4_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif 
	
}

/****************************************************
	函数名:	Protocol_To_Uart
	功能:		向缓冲区写入待发送至串口
	参数:		Protocol_Info_T协议描述信息
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
int8_t Protocol_To_Uart(Protocol_Info_T* pi){ 
	uint8_t data[PROTOCOL_SINGLE_BUFF] = {0},i = 0, index = 0; 
  uint16_t tmpData = 0; 
  
  tmpData = pi->Head;
  data[index++] = tmpData; 
  if((tmpData = IsShift(&pi->Standby1)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Standby1;
  }
  if((tmpData = IsShift(&pi->Plen)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Plen;
  }
  if((tmpData = IsShift(&pi->Module)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Module;
  }
  if((tmpData = IsShift(&pi->Serial)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Serial;
  }  
  if((tmpData = IsShift(&pi->Action)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Action;
  }    
  
  for(i = 0; i < pi->ParaLen; i++){
    if((tmpData = IsShift((uint8_t*)pi->ParameterList + i)) > 0){
      data[index++] = tmpData>>8;
      data[index++] = tmpData&0xff;
    }else{
      data[index++] = ((uint8_t*)pi->ParameterList)[i];
    }
  }
  if((tmpData = IsShift(&pi->CheckSum)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->CheckSum;
  }  
  data[index++] = pi->Tail; 
  
  pi->ProtocolDesc->Send(data, index);
	return 0;
}

//###################################自定义扩展函数区###################################
/****************************************************
	函数名:	Protocol_Send
	功能:		发送协议
	参数:		目标板,参数结构体,参数结构体长度
	注：协议历史编号统一处理
	作者:		liyao 2016年9月18日11:50:55
****************************************************/
void Protocol_Send(MODULE_ACTION ModuleAction, void* Data,uint8_t Len){ 
	Protocol_Info_T pi = {0};
	pi.ParameterList = MALLOC(Len);
	MALLOC_CHECK(pi.ParameterList, "Protocol_Send"); 
	pi.ProtocolDesc = Get_Protocol_Description(ModuleAction, SEND);
	pi.Head = 0xFD;
	pi.Plen = Len + 3;//参数个数+3   帧长度
	pi.Module = ModuleAction >> 8; 
	pi.Serial = pi.ProtocolDesc->Serial++;
	pi.Action = ModuleAction & 0x00ff;
//	pi.ParameterList = *Protocol_t; 
	memcpy(pi.ParameterList, Data, Len);
	pi.CheckSum = getCheckSum_ByProtocolInfo(&pi); 
	pi.Tail = 0xF8;
	pi.ParaLen = Len;
	pi.AllLen = pi.Plen + 5;
	Protocol_To_Uart(&pi);	
	FREE(pi.ParameterList);
}

/****************************************************
	函数名:	Protocol_Send_Transpond
	功能:		转发协议
	参数:		Protocol_Info_T结构体
	作者:		liyao 2016年9月18日11:51:35
****************************************************/
void Protocol_Send_Transpond(Protocol_Info_T* pi){  
	Protocol_To_Uart(pi);	
}

/*****************************************************************
函数名:FetchProtocols
备注: 处理上位机串口命令总函数
******************************************************************/
void FetchProtocols(void)
{
	#if UART1_PROTOCOL_RESOLVER
		UART1_Resolver->Fetch_Protocol(UART1_Resolver);
	#endif
	#if UART2_PROTOCOL_RESOLVER
		UART2_Resolver->Fetch_Protocol(UART2_Resolver);
	#endif
	#if UART3_PROTOCOL_RESOLVER
		UART3_Resolver->Fetch_Protocol(UART3_Resolver);
	#endif
	#if UART4_PROTOCOL_RESOLVER
		UART4_Resolver->Fetch_Protocol(UART4_Resolver);
	#endif
}
