#ifndef __CHIP_TOOLS__
#define __CHIP_TOOLS__
#include "stm32f0xx_hal.h"
extern uint32_t UID[3];
extern uint32_t UID_M;
extern void ChipTools_Init(void);
extern void CheckEndian(void);
#endif
