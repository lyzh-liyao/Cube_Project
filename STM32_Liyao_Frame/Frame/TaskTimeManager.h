#ifndef __TASKTIMEMANAGER_H__
#define __TASKTIMEMANAGER_H__
#include "FrameConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

static void TaskTime_Monitor(void);
static void TaskTime_Monitor_Init(void);

#define	SYSTICK_COUNT SysTick->LOAD //9000
#define SYSTICK_1US_COUNT	SYSTICK_COUNT/1000//9
#define PRINT_HEAP() do{__iar_dlmalloc_stats();}while(0)

#ifdef	LOAD_MONITOR_ON  
	#define LOAD_TIMX 				TIM3
	#define ENABLE_TIMX 		  __HAL_RCC_TIM3_CLK_ENABLE()
	#define COUNTER_ON 				LOAD_TIMX->CR1 |= TIM_CR1_CEN
	#define COUNTER_OFF 			LOAD_TIMX->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN))
#endif

#define EXTI_IT_DISABLE() __set_PRIMASK(1)
#define EXTI_IT_ENABLE()  __set_PRIMASK(0)

#define SYSTICK_IT_DISABLE() SysTick->CTRL &= (~(1ul << SysTick_CTRL_TICKINT_Pos));
#define SYSTICK_IT_ENABLE()  SysTick->CTRL |= (1ul << SysTick_CTRL_TICKINT_Pos);

#define TimeCycle(s,ms) (s*1000 + ms)
#define TASK_MAX_COUNT 100
#define TASK_FIRST_DELAY 3//任务初次运行时间延迟
typedef enum {TASK_INIT, TASK_WAIT,TASK_READY, TASK_SUSPEND, TASK_RECOVER, TASK_REMOVE}TaskState; 
typedef enum {Real_Mode,Count_Mode, Single_Mode}TASK_MODE;//real模式下 任务在跨越了多个周期只运行一次 count模式下跨越多个周期会执行多次 严格执行次数 Single模式只执行1次
//任务信息描述
typedef struct _TaskTime_T TaskTime_T;
struct _TaskTime_T{
	int8_t 	Alias;				 	  //对外ID
	int8_t 	_TaskID;		  	//内部索引
	int8_t 	Priority;				  //优先级
	TASK_MODE TaskMode;				//任务模式
	TaskState _TaskState;			//任务状态
	int16_t 	_TaskCycleCount;//周期计数器
	int16_t 	TaskCycle;		 	//周期数
	void(*Run)(void);				 	//执行的函数
	int16_t		StoreCount;			//未能按时执行次数
	uint16_t	RunCount;			 	//执行次数
	uint16_t	RunElapsed;		 	//耗时 单位us
	TaskTime_T* _next;				//下一个任务指针
	TaskTime_T* _prev;				//上一个任务指针
};

typedef struct 
{
	uint8_t TaskSize;		//存在任务数量
	uint8_t TaskFreeSize;//空闲任务数量
	TaskTime_T TThead;	//列表头
}TimeTaskInfo_T;

extern uint64_t jiffies;
extern uint8_t TaskID;
extern void TaskTime_Run(void);
extern void BaseClock_Init(void);
extern void TaskTime_Init(void);
extern int8_t TaskTime_Add(int8_t alias, uint16_t TaskCycle, void(*Run)(void), TASK_MODE TaskMode);
extern int8_t TaskTime_Remove(int8_t id);
extern int8_t TaskTime_SuspendTask(int8_t id);
extern int8_t TaskTime_RecoverTask(int8_t id);  
extern void HAL_SYSTICK_Callback(void);
extern void DelayUS(int32_t);
extern void DelayMS(int32_t);
extern void DelayS(int32_t);



#endif

