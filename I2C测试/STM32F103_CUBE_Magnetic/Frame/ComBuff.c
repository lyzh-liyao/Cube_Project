#include "ComBuff.h"
#include "stm32f1xx_hal.h"
#include "LOG.h" 
#include <stdio.h>
#include <string.h>  
//--------------------------内部变量声明-----------------------------
//UART1 接收发送者定义和缓冲区空间分配
#ifdef UART1_DMA_SENDER 
	DMA_Sender_T Uart1_DMA_Sender = {0}; 
#endif 
#ifdef UART1_DMA_RECEIVER 
	DMA_Receiver_T Uart1_DMA_Receiver = {0}; 
#endif 
 
//--------------------------内部函数声明-----------------------------
int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data);
int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len);
int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len); 
	
int8_t _Write(DMA_Sender_T* uds, uint8_t *data, uint8_t len);
int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data);
int8_t _KeepTransmit(DMA_Sender_T* uds);
/****************************************************
	函数名:	ComBuff_Init
	功能:		初始化全部通信缓冲区
	作者:		liyao 2016年4月4日22:02:12 
****************************************************/
void ComBuff_Init(void){ 
//UART1 初始化
#ifdef UART1_DMA_SENDER
  Uart1_DMA_Sender.DMA_Send_Queue = Queue_Link_Init(0);
  Uart1_DMA_Sender.Uart = &huart1;
  Uart1_DMA_Sender.Dma = huart1.hdmatx;
  Uart1_DMA_Sender.Write = _Write;
  Uart1_DMA_Sender.WriteByte = _WriteByte;
  Uart1_DMA_Sender.KeepTransmit = _KeepTransmit;
#endif 

#ifdef UART1_DMA_RECEIVER
  Uart1_DMA_Receiver.DMA_Recv_Queue = Queue_Link_Init(0);
  Uart1_DMA_Receiver.Uart = &huart1;
  Uart1_DMA_Receiver.Dma = huart1.hdmarx;
  Uart1_DMA_Receiver.Recv_Cur = 0;
  Uart1_DMA_Receiver.Reversal = 0;
  Uart1_DMA_Receiver.Data     = MALLOC(UART1_DMA_RECV_SIZE);
  Uart1_DMA_Receiver.DMA_BuffSize = UART1_DMA_RECV_SIZE;
  Uart1_DMA_Receiver.ReadByte = _ReadByte;
  Uart1_DMA_Receiver.ReadTo = _ReadTo;
  Uart1_DMA_Receiver.Read = _Read;
#endif  
}

/****************************************************
	函数名:	_Write
	功能:	通过串口DMA即刻发送一组数据
	参数:	发送者 , 回填参数
	返回值：成功0 失败-1
	作者:	liyao 2016年9月14日10:55:11
****************************************************/
int8_t _Write(DMA_Sender_T* uds,uint8_t* data, uint8_t len){ 
  if(uds->OverFlag){//发送未完成则放入缓冲区
    Log.info("DMA BUSY加入缓存\r\n");
    Queue_Link_Put(uds->DMA_Send_Queue, data, len); 
    return 0;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
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
int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data){ 
	while(uds->OverFlag); 
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
int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data){ 
	uint16_t maxCur = (udr->DMA_BuffSize -  udr->Uart->hdmarx->Instance->CNDTR);
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA缓冲区破裂\r\n");
		return -1;
	}
	if((udr->Reversal == 0 && udr->Recv_Cur < maxCur) || udr->Reversal == 1){ 
		*data = udr->Data[udr->Recv_Cur]; 
		if(++udr->Recv_Cur % UART1_DMA_RECV_SIZE == 0)
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
int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len){
	uint16_t i = 0,tmp_Recv_Cur = udr->Recv_Cur, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR);
	uint8_t tmp_Reversal = udr->Reversal;
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA缓冲区破裂\r\n");
		return -1;
	}
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
int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len){
	uint16_t i = 0,cnt = 0, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR); 
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA缓冲区破裂\r\n");
		return -1;
	}
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
	return -1;
}

/****************************************************
	函数名:	_KeepTransmit
	功能:		当发频繁数据被加入缓存时，需要此函数触发发送缓冲区数据
	参数:		发送者
	返回值：dma未发送完返回-1，成功返回0 
  作者:		liyao 2016年11月4日14:26:35	
****************************************************/
int8_t _KeepTransmit(DMA_Sender_T* uds){
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
//	#endif
	return(ch);	   
}
 

/****************************************************
	函数名:	Buff_To_Uart
	功能:		从缓冲区中取出一个字节发送至串口
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
void Buff_To_Uart(void){
	int32_t data;
	uint8_t count = 5;
	while(count--)
		if(Queue_Get(Uart_Tx_Queue,&data) == 0){ 
			#ifdef UART1_DMA_SENDER
				Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,data);
			#else
				USART_SendData_Block(DEBUG_USART, data);  
			#endif 
		}else{
			break;
		}
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
} 


/****************************************************
        函数名: HAL_UART_RxCpltCallback
        功能:   DMA接收完成中断 回调
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_RECEIVER
    if(huart == Uart1_DMA_Receiver.Uart){
      if(++Uart1_DMA_Receiver.Reversal == 2)
        Log.error("USART1_DMA_接收数据被覆盖\r\n");
    } 
    HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, Uart1_DMA_Receiver.DMA_BuffSize);
  #endif 
}
 


