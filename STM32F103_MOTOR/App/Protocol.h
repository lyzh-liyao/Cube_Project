#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################发送协议类###################################
/****************************************************
	结构体名:	STATE_PROTOCOL_T
	功能: 驱动板状态协议实体
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//编码器1方向（左轮）
	uint8_t para2;//左轮增量
	uint8_t para3;//编码器2方向（右轮）
	uint8_t para4;//右轮增量
	uint8_t para5;//头部角度
	uint8_t para6;//左翅角度
	uint8_t para7;//右翅角度
	uint8_t para8;//左轮电流
	uint8_t para9;//右轮电流
	uint8_t para10;//头部电流
	uint8_t para11;//双翅电流
	uint8_t para12;//Reserved
	uint8_t para13;//Reserved
	uint8_t para14;//错误信息按位 置1
	uint8_t checksum;
	uint8_t tail;
}STATE_PROTOCOL_T;

/****************************************************
	结构体名:	ODOMETER_PROTOCOL_T
	功能: 左右轮里程协议
	作者：tc 2015年9月12日21:10:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//左轮里程
	uint8_t para2;//左轮里程
	uint8_t para3;//左轮里程
	uint8_t para4;//左轮里程
	uint8_t para5;//右轮里程
	uint8_t para6;//右轮里程
	uint8_t para7;//右轮里程
	uint8_t para8;//右轮里程
	uint8_t para9;//保留
	uint8_t para10;//保留
	uint8_t checksum;
	uint8_t tail;
}ODOMETER_PROTOCOL_T;

/****************************************************
	结构体名:	DEADRECKONING_PROTOCOL_T
	功能: 航位信息协议
	作者：tc 2015年9月16日15:30:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//航位X坐标
	uint8_t para2;//航位X坐标
	uint8_t para3;//航位X坐标
	uint8_t para4;//航位X坐标
	uint8_t para5;//航位Y坐标
	uint8_t para6;//航位Y坐标
	uint8_t para7;//航位Y坐标
	uint8_t para8;//航位Y坐标
	uint8_t para9;//航位TH坐标
	uint8_t para10;//航位TH坐标
	uint8_t para11;//航位TH坐标
	uint8_t para12;//航位TH坐标
	uint8_t para13;//保留
	uint8_t para14;//保留
	uint8_t checksum;
	uint8_t tail;
}DEADRECKONING_PROTOCOL_T;

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

/****************************************************
	结构体名:	RESPOND_PROTOCOL_T
	功能: 执行结果反馈协议
	作者：liyao 2015年10月16日16:33:41
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//序号
	uint8_t para2;//上位机命令序列
	uint8_t para3;//上位机帧类型
	uint8_t para4;//指令是否完成
	uint8_t checksum;
	uint8_t tail;
}RESPOND_PROTOCOL_T;

/****************************************************
	结构体名:	SELFINS_PROTOCOL_T
	功能: 自检结果上报
	作者：liyao 2016年1月25日14:20:04
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//错误位图
	uint8_t para2;//
	uint8_t para3;//双翅电流
	uint8_t para4;//双翅电流
	uint8_t para5;//头部电流
	uint8_t para6;//头部电流
	uint8_t para7;//左翅码数
	uint8_t para8;//右翅码数
	uint8_t para9;//头部码数
	uint8_t para10;//头部码数
	uint8_t para11;//保留
	uint8_t para12;//保留
	uint8_t para13;//保留
	uint8_t para14;//序号
	uint8_t checksum;
	uint8_t tail;
}SELFINS_PROTOCOL_T;

/****************************************************
	结构体名:	SENDSNVERSION_PROTOCOL_T
	功能:	发送版本号及芯片ChipID
	作者：liyao 2016年5月11日09:11:26
	例:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1; //产品型号：E01
	uint8_t para2;	
	uint8_t para3;
	uint8_t para4; //功能类型：000
	uint8_t para5; 
	uint8_t para6; 
	uint8_t para7; //硬件平台:00
	uint8_t para8; 
	uint8_t para9; //软件版本：V438
	uint8_t para10; 
	uint8_t para11; 
	uint8_t para12; 
	uint8_t para13; //发布日期：20160525
	uint8_t para14; 
	uint8_t para15; 
	uint8_t para16;  
	uint8_t para17;  
	uint8_t para18;  
	uint8_t para19;  
	uint8_t para20;
	uint8_t para21;//96位唯一ID
	uint8_t para22;
	uint8_t para23;
	uint8_t para24;
	uint8_t para25;
	uint8_t para26;
	uint8_t para27;
	uint8_t para28;
	uint8_t para29;
	uint8_t para30;
	uint8_t para31;
	uint8_t para32; 
	uint8_t para33; //序号
	uint8_t checksum;
	uint8_t tail;
}SENDSNVERSION_PROTOCOL_T;
//###################################接收协议类###################################
/****************************************************
	结构体名:	RUN_PROTOCOL_T
	功能:	双轮控制命令协议实体
	作者：liyao 2015年9月8日14:10:51
	例:		fd 01 02 00 10 00 00 10 01 01 01 f8
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	int8_t para1;//左轮方向
	uint8_t para2;//左轮速度高8位
	uint8_t para3;//左轮速度低8位
	int8_t para4;//右轮方向
	uint8_t para5;//右轮速度高8位
	uint8_t para6;//右轮速度低8位
	uint8_t para7;//序号
	int8_t para8;//是否新指令
	uint8_t checksum;
	uint8_t tail;
}RUN_PROTOCOL_T;

/****************************************************
	结构体名:	ANGLE_PROTOCOL_T
	功能:	双轮指定角度命令协议实体
	作者：liyao 2015年9月8日14:10:51
	例:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//左轮方向
	uint8_t para2;//左轮速度高8位
	uint8_t para3;//左轮速度低8位
	uint8_t para4;//左轮运动码数
	uint8_t para5;//左轮运动码数
	uint8_t para6;//左轮运动码数
	uint8_t para7;//左轮运动码数
	uint8_t para8;//右轮方向
	uint8_t para9;//右轮速度高8位
	uint8_t para10;//右轮速度低8位
	uint8_t para11;//右轮运动码数
	uint8_t para12;//右轮运动码数
	uint8_t para13;//右轮运动码数
	uint8_t para14;//右轮运动码数
	uint8_t para15;//序号
	int8_t para16;//是否新指令
	uint8_t checksum;
	uint8_t tail;
}ANGLE_PROTOCOL_T;

/****************************************************
	结构体名:	HEAD_PROTOCOL_T
	功能:	头部控制命令协议实体
	作者：liyao 2015年9月8日14:10:51
	例:		fd 02 f0 00 88 01 f7 f8
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//头部角度
	uint8_t para2;//头部速度
	uint8_t para3;//序号
	uint8_t para4;//是否新指令
	uint8_t checksum;
	uint8_t tail;
}HEAD_PROTOCOL_T;

/****************************************************
	结构体名:	WING_PROTOCOL_T
	功能:	翅膀控制命令协议实体
	作者：liyao 2015年9月8日14:10:51
	例:		fd 03 10 00 20 00 88 01 f7 f8
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	int8_t para1;//左翅角度
	int8_t para2;//左翅速度
	int8_t para3;//右翅角度
	int8_t para4;//右翅速度
	int8_t para5;//序号
	int8_t para6;//是否新指令
	uint8_t checksum;
	uint8_t tail;
}WING_PROTOCOL_T;
/****************************************************
	结构体名:	PDRCORRECT_PROTOCOL_T
	功能:	航位矫正协议实体
	作者：liyao 2015年10月20日13:57:15
	例:		
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//航位X坐标
	uint8_t para2;//航位X坐标
	uint8_t para3;//航位X坐标
	uint8_t para4;//航位X坐标
	uint8_t para5;//航位Y坐标
	uint8_t para6;//航位Y坐标
	uint8_t para7;//航位Y坐标
	uint8_t para8;//航位Y坐标
	uint8_t para9;//航位TH坐标
	uint8_t para10;//航位TH坐标
	uint8_t para11;//航位TH坐标
	uint8_t para12;//航位TH坐标
	uint8_t para13;//序号
	uint8_t para14;//是否新指令
	uint8_t checksum;
	uint8_t tail;
}PDRCORRECT_PROTOCOL_T;

/****************************************************
	结构体名:	ODOMETERSCRAM_T
	功能:	双轮根据里程转指定角度
	作者：liyao 2015年12月24日16:23:07
	例:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//左轮方向
	uint8_t para2;//左轮速度高8位
	uint8_t para3;//左轮速度低8位
	uint8_t para4;//左轮运动码数
	uint8_t para5;//左轮运动码数
	uint8_t para6;//左轮运动码数
	uint8_t para7;//左轮运动码数
	uint8_t para8;//右轮方向
	uint8_t para9;//右轮速度高8位
	uint8_t para10;//右轮速度低8位
	uint8_t para11;//右轮运动码数
	uint8_t para12;//右轮运动码数
	uint8_t para13;//右轮运动码数
	uint8_t para14;//右轮运动码数
	uint8_t para15;//序号
	int8_t para16;//是否新指令
	uint8_t checksum;
	uint8_t tail;
}ODOMETERSCRAM_T;

/****************************************************
	结构体名:	COMPENSATE_T
	功能:	驱动轮补偿
	作者：liyao 2016年1月9日 19:05:07
	例:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//跑直补偿
	uint8_t para2;//参数1.1
	uint8_t para3;//参数1.2
	uint8_t para4;//参数1.3
	uint8_t para5;//参数1.4
	uint8_t para6;//参数2.1
	uint8_t para7;//参数2.2
	uint8_t para8;//参数2.3
	uint8_t para9;//参数2.4
	uint8_t para10;//参数3.1
	uint8_t para11;//参数3.2
	uint8_t para12;//参数3.3
	uint8_t para13;//参数3.4
	uint8_t para14;//参数4.1
	uint8_t para15;//参数4.2
	uint8_t para16;//参数4.3
	uint8_t para17;//参数4.4
	uint8_t para18;//参数5.1
	uint8_t para19;//参数5.2
	uint8_t para20;//参数5.3
	uint8_t para21;//参数5.4
	uint8_t para22;//参数6.1
	uint8_t para23;//参数6.2
	uint8_t para24;//参数6.3
	uint8_t para25;//参数6.4
	uint8_t para26;//参数7.1
	uint8_t para27;//参数7.2
	uint8_t para28;//参数7.3
	uint8_t para29;//参数7.4
	uint8_t para30;//参数8.1
	uint8_t para31;//参数8.2
	uint8_t para32;//参数8.3
	uint8_t para33;//参数8.4
	uint8_t para34;//参数9.1
	uint8_t para35;//参数9.2
	uint8_t para36;//参数9.3
	uint8_t para37;//参数9.4
	uint8_t para38;//参数10.1
	uint8_t para39;//参数10.2
	uint8_t para40;//参数10.3
	uint8_t para41;//参数10.4
	uint8_t para42;//序号
	uint8_t checksum;
	uint8_t tail;
}COMPENSATE_PROTOCOL_T;

/****************************************************
	结构体名:	GETSNVERSION_PROTOCOL_T
	功能:	获取版本号及芯片ChipID
	作者：liyao 2016年5月11日09:13:26
	例:	
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1; //类型 0x01获取ChipID和版本号
	uint8_t para2; //预留
	uint8_t para3; //序号
	uint8_t checksum;
	uint8_t tail;
}GETSNVERSION_PROTOCOL_T;

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
  HEAP_USE	     = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0xAA), 
  RUN_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x01),   
  HEAD_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x02),  
  WING_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x03),  
  ANGLE_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x04),  
  PDRCORRECT_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x05),  
  ODOMETERSCRAM_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x06),  
  COMPENSATE_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x10),  
  GETSNVERSION_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x11),  
  
   
  STATE_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x01),
  ASK_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x02),
  ODOMETER_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x03),
  DEADRECKONING_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x04),
  RESPOND_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x05),
  SELFINS_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x06),
  SENDSNVERSION_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x07),
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif

