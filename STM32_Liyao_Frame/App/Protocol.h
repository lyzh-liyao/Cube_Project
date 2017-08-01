#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
#include "ProtocolFrame.h" 

//###################################发送协议类###################################

/****************************************************
	结构体名:	Template_Send_T
	功能: 发送协议示例
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//接收序号
}Template_Send_T;

//###################################接收协议类###################################
/****************************************************
	结构体名:	Template_Recv_T
	功能: 接收协议示例
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//接收序号
}Template_Recv_T;


//----------------------联合体--------------------------------
/*模块编号*/
#define TEST1_MODULE  0x01
#define TEST2_MODULE  0x02 

/*协议目标板及动作*/
//发送协议
#define  SEND_TYPE    TO_MODULE_ACTION(TEST1_MODULE, TEST2_MODULE, 0x01)
//接收协议
#define  RECV_TYPE		TO_MODULE_ACTION(TEST2_MODULE, TEST1_MODULE, 0x01) //ID获取

extern void Protocol_Init(void);















#endif

