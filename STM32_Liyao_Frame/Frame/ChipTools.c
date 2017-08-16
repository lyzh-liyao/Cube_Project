#include "ChipTools.h"
#include <stdlib.h>
#include <string.h>
uint32_t UID[3];
uint32_t UID_M;
void ChipTools_Init(void){
	/*----------获取芯片唯一ID------------*/
	memcpy(UID, (void*)UID_ADDR, 12);
	UID_M=(UID[0]>>1)+(UID[1]>>2)+(UID[2]>>3);
	srand(UID_M); 
}
/****************************************************
	函数名:	CheckEndian
	功能:		大小端判别
	作者:		liyao 2017年7月26日21:15:10 
****************************************************/
void CheckEndian(void)
{  
	union { char c[4]; unsigned long l; }
	endian_test = { { 'l', '?', '?', 'b' } };
  printf("%c\r\n", ((char)endian_test.l));
}  



