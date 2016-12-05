#include "Queue.h"  
#include <string.h>
#include "LOG.h" 
/*
 *工具包
 *author：李尧 
 *time：2015年8月26日10:41:23
 */

//###################################队列相关函数###################################

QUEUE_T queue_list[QUEUE_MAXNUM] = {0}; 
uint8_t queue_locd = 0;
/****************************************************
	函数名:	queue_init
	功能:	队列初始化
	参数:	数组名，数组单个元素大小，数组元素个数
	返回值:	队列句柄 或 NULL
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
QUEUE_T* Queue_Init(void* _array,uint8_t _single_size ,uint16_t _count){ 
	uint8_t i = 0;
	QUEUE_T* queue = NULL;
	for(i = 0; i < QUEUE_MAXNUM; i++)
		if(queue_list[i].use_state == 0)
			queue = &queue_list[i];
	if(queue == NULL)
		return NULL;
	queue->use_state = 1;
	queue->single_size = _single_size;
	queue->count = _count;
	queue->start = queue->end = queue->num = queue->full_flag =0; 
	queue->data = _array;
	return queue;
}

/****************************************************
	函数名:	queue_put
	功能:	数据插入到队列
	参数:	队列句柄，数据
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
/*int8_t queue_put(QUEUE_T* queue,int32* _data){
		if(queue->num == queue->count)  
			return -1;
		switch(queue->single_size){ 
			case 1:((int8_t*)queue->data)[queue->start] = (int8_t)_data ;	break;
			case 2:((int16_t*)queue->data)[queue->start] = (int16_t)_data;	break;
			case 4:((int32_t*)queue->data)[queue->start] = (int32_t)_data;	break;
			default:return -1;
		}
		queue->start = ((++queue->start) % queue->count);
		queue->num ++;
		return 0;
}*/

/****************************************************
	函数名:	queue_get
	功能:	从队列取出数据
	参数:	队列句柄，数据容器（回填）
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
/*int8_t queue_get(QUEUE_T* queue, int32_t* _data){
		if(queue->num == 0) 
			return -1;
		switch(queue->single_size){
			case 1:*_data = ((int8_t*)queue->data)[queue->end];
										  ((int8_t*)queue->data)[queue->end] = 0;	break;
			case 2:*_data = ((int16_t*)queue->data)[queue->end];
											((int16_t*)queue->data)[queue->end] = 0;			break;
			case 4:*_data = ((int32_t*)queue->data)[queue->end];
											((int32_t*)queue->data)[queue->end] = 0;break;
			default:return -1;
		}
		queue->end = ((++queue->end) % queue->count);
		queue->num --;
		return 0;
}*/
#define QUEUE_LOCK do{if(queue->locked == 1) return -1;else queue->locked = 1;}while(0);
#define QUEUE_UNLOCK do{queue->locked = 0;}while(0);
/****************************************************
	函数名:	queue_put 
	功能:	数据插入到队列
	参数:	队列句柄，数据
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
int8_t Queue_Put(QUEUE_T* queue,void* _data){
		if(queue->full_flag == 1)  
			return -1;
		//QUEUE_LOCK;
		memcpy((char*)queue->data + (queue->start * queue->single_size),_data,queue->single_size);
		queue->start = ((++queue->start) % queue->count);
		if(queue->start == queue->end)
			queue->full_flag = 1;
		//queue->num ++;
		//QUEUE_UNLOCK
		return 0;
}

/****************************************************
	函数名:	queue_get
	功能:	从队列取出数据
	参数:	队列句柄，数据容器（回填）
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
int8_t Queue_Get(QUEUE_T* queue, void* _data){
		if(queue->full_flag == 0 && queue->start == queue->end) 
			return -1;
		//QUEUE_LOCK;
		memcpy(_data,(char*)queue->data + (queue->end * queue->single_size),queue->single_size);
		queue->end = ((++queue->end) % queue->count);
		queue->full_flag = 0;
		//queue->num --;
		//QUEUE_UNLOCK;
		return 0;
}

/****************************************************
	函数名:	queue_size
	功能:		获取队列当前成员个数
	参数:	队列句柄
	返回值：队列现有成员个数
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
uint16_t Queue_Size(QUEUE_T* queue){
	if(queue->full_flag == 1)
		return queue->count;
	else if(queue->start == queue->end)
		return 0;
	else if(queue->start > queue->end)
		return queue->start - queue->end;
	else if(queue->start < queue->end)
		return queue->count - queue->end + queue->start;
	return 0;
	//return queue->num;
}

/****************************************************
	函数名:	queue_empty_size
	功能:	获取队列剩余空位个数
	参数:	队列句柄
	返回值：队列剩余空位个数
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
uint16_t Queue_Empty_Size(QUEUE_T* queue){
	return queue->count - queue->num;
}

/****************************************************
	函数名:queue_free
	功能:	释放队列
	参数:	队列句柄
	作者:	liyao 2015年9月8日14:10:51
****************************************************/
void Queue_Free(QUEUE_T* queue){
	queue->use_state = 0;
}
/* Usage:
	QUEUE_T* queue = queue_init(test ,sizeof(uint8_t)  ,10 );  
	queue_put(queue,(int8_t)200,0,0);
	queue_put(queue,(int8_t)201,0,0);
	queue_get(queue,(int8_t*)&i,NULL,NULL); */

//###################################动态内存队列相关函数###################################

/****************************************************
	函数名:Queue_Link_Init
	功能:	获取一个链表队列
	作者:	liyao 2016年10月26日11:47:26
****************************************************/
Queue_Head_T* Queue_Link_Init(uint16_t Size){
	Queue_Head_T* queue = MALLOC(sizeof(Queue_Head_T));
	MALLOC_CHECK(queue, "Queue_Link_Init"); 
	memset(queue, 0, sizeof(Queue_Head_T));
	queue->Size = Size;
	return queue; 
}

/****************************************************
	函数名:Queue_Link_Put
	功能:	向队列中添加一个元素
	作者:	liyao 2016年10月26日
****************************************************/
int8_t Queue_Link_Put(Queue_Head_T* Queue_Head,void* Data, uint16_t Len){
	Queue_Pack_T* Queue_Pack = NULL;
	if(Queue_Head->Size > 0 && Queue_Head->Count == Queue_Head->Size){
		Log.error("队列满\r\n");
		return -1;
	}
	//分配Queue_Pack_T
	Queue_Pack = MALLOC(sizeof(Queue_Pack_T)); 
	MALLOC_CHECK(Queue_Pack, "Queue_Link_Put1"); 
	Queue_Pack->Data = CALLOC(Len, sizeof(uint8_t));
	MALLOC_CHECK(Queue_Pack->Data, "Queue_Link_Put2"); 
	//赋值Queue_Pack_T 
	Queue_Pack->Next = NULL;
	Queue_Pack->Len	 = Len;
	memcpy(Queue_Pack->Data, Data, Len); 
	//加入链表
	if(Queue_Head->Out == NULL){//首次加入
		Queue_Head->Out = Queue_Head->In = Queue_Pack;
	}else{//已有数据加入 
		Queue_Head->In->Next = Queue_Pack;
		Queue_Head->In = Queue_Pack;
	}
	Queue_Head->Count++;
	return 0;
}

/****************************************************
	函数名:Queue_Link_OutSize
	功能:	查询队列中将要获取的数据的长度
	作者:	liyao 2016年10月26日
****************************************************/
uint16_t Queue_Link_OutSize(Queue_Head_T* Queue_Head){
	if(Queue_Head->Out == NULL)//避免野指针
		return 0;
	return Queue_Head->Out->Len;
}

/****************************************************
	函数名:Queue_Link_GetData
	功能:	以数组的形式获取数据
	作者:	liyao 2016年10月26日
****************************************************/
int8_t Queue_Link_Get(Queue_Head_T* Queue_Head, void* Data){ 
	Queue_Pack_T* CurPack = Queue_Head->Out;
	if(Queue_Head->Out == NULL)
		return -1;
	//为回填指针赋值 
	memcpy(Data, CurPack->Data, CurPack->Len);
	//移动队列Out指针
	Queue_Head->Out = CurPack->Next;
	//释放队列区域空间
	FREE(CurPack->Data);
	FREE(CurPack);
	Queue_Head->Count--;
	return 0;
}

/****************************************************
	函数名:Queue_Link_Update
	功能:	更新队列指定元素数据
	作者:	liyao 2016年11月25日
****************************************************/ 
uint8_t Queue_Link_Update(Queue_Head_T* Queue_Head,void* Data, uint16_t Len, Compare cmp){
	Queue_Pack_T* tmp_pack = Queue_Head->Out;
	while(tmp_pack != NULL){
		if(cmp(tmp_pack->Data, tmp_pack->Len)){
			REALLOC(tmp_pack->Data, Len); 
			MALLOC_CHECK(tmp_pack->Data, "Queue_Link_Update"); 
			memcpy(tmp_pack->Data, Data, Len);
			return 1;
		}
		tmp_pack = tmp_pack->Next;
	}
	return 0;
}

/****************************************************
	函数名:Queue_Link_Free
	功能:	释放队列
	作者:	liyao 2016年10月26日
****************************************************/
void Queue_Link_Free(Queue_Head_T* Queue){
	Queue_Pack_T* cur = Queue->Out;
	Queue_Pack_T* next = NULL;
	while(cur != NULL){
		next = cur->Next;
		FREE(cur->Data);
		FREE(cur);
		cur = next;
	}
	if(Queue != NULL)
		FREE(Queue);
}















































