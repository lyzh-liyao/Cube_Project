#include "TaskTimeManager.h"  
#include "LOG.h"
//static TaskTime_T TaskTimeLink[TASK_MAX_COUNT] = {0};
#ifdef LOAD_MONITOR_ON 
uint32_t TIMER_Tmp = 0, TIMER_Bak = 0;
#endif
TimeTaskInfo_T OSInfo;	//系统运行状态
TaskTime_T* TaskTime_Head = &OSInfo.TThead;//链表头  
uint8_t TaskID = 0;
uint64_t jiffies = 0;//自系统启动以来产生的节拍的总数
//临时变量
static TaskTime_T *tmpTaskTime,*prevTaskTime,*nextTaskTime;

/****************************************************
	函数名:	TaskTime_Check_ID
	功能:	
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
int8_t TaskTime_Check_ID(int8_t id){
	if(id > TASK_MAX_COUNT ){
		Log.error("TaskTime_Check_ID错误\r\n");
		return -1;
	}
	return 0;
};

///****************************************************
//	函数名:	BaseClock_Init
//	功能:	Systick基础时钟配置
//	作者:	liyao 2015年9月8日14:10:51
//****************************************************/
//void BaseClock_Init(){
//	//配置滴答寄存器
//	SysTick_Config(SYSTICK_COUNT);
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
//}


/****************************************************
	函数名:	TaskTime_Init
	功能:	定时任务队列初始化
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
void TaskTime_Init(){
	TaskTime_Head->_next = TaskTime_Head->_prev = TaskTime_Head;
	TaskTime_Head->Alias = -1;	 //任务id  
	TaskTime_Head->_TaskID = -1;	 //索引
	TaskTime_Head->Priority= -1;//优先级 
	TaskTime_Head->_TaskState = TASK_SUSPEND; //任务状态
	TaskTime_Head->TaskCycle = 1000;			//运行周期
	TaskTime_Head->_TaskCycleCount = TaskTime_Head->TaskCycle;//首次运行倒计时 
	TaskTime_Head->RunCount = 0;
	TaskTime_Head->RunElapsed = 0;
	TaskTime_Head->StoreCount = 0;
	#ifdef LOAD_MONITOR_ON
		TaskTime_Head->_TaskState = TASK_INIT; //任务状态
		TaskTime_Head->Run = TaskTime_Monitor;//任务函数指针
		TaskTime_Monitor_Init();
	#endif
	
	OSInfo.TaskFreeSize = TASK_MAX_COUNT;//空闲任务数
	OSInfo.TaskSize = 0;//运行任务数
//	for(i = 0; i < TASK_MAX_COUNT; i++){  
//		TaskTimeLink[i].Alias = -1;
//		TaskTimeLink[i]._TaskID = -1;
//	}
//	EXTI_IT_ENABLE();
};

/****************************************************
	函数名:	_TaskTime_AllocID
	功能:	分配任务ID
	作者:	liyao 2017年9月12日
****************************************************/
int8_t _TaskTime_AllocID(void){
	static int8_t seq = 0;
	Realloc:	
	seq = (seq+1)%TASK_MAX_COUNT;
	tmpTaskTime = TaskTime_Head->_next;
	while(tmpTaskTime != TaskTime_Head){
		if(tmpTaskTime->_TaskID  == seq){
			goto Realloc;
		}
		tmpTaskTime = tmpTaskTime->_next;
	} 
	return seq;
}
/****************************************************
	函数名:	TaskTime_Add
	功能:	添加定时任务
	参数:	识别号,周期ms,优先级,执行函数
	返回值:	>0：任务ID（成功） -1：任务满（失败）
	作者:	liyao 2016年8月5日11:52:16
****************************************************/
int8_t TaskTime_Add(int8_t alias,uint16_t TaskCycle ,void(*Run)(void), TASK_MODE TaskMode){
	TaskTime_T* newTaskTime = NULL;
	if(OSInfo.TaskSize == TASK_MAX_COUNT){//任务满
		printf("任务满\r\n");
		return -1; 
	}
	//for(i = 0; i<TASK_MAX_COUNT; i++){//寻找任务栈空位
	//	if(TaskTimeLink[i]._TaskID == -1){
	newTaskTime = MALLOC(sizeof(TaskTime_T));
	MALLOC_CHECK(newTaskTime,"TaskTime_Add");
	//newTaskTime = &TaskTimeLink[i];
	newTaskTime->Alias = alias;			 //任务id  
	newTaskTime->_TaskID = _TaskTime_AllocID();			 //索引
	newTaskTime->Priority		= alias;//优先级 Priority
	newTaskTime->_TaskState = TASK_INIT;		 //任务状态
	newTaskTime->TaskCycle = TaskCycle;//运行周期
	newTaskTime->_TaskCycleCount = (TaskCycle + alias * TASK_FIRST_DELAY);//首次运行倒计时 
	newTaskTime->Run = Run;//任务函数指针
	newTaskTime->RunCount = 0;
	newTaskTime->RunElapsed = 0; 
	newTaskTime->StoreCount = 0;
	newTaskTime->_next = NULL;
	newTaskTime->TaskMode = TaskMode;
	OSInfo.TaskSize++;
	OSInfo.TaskFreeSize--; 
 
 
	//首个任务
	if(TaskTime_Head->_next == TaskTime_Head){
		SYSTICK_IT_DISABLE();//中断关(防止中断访问) 
		TaskTime_Head->_next = TaskTime_Head->_prev = newTaskTime; 
		  newTaskTime->_next =   newTaskTime->_prev = TaskTime_Head;
		SYSTICK_IT_ENABLE(); 
	}else{//已有任务 
		tmpTaskTime = TaskTime_Head->_next;
		while(tmpTaskTime != TaskTime_Head){//新任务放入链表
			if(tmpTaskTime->Priority <= newTaskTime->Priority){//当前任务优先级比新任务优先级高则 
				tmpTaskTime = tmpTaskTime->_next; 
				if(tmpTaskTime == TaskTime_Head){//循环到最后依然没有匹配到
					SYSTICK_IT_DISABLE();//中断关(防止中断访问) 
					prevTaskTime = TaskTime_Head->_prev;
					prevTaskTime->_next = newTaskTime;			//	A_Next = N 
					tmpTaskTime->_prev = newTaskTime;				//	B_Prev = N
					newTaskTime->_prev = prevTaskTime;			//	N_Prev = A
					newTaskTime->_next = tmpTaskTime;				//	N_Next = B
					SYSTICK_IT_ENABLE(); 
				}
				continue;
			}else{//当前任务优先级比新任务优先级低或相等 	A_Next = B    B_Prev = A
				SYSTICK_IT_DISABLE();//中断关(防止中断访问) 
				prevTaskTime = tmpTaskTime->_prev;
				prevTaskTime->_next = newTaskTime;			//	A_Next = N 
				tmpTaskTime->_prev = newTaskTime;				//	B_Prev = N
				newTaskTime->_prev = prevTaskTime;			//	N_Prev = A
				newTaskTime->_next = tmpTaskTime;				//	N_Next = B 
				SYSTICK_IT_ENABLE(); 
				break;
			}
		}
	}
	return newTaskTime->_TaskID;
}

/****************************************************
	函数名:	_TaskTime_Remove
	功能:		移除任务
	参数:		添加任务时返回的任务id
	返回值:	>0：成功 -1：失败 
	作者:		liyao 2016年8月5日11:55:36
****************************************************/
static int8_t _TaskTime_Remove(int8_t id){
	if(TaskTime_Check_ID(id) < 0){
		return -1;
	}
	tmpTaskTime = TaskTime_Head->_next;
	while(tmpTaskTime != TaskTime_Head){
		if(tmpTaskTime->_TaskID  == id){
			SYSTICK_IT_DISABLE();//中断关(防止中断访问)
			prevTaskTime = tmpTaskTime->_prev;
			nextTaskTime = tmpTaskTime->_next;
			prevTaskTime->_next = nextTaskTime;
			nextTaskTime->_prev = prevTaskTime;
			SYSTICK_IT_ENABLE(); 
			OSInfo.TaskSize--;
			OSInfo.TaskFreeSize++;
			FREE(tmpTaskTime);
//			memset(tmpTaskTime, 0, sizeof(TaskTime_T));
//			tmpTaskTime->Alias = -1;
//			tmpTaskTime->_TaskID = -1;
			break;
		}
		tmpTaskTime = tmpTaskTime->_next;
	} 
	return 0;
}

/****************************************************
	函数名:	TaskTime_Remove
	功能:		移除任务(标记)
	参数:		添加任务时返回的任务id
	返回值:	>0：成功 -1：失败 
	作者:		liyao 2016年8月5日11:55:36
****************************************************/
int8_t TaskTime_Remove(int8_t id){ 
	TaskTime_T* Cur_Task = NULL;
	if(TaskTime_Check_ID(id) < 0)
		return -1;
	
	Cur_Task = TaskTime_Head->_next;
	while(Cur_Task != TaskTime_Head){
		if(Cur_Task->_TaskID == id){
			Cur_Task->_TaskState = TASK_REMOVE;
			return 0;
		}
		Cur_Task = Cur_Task->_next;
	} 
	return -2;
}

/****************************************************
	函数名:	TaskTime_SuspendTask
	功能:		挂起任务
	参数:		任务id
	返回值:	>0：成功 -1：失败 
	作者:		liyao 2016年8月5日11:55:36
****************************************************/
int8_t TaskTime_SuspendTask(int8_t id){ 
	TaskTime_T* Cur_Task = NULL;
	if(TaskTime_Check_ID(id) < 0)
		return -1;
	Cur_Task = TaskTime_Head->_next;
	while(Cur_Task != TaskTime_Head){
		if(Cur_Task->_TaskID == id){
			Cur_Task->_TaskState = TASK_SUSPEND;
			return 0;
		}
		Cur_Task = Cur_Task->_next;
	}  
	return -2;
}

/****************************************************
	函数名:	TaskTime_RecoverTask
	功能:		恢复挂起
	参数:		任务id 
	返回值:	>0：成功 -1：失败 
	作者:		liyao 2016年8月5日11:55:36
****************************************************/
int8_t TaskTime_RecoverTask(int8_t id){
	TaskTime_T* Cur_Task = NULL; 
	if(TaskTime_Check_ID(id) < 0)
		return -1;
	Cur_Task = TaskTime_Head->_next;
	while(Cur_Task != TaskTime_Head){
		if(Cur_Task->_TaskID == id){
			Cur_Task->_TaskState = TASK_RECOVER;
			return 0;
		}
		Cur_Task = Cur_Task->_next;
	}   
	return -2;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
	jiffies++;
	TaskTime_T *tmpTaskTimeH = TaskTime_Head; 
	#ifdef LOAD_MONITOR_ON
		COUNTER_ON; 
	#endif 
	do{
		switch(tmpTaskTimeH->_TaskState){
			case TASK_INIT:
			case TASK_WAIT:   
				if(--tmpTaskTimeH->_TaskCycleCount <= 0){
					tmpTaskTimeH->_TaskState = TASK_READY; //状态置为待执行 
					tmpTaskTimeH->_TaskCycleCount = tmpTaskTimeH->TaskCycle;//填充计数
				}
				break;
			case TASK_READY:
				if(--tmpTaskTimeH->_TaskCycleCount <= 0){
					tmpTaskTimeH->StoreCount++;	//累加未能执行次数
					tmpTaskTimeH->_TaskCycleCount = tmpTaskTimeH->TaskCycle;//填充计数
				}
				break;
			case TASK_SUSPEND: 
				break;
			case TASK_RECOVER: 
				tmpTaskTimeH->_TaskCycleCount = tmpTaskTimeH->TaskCycle;//填充计数
				//tmpTaskTimeH->RunCount = tmpTaskTimeH->TaskCycle;
				tmpTaskTimeH->_TaskState = TASK_WAIT;
			
				break;
			case TASK_REMOVE:  
				break;
			default:
				break;
		}
		tmpTaskTimeH = tmpTaskTimeH->_next; 
	}while(tmpTaskTimeH != TaskTime_Head);
//	#ifdef LOAD_MONITOR_ON
//		COUNTER_OFF; 
//	#endif 
}

/****************************************************
	函数名:	TaskTime_Run
	功能:		定时任务队列运行函数
	注意:		调用间隔必须大于最小任务的运行周期
	作者:		liyao 2016年8月5日14:37:00
****************************************************/
void TaskTime_Run(void){
	#ifdef LOAD_MONITOR_ON
		COUNTER_OFF;
	#endif
	tmpTaskTime = TaskTime_Head;
	do{ 
		switch(tmpTaskTime->_TaskState){
			case TASK_INIT:
				break;
			case TASK_WAIT:
				break;
			case TASK_READY:
				#ifdef LOAD_MONITOR_ON
					TIMER_Tmp = LOAD_TIMX->CNT;
					COUNTER_ON; 
				#endif
				tmpTaskTime->_TaskState = TASK_WAIT;
				do{
					tmpTaskTime->Run();
					tmpTaskTime->RunCount++;
				}while(--tmpTaskTime->StoreCount >= 0 && tmpTaskTime->TaskMode == Count_Mode);
				EXTI_IT_DISABLE();
				tmpTaskTime->StoreCount = 0;
				EXTI_IT_ENABLE();
				if(tmpTaskTime->TaskMode == Single_Mode){
					_TaskTime_Remove(tmpTaskTime->_TaskID);
					return;
				}
				#ifdef LOAD_MONITOR_ON
					COUNTER_OFF; 
					if(TIMER_Bak > 0){
						tmpTaskTime->RunElapsed += (TIMER_Bak - TIMER_Tmp);
						TIMER_Bak = 0;
					}else{
						uint16_t res = LOAD_TIMX->CNT - TIMER_Tmp;
						tmpTaskTime->RunElapsed += res;
					}
				#endif
				break;
			case TASK_SUSPEND: 
				break;
			case TASK_RECOVER: 
				break;
			case TASK_REMOVE: 
				_TaskTime_Remove(tmpTaskTime->_TaskID);
				return;
			default:
				break;
		}
		tmpTaskTime = tmpTaskTime->_next;
	}while(tmpTaskTime != TaskTime_Head);
}

#ifdef LOAD_MONITOR_ON 
/****************************************************
	函数名:	TaskTime_Monitor_Init
	功能:		负载监视定时器配置 初始化
	作者:		liyao 2016年8月8日16:41:22
****************************************************/
static void TaskTime_Monitor_Init(void){ 
	ENABLE_TIMX; 
  LOAD_TIMX->ARR = (uint32_t)0xFFFF ; 
#if MCU_TYPE == 103
  LOAD_TIMX->PSC = (uint32_t)7200;
#elif MCU_TYPE == 030 
  LOAD_TIMX->PSC = (uint32_t)4800;
#endif
	LOAD_TIMX->CNT = 0;
}

/****************************************************
	函数名:	TaskTime_Monitor
	功能:		负载监视定时器任务
	作者:		liyao 2016年8月8日16:41:22
****************************************************/
static void TaskTime_Monitor(void){
	float CPU_Use = 0;
	mprintf("-----系统监视-----\r\n");
	tmpTaskTime = TaskTime_Head;
	do{
		mprintf("编号:%d\t索引:%d\t优先级:%d\t间隔:%dms\t运行次数:%d\t超时次数:%d\t使用率:%.2f%%\r\n",
			tmpTaskTime->Alias,
			tmpTaskTime->_TaskID,
			tmpTaskTime->Priority,
			tmpTaskTime->TaskCycle,
			tmpTaskTime->RunCount,
			tmpTaskTime->StoreCount,
			tmpTaskTime->RunElapsed/100.0
		);
		SYSTICK_IT_DISABLE();
		tmpTaskTime->RunCount = tmpTaskTime->StoreCount = tmpTaskTime->RunElapsed = 0;
		SYSTICK_IT_ENABLE();
		tmpTaskTime = tmpTaskTime->_next;
	}while(tmpTaskTime != TaskTime_Head);
	CPU_Use = LOAD_TIMX->CNT/100.0;
	if(CPU_Use > 100.00)
		CPU_Use = 100.00;
	mprintf("CPU使用率：%.2f%%,计时器：%d\r\n", CPU_Use,LOAD_TIMX->CNT);
	//mprintf("时间戳:%d\r\n", TIM2->CNT);
	TIMER_Bak = LOAD_TIMX->CNT;
	LOAD_TIMX->CNT = 0;
}
#endif

/****************************************************
	函数名:	delayUS
	功能:	微秒延时
	参数:	微秒值
	作者:	liyao 2016年8月9日15:35:35
****************************************************/
void DelayUS(int32_t us){ 
	uint32_t SysTickVal = SysTick->VAL;
	uint32_t SysTickVal_Last = SysTickVal;
	us *= SYSTICK_1US_COUNT;
	while(us > 0){
		SysTickVal = SysTick->VAL;
		if(SysTickVal_Last > SysTickVal){
			us -= (SysTickVal_Last - SysTickVal);
		}else if(SysTickVal_Last < SysTickVal){
			us -= (SysTickVal_Last + (SYSTICK_COUNT - SysTickVal)); 
		} 
		SysTickVal_Last = SysTickVal;
	}
}

/****************************************************
	函数名:	delayMS
	功能:	毫秒延时
	参数:	毫秒值
	作者:	liyao 2016年8月9日15:34:42
****************************************************/
void DelayMS(int32_t ms){ 
	uint32_t SysTickVal = SysTick->VAL;
	uint32_t SysTickVal_Last = SysTickVal;
	ms *= SYSTICK_COUNT;
	while(ms > 0){ 
		SysTickVal = SysTick->VAL;
		if(SysTickVal_Last > SysTickVal){
			ms -= (SysTickVal_Last - SysTickVal);
		}else if(SysTickVal_Last < SysTickVal){
			ms -= (SysTickVal_Last + (SYSTICK_COUNT - SysTickVal)); 
		}
		SysTickVal_Last = SysTickVal;
	} 
}

/****************************************************
	函数名:	delayS
	功能:	秒延时
	参数:	秒值
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
void DelayS(int32_t s){ 
	uint32_t SysTickVal = SysTick->VAL;
	uint32_t SysTickVal_Last = SysTickVal;
	s *= SYSTICK_COUNT * 1000;
	while(s > 0){
		SysTickVal = SysTick->VAL;
		if(SysTickVal_Last > SysTickVal){
			s -= (SysTickVal_Last - SysTickVal);
		}else if(SysTickVal_Last < SysTickVal){
			s -= (SysTickVal_Last + (SYSTICK_COUNT - SysTickVal)); 
		} 
		SysTickVal_Last = SysTickVal;
	} 
}






