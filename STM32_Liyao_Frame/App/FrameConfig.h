#ifndef __FRAME_CONFIG_H__
#define __FRAME_CONFIG_H__
#include "usart.h"
//#define LL_LIB
#define DEBUG_USART   huart1 //调试串口
#define PRINT_ERR 1          //打开调试
//#define UART1_DMA_SENDER 1   //串口DMA发送者
//#define UART1_DMA_RECEIVER 100 //串口DMA接收者
//#define PROTOCOL_RESOLVER_1 1 //串口DMA数据解析者
//#define PROTOCOL_RESOLVER_IT_2 1 //串口IT数据解析者
#define PROTOCOL_CHECKSUM 0
#define LOG_OUT 1
#define MCU_TYPE 030
#define PROTOCOL_VERSION 3 //1 E01老版本协议  2 E01S新版本协议 3 灵悦智汇协议
//#define LOAD_MONITOR_ON
#endif

