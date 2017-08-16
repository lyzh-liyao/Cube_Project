#include "iap.h"
#include <string.h>
#include "stm32f0xx_hal.h" 
IAP_FUN  JumpApp;
__ASM void MSR_MSP(unsigned int addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


//跳转到应用程序段
//AppAddr:用户代码起始地址.
void iap_load_app(unsigned int AppAddr)
{
	if(((*(unsigned int*)AppAddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{
		JumpApp = (IAP_FUN)*(unsigned int*)(AppAddr+4);				//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(unsigned int*)AppAddr);					     				//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		__set_PRIMASK(1);															 				// 关闭全局中断
		JumpApp();									    						   				//跳转到APP.
	}
}


