#include "List.h" 
#include <string.h>
#include "LOG.h" 

/*每个节点12字节开销*/
/****************************************************
	函数名:List_Init
	功能:	获取一个链表头
	作者:	liyao 2016年10月26日11:47:26
****************************************************/
List_Head_T* List_Init(uint16_t Size){
	List_Head_T* ListHead = MALLOC(sizeof(List_Head_T));
	MALLOC_CHECK(ListHead, "List_Init"); 
	memset(ListHead, 0, sizeof(List_Head_T));
	ListHead->Size = Size;
	return ListHead;
}

/****************************************************
	函数名:List_Add
	功能:	向链表头部添加一个元素
	作者:	liyao 2016年10月26日
****************************************************/
int8_t List_Add(List_Head_T* List_Head, void* Data, uint16_t Len){
	List_Node_T* ListNode = NULL;
	if(List_Head->Size > 0 && List_Head->Count == List_Head->Size){
		Log.error("链表满\r\n");
		return -1;
	}
	//分配ListNode_T
	ListNode = MALLOC(sizeof(List_Node_T)); 
	MALLOC_CHECK(ListNode, "List_Add"); 
	ListNode->Data = MALLOC(Len);
	MALLOC_CHECK(ListNode->Data, "List_Add"); 
	//赋值ListNode_T 
	ListNode->Next = NULL;
	ListNode->Len	 = Len;
	memcpy(ListNode->Data, Data, Len); 
	//加入链表
	if(List_Head->Head == NULL){//首次加入
		List_Head->Head = List_Head->Tail = ListNode;
	}else{//已有数据加入 
		ListNode->Next = List_Head->Head; 
		List_Head->Head = ListNode;
	}
	List_Head->Count++;
	return 0;
}

/****************************************************
	函数名:List_Add_Tail
	功能:	向链表尾部添加一个元素
	作者:	liyao 2016年10月26日
****************************************************/
int8_t List_Add_Tail(List_Head_T* List_Head, void* Data, uint16_t Len){
List_Node_T* ListNode = NULL;
	if(List_Head->Size > 0 && List_Head->Count == List_Head->Size){
		Log.error("链表满\r\n");
		return -1;
	}
	//分配ListNode_T
	ListNode = MALLOC(sizeof(List_Node_T)); 
	MALLOC_CHECK(ListNode, "List_Add_Tail"); 
	ListNode->Data = MALLOC(Len);
	MALLOC_CHECK(ListNode->Data, "List_Add_Tail"); 
	//赋值ListNode_T 
	ListNode->Next = NULL;
	ListNode->Len	 = Len;
	memcpy(ListNode->Data, Data, Len); 
	//加入链表
	if(List_Head->Head == NULL){//首次加入
		List_Head->Head = List_Head->Tail = ListNode;
	}else{//已有数据加入 
		List_Head->Tail->Next = ListNode;
		List_Head->Tail = ListNode;
	}
	List_Head->Count++;
	return 0;
}

/****************************************************
	函数名:List_Remove
	功能: 移除链表节点
	作者:	liyao 2016年10月26日
****************************************************/
int8_t List_Remove(List_Head_T* List_Head, List_Node_T* List_Node){
	List_Node_T* cur = List_Head->Head;
	List_Node_T* last = NULL;
	List_Node_T* next = NULL;
	while(cur != NULL){
		if(cur == List_Node){
			next = List_Node->Next;
			FREE(cur->Data);
			FREE(cur); 
			if(last != NULL)
				last->Next = next;
			return 0;
			
		}
		last = cur;
		cur = cur->Next;
	}
	Log.waring("List_Remove未找到节点\r\n");
	return -1;
}

/****************************************************
	函数名:List_Free
	功能:	释放链表
	作者:	liyao 2016年10月26日
****************************************************/
int8_t List_Free(List_Head_T* List_Head){
	List_Node_T* cur = List_Head->Head;
	List_Node_T* next = NULL;
	while(cur != NULL){
		next = cur->Next;
		FREE(cur->Data);
		FREE(cur);
		cur = next;
	}
	if(List_Head != NULL)
		FREE(List_Head);
	return 0;
}


