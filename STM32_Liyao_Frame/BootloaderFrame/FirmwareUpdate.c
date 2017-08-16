#include "FirmwareUpdate.h"
#include "stm32f0xx_hal.h"
#include "iap.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash.h"

	
Firmware_Info_T _Firmware_Info = {0};
Firmware_Info_T* Firmware_Info = &_Firmware_Info;
int32_t bytecount = 0,basetime = 0;
char tmpData[4] = {0};
uint8_t Page_Count = 0,i = 0,data;
QUEUE_T* pro_queue;
uint8_t plan = 0;
char Program_Buffer[4096] = {0};
uint8_t isJump = 0;

void mprintf(char* ptr){
	while(0 != *ptr){
		LL_USART_TransmitData8(USART4, *ptr++);
		while((USART4->ISR & UART_FLAG_TC)==RESET) ;
	}
}

volatile int TargetTime = 0;
void SYSTICK_IRQHandler(void){
	if(TargetTime-- < 0)
		TargetTime= 0;
}

/****************************************************
	函数名:	_Uart_Init
	功能:		初始化IO和USART外设
	作者:		liyao 2017年8月9日    
****************************************************/
__weak void Uart_Init(void){
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART4);
	
	GPIOA->MODER |= 0x0A;
	GPIOA->OSPEEDR |= 0x0F;
	GPIOA->PUPDR |= 0x05;
	GPIOA->AFR[0] = 0x44;
	
  NVIC_SetPriority(USART3_6_IRQn, 0);
  NVIC_EnableIRQ(USART3_6_IRQn);
	
	USART4->CR1 = 0x0D;
	USART4->CR3 = 0x1000;
	USART4->BRR = 0x1A1;
	USART4->ISR = 0x6000D0;
}

/****************************************************
	函数名:	FirmwareUpdate_Init
	功能:		初始化在线更新功能框架
	作者:		liyao 2017年8月9日    
****************************************************/
void FirmwareUpdate_Init(void){
	Uart_Init();
	LL_SYSTICK_EnableIT();
	SET_BIT(USART->CR1, USART_CR1_RXNEIE);
	pro_queue = Queue_Init(Program_Buffer,sizeof(char),4096);
	memset(Firmware_Info, 0, sizeof(Firmware_Info_T));
}

/****************************************************
	函数名:	DelayMS
	功能:		延时函数
	作者:		liyao 2017年8月9日     
****************************************************/
void  DelayMS(int ms){
	TargetTime = ms;
	while(TargetTime > 0);
}
/*--------------------------FLASH操作函数原型-----------------------------*/
__INLINE void FLASH_PageErase(uint32_t PageAddress)
{
    /* Proceed to erase the page */
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    WRITE_REG(FLASH->AR, PageAddress);
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
}

static void FLASH_Program_HalfWord(uint32_t Address, uint16_t Data)
{
	SET_BIT(FLASH->CR, FLASH_CR_PG);
  *(__IO uint16_t*)Address = Data;
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

static void WaitBusy(void){ 
	while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
				CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
}
/*--------------------------FLASH操作函数END-----------------------------*/
void FirmwareUpdate_Run(void){
	#ifdef PRINT_ERR
		mprintf("bootloader start.......\r\n");
	#else
		mprintf("ok");
	#endif
	TargetTime = 5000;
	while(1){
		if(isJump || (Firmware_Info->State < 2 && TargetTime == 0)){ 
			mprintf("jump"); 
			DelayMS(10);
			iap_load_app(FLASH_APP_ADDR);
		}
		if(Firmware_Info->State == 7){//根据固件大小擦除flash页
			Page_Count = Firmware_Info->Firmware_Size/PAGE_SIZE+1;
				WRITE_REG(FLASH->KEYR, FLASH_KEY1);WRITE_REG(FLASH->KEYR, FLASH_KEY2);//			HAL_FLASH_Unlock();
			for(i = 0;i < Page_Count;i++){
				FLASH_PageErase(FLASH_APP_ADDR+(i*PAGE_SIZE));WaitBusy();//			HAL_FLASHEx_Erase(&f, &PageError);
			}
			Firmware_Info->State++;
			mprintf("ok");
			
		}
		
		if(Queue_Get(pro_queue,&data) == 0)//从队列中取出数据写入flash
		{
			Firmware_Info->Firmware_Real_Sum += data;//校验和累加计算
			tmpData[bytecount%4] = data;
			bytecount++;
			if(bytecount%4==0 && bytecount!=0){//4字节写入
				FLASH_Program_HalfWord(FLASH_APP_ADDR+(bytecount-4),tmpData[0]| (tmpData[1]<<8));WaitBusy();//HAL_FLASH_Program(FLASH_APP_ADDR+(bytecount-4), tmpData[0]| (tmpData[1]<<8) | (tmpData[2]<<16) | (tmpData[3]<<24));
				FLASH_Program_HalfWord(FLASH_APP_ADDR+(bytecount-2),tmpData[2]| (tmpData[3]<<8));WaitBusy();
				
			}
			
			
			if(bytecount >= (Firmware_Info->Firmware_Size*plan/100)){
					plan += 10;
				} 
			if(bytecount == Firmware_Info->Firmware_Size){
				Firmware_Info->State++;
				#ifdef PRINT_ERR
					mprintf("数据接收完毕,下一步准备校验和\r\n");
				#endif
			}
		}
		
		if(Firmware_Info->State == 9){ 
			SET_BIT(FLASH->CR, FLASH_CR_LOCK);//			FLASH_Lock();
			/*校验和*/
			if(Firmware_Info->Firmware_Real_Sum == Firmware_Info->Firmware_Check_Sum){
				#ifdef PRINT_ERR
				mprintf("固件校验和成功准备跳转\r\n");
				#endif
			}else{ 
				#ifdef PRINT_ERR
				mprintf("固件校验和失败\r\n");
				#else
				mprintf("error3");
				NVIC_SystemReset();
				#endif
				return;
			}
			DelayMS(10);
			mprintf("ok");
			iap_load_app(FLASH_APP_ADDR);
		}		
	}
}

/****************************************************
	函数名:	UART_IRQHandler
	功能:		串口中断处理函数
	作者:		liyao 2017年8月9日    
****************************************************/
uint8_t IsMark = 0;
uint8_t Check_Sum = 0;
void UART_IRQHandler(void){
if(((READ_REG(USART->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t CharRcv =  READ_REG(USART->RDR);
		//LL_USART_TransmitData8(USART,CharRcv);
	if(Firmware_Info->State < 8){
			if(IsMark == 1){
				if(CharRcv == 0x7d){
					CharRcv = 0xfd;
				}else if(CharRcv == 0x78){
					CharRcv = 0xf8;
				}else if(CharRcv == 0x7e){
					CharRcv = 0xfe;
				}
				IsMark = 0;
			}else	if(CharRcv == 0xFE){ 
				IsMark = 1;
				return;
			}
	}
		switch(Firmware_Info->State){
			case 0://接收帧头
				if(CharRcv == 0xfd)
					Firmware_Info->State++;
				else
					Firmware_Info->State = 0;
				Firmware_Info->Head = CharRcv;
				#ifdef PRINT_ERR
				mprintf("收到帧头\r\n");
				#endif
				break;
			case 1://接收帧类型
					Firmware_Info->State++;
					Firmware_Info->Type = CharRcv;
				if(Firmware_Info->Type != 0xA1 && Firmware_Info->Type != 0xA0)
					Firmware_Info->State = 0;
					#ifdef PRINT_ERR
					mprintf("收到帧类型\r\n");
					#endif
					break;
			case 2://接收固件大小高8位
				Firmware_Info->Firmware_Size = CharRcv << 8;
				Firmware_Info->State++;
				#ifdef PRINT_ERR
				mprintf("收到固件大小高8位\r\n");
				#endif
				break;
			case 3://接收固件大小低8位
				Firmware_Info->Firmware_Size |= CharRcv;
				Firmware_Info->State++;
				#ifdef PRINT_ERR
				mprintf("收到固件大小低8位\r\n"); 
				#endif
				break;
			case 4://接收固件校验和
				Firmware_Info->Firmware_Check_Sum = CharRcv;
				//Firmware_Info->Firmware_Real_Sum = Firmware_Info->Firmware_Size;
				Firmware_Info->State++;
				#ifdef PRINT_ERR 
				mprintf("收到固件校验和\r\n");
				#endif
				break; 
			case 5://接收帧校验和
				Firmware_Info->State++;
				Firmware_Info->Sum = CharRcv;
				#ifdef PRINT_ERR 
				mprintf("收到帧校验和\r\n");
				#endif
				break;
			case 6://接收帧尾，如有误则重置返回-1
				Check_Sum = 0;
				if(CharRcv == 0xf8){
					if(Firmware_Info->Type == 0xA0){
						Check_Sum += Firmware_Info->Type;
						Check_Sum += (Firmware_Info->Firmware_Size>>8);
						Check_Sum += (Firmware_Info->Firmware_Size&0xff);
						Check_Sum += Firmware_Info->Firmware_Check_Sum;
					}else if(Firmware_Info->Type == 0xA1){
						Check_Sum += Firmware_Info->Type; 
					}
					Firmware_Info->State++;
					#ifdef PRINT_ERR
					mprintf("收到帧尾");
					#endif
				}
				
				if(((uint8_t)Check_Sum) == Firmware_Info->Sum){
					if(Firmware_Info->Type == 0xA1)
						isJump = 1;
					break;
				}else{ 
					#ifdef PRINT_ERR
						mprintf("校验和有误,重新接收\r\n");
					#else
						mprintf("error1");
						NVIC_SystemReset();
					#endif
				}
				break;
			case 7://等待擦除flash 期间如果来数据则返回错误重置 
					#ifdef PRINT_ERR
						mprintf("擦除中收到数据，打断重新接收\r\n");
					#else
						mprintf("error2");
						NVIC_SystemReset();
					#endif
				break;
			case 8://接收固件数据
				Queue_Put(pro_queue, &CharRcv);
				break;
		}
	}
	
}








