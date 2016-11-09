#ifndef __LOG_H__
#define __LOG_H__
#include <stdint.h>
#include <stdlib.h>
#include "FrameConfig.h"
/*------------------------------LOG------------------------------*/
typedef struct _Log_T Log_T;
struct _Log_T{
	void(*info)(const char*);
	void(*waring)(const char*);
	void(*error)(const char*);
};

extern Log_T Log;
extern void Log_Init(void);


/*------------------------------MEM_LOG------------------------------*/

#define POINT_COUNT 500
#define MALLOC_CHECK(var, fun) do{if(var == NULL){ Log.error(#fun"ø’º‰∑÷≈‰ ß∞‹");}}while(0)
#define FREE_CHECK(var,len) do{}while(0);
	
//#define MALLOC(len) os_malloc(len)
//#define CALLOC(len, size) os_malloc(len)
//#define FREE(point) os_free(point)
#define MALLOC(len) malloc(len%2?len+1:len)
#define CALLOC(len, size) malloc(len%2?len+1:len) 
//#define MALLOC(len) malloc(len)
//#define CALLOC(len, size) malloc(len) 
#define FREE(point) free(point)
typedef struct{
	char* point;
	uint16_t len;
}Mem_Info_T;
 
extern uint32_t MEM_USE;
void* os_malloc(uint16_t len);
void* os_calloc(uint16_t len, uint16_t size);
void os_free(void* point);



#endif 

