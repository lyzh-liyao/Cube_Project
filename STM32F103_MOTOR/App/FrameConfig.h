#ifndef __FRAME_CONFIG_H__
#define __FRAME_CONFIG_H__
#include "usart.h"
#define DEBUG_USART   huart1 //调试串口
#define UART1_DMA_SENDER 1   //串口发送者
#define UART1_DMA_RECEIVER 1 //串口接收者
#define UART1_PROTOCOL_RESOLVER 1 //串口数据解析者
#define LOG_OUT 1
#define PROTOCOL_VERSION  1  //1 E01老版本协议  2 E01S新版本协议
#endif

