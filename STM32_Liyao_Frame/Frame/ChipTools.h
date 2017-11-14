#ifndef __CHIP_TOOLS__
#define __CHIP_TOOLS__

#include "FrameConfig.h" 
#if MCU_TYPE == 030
	#include "stm32f0xx_hal.h"
#elif MCU_TYPE == 103
	#include "stm32f1xx_hal.h"
#endif
#define TRUE 1
#define FALSE 0
#define BSET(offset) 	 				(1 << offset)			//offset位置1其余0
#define BRESET(offset) 				(~(1 << offset))		//offset位置0其余1
#define BitSet(Data, index) 	((Data) | BSET(index))
#define BitReSet(Data, index) ((Data) &  BRESET(index))

#define BitGet(Data, offset)				(((Data) >> (offset)) & 0x01)
#define NBitGet(Data, offset)				((Data)^ (0x01<<(offset)))

#define VECTOR_SIZE 48*4
#define FLASH_APP_ADDR		0x08001000  	//第一个应用程序起始地址(存放在FLASH)
#if MCU_TYPE == 030
	#define UID_ADDR 0x1FFFF7AC
#elif MCU_TYPE == 103
	#define UID_ADDR 0x1FFFF7E8
#endif


extern uint32_t UID[3];
extern uint32_t UID_M;
extern void ChipTools_Init(void);
extern void CheckEndian(void);
extern void SYSCFG_MemoryRemapConfig(uint32_t SYSCFG_MemoryRemap);
int litter_big_convert(uint8_t* Dest, const uint8_t* Src, int length);
#endif
