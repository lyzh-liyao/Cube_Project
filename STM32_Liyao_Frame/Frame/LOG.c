#include "LOG.h" 
#include <stdio.h>
#include <stdlib.h>
//------------------------------LOG------------------------------
Log_T Log = {0};

void _error(const char* message){ 
	#ifdef LOG_OUT
		printf("Log->ERROR:%s", message); 
	#endif
}

void _warning(const char* message){ 
	#ifdef LOG_OUT
		printf("Log->WARNING:%s", message); 
	#endif
}

void _info(const char* message){ 
	#ifdef LOG_OUT
		printf("Log->INFO:%s", message); 
	#endif
}

void Log_Init(void){
	Log.error = _error;
	Log.waring = _warning;
	Log.info = _info;
}

/*------------------------------MEM_LOG------------------------------*/
uint32_t MEM_USE = 0;
Mem_Info_T MemList[POINT_COUNT] = {0};


void* os_malloc(uint16_t len){
  
  if(len == 0)
    Log.error("mallocÎª0\r\n");
  //printf("·ÖÅä%d\r\n", len);
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == NULL){
			MemList[is].point = malloc(len);
			MemList[is].len	 = len;
			MEM_USE += len;
      if(MemList[is].point == NULL)
        Log.error("malloc·µ»ØNULL\r\n");
			//printf("s:%d\r\n", len);
			return MemList[is].point;
		}
	}
	Log.error("·ÖÅäÊ§°Ü\r\n");
	return NULL;
}

void* os_calloc(uint16_t len, uint16_t size){
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == NULL){
			MemList[is].point = calloc(len, size);
			MemList[is].len	 = len;
			MEM_USE += len;
			//printf("s:%d\r\n", len);
			return MemList[is].point;
		}
	}
	Log.error("·ÖÅäÊ§°Ü\r\n");
	return NULL;
}

void os_free(void* point){
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == point){
			MEM_USE -= MemList[is].len;
			//printf("r:%d\r\n", MemList[i].len);
			MemList[is].point = NULL;
			MemList[is].len	 = 0; 

			free(point);			
			return;
		}
	}
	free(point);
	Log.error("ÊÍ·ÅÊ§°Ü\r\n");
}



