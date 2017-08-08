#ifndef __COMBUFF_H__
#define __COMBUFF_H__  
#include "FrameConfig.h" 
#if MCU_TYPE == 103
  #include "stm32f1xx_hal.h"
#elif MCU_TYPE == 030 
  #include "stm32f0xx_hal.h"
#endif
#include "Queue.h"   


/**************************************************** 
	功能: 	向串口发送数据(阻塞)
****************************************************/
#define USART_SendData_Block(USARTx, data) {\
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);\
					USART_ClearFlag(USARTx, USART_FLAG_TC);\
					USART_SendData(USARTx,data);\
				}

#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
//发送者结构体
typedef struct _DMA_Sender_T DMA_Sender_T;
struct _DMA_Sender_T{
	Queue_Head_T* DMA_Send_Queue;
	uint8_t OverFlag;
	DMA_HandleTypeDef* Dma;
  UART_HandleTypeDef* Uart;
  uint8_t* Data;
  uint8_t Len;
	
	int8_t (*WriteByte)(DMA_Sender_T* Uds,uint8_t Data); 
	int8_t (*Write)(DMA_Sender_T* Uds, uint8_t *Data, uint8_t Len);
  int8_t (*KeepTransmit)(DMA_Sender_T* uds);
};
#endif
#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER
//接收者结构体
typedef struct _DMA_Receiver_T DMA_Receiver_T;
struct _DMA_Receiver_T{
	Queue_Head_T* DMA_Recv_Queue;
	uint16_t Recv_Cur; 
  uint8_t* Data;
  uint16_t DMA_BuffSize;
	int8_t Reversal;
	DMA_HandleTypeDef* Dma;
  UART_HandleTypeDef* Uart;
	int8_t (*ReadByte)(DMA_Receiver_T* udr,uint8_t* data);
	int16_t (*ReadTo)(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len);
	int16_t (*Read)(DMA_Receiver_T* udr, uint8_t *data, uint8_t len);
	int16_t (*Lseek)(DMA_Receiver_T* udr, int16_t offset);
};
#endif
/*************串口接收者*************/	
#ifdef UART1_DMA_RECEIVER				
	#define UART1_DMA_RECV_SIZE UART1_DMA_RECEIVER 
	extern DMA_Receiver_T Uart1_DMA_Receiver;
#endif

#ifdef UART2_DMA_RECEIVER				
	#define UART2_DMA_RECV_SIZE UART2_DMA_RECEIVER 
	extern DMA_Receiver_T Uart2_DMA_Receiver;
#endif

#ifdef UART3_DMA_RECEIVER				
	#define UART3_DMA_RECV_SIZE UART3_DMA_RECEIVER 
	extern DMA_Receiver_T Uart3_DMA_Receiver;
#endif

#ifdef UART4_DMA_RECEIVER				
	#define UART4_DMA_RECV_SIZE UART4_DMA_RECEIVER 
	extern DMA_Receiver_T Uart4_DMA_Receiver;
#endif
/*************串口发送者*************/	
#ifdef UART1_DMA_SENDER		
	//#define UART1_DMA_SEND_SIZE 100 
	extern DMA_Sender_T Uart1_DMA_Sender; 
#endif

#ifdef UART2_DMA_SENDER		
	//#define UART2_DMA_SEND_SIZE 100 
	extern DMA_Sender_T Uart2_DMA_Sender; 
#endif

#ifdef UART3_DMA_SENDER		
	//#define UART3_DMA_SEND_SIZE 100 
	extern DMA_Sender_T Uart3_DMA_Sender; 
#endif

#ifdef UART4_DMA_SENDER		
	//#define UART4_DMA_SEND_SIZE 100 
	extern DMA_Sender_T Uart4_DMA_Sender; 
#endif

extern QUEUE_T* Uart_Tx_Queue;
extern QUEUE_T* Nrf_Tx_Queue;

extern void Buff_To_Uart(void);
extern void Buff_To_NRF(void);
//extern void WriteByteToUSART(USART_TypeDef* USARTx, u8 data);
extern void ComBuff_Init(void);
extern void ComBuff_Configuration(void);


extern void PaddingProtocol(void);
extern void SenderKeepTransmit(void);
extern uint8_t PaddingProtocol_IT(void);


//--------------------------------printf实现-------------------------------------
/*
#include <stdarg.h>
#define mprintf mprintf
extern void mprintf(const char* fmt, ...);
extern void mvprintf(const char* fmt,va_list vp);

*/
#endif
