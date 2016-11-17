#ifndef __LIST_H__
#define __LIST_H__    
#include <stdint.h>
#include "FrameConfig.h"
typedef struct _List_Node_T List_Node_T;
struct _List_Node_T{
	void* Data;
	uint16_t Len;
	List_Node_T* Next;
};

typedef struct _List_Head_T List_Head_T;
struct _List_Head_T{
	List_Node_T* Head;
	List_Node_T* Tail;
	uint16_t Size;
	uint16_t Count;
};

extern List_Head_T* List_Init(uint16_t Size);
extern int8_t List_Add(List_Head_T* List_Head, void* Data, uint16_t Len);
extern int8_t List_Add_Tail(List_Head_T* List_Head, void* Data, uint16_t Len);
extern int8_t List_Remove(List_Head_T* List_Head, List_Node_T* List_Node);
extern int8_t List_Free(List_Head_T* List_Head);
#endif

