#include "ComBuff.h"
#if MCU_TYPE == 103
  #include "stm32f1xx_hal.h"
#elif MCU_TYPE == 030 
  #include "stm32f0xx_hal.h"
#endif
#include "LOG.h" 
#include <stdio.h>
#include <string.h>  
#include "ProtocolFrame.h"
//--------------------------内部变量声明-----------------------------
//UART1 接收发送者定义和缓冲区空间分配
#ifdef UART1_DMA_SENDER 
	DMA_Sender_T Uart1_DMA_Sender = {0}; 
#endif 
#ifdef UART1_DMA_RECEIVER 
	DMA_Receiver_T Uart1_DMA_Receiver = {0}; 
#endif 

#ifdef UART2_DMA_SENDER 
	DMA_Sender_T Uart2_DMA_Sender = {0}; 
#endif 
#ifdef UART2_DMA_RECEIVER 
	DMA_Receiver_T Uart2_DMA_Receiver = {0}; 
#endif 
#ifdef UART3_DMA_SENDER 
	DMA_Sender_T Uart3_DMA_Sender = {0}; 
#endif 
#ifdef UART3_DMA_RECEIVER 
	DMA_Receiver_T Uart3_DMA_Receiver = {0}; 
#endif 
#ifdef UART4_DMA_SENDER 
	DMA_Sender_T Uart4_DMA_Sender = {0}; 
#endif 
#ifdef UART4_DMA_RECEIVER 
	DMA_Receiver_T Uart4_DMA_Receiver = {0}; 
#endif 
//--------------------------内部函数声明-----------------------------
static int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data);
static int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len);
static int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len); 
	
static int8_t _Write(DMA_Sender_T* uds, uint8_t *data, uint8_t len);
static int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data);
static int8_t _KeepTransmit(DMA_Sender_T* uds);

/****************************************************
	函数名:	DMA_Sender_Init
	功能:		初始化发送者
	作者:		liyao 2016年11月16日
****************************************************/
void DMA_Sender_Init(DMA_Sender_T* Uart_DMA_Sender, UART_HandleTypeDef* huart){
  Uart_DMA_Sender->DMA_Send_Queue = Queue_Link_Init(0);
  Uart_DMA_Sender->Uart = huart;
  Uart_DMA_Sender->Write = _Write;
  Uart_DMA_Sender->WriteByte = _WriteByte;
  Uart_DMA_Sender->KeepTransmit = _KeepTransmit;
}

/****************************************************
	函数名:	DMA_Receiver_Init
	功能:		初始化接收者
	作者:		liyao 2016年11月16日
****************************************************/
void DMA_Receiver_Init(DMA_Receiver_T* Uart_DMA_Receiver, UART_HandleTypeDef* huart,uint16_t BuffSize){
 
  Uart_DMA_Receiver->DMA_Recv_Queue = Queue_Link_Init(0);
  Uart_DMA_Receiver->Uart = huart;
  Uart_DMA_Receiver->Recv_Cur = 0;
  Uart_DMA_Receiver->Reversal = 0;
  Uart_DMA_Receiver->Data     = MALLOC(BuffSize);
  MALLOC_CHECK(Uart_DMA_Receiver->Data,"");
  Uart_DMA_Receiver->DMA_BuffSize = BuffSize;
  Uart_DMA_Receiver->ReadByte = _ReadByte;
  Uart_DMA_Receiver->ReadTo = _ReadTo;
  Uart_DMA_Receiver->Read = _Read;
  huart->hdmarx->Instance->CNDTR = BuffSize;
  HAL_UART_Receive_DMA(huart, Uart_DMA_Receiver->Data, BuffSize); 
}
/****************************************************
	函数名:	ComBuff_Init
	功能:		初始化全部通信缓冲区
	作者:		liyao 2016年4月4日22:02:12 
****************************************************/
void ComBuff_Init(void){ 
//UART1 初始化
#ifdef UART1_DMA_SENDER
  DMA_Sender_Init(&Uart1_DMA_Sender, &huart1);
#endif  
#ifdef UART1_DMA_RECEIVER
  DMA_Receiver_Init(&Uart1_DMA_Receiver, &huart1, UART1_DMA_RECV_SIZE); 
#endif  
#ifdef UART2_DMA_SENDER
  DMA_Sender_Init(&Uart2_DMA_Sender, &huart2);
#endif  
#ifdef UART2_DMA_RECEIVER
  DMA_Receiver_Init(&Uart2_DMA_Receiver, &huart2, UART2_DMA_RECV_SIZE); 
#endif 
#ifdef UART3_DMA_SENDER
  DMA_Sender_Init(&Uart3_DMA_Sender, &huart3);
#endif  
#ifdef UART3_DMA_RECEIVER
  DMA_Receiver_Init(&Uart3_DMA_Receiver, &huart3, UART3_DMA_RECV_SIZE); 
#endif
#ifdef UART4_DMA_SENDER
  DMA_Sender_Init(&Uart4_DMA_Sender, &huart4);
#endif  
#ifdef UART4_DMA_RECEIVER
  DMA_Receiver_Init(&Uart4_DMA_Receiver, &huart4, UART4_DMA_RECV_SIZE); 
#endif 
 }

/****************************************************
	函数名:	_Write
	功能:	通过串口DMA即刻发送一组数据
	参数:	发送者 , 回填参数
	返回值：成功0 失败-1
	作者:	liyao 2016年9月14日10:55:11
****************************************************/
static int8_t _Write(DMA_Sender_T* uds,uint8_t* data, uint8_t len){ 
  if(uds->OverFlag){//发送未完成则放入缓冲区
    Log.info("DMA BUSY加入缓存\r\n");
    Queue_Link_Put(uds->DMA_Send_Queue, data, len); 
    return 0;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
    Queue_Link_Put(uds->DMA_Send_Queue, data, len); 
    uds->Data = MALLOC(cnt); 
    MALLOC_CHECK(uds->Data,"_Write");
    Queue_Link_Get(uds->DMA_Send_Queue, uds->Data);
    uds->Len = cnt;
  }else{
    uds->Data = MALLOC(len);
    MALLOC_CHECK(uds->Data,"_Write");
    memcpy(uds->Data, data, len);
    uds->Len = len;
  }
  HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len);
  uds->OverFlag = 1; 
  return 0;
}

/****************************************************
	函数名:	_WriteByte
	功能:		通过串口即刻发送1字节
	参数:		发送者 , 回填参数
	返回值：成功0 失败-1
	作者:		liyao 2016年9月14日10:55:07
****************************************************/
static int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data){ 
	while(uds->OverFlag) ;
    uds->Data = MALLOC(1); 
    MALLOC_CHECK(uds->Data,"_Write");
    uds->Data[0] = data;
	  HAL_UART_Transmit_DMA(uds->Uart, uds->Data, 1); 
	  uds->OverFlag = 1; 
	return 0;
}


/****************************************************
	函数名:	_ReadByte
	功能:		从串口缓冲区读取1个字节
	参数:		接收者 , 回填参数
	返回值：成功0 失败-1
	作者:		liyao 2016年9月8日15:17:44
****************************************************/
static int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data){ 
	uint16_t maxCur = (udr->DMA_BuffSize -  udr->Uart->hdmarx->Instance->CNDTR);
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA缓冲区破裂\r\n");
		return -1;
	}
	if((udr->Reversal == 0 && udr->Recv_Cur < maxCur) || udr->Reversal == 1){ 
		*data = udr->Data[udr->Recv_Cur]; 
		if(++udr->Recv_Cur % udr->DMA_BuffSize == 0)
			udr->Recv_Cur = udr->Reversal = 0;
		return 0;
	}
	return -1;
}

/****************************************************
	函数名:	_ReadTo
	功能:		从串口缓冲区读取N个字节,直到读到value
	参数:		接收者,匹配值,回填数组,回填数组长度
	返回值：成功读取到的字节数 缓冲区无指定数据-1 回填数组空间不够 -2
	作者:		liyao 2016年9月8日15:17:49	
****************************************************/
static int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len){
	uint16_t i = 0,tmp_Recv_Cur = udr->Recv_Cur, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR);
	uint8_t tmp_Reversal = udr->Reversal;
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA缓冲区破裂\r\n");
		return -1;
	}
  if((udr->Recv_Cur ==  maxCur) && (0 == udr->Reversal))
    return 0;
	for(i = 0; i < len; i++){ 
		if((tmp_Reversal == 0 && tmp_Recv_Cur < maxCur) || tmp_Reversal == 1){
			data[i] = udr->Data[tmp_Recv_Cur]; 
			if(++tmp_Recv_Cur % udr->DMA_BuffSize == 0)
				tmp_Recv_Cur = tmp_Reversal = 0;
			if(data[i] == value){
				udr->Recv_Cur = tmp_Recv_Cur;
				udr->Reversal = tmp_Reversal;
				return i+1;
			}
		}else{
			//Log.waring("无指定数据\r\n");
			return -1;
		}
	}
	Log.error("回填数组空间不够或无指定数据\r\n");
	return -2;
}

/****************************************************
	函数名:	_Read
	功能:		从串口缓冲区读取N个字节
	参数:		接收者,回填数组,回填数组长度
	返回值：成功读取到的字节数 缓冲区无数据-1 
	作者:		liyao 2016年9月8日15:17:49	
****************************************************/
static int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len){
	uint16_t i = 0,cnt = 0, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR); 
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA缓冲区破裂\r\n");
		return -1;
	}
  if((udr->Recv_Cur ==  maxCur) && (0 == udr->Reversal))
    return 0;
	for(i = 0; i < len; i++){
		if((udr->Reversal == 0 && udr->Recv_Cur < maxCur) || udr->Reversal == 1){
			data[i] = udr->Data[udr->Recv_Cur]; 
			cnt++;
			if(++udr->Recv_Cur % udr->DMA_BuffSize == 0)
				udr->Recv_Cur = udr->Reversal = 0;
		}else{
			return cnt;
		}
	} 
	if(i == len)
			return cnt;
	return -1;
}

/****************************************************
	函数名:	_KeepTransmit
	功能:		当发频繁数据被加入缓存时，需要此函数触发发送缓冲区数据
	参数:		发送者
	返回值：dma未发送完返回-1，成功返回0 
  作者:		liyao 2016年11月4日14:26:35	
****************************************************/
static int8_t _KeepTransmit(DMA_Sender_T* uds){
  if(uds->OverFlag){//发送未完成则放入缓冲区 
    return -1;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
    uds->Data = MALLOC(cnt); 
    MALLOC_CHECK(uds->Data,"_Write");
    Queue_Link_Get(uds->DMA_Send_Queue, uds->Data);
    uds->Len = cnt; 
    HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len);
  } 
  return 0;
}
 
/****************************************************
	函数名:	fputc
	功能: 	printf重定向
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
int fputc(int ch,FILE *f)
{
//	#ifndef PRINT_ERR
//		Queue_Put(Uart_Tx_Queue, &ch); 
//	#else
  while(HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
	//while(HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
//	#endif
	return(ch);	   
}
 

/****************************************************
	函数名:	Buff_To_Uart
	功能:		从缓冲区中取出一个字节发送至串口
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
void Buff_To_Uart(void){
//	int32_t data;
//	uint8_t count = 5;
//	while(count--)
//		if(Queue_Get(Uart_Tx_Queue,&data) == 0){ 
//			#ifdef UART1_DMA_SENDER
//				Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,data);
//			#else
////				USART_SendData_Block(DEBUG_USART, data);  
//			#endif 
//		}else{
//			break;
//		}
}
 

//--------------------------------系统回调函数-------------------------------------

 
/****************************************************
        函数名: HAL_UART_TxCpltCallback
        功能:   DMA发送完成中断 回调
        作者:   liyao 2016年9月14日10:55:11
****************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_SENDER
    if(huart == Uart1_DMA_Sender.Uart){
        Uart1_DMA_Sender.OverFlag = 0;
        FREE(Uart1_DMA_Sender.Data);
    }
  #endif
  #ifdef UART2_DMA_SENDER
    if(huart == Uart2_DMA_Sender.Uart){
        Uart2_DMA_Sender.OverFlag = 0;
        FREE(Uart2_DMA_Sender.Data);
    }
  #endif
  #ifdef UART3_DMA_SENDER
    if(huart == Uart3_DMA_Sender.Uart){
        Uart3_DMA_Sender.OverFlag = 0;
        FREE(Uart3_DMA_Sender.Data);
    }
  #endif
  #ifdef UART4_DMA_SENDER
    if(huart == Uart4_DMA_Sender.Uart){
        Uart4_DMA_Sender.OverFlag = 0;
        FREE(Uart4_DMA_Sender.Data);
    }
  #endif    
} 


/****************************************************
        函数名: HAL_UART_RxCpltCallback
        功能:   DMA接收完成中断 回调
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
#if  UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_RECEIVER
    if(huart == Uart1_DMA_Receiver.Uart){
      if(++Uart1_DMA_Receiver.Reversal == 2)
        Log.error("USART1_DMA_接收数据被覆盖\r\n");
      HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, Uart1_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif 
  #ifdef UART2_DMA_RECEIVER
    if(huart == Uart2_DMA_Receiver.Uart){
      if(++Uart2_DMA_Receiver.Reversal == 2)
        Log.error("USART2_DMA_接收数据被覆盖\r\n");
      HAL_UART_Receive_DMA(huart, Uart2_DMA_Receiver.Data, Uart2_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif 
  #ifdef UART3_DMA_RECEIVER
    if(huart == Uart3_DMA_Receiver.Uart){
      if(++Uart3_DMA_Receiver.Reversal == 2)
        Log.error("USART3_DMA_接收数据被覆盖\r\n");
      HAL_UART_Receive_DMA(huart, Uart3_DMA_Receiver.Data, Uart3_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif  
  #ifdef UART4_DMA_RECEIVER
    if(huart == Uart4_DMA_Receiver.Uart){
      if(++Uart4_DMA_Receiver.Reversal == 2)
        Log.error("USART4_DMA_接收数据被覆盖\r\n");
      HAL_UART_Receive_DMA(huart, Uart4_DMA_Receiver.Data, Uart4_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif      
}
#endif
//--------------------------------快捷调用-------------------------------------
/****************************************************
        函数名: SenderKeepTransmit
        功能:   持续传输发送者缓冲区中的缓冲数据
        作者:   liyao 2016年9月14日10:55:11
****************************************************/   
__weak void SenderKeepTransmit(void){
#if UART1_DMA_SENDER == 1
  Uart1_DMA_Sender.KeepTransmit(&Uart1_DMA_Sender);
#endif
#if UART2_DMA_SENDER == 1
  Uart2_DMA_Sender.KeepTransmit(&Uart2_DMA_Sender);
#endif
#if UART3_DMA_SENDER == 1
  Uart3_DMA_Sender.KeepTransmit(&Uart3_DMA_Sender);
#endif
#if UART4_DMA_SENDER == 1
  Uart4_DMA_Sender.KeepTransmit(&Uart4_DMA_Sender);
#endif
}

/****************************************************
        函数名: PaddingProtocol
        功能:   从串口缓冲区中读取数据到协议解析器
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
__weak void PaddingProtocol(void){
	#define BUFFSIZE 100
	int8_t cnt = 0;
	uint8_t data[BUFFSIZE] = {0};  
	#if PROTOCOL_RESOLVER_1 && UART1_DMA_RECEIVER
    if((cnt = Uart1_DMA_Receiver.ReadTo(&Uart1_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
      ProtocolResolver_1->Protocol_Put(ProtocolResolver_1,data,cnt);
  #endif
	#if PROTOCOL_RESOLVER_2 && UART2_DMA_RECEIVER
		if((cnt = Uart2_DMA_Receiver.ReadTo(&Uart2_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_2->Protocol_Put(ProtocolResolver_2,data,cnt);  
	#endif
	#if PROTOCOL_RESOLVER_3 && UART3_DMA_RECEIVER
		if((cnt = Uart3_DMA_Receiver.ReadTo(&Uart3_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_3->Protocol_Put(ProtocolResolver_3,data,cnt);  
	#endif
	#if PROTOCOL_RESOLVER_4 && UART4_DMA_RECEIVER
		if((cnt = Uart4_DMA_Receiver.ReadTo(&Uart4_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_4->Protocol_Put(ProtocolResolver_4,data,cnt);  
	#endif
}

