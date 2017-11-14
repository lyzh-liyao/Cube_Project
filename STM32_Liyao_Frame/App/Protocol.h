#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
#include "ProtocolFrame.h" 

//###################################发送协议类###################################

/****************************************************
	结构体名:	Ptcl_Ack_T
	功能: 应答协议实体
	作者：liyao 2017年10月13日
****************************************************/
typedef struct{
	uint8_t Seq;
}__attribute((packed))Ptcl_Ack_T;

/****************************************************
	结构体名:	OP_Ack_T
	功能: 操作应答协议实体
	作者：liyao 2017年10月13日
****************************************************/
typedef struct{
	uint8_t Action;
	uint8_t Seq;
	uint8_t Result;
}__attribute((packed))OP_Ack_T;
//###################################接收协议类###################################
/****************************************************
	结构体名:	Super_Ack_T
	功能: 上位机应答协议实体
	作者：liyao 2017年10月13日
****************************************************/
typedef struct{	
	uint8_t Action;
	uint8_t Seq;
	uint8_t Reserved;
}__attribute((packed))Super_Ack_T;


/*模块编号*/
#define REMOTE_MODULE  0x01
#define THIS_MODULE  	 0x02 

/*协议目标板及动作*/
//发送协议
#define  PTCL_ACK    		TO_MODULE_ACTION(THIS_MODULE, REMOTE_MODULE, 0xF0)	//协议应答
#define  OP_ACK    			TO_MODULE_ACTION(THIS_MODULE, REMOTE_MODULE, 0xF1)	//操作应答
//接收协议
#define  SUPER_ACK    	TO_MODULE_ACTION(REMOTE_MODULE, THIS_MODULE, 0xF0)		//上位机应答

extern void Protocol_Init(void);















#endif

