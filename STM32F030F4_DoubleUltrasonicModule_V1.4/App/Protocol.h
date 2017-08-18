#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
#include "ProtocolFrame.h" 

//控制指令
#define OPEN_ULT 1					//开启超声波
#define CLOSE_ULT 2					//关闭超声波
#define CHANGE_INTERVAL 3		//上报周期
#define BARD_RATE 4					//波特率
#define SENSITIVITY 5       //灵敏度
#define DEAD_ZONE 6					//盲区距离

//控制指令应答
#define OP_ACK_OK 1
#define OP_ACK_ERR 0
//###################################发送协议类###################################
/****************************************************
	结构体名:	Distance_T
	功能: 超声波数据上报
	作者：liyao 2017年7月20日17:03:51
****************************************************/
typedef struct{
	uint8_t ID;
	uint8_t Distance_H;
	uint8_t Distance_L;
	uint8_t HOLD1;
	uint8_t HOLD2;
}Distance_T;

///****************************************************
//	结构体名:	ASK_PROTOCOL_T
//	功能: 应答协议实体
//	作者：liyao 2015年9月8日14:10:51
//****************************************************/
//typedef struct{
//	uint8_t para1;//接收序号
//}ASK_PROTOCOL_T;

//###################################接收协议类###################################
/****************************************************
	结构体名:	Allot_Seq_T
	功能: 上位机设置ID
	作者：liyao 2017年7月19日21:48:58
****************************************************/ 
typedef struct{
	uint8_t SN_H;
	uint8_t SN_L;
	uint8_t ID;
}Allot_ID_T; 

/****************************************************
	结构体名:	OP_Ctrl_T
	功能: 上位机控制协议
	作者：liyao 2017年7月19日21:48:58
****************************************************/ 
typedef struct{
	uint8_t ID;
	uint8_t Cmd;
	uint8_t Data_H;
	uint8_t Data_L;
}OP_Ctrl_T; 

/****************************************************
	结构体名:	Main_Answer_T
	功能: 上位机应答协议
	作者：liyao 2017年7月19日21:48:58
****************************************************/ 
typedef struct{
	uint8_t ID;
	uint8_t Seq;
	uint8_t Action;
}Main_Answer_T; 


/****************************************************
	结构体名:	HEARTBEAT_PROTOCOL_T
	功能:	心跳协议实体
	作者：liyao 2017年7月19日21:35:57
	例:		
****************************************************/
typedef struct{
	uint8_t Seq;//心跳号
	uint8_t para2;
}Main_Heart_T;


//----------------------联合体--------------------------------
/*模块编号*/
#define ULT_MODULE  0x02
#define MAIN_MODULE 0x01 

/*协议目标板及动作*/
//发送协议
#define  SN_REPORT    TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0x01) //SN上报
#define  DISTANCE_REPORT    TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0x02) //超声波数据上报
#define  ANSWER       TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0xF0)	//协议应答
#define  OP_ANSWER    TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0xF1) //操作应答
//接收协议
#define  ALLOT_ID		TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0x01) //ID获取
#define  OP_CTRL			TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0x02) //控制指令
#define  MAIN_ANSWER	TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0xF0) //上位机应答
#define  MAIN_HEART	  TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0xE0) //心跳

extern void Protocol_Init(void);
extern void RobSend(uint8_t* data, uint8_t len);















#endif

