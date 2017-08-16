#ifndef __FIRMWARE_UPDATE_H__
#define __FIRMWARE_UPDATE_H__
#include <stdint.h>

//#define PRINT_ERR
#define USART USART4	
#define PAGE_SIZE          0x400U
#define UART_FLAG_TC                        (0x00000040U)              /*!< UART transmission complete                */
/*----------协议格式--------------------*/
/*
									程序烧写协议
帧头 指令  长度高8 长度低8 固件校验和 协议校验和 	帧尾
0xFD 0xA0   0xXX    0xXX  		0xXX			 0xXX			0xF8

									程序跳转协议
帧头 指令  长度高8 长度低8 固件校验和 协议校验和 	帧尾
0xFD 0xA1   0xXX    0x00  		0x00			 0xXX			0xF8


返回值：error1    校验和错误
				error2		擦除中收到数据，打断重新接收
				error3		固件校验和错误
*/
typedef struct {
	uint8_t Head;
	uint16_t Firmware_Size;
	uint16_t Firmware_Check_Sum;
	uint8_t Firmware_Real_Sum;
	uint8_t Tail;
	uint8_t State;
	
	uint8_t Type;//帧类型
	uint8_t Len;//帧长度
	uint8_t Sum;//帧校验和
	
}Firmware_Info_T;
extern Firmware_Info_T* Firmware_Info;

extern void FirmwareUpdate_Init(void);
extern void SYSTICK_IRQHandler(void);
extern void FirmwareUpdate_Run(void);
extern void UART_IRQHandler(void);
void Uart_Init(void);
#endif



