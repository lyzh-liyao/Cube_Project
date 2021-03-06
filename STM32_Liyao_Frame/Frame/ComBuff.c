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
#include "TaskTimeManager.h"
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
#ifdef UART5_DMA_SENDER 
	DMA_Sender_T Uart5_DMA_Sender = {0}; 
#endif 
#ifdef UART5_DMA_RECEIVER 
	DMA_Receiver_T Uart5_DMA_Receiver = {0};  
#endif 
//--------------------------内部函数声明-----------------------------
#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER || UART5_DMA_RECEIVER
static int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data);
static int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len);
static int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len); 
#endif

#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER || UART5_DMA_SENDER
static int8_t _Write(DMA_Sender_T* uds, uint8_t *data, uint8_t len);
static int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data);
static int8_t _KeepTransmit(DMA_Sender_T* uds);
#endif

	
#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
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
		
	WRITE_REG(huart->Instance->ICR, UART_CLEAR_TCF);//开启发送完成中断便于持续发送
	SET_BIT(huart->Instance->CR1, USART_ISR_TC);
}
#endif

#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER || UART5_DMA_RECEIVER
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
#endif


#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER || UART5_DMA_SENDER
/****************************************************
	函数名:	_Write
	功能:	通过串口DMA即刻发送一组数据
	参数:	发送者 , 回填参数
	返回值：成功0 失败-1
	作者:	liyao 2016年9月14日10:55:11
****************************************************/
static int8_t _Write(DMA_Sender_T* uds,uint8_t* data, uint8_t len){ 
  if(uds->OverFlag){//发送未完成则放入缓冲区
//    Log.info("DMA BUSY加入缓存\r\n"); 
    Queue_Link_Push(uds->DMA_Send_Queue, data, len); 
    return 0;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
		uds->Data = MALLOC(cnt); 
    MALLOC_CHECK(uds->Data,"_Write");
		if(uds->Data == NULL)
			return -1;
    Queue_Link_Pop(uds->DMA_Send_Queue, uds->Data);
    uds->Len = cnt;
    Queue_Link_Push(uds->DMA_Send_Queue, data, len); 
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
		uds->Len = 1;
	  HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len); 
	  uds->OverFlag = 1; 
	return 0;
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
    Queue_Link_Pop(uds->DMA_Send_Queue, uds->Data);
    uds->Len = cnt; 
    HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len);
  } 
  return 0;
}
#endif

#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER || UART5_DMA_RECEIVER
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
		Log.error("DMA缓冲区破裂,丢弃当前数据重置\r\n");
		HAL_UART_DMAStop(udr->Uart);
		HAL_UART_Receive_DMA(udr->Uart, udr->Data, udr->DMA_BuffSize);
		udr->Recv_Cur = udr->Reversal = 0;//缓冲区破裂后容错抛弃数据
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
		Log.error("DMA缓冲区破裂,丢弃当前数据重置\r\n");
		HAL_UART_DMAStop(udr->Uart);
		HAL_UART_Receive_DMA(udr->Uart, udr->Data, udr->DMA_BuffSize);
		udr->Recv_Cur = udr->Reversal = 0;//缓冲区破裂后容错抛弃数据
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
		Log.error("DMA缓冲区破裂,丢弃当前数据重置\r\n");
		HAL_UART_DMAStop(udr->Uart);
		HAL_UART_Receive_DMA(udr->Uart, udr->Data, udr->DMA_BuffSize);
		udr->Recv_Cur = udr->Reversal = 0;//缓冲区破裂后容错抛弃数据
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
#endif

 
/****************************************************
	函数名:	fputc
	功能: 	printf重定向
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
__weak int fputc(int ch,FILE *f)
{
//	#ifndef PRINT_ERR
//		Queue_Put(Uart_Tx_Queue, &ch); 
//	#else
#ifdef LL_LIB
	LL_USART_TransmitData8(DEBUG_USART,ch);
#else
	#ifdef DEBUG_USART
		 HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 1000);
	#endif
#endif
//  
	//Uart1_DMA_Sender.Write(&Uart1_DMA_Sender,(uint8_t*)&ch, 1);
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


__weak void Uart1_DMA_IDLE_CallBack(void){
#if PROTOCOL_RESOLVER_1  && UART1_DMA_RECEIVER		//串口DMA + 协议解析
	uint8_t UartData[SINGLE_BUFFSIZE];
	int8_t cnt = 1;
	while((cnt = Uart1_DMA_Receiver.ReadTo(&Uart1_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
		ProtocolResolver_1->Protocol_Put(ProtocolResolver_1, UartData, cnt);
#endif
}
__weak void Uart2_DMA_IDLE_CallBack(void){
#if PROTOCOL_RESOLVER_2  && UART2_DMA_RECEIVER		//串口DMA + 协议解析
	uint8_t UartData[SINGLE_BUFFSIZE];
	int8_t cnt = 1;
	while((cnt = Uart2_DMA_Receiver.ReadTo(&Uart2_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
		ProtocolResolver_2->Protocol_Put(ProtocolResolver_2, UartData, cnt);
#endif
}
__weak void Uart3_DMA_IDLE_CallBack(void){
#if PROTOCOL_RESOLVER_3  && UART3_DMA_RECEIVER		//串口DMA + 协议解析
	uint8_t UartData[SINGLE_BUFFSIZE];
	int8_t cnt = 1;
	while((cnt = Uart3_DMA_Receiver.ReadTo(&Uart3_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
				ProtocolResolver_3->Protocol_Put(ProtocolResolver_3, UartData, cnt); 
#endif	
}

__weak void Uart4_DMA_IDLE_CallBack(void){
#if PROTOCOL_RESOLVER_4  && UART4_DMA_RECEIVER		//串口DMA + 协议解析
	uint8_t UartData[SINGLE_BUFFSIZE];
	int8_t cnt = 1;
	while((cnt = Uart4_DMA_Receiver.ReadTo(&Uart4_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
		ProtocolResolver_4->Protocol_Put(ProtocolResolver_4, UartData, cnt);
#endif	
}

__weak void Uart5_DMA_IDLE_CallBack(void){
#if PROTOCOL_RESOLVER_5  && UART5_DMA_RECEIVER		//串口DMA + 协议解析
	uint8_t UartData[SINGLE_BUFFSIZE];
	int8_t cnt = 1;
	while((cnt = Uart5_DMA_Receiver.ReadTo(&Uart5_DMA_Receiver, PROTOCOL_TAIL, UartData, SINGLE_BUFFSIZE))>0)
		ProtocolResolver_5->Protocol_Put(ProtocolResolver_5, UartData, cnt);
#endif	
}

__weak void Uart1_IT_IDLE_CallBack(uint8_t* Data, uint8_t Len){
#if PROTOCOL_RESOLVER_1
	ProtocolResolver_1->Protocol_Put(ProtocolResolver_1, Data , Len);
#endif
}

__weak void Uart2_IT_IDLE_CallBack(uint8_t* Data, uint8_t Len){
#if PROTOCOL_RESOLVER_2
	ProtocolResolver_2->Protocol_Put(ProtocolResolver_2, Data , Len);
#endif
}

__weak void Uart3_IT_IDLE_CallBack(uint8_t* Data, uint8_t Len){
#if PROTOCOL_RESOLVER_3
	ProtocolResolver_3->Protocol_Put(ProtocolResolver_3, Data , Len);
#endif
}

__weak void Uart4_IT_IDLE_CallBack(uint8_t* Data, uint8_t Len){
#if PROTOCOL_RESOLVER_4
	ProtocolResolver_4->Protocol_Put(ProtocolResolver_4, Data, Len);
#endif
}

__weak void Uart5_IT_IDLE_CallBack(uint8_t* Data, uint8_t Len){
#if PROTOCOL_RESOLVER_5
	ProtocolResolver_5->Protocol_Put(ProtocolResolver_5, Data , Len);
#endif
}
 
/****************************************************
        函数名: PaddingProtocol
        功能:   从串口缓冲区中读取数据到协议解析器
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
int8_t ComBuff_IRQHandler(void){
	#if UART1_DMA_RECEIVER		//串口DMA + 协议解析
		if((READ_REG(USART1->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART1->ICR, UART_CLEAR_IDLEF);
			Uart1_DMA_IDLE_CallBack();
			return 1;
		}
  #elif UART1_IT_RECEIVER											//串口IT + 协议解析
		static uint8_t UartBuff1[UART1_IT_RECEIVER];
		static uint8_t Index1 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART1->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART1->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART1->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART1->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART1->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART1->DR);	
		#endif
			UartBuff1[Index1++%UART1_IT_RECEIVER] = SUartData;
			return 1;
		}
		if((READ_REG(USART1->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART1->ICR, UART_CLEAR_IDLEF);
			if(Index1 > UART1_IT_RECEIVER){
				Index1 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			Uart1_IT_IDLE_CallBack(&UartBuff1, Index1);
			Index1 = 0;
			return 1;
		}
	#endif
	
		
	#if UART2_DMA_RECEIVER
		if((READ_REG(USART2->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART2->ICR, UART_CLEAR_IDLEF);
			Uart2_DMA_IDLE_CallBack();
			return 1;
		}
	#elif UART2_IT_RECEIVER
		static uint8_t UartBuff2[UART2_IT_RECEIVER];
		static uint8_t Index2 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART2->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART2->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART2->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART2->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART2->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART2->DR);
		#endif
			UartBuff2[Index2++%UART2_IT_RECEIVER] = SUartData;
			return 1;
		}
		if((READ_REG(USART2->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART2->ICR, UART_CLEAR_IDLEF);
			if(Index2 > UART2_IT_RECEIVER){
				Index2 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			Uart2_IT_IDLE_CallBack(UartBuff2, Index2);
			Index2 = 0;
			return 1;
		}
	#endif
	#if UART3_DMA_RECEIVER
		if((READ_REG(USART3->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART3->ICR, UART_CLEAR_IDLEF);
			Uart3_DMA_IDLE_CallBack();
			return 1;
		}
	#elif UART3_IT_RECEIVER
	static uint8_t UartBuff3[UART3_IT_RECEIVER];
	static uint8_t Index3 = 0;
	#if MCU_TYPE == 030
	if(((READ_REG(USART3->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART3->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t SUartData =  READ_REG(USART3->RDR);
	#elif MCU_TYPE == 103
	if(((READ_REG(USART3->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART3->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t SUartData =  READ_REG(USART3->DR);
	#endif
		UartBuff3[Index3++%UART3_IT_RECEIVER] = SUartData;
		return 1;
	}else if((READ_REG(USART3->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART3->ICR, UART_CLEAR_IDLEF);
			if(Index3 > UART3_IT_RECEIVER){
				Index3 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			Uart3_IT_IDLE_CallBack(UartBuff3, Index3);
			Index3 = 0;
			return 1;
	}
	#endif
	#if UART4_DMA_RECEIVER
		if((READ_REG(USART4->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART4->ICR, UART_CLEAR_IDLEF);
			Uart4_DMA_IDLE_CallBack();
			return 1;
		}
	#elif UART4_IT_RECEIVER
		static uint8_t UartBuff4[UART4_IT_RECEIVER];
		static uint8_t Index4 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART4->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART4->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART4->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART4->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART4->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART4->DR);
		#endif
			UartBuff4[Index4++%UART4_IT_RECEIVER] = SUartData;
			return 1;
		}
		if((READ_REG(USART4->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART4->ICR, UART_CLEAR_IDLEF);
			if(Index4 > UART4_IT_RECEIVER){
				Index4 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			Uart4_IT_IDLE_CallBack(UartBuff4 , Index4);
			Index4 = 0;
			return 1;
		}
	#endif
	#if UART5_DMA_RECEIVER
		if((READ_REG(USART5->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART5->ICR, UART_CLEAR_IDLEF);
			Uart5_DMA_IDLE_CallBack();
			return 1;
		}
	#elif UART5_IT_RECEIVER
		static uint8_t UartBuff5[UART5_IT_RECEIVER];
		static uint8_t Index5 = 0;
		#if MCU_TYPE == 030
		if(((READ_REG(USART5->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART5->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART5->RDR);
		#elif MCU_TYPE == 103
		if(((READ_REG(USART5->SR) & USART_SR_RXNE) != RESET) && ((READ_REG(USART5->CR1) & USART_CR1_RXNEIE) != RESET)){
			uint8_t SUartData =  READ_REG(USART5->DR);
		#endif
			UartBuff5[Index5++%UART5_IT_RECEIVER] = SUartData;
			return 1;
		}else if((READ_REG(USART5->ISR) & USART_ISR_IDLE) != RESET){
			WRITE_REG(USART5->ICR, UART_CLEAR_IDLEF);
			if(Index5 > UART5_IT_RECEIVER){
				Index5 = 0;
				Log.waring("串口空闲中断前接收过多数据缓冲区移除\r\n");
				return 1;
			}
			Uart5_IT_IDLE_CallBack( UartBuff5 , Index5);
			Index5 = 0;
			return 1;
		}
	#endif 
		
		if((( 0
#ifdef UART1_DMA_SENDER
		| READ_REG(USART1->ISR)
#endif
#ifdef UART2_DMA_SENDER
		| READ_REG(USART2->ISR)
#endif
#ifdef UART3_DMA_SENDER
		| READ_REG(USART3->ISR)
#endif
#ifdef UART4_DMA_SENDER
		| READ_REG(USART4->ISR)
#endif
#ifdef UART5_DMA_SENDER
		| READ_REG(USART5->ISR)
#endif
		) & USART_ISR_TC) != RESET){
			TaskTime_Add(TaskID++, TimeCycle(0, 0), SenderKeepTransmit, Single_Mode);
		}
		return 0;
}

//--------------------------------系统回调函数-------------------------------------

#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER || UART5_DMA_SENDER
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
  #ifdef UART5_DMA_SENDER
    if(huart == Uart5_DMA_Sender.Uart){
        Uart5_DMA_Sender.OverFlag = 0;
        FREE(Uart5_DMA_Sender.Data);
    }
  #endif    
} 
#endif

/****************************************************
        函数名: HAL_UART_RxCpltCallback
        功能:   DMA接收完成中断 回调
        作者:   liyao 2016年9月14日10:55:11
****************************************************/ 
#if  UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER || UART5_DMA_RECEIVER
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_RECEIVER
    if(huart == Uart1_DMA_Receiver.Uart){
      if(++Uart1_DMA_Receiver.Reversal == 2){
        Log.error("USART1_DMA_接收数据被覆盖\r\n");
				Uart1_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, Uart1_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif 
  #ifdef UART2_DMA_RECEIVER
    if(huart == Uart2_DMA_Receiver.Uart){
      if(++Uart2_DMA_Receiver.Reversal == 2){
        Log.error("USART2_DMA_接收数据被覆盖\r\n");
				Uart2_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart2_DMA_Receiver.Data, Uart2_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif 
  #ifdef UART3_DMA_RECEIVER
    if(huart == Uart3_DMA_Receiver.Uart){
      if(++Uart3_DMA_Receiver.Reversal == 2){
        Log.error("USART3_DMA_接收数据被覆盖\r\n");
				Uart3_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart3_DMA_Receiver.Data, Uart3_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif  
  #ifdef UART4_DMA_RECEIVER
    if(huart == Uart4_DMA_Receiver.Uart){
      if(++Uart4_DMA_Receiver.Reversal == 2){
        Log.error("USART4_DMA_接收数据被覆盖\r\n");
				Uart4_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart4_DMA_Receiver.Data, Uart4_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif   
  #ifdef UART5_DMA_RECEIVER
    if(huart == Uart5_DMA_Receiver.Uart){
      if(++Uart5_DMA_Receiver.Reversal == 2){
        Log.error("USART4_DMA_接收数据被覆盖\r\n");
				Uart5_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart5_DMA_Receiver.Data, Uart5_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif      
}
#endif
 
#if  UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER || UART5_DMA_RECEIVER ||\
		 UART1_DMA_SENDER	  || UART2_DMA_SENDER   || UART3_DMA_SENDER   || UART4_DMA_SENDER   || UART5_DMA_SENDER
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	#ifdef UART1_DMA_SENDER
	if(huart == Uart1_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart1_DMA_Sender.Uart, Uart1_DMA_Sender.Data, Uart1_DMA_Sender.Len); 
		Log.error("huart1 TX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	#ifdef UART2_DMA_SENDER
	if(huart == Uart2_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart2_DMA_Sender.Uart, Uart2_DMA_Sender.Data, Uart2_DMA_Sender.Len); 
		Log.error("huart2 TX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	#ifdef UART3_DMA_SENDER
	if(huart == Uart3_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart3_DMA_Sender.Uart, Uart3_DMA_Sender.Data, Uart3_DMA_Sender.Len); 
		Log.error("huart3 TX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	#ifdef UART4_DMA_SENDER
	if(huart == Uart4_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart4_DMA_Sender.Uart, Uart4_DMA_Sender.Data, Uart4_DMA_Sender.Len); 
		Log.error("huart4 TX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	#ifdef UART5_DMA_SENDER
	if(huart == Uart5_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart5_DMA_Sender.Uart, Uart5_DMA_Sender.Data, Uart5_DMA_Sender.Len); 
		Log.error("huart5 TX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	
	#ifdef UART1_DMA_RECEIVER
	if(huart == Uart1_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, UART1_DMA_RECV_SIZE); 
		Log.error("huart1 RX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	#ifdef UART2_DMA_RECEIVER
	if(huart == Uart2_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart2_DMA_Receiver.Data, UART2_DMA_RECV_SIZE);
		Log.error("huart2 RX DMA error:0x%X\r\n", huart->ErrorCode); 
	}
	#endif
	#ifdef UART3_DMA_RECEIVER
	if(huart == Uart3_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart3_DMA_Receiver.Data, UART3_DMA_RECV_SIZE);
		Log.error("huart3 RX DMA error:0x%X\r\n", huart->ErrorCode); 
	}
	#endif
	#ifdef UART4_DMA_RECEIVER
	if(huart == Uart4_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart4_DMA_Receiver.Data, UART4_DMA_RECV_SIZE); 
		Log.error("huart4 RX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
	#ifdef UART5_DMA_RECEIVER
	if(huart == Uart5_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart5_DMA_Receiver.Data, UART5_DMA_RECV_SIZE); 
		Log.error("huart5 RX DMA error:0x%X\r\n", huart->ErrorCode);
	}
	#endif
//	while(1);
}
#endif
//--------------------------------快捷调用-------------------------------------
#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER || UART5_DMA_SENDER
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
#if UART5_DMA_SENDER == 1
  Uart5_DMA_Sender.KeepTransmit(&Uart5_DMA_Sender);
#endif
}
#else
__weak void SenderKeepTransmit(void){;}
#endif


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
#ifdef UART5_DMA_SENDER
  DMA_Sender_Init(&Uart5_DMA_Sender, &huart5);
#endif  
#ifdef UART5_DMA_RECEIVER
  DMA_Receiver_Init(&Uart5_DMA_Receiver, &huart5, UART5_DMA_RECV_SIZE); 
#endif 

#ifdef UART1_IT_RECEIVER
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
#endif
#ifdef UART2_IT_RECEIVER
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
#endif
#ifdef UART3_IT_RECEIVER
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
#endif
#ifdef UART4_IT_RECEIVER
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
#endif
#ifdef UART5_IT_RECEIVER
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
#endif

#if UART1_IT_RECEIVER || UART1_DMA_RECEIVER
	WRITE_REG(USART1->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART1->CR1, USART_ISR_IDLE);
#endif
#if UART2_IT_RECEIVER || UART2_DMA_RECEIVER
	WRITE_REG(USART2->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART2->CR1, USART_ISR_IDLE);
#endif
#if UART3_IT_RECEIVER || UART3_DMA_RECEIVER
	WRITE_REG(USART3->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART3->CR1, USART_ISR_IDLE);
#endif
#if UART4_IT_RECEIVER || UART4_DMA_RECEIVER
	WRITE_REG(USART4->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART4->CR1, USART_ISR_IDLE);
#endif
#if UART5_IT_RECEIVER || UART5_DMA_RECEIVER
	WRITE_REG(USART5->ICR, UART_CLEAR_IDLEF);
	SET_BIT(USART5->CR1, USART_ISR_IDLE);
#endif


#ifdef __TASKTIMEMANAGER_H__
	/*-----------持续传输任务-----------------*/
//  TaskTime_Add(TaskID++, TimeCycle(0,10), SenderKeepTransmit, Real_Mode);
	/*-----------协议解析任务-----------------*/
//  TaskTime_Add(TaskID++, TimeCycle(0,10), PaddingProtocol, Real_Mode);
#endif
}
//--------------------------------printf实现-------------------------------------
/*
void printch(char ch);
void printdec(int dec);
void printflt(double flt);
void printstr(char* str);
void printbin(int bin);
void printhex(int hex);
void StringFormat(char* target, uint8_t type,const char* fmt,va_list vp);

void printch(char ch)
{
#ifdef LL_LIB
		LL_USART_TransmitData8(DEBUG_USART,ch);
#else
	while(HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
#endif
}

void mvprintf(const char* fmt,va_list vp){
	StringFormat(NULL, 0, fmt, vp);
}

void msprintf(char* target,const char* fmt,va_list vp){
	StringFormat(target, 1, fmt, vp);
}

void mprintf(const char* fmt, ...)
{
    va_list vp;
    va_start(vp, fmt);
		mvprintf(fmt, vp);
    va_end(vp);
}

void StringFormat(char* target,uint8_t type,const char* fmt,va_list vp){
	double vargflt = 0;
    int  vargint = 0;
    char* vargpch = NULL;
    char vargch = 0;
    const char* pfmt = NULL;
    pfmt = fmt;

    while(*pfmt)
    {
        if(*pfmt == '%')
        {
            switch(*(++pfmt))
            {
                
                case 'c':
                    printch(vargch);
                    break;
                case 'd':
                case 'i':
                    vargint = va_arg(vp, int);
                    printdec(vargint);
                    break;
                case 'f':
                    vargflt = va_arg(vp, double);
                    printflt(vargflt);
                    break;
                case 's':
                    vargpch = va_arg(vp, char*);
                    printstr(vargpch);
                    break;
                case 'b':
                case 'B':
                    vargint = va_arg(vp, int);
                    printbin(vargint);
                    break;
                case 'x':
                case 'X':
                    vargint = va_arg(vp, int);
                    printhex(vargint);
                    break;
                case '%':
                    printch('%');
                    break;
                default:
                    break;
            }
            pfmt++;
        }
        else
        {
						if(type == 0)
							printch(*pfmt++);
						else if(type == 1)
							*target++ = *pfmt++;
        }
    }
}


void printdec(int dec)
{
	char buff[20] = {0};
	int8_t i = 0;
	int num = dec;
	do{
		num /= 10;
		i++;
	}while(num != 0);
	
	if(dec < 0){
		buff[0] = '-';
		i++;
	}
	while(--i >= 0){
		buff[i] = dec%10 + '0';
		dec /= 10;
	}
	printstr(buff);
}

void printflt(double flt)
{
    int tmpint = 0;
    tmpint = (int)flt;
    printdec(tmpint);
    printch('.');
    flt = flt - tmpint;
    tmpint = (int)(flt * 1000000);
    printdec(tmpint);
}

void printstr(char* str)
{
    while(*str)
    {
        printch(*str++);
    }
}

void printbin(int bin)
{
    if(bin == 0)
    {
        printstr("0b");
        return;
    }
    printbin(bin/2);
    printch( (char)(bin%2 + '0'));
}

void printhex(int hex)
{
    if(hex==0)
    {
        printstr("0x");
        return;
    }
    printhex(hex/16);
    if(hex < 10)
    {
        printch((char)(hex%16 + '0'));
    }
    else
    {
        printch((char)(hex%16 - 10 + 'a' ));
    }
}
*/

