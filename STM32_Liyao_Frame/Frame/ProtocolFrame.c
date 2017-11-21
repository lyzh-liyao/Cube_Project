#include "ProtocolFrame.h" 
#include "Protocol.h"  
#include "FrameConfig.h"
#include "LOG.h" 
#include "List.h"
#include <string.h>
#include "TaskTimeManager.h"
#include "ComBuff.h"
static int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
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
 


#if PROTOCOL_RESOLVER_1	|| PROTOCOL_RESOLVER_IT_1
	#define RESOLVER_1_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _ProtocolResolver_1; 
	Protocol_Resolver_T* ProtocolResolver_1 = &_ProtocolResolver_1;
#endif
#if PROTOCOL_RESOLVER_2 || PROTOCOL_RESOLVER_IT_2
	#define RESOLVER_2_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _ProtocolResolver_2;
	Protocol_Resolver_T* ProtocolResolver_2 = &_ProtocolResolver_2;
#endif
#if PROTOCOL_RESOLVER_3 || PROTOCOL_RESOLVER_IT_3
	#define RESOLVER_3_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _ProtocolResolver_3; 
	Protocol_Resolver_T* ProtocolResolver_3 = &_ProtocolResolver_3;
#endif
#if PROTOCOL_RESOLVER_4 || PROTOCOL_RESOLVER_IT_4
	#define RESOLVER_4_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _ProtocolResolver_4;
	Protocol_Resolver_T* ProtocolResolver_4 = &_ProtocolResolver_4;
#endif
#if PROTOCOL_RESOLVER_5 || PROTOCOL_RESOLVER_IT_5
	#define RESOLVER_5_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _ProtocolResolver_5;
	Protocol_Resolver_T* ProtocolResolver_5 = &_ProtocolResolver_5;
#endif
//-----------------------------------------------------  
//###################################对内函数区###################################
/****************************************************
	函数名:	clean_recv_buf
	功能:		清除协议栈正在写入的协议
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
static void _clean_recv_buf(Protocol_Resolver_T* pr){
	memset(&pr->pi, 0, sizeof(Protocol_Info_T)); 
	FREE(pr->ParaData);
	pr->Recv_State = 0;
	pr->Cnt = 0;
//	pr->Index = 0;
	pr->Is_FE = 0;
	pr->CheckSum = 0;
}

/****************************************************
	函数名:	_Fetch_Protocol
	功能:		提取并执行已经缓存的协议
	作者:		liyao 2016年9月8日10:54:34
****************************************************/
static void _Fetch_Protocol(Protocol_Resolver_T* pr){
	Protocol_Info_T pi;
	while(Queue_Link_Pop(pr->Protocol_Queue,&pi) == 0){ 
		if(pi.ProtocolDesc->Check != NULL){		//判断并调用检查函数
			if(pi.ProtocolDesc->Check(&pi) < 0){
				Log.waring("协议校验不通过\r\n");
				break;
			}
		}
		if(pi.ProtocolDesc->Ack != NULL)	//判断并调用应答函数
			pi.ProtocolDesc->Ack(&pi);
		
		if(pi.ProtocolDesc->ProtocolType == RECEIVE){	//判断并调用处理函数
			if(pi.ProtocolDesc->Handle != NULL){
				pi.ProtocolDesc->Handle(&pi);
			}else{
				Log.waring("收到协议但是无处理函数\r\n");
			}
		}else if(pi.ProtocolDesc->ProtocolType == TRANSPOND){//判断并调用转发函数
			if(pi.ProtocolDesc->Transpond != NULL){
				Protocol_Send_Transpond(&pi);
			}else{
				Log.waring("收到转发协议但是无转发函数\r\n");
			}
		}
		FREE(pi.ParameterList);
	}
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
static uint8_t getCheckSum_ByProtocolInfo(Protocol_Info_T* pi){
	uint8_t sum = 0; 
  sum += pi->Action;
  sum += pi->SrcModule;
  sum += pi->TargetModule;
  sum += pi->Serial;
	sum += pi->DataLen;
  for(uint8_t i = 0; i < pi->DataLen; i++)
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
			tmp_Desc_Head = Send_Desc_P; 	//发送协议库列表头
			break;
		case (uint8_t)RECEIVE: 
			tmp_Desc_Head = Recv_Desc_P;	//接收协议库列表头
			break;
		case (uint8_t)TRANSPOND: 
			tmp_Desc_Head = Transpond_Desc_P;	//转发协议库列表头
			break;  
	}
	Desc_T->ProtocolType = Protocol_Type;
	List_Add(tmp_Desc_Head, Desc_T, sizeof(Protocol_Desc_T)); 
	return -1;
}

/****************************************************
	函数名:	Get_Protocol_Description
	功能:		获取协议描述信息
	作者:		liyao 2016年9月18日16:12:16  
****************************************************/
Protocol_Desc_T* Get_Protocol_Description(uint32_t ModuleAction, PROTOCOL_TYPE Protocol_Type){
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
		//if(pdt->SrcModule == ModuleAction->Src && pdt->TargetModule == ModuleAction->Target && pdt->Action == ModuleAction->Action)
		if(pdt->ModuleAction == ModuleAction)
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
#if PROTOCOL_RESOLVER_1 || PROTOCOL_RESOLVER_IT_1
//	ProtocolResolver_1->Protocol_Queue = Queue_Init( _UART1_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_1_RPQUEUE_SIZE);
	ProtocolResolver_1->Protocol_Queue = Queue_Link_Init(RESOLVER_1_RPQUEUE_SIZE); 
	ProtocolResolver_1->Protocol_Put = _Protocol_Put;
	ProtocolResolver_1->Fetch_Protocol = _Fetch_Protocol;
	
	WRITE_REG(USART1->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART1->CR1, USART_ISR_IDLE);
	#if PROTOCOL_RESOLVER_IT_1
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	#endif
#endif  

#if PROTOCOL_RESOLVER_2 || PROTOCOL_RESOLVER_IT_2
//	ProtocolResolver_2->Protocol_Queue = Queue_Init( _UART2_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_2_RPQUEUE_SIZE);
	ProtocolResolver_2->Protocol_Queue = Queue_Link_Init(RESOLVER_2_RPQUEUE_SIZE); 
	ProtocolResolver_2->Protocol_Put = _Protocol_Put;
	ProtocolResolver_2->Fetch_Protocol = _Fetch_Protocol;
		
	WRITE_REG(USART2->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART2->CR1, USART_ISR_IDLE);
	#if PROTOCOL_RESOLVER_IT_2
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	#endif
#endif  
	
#if PROTOCOL_RESOLVER_3 || PROTOCOL_RESOLVER_IT_3
//	ProtocolResolver_3->Protocol_Queue = Queue_Init( _UART3_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_3_RPQUEUE_SIZE);
	ProtocolResolver_3->Protocol_Queue = Queue_Link_Init(RESOLVER_3_RPQUEUE_SIZE); 
	ProtocolResolver_3->Protocol_Put = _Protocol_Put;
	ProtocolResolver_3->Fetch_Protocol = _Fetch_Protocol;
			
	WRITE_REG(USART3->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART3->CR1, USART_ISR_IDLE);
	#if PROTOCOL_RESOLVER_IT_3
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
	#endif
#endif 
	
#if PROTOCOL_RESOLVER_4 || PROTOCOL_RESOLVER_IT_4
//	ProtocolResolver_4->Protocol_Queue = Queue_Init( _UART4_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_4_RPQUEUE_SIZE);
	ProtocolResolver_4->Protocol_Queue = Queue_Link_Init(RESOLVER_4_RPQUEUE_SIZE); 
	ProtocolResolver_4->Protocol_Put = _Protocol_Put;
	ProtocolResolver_4->Fetch_Protocol = _Fetch_Protocol;
	WRITE_REG(USART4->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART4->CR1, USART_ISR_IDLE);
	#if PROTOCOL_RESOLVER_IT_4
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
	#endif	
#endif
	
#if PROTOCOL_RESOLVER_5 || PROTOCOL_RESOLVER_IT_5
//	ProtocolResolver_5->Protocol_Queue = Queue_Init( _UART5_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_5_RPQUEUE_SIZE);
	ProtocolResolver_5->Protocol_Queue = Queue_Link_Init(RESOLVER_5_RPQUEUE_SIZE); 
	ProtocolResolver_5->Protocol_Put = _Protocol_Put;
	ProtocolResolver_5->Fetch_Protocol = _Fetch_Protocol;
	WRITE_REG(USART5->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART5->CR1, USART_ISR_IDLE);
	#if PROTOCOL_RESOLVER_IT_5
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
	#endif	
#endif

#ifdef __TASKTIMEMANAGER_H__
//	/*-----------协议执行任务-----------------*/
//	TaskTime_Add(TaskID++, TimeCycle(0,10), FetchProtocols, Real_Mode);
#endif
}


/****************************************************
	函数名:	IsShift
	参数:		原字符
	功能: 	字符转义
****************************************************/
static void IsShift(uint8_t Data, uint8_t* Buff, uint8_t* len){
  if(Data == 0xFD || Data == 0xF8 || Data == 0xFE){//??
		 Buff[(*len)++] = 0xFE;
     switch(Data){
        case 0xFD:
					Buff[(*len)++] = 0x7D;break;
        case 0xF8:
					Buff[(*len)++] = 0x78;break;
        case 0xFE:
					Buff[(*len)++] = 0x7E;break;
    }
  }else{
      Buff[(*len)++] = Data;
  }
}

/****************************************************
  函数名:  Protocol_Serialization
  功能:    将Protocol_Info对象序列化为数组
  参数:    Protocol_Info_T协议描述信息, 回填数组, 数组大小
	作者:    liyao 2016年12月14日14:22:44
****************************************************/
int8_t Protocol_Serialization(Protocol_Info_T* pi, uint8_t* data, uint8_t len){ 
	uint8_t index = 0;
	data[index++] = pi->Head;
  IsShift(pi->Action, data, &index);
	IsShift(pi->SrcModule, data, &index);
	IsShift(pi->TargetModule, data, &index);
	IsShift(pi->Serial, data, &index);
	IsShift(pi->DataLen, data, &index);
	for(uint8_t i = 0; i < pi->DataLen; i++){//处理参数转义
		IsShift(((uint8_t*)(pi->ParameterList))[i], data, &index);
  }
	IsShift(pi->CheckSum, data, &index);
  data[index++] = pi->Tail; 
  if(index > len)
    return -1; 
  return index;
}


//###################################自定义扩展函数区###################################
/****************************************************
	函数名:	Protocol_Send
	功能:		发送协议
	参数:		目标板,参数结构体,参数结构体长度
	注：协议历史编号统一处理
	作者:		liyao 2016年9月18日11:50:55
****************************************************/
void Protocol_Send(uint32_t ModuleAction, void* Data,uint8_t Len){ 
	Protocol_Info_T pi = {0};
	int8_t cnt = 0;
	pi.ParameterList = MALLOC(Len);
	MALLOC_CHECK(pi.ParameterList, "Protocol_Send"); 
	pi.ProtocolDesc = Get_Protocol_Description(ModuleAction, SEND);
	if(pi.ProtocolDesc != NULL && pi.ProtocolDesc->ProtocolSize != Len)
		Log.waring("协议数据长度不匹配\r\n");
	else if(pi.ProtocolDesc == NULL){
		Log.waring("未匹配到发送协议\r\n");
		return;
	}
	pi.DataLen = Len;
	pi.Head = PROTOCOL_HEAD;
	pi.AllLen = Len + 8;//参数个数+3   帧长度
	pi.SrcModule = (ModuleAction >> SRC_MODULE_Pos) & 0xFF;
	pi.TargetModule = (ModuleAction >> TARGET_MODULE_Pos) & 0xFF;
	pi.Action = (ModuleAction >> ACTION_MODULE_Pos) & 0xFF;
	pi.Serial = pi.ProtocolDesc->Serial++;
	memcpy(pi.ParameterList, Data, Len);
	pi.CheckSum = getCheckSum_ByProtocolInfo(&pi); 
	pi.Tail = PROTOCOL_TAIL;
	
	uint8_t* data = MALLOC(pi.AllLen * 2);
	MALLOC_CHECK(data, "Protocol_Send"); 
	if(data == NULL)
		return;
	if((cnt = Protocol_Serialization(&pi, data, pi.AllLen * 2)) == -1)
		Log.error("Protocol_Send序列化失败\r\n");
	pi.ProtocolDesc->Send(data, cnt);
	FREE(pi.ParameterList);
	FREE(data);
}

/****************************************************
	函数名:	Protocol_Send_Transpond
	功能:		转发协议
	参数:		Protocol_Info_T结构体
	作者:		liyao 2016年9月18日11:51:35
****************************************************/
void Protocol_Send_Transpond(Protocol_Info_T* pi){  
	uint8_t* data = MALLOC(pi->AllLen * 2);
	int8_t cnt = 0;
	MALLOC_CHECK(data, "Protocol_Send_Transpond"); 
	if((cnt = Protocol_Serialization(pi, data, pi->AllLen * 2)) == -1)
		Log.error("Protocol_Send序列化失败\r\n"); 
	pi->ProtocolDesc->Transpond(data, cnt);
	FREE(data);
}

/*****************************************************************
函数名:FetchProtocols
备注: 处理上位机串口命令总函数
******************************************************************/
void FetchProtocols(void)
{
	#if PROTOCOL_RESOLVER_1 || PROTOCOL_RESOLVER_IT_1
		ProtocolResolver_1->Fetch_Protocol(ProtocolResolver_1);
	#endif
	#if PROTOCOL_RESOLVER_2 || PROTOCOL_RESOLVER_IT_2
		ProtocolResolver_2->Fetch_Protocol(ProtocolResolver_2);
	#endif
	#if PROTOCOL_RESOLVER_3 || PROTOCOL_RESOLVER_IT_3
		ProtocolResolver_3->Fetch_Protocol(ProtocolResolver_3);
	#endif
	#if PROTOCOL_RESOLVER_4 || PROTOCOL_RESOLVER_IT_4
		ProtocolResolver_4->Fetch_Protocol(ProtocolResolver_4);
	#endif
	#if PROTOCOL_RESOLVER_5 || PROTOCOL_RESOLVER_IT_5
		ProtocolResolver_5->Fetch_Protocol(ProtocolResolver_5);
	#endif
}



/****************************************************
        函数名: PaddingProtocol
        功能:   从串口缓冲区中读取数据到协议解析器
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
int8_t PaddingProtocol(void){
	#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER || UART5_DMA_RECEIVER
	int8_t cnt = 1;
	static uint8_t UartData[SINGLE_BUFFSIZE] = {0};
	#endif 
	#if PROTOCOL_RESOLVER_1  && UART1_DMA_RECEIVER		//串口DMA + 协议解析
		if((READ_REG(USART1->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART1->ICR, UART_CLEAR_IDLEF);
			while((cnt = Uart1_DMA_Receiver.ReadTo(&Uart1_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
				ProtocolResolver_1->Protocol_Put(ProtocolResolver_1, UartData, cnt);
			return 1;
		}
  #elif PROTOCOL_RESOLVER_IT_1											//串口IT + 协议解析
		static uint8_t UartBuff1[UART_BUFFSIZE];
		static uint8_t Index1 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART1->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART1->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART1->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART1->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART1->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART1->DR);	
		#endif
			UartBuff1[Index1++%UART_BUFFSIZE] = SUartData;
			return 1;
		}
		if((READ_REG(USART1->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART1->ICR, UART_CLEAR_IDLEF);
			if(Index1 > UART_BUFFSIZE){
				Index1 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			ProtocolResolver_1->Protocol_Put(ProtocolResolver_1, UartBuff1 , Index1);
			Index1 = 0;
			return 1;
		}
	#endif
	
		
	#if PROTOCOL_RESOLVER_2 && UART2_DMA_RECEIVER
		if((READ_REG(USART2->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART2->ICR, UART_CLEAR_IDLEF);
			while((cnt = Uart2_DMA_Receiver.ReadTo(&Uart2_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
				ProtocolResolver_2->Protocol_Put(ProtocolResolver_2, UartData, cnt);
			return 1;
		}
	#elif PROTOCOL_RESOLVER_IT_2
		static uint8_t UartBuff2[UART_BUFFSIZE];
		static uint8_t Index2 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART2->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART2->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART2->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART2->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART2->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART2->DR);
		#endif
			UartBuff2[Index2++%UART_BUFFSIZE] = SUartData;
			return 1;
		}
		if((READ_REG(USART2->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART2->ICR, UART_CLEAR_IDLEF);
			if(Index2 > UART_BUFFSIZE){
				Index2 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			ProtocolResolver_2->Protocol_Put(ProtocolResolver_2, UartBuff2 , Index2);
			Index2 = 0;
			return 1;
		}
	#endif
	#if PROTOCOL_RESOLVER_3 && UART3_DMA_RECEIVER
		if((READ_REG(USART3->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART3->ICR, UART_CLEAR_IDLEF);
			while((cnt = Uart3_DMA_Receiver.ReadTo(&Uart3_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
				ProtocolResolver_3->Protocol_Put(ProtocolResolver_3, UartData, cnt);  
			return 1;
		}
	#elif PROTOCOL_RESOLVER_IT_3
	static uint8_t UartBuff3[UART_BUFFSIZE];
	static uint8_t Index3 = 0;
	#if MCU_TYPE == 030
	if(((READ_REG(USART3->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART3->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t SUartData =  READ_REG(USART3->RDR);
	#elif MCU_TYPE == 103
	if(((READ_REG(USART3->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART3->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t SUartData =  READ_REG(USART3->DR);
	#endif
		UartBuff3[Index3++%UART_BUFFSIZE] = SUartData;
		return 1;
	}else if((READ_REG(USART3->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART3->ICR, UART_CLEAR_IDLEF);
			if(Index3 > UART_BUFFSIZE){
				Index3 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			ProtocolResolver_3->Protocol_Put(ProtocolResolver_3, UartBuff3 , Index3);
			Index3 = 0;
			return 1;
	}
	#endif
	#if PROTOCOL_RESOLVER_4 && UART4_DMA_RECEIVER
		if((READ_REG(USART4->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART4->ICR, UART_CLEAR_IDLEF);
			while((cnt = Uart4_DMA_Receiver.ReadTo(&Uart4_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
				ProtocolResolver_4->Protocol_Put(ProtocolResolver_4, UartData, cnt);
			return 1;
		}
	#elif PROTOCOL_RESOLVER_IT_4
		static uint8_t UartBuff4[UART_BUFFSIZE];
		static uint8_t Index4 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART4->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART4->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART4->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART4->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART4->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART4->DR);
		#endif
			UartBuff4[Index4++%UART_BUFFSIZE] = SUartData;
			return 1;
		}
		if((READ_REG(USART4->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART4->ICR, UART_CLEAR_IDLEF);
			if(Index4 > UART_BUFFSIZE){
				Index4 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			ProtocolResolver_4->Protocol_Put(ProtocolResolver_4, UartBuff4 , Index4);
			Index4 = 0;
			return 1;
		}
	#endif
	#if PROTOCOL_RESOLVER_5 && UART5_DMA_RECEIVER
		if((READ_REG(USART5->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART5->ICR, UART_CLEAR_IDLEF);
			while((cnt = Uart5_DMA_Receiver.ReadTo(&Uart5_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
				ProtocolResolver_5->Protocol_Put(ProtocolResolver_5, UartData, cnt);
			return 1;
		}
	#elif PROTOCOL_RESOLVER_IT_5
		static uint8_t UartBuff5[UART_BUFFSIZE];
		static uint8_t Index5 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART5->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART5->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART5->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART5->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART5->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART5->DR);
		#endif
			UartBuff5[Index5++%UART_BUFFSIZE] = SUartData;
			return 1;
		}else if((READ_REG(USART5->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART5->ICR, UART_CLEAR_IDLEF);
			if(Index5 > UART_BUFFSIZE){
				Index5 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			ProtocolResolver_5->Protocol_Put(ProtocolResolver_5, UartBuff5 , Index5);
			Index5 = 0;
			return 1;
		}
	#endif 
		
		
	#if UART1_DMA_SENDER || UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER || UART5_DMA_SENDER
		if(((READ_REG(USART1->ISR)| READ_REG(USART2->ISR)|READ_REG(USART3->ISR)|READ_REG(USART4->ISR)| READ_REG(USART5->ISR)) & USART_ISR_TC) != RESET){
			TaskTime_Add(TaskID++, TimeCycle(0, 0), SenderKeepTransmit, Single_Mode);
		}
	#endif
		return 0;
}
		
//帧头	帧类型	源模块	目标模块	历史数据编号	数据长度	数据	校验和	帧尾
static int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len){
	uint8_t data = 0; 
	for(uint8_t i = 0; i < len; i++){
		data = datas[i];
#ifdef COMBUFF_OUT
		printf("%.2X", data);
#endif
    if(pr->pi.Head != PROTOCOL_HEAD && data != PROTOCOL_HEAD)
      continue;
		if(pr->pi.Head == PROTOCOL_HEAD && data == PROTOCOL_HEAD){ //协议被切断抛弃
			_clean_recv_buf(pr);
			Log.error("协议中途出现0xFD\r\n");
			pr->pi.Head = PROTOCOL_HEAD;
			pr->Recv_State++; 
			continue;
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
			case 1:	//处理指令码
						pr->pi.Action = data;
						pr->Recv_State++; 
						break;
			case 2: //处理源模块
						pr->pi.SrcModule = data;
						pr->Recv_State++; 
						break;
			case 3: //处理目标板
						pr->pi.TargetModule = data;
						pr->Recv_State++; 
						break;
			case 4: //处理编号
						pr->pi.Serial = data;
						pr->Recv_State++; 
						break;
			case 5: //处理数据长度
						pr->Recv_State++; 
						pr->pi.DataLen = pr->Cnt = data;
						pr->pi.AllLen = data + 8;
//						if(pr->pi.AllLen > len){
//							Log.error("数据长度错误:%X\r\n", data);
//							_clean_recv_buf(pr);
//							return DATALEN_ERR_P;
//						}
						if(data == 0)
							pr->Recv_State++;
						pr->ParaData = MALLOC(data);//申请存放参数的空间
						MALLOC_CHECK(pr->ParaData,"");
						break;
			case 6: //处理参数 
						pr->ParaData[pr->pi.DataLen - pr->Cnt--] = data;
							if(pr->Cnt == 0){
								pr->Recv_State++;
								break;
						}
						break;
			case 7: //处理校验和校验  
						pr->pi.CheckSum = data;
            #ifdef PROTOCOL_CHECKSUM
              pr->Recv_State++; 
              break;
            #else
						/*校验和暂时关闭*/
						if(((uint8_t)pr->CheckSum & 0xFF) != data){
							Log.error("协议校验和错误:%X\r\n", pr->CheckSum);
							_clean_recv_buf(pr);
							return CHECKSUM_ERR_P;
						}else{ 
							pr->Recv_State++; 
						} 
						break;
						#endif
			case 8: //处理帧尾 帧类型和长度进行匹配 
						if(data != PROTOCOL_TAIL){
							_clean_recv_buf(pr);
							Log.error("帧尾位置非0xF8错误\r\n");
							return TAIL_ERR_P;
						}
						pr->pi.Tail = data;
						
						List_Head_T* List[2] = {Recv_Desc_P, Transpond_Desc_P};//循环匹配 接收协议库 和 转发协议库
						List_Node_T* Cur_Node = NULL;
						for(int i = 0; i < 2; i++){
							Cur_Node = List[i]->Head;
							while(Cur_Node != NULL){
								Protocol_Desc_T* pdt = Cur_Node->Data;
								uint32_t tmpModuleAction = TO_MODULE_ACTION(pr->pi.SrcModule, pr->pi.TargetModule, pr->pi.Action);
								if( (tmpModuleAction == pdt->ModuleAction || tmpModuleAction == TO_BROADCAST_MODULE_ACTION(pdt->ModuleAction))//目标模块匹配或者广播匹配
									&& (pr->pi.DataLen == pdt->ProtocolSize || pdt->ProtocolSize == ELONGATE_SIZE))//帧长度匹配 
								{
	//								printf("%X, %X, %X, %X, %X, ",tmpModuleAction, pdt->ModuleAction, );
									pr->pi.ParameterList = MALLOC(pr->pi.DataLen);
									MALLOC_CHECK(pr->pi.ParameterList, "_Protocol_Put");
									memcpy(pr->pi.ParameterList, pr->ParaData, pr->pi.DataLen);
									pr->pi.ProtocolDesc = pdt;
									break;
								}
								Cur_Node = Cur_Node->Next;
							}
							if(Cur_Node != NULL)
								break;
						}
						 
						if(Cur_Node == NULL){//校验不通过 
							FREE(pr->pi.ParameterList);
							_clean_recv_buf(pr);
							Log.error("现有协议库无匹配当前协议\r\n");
							return EQUALS_ERR_P;
						}else{
							if(Queue_Link_Push(pr->Protocol_Queue, &pr->pi, sizeof(Protocol_Info_T)) < 0)//将协议信息放入协议缓冲队列  
								FREE(pr->pi.ParameterList); 
              //ProtocolResolver_1->Fetch_Protocol(ProtocolResolver_1);
							_clean_recv_buf(pr); 
							/*-----------协议执行任务-----------------*/
							TaskTime_Add(TaskID++, TimeCycle(0,0), FetchProtocols, Single_Mode);
						}
						break;
		}
	}; 
	return 0;
}

