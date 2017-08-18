#include "InfraredDriver.h"
#include <string.h>
#include "tim.h"
#ifdef INFRARED_SEND
Infrared_s_t infrared_s;
Infrared_s_t* Infrared_s = &infrared_s;
void _InfraredSendBit(uint8_t PWM_Cnt, int16_t Bit);
void _InfraredClean_S(void);

/****************************************************
	函数名:	INF_S_TIM_PWM_PulseFinishedCallback
	功能:		PWM每周期回调
	作者:		liyao 2017年8月16日
****************************************************/
void INF_S_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if(htim == &INF_CAL_TIM){
		if(Infrared_s->PWM_Cnt > 0){	//脉冲发送中
			Infrared_s->PWM_Cnt--;
		}else{	//脉冲发送完毕处理状态机
			switch (Infrared_s->InfState){
				case InfStart_S://状态（InfStart_S）：发送Start信号
					Infrared_s->InfState = InfHop_S;
					_InfraredClean_S();
					Infrared_s->Data = ((uint8_t*)&Infrared_s->BuffData)[Infrared_s->BuffIndex++];//准备要发送的数据
					_InfraredSendBit(PWM_START,INF_START);//发送开始信号
					break;
				case InfHop_S:	//状态（InfHop_S）：发送跳变数据
					Infrared_s->InfState = InfSData_S;
					_InfraredSendBit(PWM_BIT,__HAL_TIM_GET_COMPARE(&INF_CAL_TIM, INF_CHANNEL) == INF_RESET?INF_SET:INF_RESET);//发送当前相反数据
					break;
				case InfSData_S://状态（InfSData_S）：发送数据位
					Infrared_s->InfState = InfCheck_S;
					if((Infrared_s->Data >> (7 - Infrared_s->Index++))& 0x01){//发送bit位
						_InfraredSendBit(PWM_BIT,INF_SET);
					}else{
						_InfraredSendBit(PWM_BIT,INF_RESET);
					}
					break;
				case InfCheck_S://状态（InfCheck_S）：检查是否发送完一个字节或者发送完毕
					Infrared_s->InfState = InfHop_S;//跳转到发送跳变数据状态
					if(Infrared_s->Index == 8){//检查当前字节是否发送完成
						Infrared_s->Index = 0;
						if(Infrared_s->BuffIndex < sizeof(Infrared_Data_t)){
							Infrared_s->Data = ((uint8_t*)&Infrared_s->BuffData)[Infrared_s->BuffIndex++];//遍历下一字节
						}else{
							Infrared_s->InfState = InfSStop_S;//跳转到停止状态
						}
					}
//					_InfraredSendBit(PWM_BIT,__HAL_TIM_GET_COMPARE(&INF_CAL_TIM, INF_CHANNEL) == INF_RESET?INF_SET:INF_RESET);
					break;
				case InfSStop_S://状态（InfSStop_S）：发送Stop信号
					Infrared_s->InfState = InfFree_S;//标志数据接收完成
					_InfraredSendBit(PWM_STOP,INF_STOP);//发送停止信号
					HAL_TIM_PWM_Stop_IT(&INF_CAL_TIM, INF_CHANNEL);//关闭PWM
					break;
				case InfFree_S:
					break;
			}
		}
	}
}
	
void Infrared_s_Init(void){
	memset(Infrared_s, 0, sizeof(Infrared_s_t));
}

/****************************************************
	函数名:	_InfraredClean
	功能:		清理Infrared变量
	作者:		liyao 2017年8月16日
****************************************************/
void _InfraredClean_S(void){
	Infrared_s->Data = Infrared_s->Index = Infrared_s->PWM_Cnt = Infrared_s->BuffIndex = 0;
}

/****************************************************
	函数名:	_InfraredSendBit
	功能:		发送Bit
	作者:		liyao 2017年8月16日
****************************************************/
void _InfraredSendBit(uint8_t PWM_Cnt, int16_t Bit){
	Infrared_s->PWM_Cnt = PWM_Cnt;
	__HAL_TIM_SET_COUNTER(&INF_CAL_TIM, 0);
	__HAL_TIM_SET_COMPARE(&INF_CAL_TIM, INF_CHANNEL, Bit);
}

/****************************************************
	函数名:	InfraredSendData
	功能:		发送1字节红外数据
	作者:		liyao 2017年8月16日
	备注:		start信号（低电平800us）+高起始位+ data + ~data + 高电平持续
					读数据间隔400us
****************************************************/
int InfraredSendData(Infrared_s_t* Inf, Infrared_Data_t* Data){
	if(Inf->InfState != InfFree_S){
		return -1;
	} 
	
	
	Inf->InfState = InfStart_S; 
	memcpy(&Inf->BuffData, Data, sizeof(Infrared_Data_t));
	__HAL_TIM_SET_COMPARE(&INF_CAL_TIM, INF_CHANNEL, INF_SET);
	HAL_TIM_PWM_Start_IT(&INF_CAL_TIM, INF_CHANNEL);
	return 0;
}
#endif


#ifdef INFRARED_RECV
Infrared_r_t infrared_r;
Infrared_r_t* Infrared_r = &infrared_r;
/****************************************************
	函数名:	INF_R_GPIO_EXTI_Callback
	功能:		红外接收数据外中断回调
	作者:		liyao 2017年8月16日
****************************************************/
void INF_R_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == INF_DATA_Pin){	
		if(Infrared_r->InfState == InfNULL_R)	//避免系统未初始化时突发中断
			return;
		if(Infrared_r->InfState == InfReady_R && HAL_GPIO_ReadPin(INF_DATA_GPIO_Port, INF_DATA_Pin) == GPIO_PIN_RESET){ //首个下降沿中断
			Infrared_r->InfState = InfStart_R;		//推动状态机
			INF_TIMER_START();//计时开始
		}else if(Infrared_r->InfState == InfStart_R && HAL_GPIO_ReadPin(INF_DATA_GPIO_Port, INF_DATA_Pin) == GPIO_PIN_SET){//继首个上升沿
			uint16_t Timed = INF_TIMER_GETCOUNT();
			if(Timed > INF_START_DELAY && Timed < (INF_START_DELAY+INF_DATA_DELAY)){//计算下降沿到上升沿经过的时间是否符合开始信号
				Infrared_r->InfState = InfRcvData_R;//推动状态机				
				INF_TIMER_START_CD(INF_DATA_DELAY); //倒计时开始
			}else{		//不符合开始信号重置状态机
				Infrared_r->InfState = InfReady_R;
				INF_TIMER_STOP();
			}
		}else if(Infrared_r->InfState == InfWaitHop_R){	//收到跳变信号 开启定时器计时中断
			Infrared_r->InfState = InfRcvData_R;			//推动状态机
			INF_TIMER_START_CD(INF_DATA_DELAY);				//倒计时开始
		}
	}
}

/****************************************************
	函数名:	INF_R_TIM_PeriodElapsedCallback
	功能:		红外计时所用定时器回调
	作者:		liyao 2017年8月16日
****************************************************/
void INF_R_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &INF_DELAY_TIM){
		HAL_TIM_Base_Stop_IT(&INF_DELAY_TIM);
		if(Infrared_r->InfState == InfRcvData_R){		//到达读数时间和状态
			Infrared_r->InfState = InfWaitHop_R;			//推动状态机
			uint8_t bit = (uint8_t)HAL_GPIO_ReadPin(INF_DATA_GPIO_Port, INF_DATA_Pin); //读数
			Infrared_r->Recv_Data |= (bit << (7 - Infrared_r->Recv_Cnt));							 //存数
			HAL_GPIO_TogglePin(SYNC_GPIO_Port, SYNC_Pin);	//反转调试线
			if(Infrared_r->Recv_Cnt++ == 7){	//1字节收满
				uint8_t* data = (uint8_t*)&Infrared_r->InfData;
				data[Infrared_r->InfIndex++] = Infrared_r->Recv_Data;//存进缓冲区
				if(Infrared_r->InfIndex == sizeof(Infrared_Data_t)){ //缓冲区存满
					Infrared_r->InfState = InfFullData_R;//标记数据已收满
					HAL_GPIO_WritePin( SYNC_GPIO_Port, SYNC_Pin,GPIO_PIN_RESET);
					return;
				}else{//缓冲区未存满继续接收下个字节
					Infrared_r->InfState = InfWaitHop_R;
					Infrared_r->Recv_Cnt = Infrared_r->Recv_Data = 0; 	 //清0
				}
			}
			INF_TIMER_START_CD(INF_DATA_TIMEOUT);				//跳变等待超时倒计时开始
		}else if(Infrared_r->InfState == InfWaitHop_R){//指定时间内未产生跳变 数据重置
			Infrared_r->InfState = InfReady_R;
			Infrared_r->Recv_Cnt = Infrared_r->Recv_Data = Infrared_r->InfIndex = 0; 	 //清0
		}
	}
}

/****************************************************
	函数名:	InfraredRecvData
	功能:		红外数据接收函数
	作者:		liyao 2017年8月16日
****************************************************/
int InfraredRecvData(Infrared_r_t* Inf_r, Infrared_Data_t* Data){
	if(Inf_r->InfState == InfFullData_R){
		memcpy(Data, &Inf_r->InfData, sizeof(Infrared_Data_t));
		Infrared_r->InfIndex = Infrared_r->Recv_Cnt = Infrared_r->Recv_Data = 0;
		Infrared_r->InfState = InfReady_R;
		return 1;
	}
	return -2;
}

/****************************************************
	函数名:	Infrared_r_Init
	功能:		红外数据接收初始化函数
	作者:		liyao 2017年8月16日
****************************************************/
void Infrared_r_Init(void){
	memset(Infrared_r, 0, sizeof(Infrared_r_t));
	Infrared_r->InfState = InfReady_R;//Infrared_r->InfIndex
}

#endif


#include "UltrasonicDriver.h"
extern Ultrasonic_T Ultrasonic;
void InfraredSendSelf(void){
	if(Ultrasonic.ID){
		Log.error("超声波未设置ID\r\n");
		return;
	}
	Infrared_Data_t data = {0};
	data.ID = Ultrasonic.ID;
	data.CMD = INF_SELF_CMD;
	data.Data = 0;
	data.CheckSum = (uint8_t)(data.ID+data.CMD+data.Data);
	InfraredSendData(Infrared_s, &data);
	
}











