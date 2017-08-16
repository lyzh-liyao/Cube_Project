#include "LOG.h" 
#include <stdarg.h> 
//------------------------------LOG------------------------------
Log_T Log = {0};

static void _error(const char* fmt, ...){ 
	va_list ap;
	va_start(ap, fmt);
	#ifdef LOG_OUT
		printf("Log->ERROR:"); 
		vprintf(fmt, ap);
	#endif
	va_end(ap);
}

static void _warning(const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	#ifdef LOG_OUT
		printf("Log->WARNING:"); 
		vprintf(fmt, ap);
	#endif
	va_end(ap);
}

static void _info(const char* fmt, ...){ 
	va_list ap;
	va_start(ap, fmt);
	#ifdef LOG_OUT
		printf("Log->INFO:"); 
		vprintf(fmt, ap);
	#endif
	va_end(ap);
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
	MEM_USE += len;
	return malloc(len);
  //mprintf("∑÷≈‰%d\r\n", len);
//	uint16_t is = 0; 
//	for(is = 0; is < POINT_COUNT; is++){
//		if(MemList[is].point == NULL){
//			MemList[is].point = malloc(len);
//			MemList[is].len	 = len;
//			MEM_USE += len;
//      if(MemList[is].point == NULL)
//        Log.error("malloc∑µªÿNULL\r\n");
//			//mprintf("s:%d\r\n", len);
//			return MemList[is].point;
//		}
//	}
//	Log.error("∑÷≈‰ ß∞‹\r\n");
//	return NULL;
}

void* os_calloc(uint16_t len, uint16_t size){
	MEM_USE += (len * size);
	return calloc(len, size);
	/*
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == NULL){
			MemList[is].point = calloc(len, size);
			MemList[is].len	 = len;
			MEM_USE += len;
			//mprintf("s:%d\r\n", len);
			return MemList[is].point;
		}
	}
	Log.error("∑÷≈‰ ß∞‹\r\n");
	return NULL;*/
}

void os_free(void* point){
	/*uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == point){
			MEM_USE -= MemList[is].len;
			//mprintf("r:%d\r\n", MemList[i].len);
			MemList[is].point = NULL;
			MemList[is].len	 = 0; 

			free(point);			
			return;
		}
	}*/
	free(point);
	//Log.error(" Õ∑≈ ß∞‹\r\n");
}



