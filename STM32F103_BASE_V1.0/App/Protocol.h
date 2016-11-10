#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################发送协议类###################################

///****************************************************
//	结构体名:	ASK_PROTOCOL_T
//	功能: 应答协议实体
//	作者：liyao 2015年9月8日14:10:51
//****************************************************/
//typedef struct{
//	uint8_t para1;//接收序号
//}ASK_PROTOCOL_T;

/****************************************************
	结构体名:	STATE_PROTOCOL_T
	功能: 四轴状态协议实体
	作者：liyao 2016年4月4日13:03:42
****************************************************/ 
typedef struct{
	uint8_t para1;
}State_P_T; 
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
 


//----------------------联合体--------------------------------
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) (SrcModule<<4|TargetModule)<<8|Action
 
/*模块编号*/
typedef enum{ 
	PAD_MODULE	 = 0x01,
	MAIN_MODULE = 0x02, 
	ULT_MODULE	 = 0x03,
	MOTOR_MODULE= 0x04,
	INFRARED_MODULE = 0x05,
	PROJECTION_MODULE = 0x06,
	CHARGING_MODULE = 0x07,
	FACEMOT_MODULE = 0x08,
	HEADWING_MODULE = 0x09,
	SLIDING_MODULE = 0x0A,
	LIGHT_MODULE = 0x0B,
	POWER_MODULE = 0x0C,
	WAREHOUSE_MODULE = 0x0D, 
}MODULE;
 

/*协议目标板及动作*/
typedef enum { 
	REPORT_STATE = TO_MODULE_ACTION(HEADWING_MODULE, PAD_MODULE, 0x82),	
	HEART_BEAT	 = TO_MODULE_ACTION(PAD_MODULE, HEADWING_MODULE, 0x80),
  HEAP_USE	 = TO_MODULE_ACTION(PAD_MODULE, HEADWING_MODULE, 0xAA), 
  
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif

