#ifndef __QUEUE_H
#define __QUEUE_H  
#include <stdint.h>
 

//###################################队列###################################
#define QUEUE_MAXNUM 10

/****************************************************
	结构体名:	QUEUE_T
	功能:	队列头实体
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
	void* data;
	
	uint8_t single_size;
	uint16_t count;
	
	uint16_t start;
	uint16_t end;
	uint16_t num;
	uint8_t full_flag;
	
	uint8_t use_state; 
	uint8_t locked;
}QUEUE_T;

extern QUEUE_T* Queue_Init(void* _array,uint8_t _single_size ,uint16_t _count);
/*extern int8_t queue_put(QUEUE_T* , int32_t );
extern int8_t queue_get(QUEUE_T* , int32_t* );*/
extern int8_t Queue_Put(QUEUE_T* queue,void* _data);
extern int8_t Queue_Get(QUEUE_T* queue,void* _data);
extern uint16_t Queue_Size(QUEUE_T* queue);
extern uint16_t Queue_Empty_size(QUEUE_T* queue);
extern void Queue_Free(QUEUE_T* queue);


//###################################动态内存队列相关函数################################### 
/*每个节点12字节开销*/

typedef uint8_t (*Compare)(void* Data, uint16_t Len);
typedef struct _Queue_Pack_T Queue_Pack_T; 
struct _Queue_Pack_T{
	void* Data;
	uint16_t Len;
	Queue_Pack_T* Next;
};

typedef struct{
	Queue_Pack_T* Out;
	Queue_Pack_T* In;
	uint16_t Size;
	uint16_t Count;
}Queue_Head_T;

extern Queue_Head_T* Queue_Link_Init(uint16_t Size);
extern int8_t Queue_Link_Put(Queue_Head_T* Queue_Head,void* Data, uint16_t Len);
extern uint16_t Queue_Link_OutSize(Queue_Head_T* Queue_Head);
extern int8_t Queue_Link_Get(Queue_Head_T* Queue_Head, void* Data);
extern uint8_t Queue_Link_Update(Queue_Head_T* Queue_Head,void* Data, uint16_t Len, Compare cmp);
extern void Queue_Link_Free(Queue_Head_T* Queue_Head);


#endif
