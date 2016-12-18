#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################发送协议类###################################
/****************************************************
	结构体名:	STATE_PROTOCOL_T
	功能: 四轴状态协议实体
	作者：liyao 2016年4月4日13:03:42
****************************************************/ 
typedef struct{
	uint8_t para1;
}State_P_T; 

/****************************************************
	结构体名:	STATE_PROTOCOL_T
	功能: 驱动板状态协议实体
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//编码器1方向（左轮）
	uint8_t para2;//左轮速度高8
	uint8_t para3;//左轮速度低8
	uint8_t para4;//编码器2方向（右轮）
	uint8_t para5;//右轮速度高8
	uint8_t para6;//右轮速度低8
	uint8_t para7;
	uint8_t para8;
	uint8_t para9;
	uint8_t para10;
	uint8_t para11; 
	uint8_t para12;
	uint8_t para13;
}STATE_PROTOCOL_T;

/****************************************************
	结构体名:	ASK_PROTOCOL_T
	功能: 应答协议实体
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//序号
	uint8_t checksum;
	uint8_t tail;
}ASK_PROTOCOL_T;

//###################################接收协议类###################################
/****************************************************
	结构体名:	HEARTBEAT_PROTOCOL_T
	功能:	心跳协议实体
	作者：liyao 2016年4月4日13:06:27
	例:		fd 00 09 02 57 f8
****************************************************/
typedef struct{
	uint8_t para1;//心跳号
}HeartBeat_P_T;
 
/****************************************************
	结构体名:	RUN_PROTOCOL_T
	功能:	双轮控制命令协议实体
	作者：liyao 2015年9月8日14:10:51
	例:		fd 01 01 00 32 01 00 32 ff 00 f8
****************************************************/
typedef struct{
	int8_t para1;//左轮方向
	uint8_t para2;//左轮速度高8位
	uint8_t para3;//左轮速度低8位
	int8_t para4;//右轮方向
	uint8_t para5;//右轮速度高8位
	uint8_t para6;//右轮速度低8位
	uint8_t serial;
}Run_Protocol_T;

//----------------------联合体--------------------------------
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) (SrcModule<<4|TargetModule)<<8|Action
 
/*模块编号*/
typedef enum{ 
	PAD_MODULE	 = 0x01,
	RASPBERRY_MODULE = 0x02, 
	MOTOR_MODULE = 0x03, 
}MODULE;
 

/*协议目标板及动作*/
typedef enum { 
	REPORT_STATE = TO_MODULE_ACTION(MOTOR_MODULE, PAD_MODULE, 0x82),
	
	HEART_BEAT	 = TO_MODULE_ACTION(PAD_MODULE, MOTOR_MODULE, 0x80),
  HEAP_USE	 = TO_MODULE_ACTION(PAD_MODULE, MOTOR_MODULE, 0xAA), 
	
	RUN_PROTOCOL = TO_MODULE_ACTION(PAD_MODULE, MOTOR_MODULE, 0x01),
  
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif

