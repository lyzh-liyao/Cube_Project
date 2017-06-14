#include "main.h"
#include "mfrc522.h"
#include "stdint.h" 
#include "Log.h"
#include "spi.h"
#include "TaskTimeManager.h"
//#include <string.h>
//#include <stdio.h>
#define MAXRLEN 18          
/*驱动接口*/ 
/******** MFRC522 PIN description ************
*	SPI.NSS==UART.RX==IIC.SDA
*	SPI.SCK==UART.DTRQ==IIC.ADR1
*	SPI.SI==UART.MX==IIC.ADR0
*	SPI.SO==UART.TX==IIC.SCL
* *******************************************/
//#define     SET_RC522_RST  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET)
//#define     CLR_RC522_RST  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET)
//#define     RC522_SO   HAL_GPIO_ReadPin(MISO_GPIO_Port, MISO_Pin)	
//#define     SET_RC522_SI  HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, GPIO_PIN_SET)     
//#define     CLR_RC522_SI  HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, GPIO_PIN_RESET)
//#define     SET_RC522_SCK  HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET)
//#define     CLR_RC522_SCK  HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET)
//#define     SET_RC522_NSS  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET)
//#define     CLR_RC522_NSS  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET)

#define RST522_1 HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET)
#define RST522_0 HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET)
#define NSS_1() HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET)
#define NSS_0() HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET)
#define _NOP() __NOP()

//void test2(){CLR_RC522_SI;CLR_RC522_SCK;SET_RC522_NSS;while(1);}
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
	
   ClearBitMask(Status2Reg,0x08);
   WriteRawRC(BitFramingReg,0x07);
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
	char status=0;
	unsigned int  unLen;
	unsigned char ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	//CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char UnlockBlock0(void)
{
	char status=0;
	unsigned int  unLen;
	unsigned char ucComMF522Buf[4] = {0}; 
 
	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  if (status != MI_OK)
	{
		printf("PcdHalt != MI_OK\r\n");  
	}else{
		printf("卡休眠\r\n"); 
	}
	printf("recv1:%d  %X, %X, %X, %X\r\n", unLen,ucComMF522Buf[0],ucComMF522Buf[1],ucComMF522Buf[2],ucComMF522Buf[3]);
	unLen = 0;
	//-----------
//	ClearBitMask(Status2Reg,0x08);
//  SetBitMask(TxControlReg,0x03);
	
	
  WriteRawRC(BitFramingReg,0x07);
	memset(ucComMF522Buf, 0, 4);
  ucComMF522Buf[0] = 0x40;
	ucComMF522Buf[1] = 0x00;
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
	printf("recv1:%d  %X, %X, %X, %X\r\n", unLen,ucComMF522Buf[0],ucComMF522Buf[1],ucComMF522Buf[2],ucComMF522Buf[3]);
	unLen = 0;
	 
  
  WriteRawRC(BitFramingReg,0x00);
	memset(ucComMF522Buf, 0, 4);
	ucComMF522Buf[0] = 0x43;
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
	printf("recv1:%d  %X, %X, %X, %X\r\n", unLen,ucComMF522Buf[0],ucComMF522Buf[1],ucComMF522Buf[2],ucComMF522Buf[3]);
	unLen = 0;
	
	
	
	uint8_t data[] = {0xDE, 0x4B, 0xA2, 0x69, 0x5E, 0x08, 0x04, 0x00, 0x01, 0x0D, 0x63, 0xE3, 0xC1, 0xBB, 0x77, 0x1D};
	status = PcdWrite(0, data);
	printf("recv1:%d  %X, %X, %X, %X\r\n", unLen,ucComMF522Buf[0],ucComMF522Buf[1],ucComMF522Buf[2],ucComMF522Buf[3]);
	unLen = 0;
//	ucComMF522Buf[0] = 0xa0;
//	ucComMF522Buf[1] = 0x00;
//	ucComMF522Buf[2] = 0x5f;
//	ucComMF522Buf[3] = 0xb1; 
//	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//  if (status == MI_OK)
//  {    
//  } 

	return status;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
//    SET_RC522_RST;
//    DelayUS(10);
//    CLR_RC522_RST;
//    DelayUS(10);
//    SET_RC522_RST;
//    DelayUS(10);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    DelayUS(10);
    
    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);     
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
	if (type == 'A')                     //ISO14443_A
	{ 
		ClearBitMask(Status2Reg,0x08);

 /*     WriteRawRC(CommandReg,0x20);    //as default   
       WriteRawRC(ComIEnReg,0x80);     //as default
       WriteRawRC(DivlEnReg,0x0);      //as default
	   WriteRawRC(ComIrqReg,0x04);     //as default
	   WriteRawRC(DivIrqReg,0x0);      //as default
	   WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
	   WriteRawRC(WaterLevelReg,0x08); //as default
       WriteRawRC(ControlReg,0x20);    //as default
	   WriteRawRC(CollReg,0x80);    //as default
*/
		WriteRawRC(ModeReg,0x3D);//3F
/*	   WriteRawRC(TxModeReg,0x0);      //as default???
	   WriteRawRC(RxModeReg,0x0);      //as default???
	   WriteRawRC(TxControlReg,0x80);  //as default???

	   WriteRawRC(TxSelReg,0x10);      //as default???
   */
		WriteRawRC(RxSelReg,0x86);//84
 //      WriteRawRC(RxThresholdReg,0x84);//as default
 //      WriteRawRC(DemodReg,0x4D);      //as default

 //      WriteRawRC(ModWidthReg,0x13);//26
		WriteRawRC(RFCfgReg,0x7F);   //4F
	/*   WriteRawRC(GsNReg,0x88);        //as default???
	   WriteRawRC(CWGsCfgReg,0x20);    //as default???
       WriteRawRC(ModGsCfgReg,0x20);   //as default???
*/
		WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		WriteRawRC(TReloadRegH,0);
		WriteRawRC(TModeReg,0x8D);
		WriteRawRC(TPrescalerReg,0x3E);
		
  //     PcdSetTmo(106);
		DelayMS(10);
		PcdAntennaOn();
	}
	else{ return (char)-1; }

	return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char res = 0; 
	Address = ((Address<<1)&0x7E)|0x80;
//	NSS_0(); 
//	if(HAL_SPI_TransmitReceive(&hspi1, &Address, &res, 1, 100) != HAL_OK)
//		Log.error("HAL_SPI_TransmitReceive error\r\n");  
//		
//	NSS_1(); 
	NSS_0(); 
	if(HAL_SPI_Transmit(&hspi1, &Address, 1, 100) != HAL_OK)
		Log.error("HAL_SPI_Transmit error\r\n");  
	if(HAL_SPI_Receive(&hspi1, &res,  1, 100) != HAL_OK)
		Log.error("HAL_SPI_Receive error\r\n");  
	NSS_1(); 
	return res; 
     /*unsigned char i, ucAddr;
     unsigned char ucResult=0;

     CLR_RC522_SCK;
     CLR_RC522_NSS;
     ucAddr = ((Address<<1)&0x7E)|0x80;

     for(i=8;i>0;i--)
     {
				if((ucAddr&0x80)==0x80)
					SET_RC522_SI;
				else
					CLR_RC522_SI;
         SET_RC522_SCK;
         ucAddr <<= 1;
         CLR_RC522_SCK;
     }

     for(i=8;i>0;i--)
     {
         SET_RC522_SCK;
         ucResult <<= 1;
         ucResult |= RC522_SO;
         CLR_RC522_SCK;
     }

     SET_RC522_NSS;
     SET_RC522_SCK;
     return ucResult;*/
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)	//降低SPI的写速度很重要，不然有可能寻不到卡
{  
	Address = ((Address<<1)&0x7E);
	unsigned char data[] = {0,0};
	data[0] = Address;
	data[1] = value;
//	NSS_0();
//	if(HAL_SPI_Transmit(&hspi1, data, 2, 100) != HAL_OK)
//		Log.error("HAL_SPI_Transmit error1\r\n");   
//	NSS_1();
	NSS_0();
	if(HAL_SPI_Transmit(&hspi1, &Address, 1, 100) != HAL_OK)
		Log.error("HAL_SPI_Transmit error1\r\n"); 
	if(HAL_SPI_Transmit(&hspi1, &value, 1, 100) != HAL_OK)
		Log.error("HAL_SPI_Transmit error2\r\n");
	NSS_1(); 
	/*unsigned char i, ucAddr;
	CLR_RC522_SCK;
	CLR_RC522_NSS;
	ucAddr = ((Address<<1)&0x7E);

	for(i=8;i>0;i--)
	{
			if((ucAddr&0x80)==0x80)
				SET_RC522_SI;
			else
				CLR_RC522_SI;
			DelayUS(1);
			SET_RC522_SCK;
			ucAddr <<= 1;
			DelayUS(1);
			CLR_RC522_SCK;
			DelayUS(1);
	}

	for(i=8;i>0;i--)
	{
		if((value&0x80)==0x80)
			SET_RC522_SI;
		else
			CLR_RC522_SI;
			SET_RC522_SCK;
			DelayUS(1);
			value <<= 1;
			CLR_RC522_SCK;
			DelayUS(1);
	}
	SET_RC522_NSS;
	SET_RC522_SCK;*/
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char  Command, 
                  unsigned char *pInData, 
                  unsigned char InLenByte,
                  unsigned char *pOutData, 
                  unsigned int *pOutLenBit)
{
	char  status = MI_ERR;
	unsigned char  irqEn   = 0x00;
	unsigned char  waitFor = 0x00;
	unsigned char  lastBits;
	unsigned char  n;
	unsigned int i;
	switch (Command)
	{
			case PCD_AUTHENT:
				 irqEn   = 0x12;
				 waitFor = 0x10;
				 break;
			case PCD_TRANSCEIVE:
				 irqEn   = 0x77;
				 waitFor = 0x30;
				 break;
			default:
				 break;
	}
    
	WriteRawRC(ComIEnReg,irqEn|0x80);  //使能接受和发送中断请求
	ClearBitMask(ComIrqReg,0x80);     //置ComIrqReg为0xff,                
	WriteRawRC(CommandReg,PCD_IDLE); //取消当前命令
	SetBitMask(FIFOLevelReg,0x80);        
     
  for (i=0; i<InLenByte; i++)
  { 
		WriteRawRC(FIFODataReg, pInData[i]); }
    WriteRawRC(CommandReg, Command);
    if (Command == PCD_TRANSCEIVE)
    { 
			SetBitMask(BitFramingReg,0x80); 
		}          //开始发送
     i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
     do 
     {
				n = ReadRawRC(ComIrqReg);
				i--;
				//printf("i=%d,n=0x%x,waitFor=%d\r\n",i,n,waitFor);
     }
     while ((i!=0) && !(n&0x01) && !(n&waitFor));
     ClearBitMask(BitFramingReg,0x80);  //发送结束

		if (i!=0)
		{  
			if(!(ReadRawRC(ErrorReg)&0x1B))
			{
				status = MI_OK;
				if (n & irqEn & 0x01)
				{ status = MI_NOTAGERR;  }//printf("n=%x\r\n",n);
				if (Command == PCD_TRANSCEIVE)
				{
					n = ReadRawRC(FIFOLevelReg);
					lastBits = ReadRawRC(ControlReg) & 0x07;   //得出接受字节中的有效位，如果为0，全部位都有效
					if (lastBits)
					{ *pOutLenBit = (n-1)*8 + lastBits; }
					else
					{ *pOutLenBit = n*8; }
					if (n == 0)
					{ n = 1; }
					if(n > MAXRLEN)
					{ n = MAXRLEN; }
					for (i=0; i<n; i++)
					{ pOutData[i] = ReadRawRC(FIFODataReg); }
				}
			}
			else
			{ status = MI_ERR; }         
		}
		//printf("return:status=%d,pOutLenBit=%d\r\n",status,*pOutLenBit);
		SetBitMask(ControlReg,0x80);           // stop timer now
		WriteRawRC(CommandReg,PCD_IDLE); 
		return status;
} 
char PcdComMF5222(unsigned char  Command, 
                  unsigned char *pInData, 
                  unsigned char InLenByte,
                  unsigned char *pOutData, 
                  unsigned int *pOutLenBit)
{
	char  status = MI_ERR;
	unsigned char  irqEn   = 0x00;
	unsigned char  waitFor = 0x00;
	unsigned char  lastBits;
	unsigned char  n;
	unsigned int i;
	switch (Command)
	{
			case PCD_AUTHENT:
				 irqEn   = 0x12;
				 waitFor = 0x10;
				 break;
			case PCD_TRANSCEIVE:
				 irqEn   = 0x77;
				 waitFor = 0x30;
				 break;
			default:
				 break;
	}
    
	WriteRawRC(ComIEnReg,irqEn|0x80);  //使能接受和发送中断请求
	ClearBitMask(ComIrqReg,0x80);     //置ComIrqReg为0xff,                
	WriteRawRC(CommandReg,PCD_IDLE); //取消当前命令
	SetBitMask(FIFOLevelReg,0x80);        
     
  for (i=0; i<InLenByte; i++)
  { 
		WriteRawRC(FIFODataReg, pInData[i]); }
    WriteRawRC(CommandReg, Command);
    if (Command == PCD_TRANSCEIVE)
    { 
			SetBitMask(BitFramingReg,0x80); 
		}          //开始发送
     i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
     do 
     {
				n = ReadRawRC(ComIrqReg);
				i--;
				printf("i=%d,n=0x%x,waitFor=%d\r\n",i,n,waitFor);
     }
     while ((i!=0) && !(n&0x01) && !(n&waitFor));
     ClearBitMask(BitFramingReg,0x80);  //发送结束

		if (i!=0)
		{  
			if(!(ReadRawRC(ErrorReg)&0x1B))
			{
				status = MI_OK;
				if (n & irqEn & 0x01)
				{ status = MI_NOTAGERR;  }//printf("n=%x\r\n",n);
				if (Command == PCD_TRANSCEIVE)
				{
					n = ReadRawRC(FIFOLevelReg);
					lastBits = ReadRawRC(ControlReg) & 0x07;   //得出接受字节中的有效位，如果为0，全部位都有效
					if (lastBits)
					{ *pOutLenBit = (n-1)*8 + lastBits; }
					else
					{ *pOutLenBit = n*8; }
					if (n == 0)
					{ n = 1; }
					if(n > MAXRLEN)
					{ n = MAXRLEN; }
					for (i=0; i<n; i++)
					{ pOutData[i] = ReadRawRC(FIFODataReg); }
				}
			}
			else
			{ status = MI_ERR; }         
		}
		//printf("return:status=%d,pOutLenBit=%d\r\n",status,*pOutLenBit);
		SetBitMask(ControlReg,0x80);           // stop timer now
		WriteRawRC(CommandReg,PCD_IDLE); 
		return status;
} 
/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
		DelayMS(3);
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
  ClearBitMask(TxControlReg, 0x03);
	DelayMS(3);
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
       // memcpy(ucComMF522Buf, pValue, 4);
			ucComMF522Buf[0]=pValue[0],ucComMF522Buf[1]=pValue[1],ucComMF522Buf[2]=pValue[2],ucComMF522Buf[3]=pValue[3];
 //       for (i=0; i<16; i++)
 //       {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}
