#ifndef __CHIP_TOOLS__
#define __CHIP_TOOLS__
#include "stm32f0xx_hal.h"

#define TRUE 1
#define FALSE 0
#define BSET(offset) (1 << offset)			//offset位置1其余0
#define BRESET()		 (~(1 << offset))		//offset位置0其余1

extern uint32_t UID[3];
extern uint32_t UID_M;
extern void ChipTools_Init(void);
extern void CheckEndian(void);
#endif
