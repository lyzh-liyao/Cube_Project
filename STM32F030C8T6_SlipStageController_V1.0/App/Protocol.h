#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################发送协议类###################################

/****************************************************
	结构体名:	ASK_PROTOCOL_T
	功能: 应答协议实体
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//接收序号
}ASK_PROTOCOL_T;

 
typedef struct{
	uint8_t para1;//运动方向 1右 0左
	uint8_t para2;//移动距离高8 mm
	uint8_t para3;//移动距离低8 mm
	uint8_t para4;//小数部分高8	mm
	uint8_t para5;//小数部分低8 mm
	uint8_t para6;//速度 0-100
}CMD_PROTOCOL_T; 

typedef struct{
	uint8_t para1; //运动方向 1右 0左
	uint8_t para2; //转动角度高8 度
	uint8_t para3; //转动角度低8 度
	uint8_t para4; //转动角度小数高8 度
	uint8_t para5; //转动角度小数低8 度
	uint8_t para6; //速度 0-100 
}CMD_A_PROTOCOL_T;  

//###################################接收协议类###################################
/****************************************************
	结构体名:	HEARTBEAT_PROTOCOL_T
	功能:	心跳协议实体
	作者：liyao 2016年4月4日13:06:27
	例:		fd 00 09 02 57 f8
****************************************************/
typedef struct{
	uint8_t para1;//心跳号
}HEARTBEAT_PROTOCOL_T;


//----------------------联合体--------------------------------
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) (SrcModule<<4|TargetModule)<<8|Action
 
/*模块编号*/
typedef enum{ 
	SERVER_MODULE	 = 0x01,
	CONTROL_MODULE = 0x02,
}MODULE;
 

/*协议目标板及动作*/
typedef enum { 
  ANSWER         = TO_MODULE_ACTION(CONTROL_MODULE, SERVER_MODULE, 0xF0),
	CMD_PROTOCOL   = TO_MODULE_ACTION(SERVER_MODULE, CONTROL_MODULE, 0x01),
	CMD_A_PROTOCOL = TO_MODULE_ACTION(SERVER_MODULE, CONTROL_MODULE, 0x02),
}MODULE_ACTION; 
 
extern void Protocol_Init(void);
 















#endif

