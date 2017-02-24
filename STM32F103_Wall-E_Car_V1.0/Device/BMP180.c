///*  $Date: 2011/09/15 15:09:57 $
// *  Revision: 1.14 */
///*
//* Copyright (C) 2011 Bosch Sensortec GmbH
//*
//* BMP180 pressure sensor API
//*
//* Usage:  Application Programming Interface for BMP180 configuration
//*         and data read out

//* Author:    venkatakrishnan.venkataraman@in.bosch.com

//* Licensed under the Apache License, Version 2.0 (the "License"); you may not
//  use this file except in compliance with the License and the following
//  stipulations.The Apache License , Version 2.0 is applicable unless otherwise
//  stated by the stipulations of the disclaimer below.

//* You may obtain a copy of the License at

//   http://www.apache.org/licenses/LICENSE-2.0

//* Disclaimer

//* Common:
//* Bosch Sensortec products are developed for the consumer goods industry. They
//* may only be used within the parameters of the respective valid product data
//* sheet.Bosch Sensortec products are provided with the express understanding
//* that there is no warranty of fitness for a particular purpose.They are not
//* fit for use in life-sustaining, safety or security sensitive systems or any
//* system or device that may lead to bodily harm or property damage if the
//* system or device malfunctions. In addition,Bosch Sensortec products are not
//* fit for use in products which interact with motor vehicle systems.The resale
//* and/or use of products are at the purchaser's own risk and his own
//* responsibility. The examination of fitness for the intended use is the sole
//* responsibility of the Purchaser.
//*
//* The purchaser shall indemnify Bosch Sensortec from all third party claims,
//* including any claims for incidental, or consequential damages, arising from
//* any product use not covered by the parameters of the respective valid
//* product data sheet or not approved by Bosch Sensortec and reimburse Bosch
//* Sensortec for all costs in connection with such claims.
//*
//* The purchaser must monitor the market for the purchased products,
//* particularly with regard to product safety and inform Bosch Sensortec
//* without delay of all security relevant incidents.
//*
//* Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
//* from the valid technical specifications of the product series. They are
//* therefore not intended or fit for resale to third parties or for use in end
//* products. Their sole purpose is internal client testing. The testing of an
//* engineering sample may in no way replace the testing of a product series.
//* Bosch Sensortec assumes no liability for the use of engineering samples. By
//* accepting the engineering samples, the Purchaser agrees to indemnify Bosch
//* Sensortec from all claims arising from the use of engineering samples.
//*
//* Special:
//* This software module (hereinafter called "Software") and any information on
//* application-sheets (hereinafter called "Information") is provided free of
//* charge for the sole purpose to support your application work. The Software
//* and Information is subject to the following terms and conditions:
//*
//* The Software is specifically designed for the exclusive use for Bosch
//* Sensortec products by personnel who have special experience and training. Do
//* not use this Software if you do not have the proper experience or training.
//*
//* This Software package is provided `` as is `` and without any expressed or
//* implied warranties, including without limitation, the implied warranties of
//* merchantability and fitness for a particular purpose.
//*
//* Bosch Sensortec and their representatives and agents deny any liability for
//* the functional impairment of this Software in terms of fitness, performance
//* and safety. Bosch Sensortec and their representatives and agents shall not
//* be liable for any direct or indirect damages or injury, except as
//* otherwise stipulated in mandatory applicable law.
//*
//* The Information provided is believed to be accurate and reliable. Bosch
//* Sensortec assumes no responsibility for the consequences of use of such
//* Information nor for any infringement of patents or other rights of third
//* parties which may result from its use. No license is granted by implication
//* or otherwise under any patent or patent rights of Bosch. Specifications
//* mentioned in the Information are subject to change without notice.
//*
//* It is not allowed to deliver the source code of the Software to any third
//* party without permission of Bosch Sensortec.*/

///*! \file bmp180.c
//    \brief This file contains all function implementations for the BMP180 API
//    Details.*/

//#include "bmp180.h"
//#include "stm32f1xx_hal.h"
//#include "TaskTimeManager.h"
//#include "i2c.h"
// 
//struct bmp180_t *p_bmp180;          /**< pointer to BMP180 device area */

//BMP180_BUS_WR_RETURN_TYPE(bus_write)(unsigned char DevAddress, unsigned char Reg, unsigned char *pData, unsigned char Size){
//	
//	if(HAL_OK != HAL_I2C_Mem_Write(&hi2c1, DevAddress, Reg, I2C_MEMADD_SIZE_8BIT, pData, Size, 100)){
//			printf("bus_write error\r\n");
//		return 1;
//	}
//	return 0;
//}
//BMP180_BUS_RD_RETURN_TYPE(bus_read)(unsigned char DevAddress, unsigned char Reg, unsigned char *pData, unsigned char Size){
//	if(HAL_OK != HAL_I2C_Mem_Read(&hi2c1, DevAddress, Reg, I2C_MEMADD_SIZE_8BIT, pData, Size, 100)){
//			printf("bus_read error\r\n");
//		return 1;
//	}
//	return 0;
//}
//BMP180_MDELAY_RETURN_TYPE(delay_msec)(BMP180_MDELAY_DATA_TYPE m){
//	DelayMS(m);
//}

///** initialize BMP180

//  This function initializes the BMP180 pressure sensor.
//  The function automatically detects the sensor type and stores this for all
//  future communication and calculation steps
//  \param *bmp180_t pointer to bmp180 device data structure
//  \return result of communication routines */

//int bmp180_init(struct bmp180_t *bmp180)
//{
//   char comres = 0;
//   unsigned char data;
// 
//   p_bmp180 = bmp180;   
//	 p_bmp180->bus_read = bus_read;
//	 p_bmp180->bus_write = bus_write;
//	p_bmp180->delay_msec = delay_msec;
//	/* assign BMP180 ptr */
//   p_bmp180->sensortype = E_SENSOR_NOT_DETECTED;
//   p_bmp180->dev_addr = BMP180_I2C_ADDR;   /* preset BMP180 I2C_addr */
//   comres += p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
//		BMP180_CHIP_ID__REG, &data, 1);  /* read Chip Id */

//   p_bmp180->chip_id = BMP180_GET_BITSLICE(data, BMP180_CHIP_ID);
//   p_bmp180->number_of_samples = 1;
//   p_bmp180->oversampling_setting = 0;
//   p_bmp180->sw_oss = 0;
//	if (p_bmp180->chip_id == BMP180_CHIP_ID) {
//		/* get bitslice */
//		p_bmp180->sensortype = BOSCH_PRESSURE_BMP180;
//		/* read Version reg */
//		comres += p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
//		BMP180_VERSION_REG, &data, 1);

//		/* get ML Version */
//		p_bmp180->ml_version = BMP180_GET_BITSLICE(data,\
//		BMP180_ML_VERSION);
//		/* get AL Version */
//		p_bmp180->al_version = BMP180_GET_BITSLICE(data,\
//		BMP180_AL_VERSION);
//		bmp180_get_cal_param(); /*readout bmp180 calibparam structure*/
//	}
//   return comres;
//}
///** read out parameters cal_param from BMP180 memory
//   \return result of communication routines*/

//int bmp180_get_cal_param(void)
//{
//   int comres;
//   unsigned char data[22];
//   comres = p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
//	BMP180_PROM_START__ADDR, data, BMP180_PROM_DATA__LEN);

//   /*parameters AC1-AC6*/
//   p_bmp180->cal_param.ac1 =  (data[0] << 8) | data[1];
//   p_bmp180->cal_param.ac2 =  (data[2] << 8) | data[3];
//   p_bmp180->cal_param.ac3 =  (data[4] << 8) | data[5];
//   p_bmp180->cal_param.ac4 =  (data[6] << 8) | data[7];
//   p_bmp180->cal_param.ac5 =  (data[8] << 8) | data[9];
//   p_bmp180->cal_param.ac6 = (data[10] << 8) | data[11];

//   /*parameters B1,B2*/
//   p_bmp180->cal_param.b1 =  (data[12] << 8) | data[13];
//   p_bmp180->cal_param.b2 =  (data[14] << 8) | data[15];

//   /*parameters MB,MC,MD*/
//   p_bmp180->cal_param.mb =  (data[16] << 8) | data[17];
//   p_bmp180->cal_param.mc =  (data[18] << 8) | data[19];
//   p_bmp180->cal_param.md =  (data[20] << 8) | data[21];
//   return comres;
//}
///** calculate temperature from ut
//  ut was read from the device via I2C and fed into the
//  right calc path for BMP180
//  \param ut parameter ut read from device
//  \return temperature in steps of 0.1 deg celsius
//  \see bmp180_read_ut()*/

//short bmp180_get_temperature(unsigned long ut)
//{
//   short temperature;
//   long x1, x2;
//	if (p_bmp180->sensortype == BOSCH_PRESSURE_BMP180) {
//		x1 = (((long) ut - (long) p_bmp180->cal_param.ac6) * \
//		(long) p_bmp180->cal_param.ac5) >> 15;
//		x2 = ((long) p_bmp180->cal_param.mc << 11) / \
//		(x1 + p_bmp180->cal_param.md);
//		p_bmp180->param_b5 = x1 + x2;
//	}
//   temperature = ((p_bmp180->param_b5 + 8) >> 4);  /* temperature in 0.1 deg C*/
//   return temperature;
//}
///** calculate pressure from up
//  up was read from the device via I2C and fed into the
//  right calc path for BMP180
//  In case of BMP180 averaging is done through oversampling by the sensor IC

//  \param ut parameter ut read from device
//  \return temperature in steps of 1.0 Pa
//  \see bmp180_read_up()*/

//long bmp180_get_pressure(unsigned long up)
//{
//   long pressure, x1, x2, x3, b3, b6;
//   unsigned long b4, b7;

//   b6 = p_bmp180->param_b5 - 4000;
//   /*****calculate B3************/
//   x1 = (b6*b6) >> 12;
//   x1 *= p_bmp180->cal_param.b2;
//   x1 >>= 11;

//   x2 = (p_bmp180->cal_param.ac2*b6);
//   x2 >>= 11;

//   x3 = x1 + x2;

//   b3 = (((((long)p_bmp180->cal_param.ac1)*4 + x3) << \
//		p_bmp180->oversampling_setting)+2) >> 2;

//   /*****calculate B4************/
//   x1 = (p_bmp180->cal_param.ac3 * b6) >> 13;
//   x2 = (p_bmp180->cal_param.b1 * ((b6*b6) >> 12)) >> 16;
//   x3 = ((x1 + x2) + 2) >> 2;
//   b4 = (p_bmp180->cal_param.ac4 * (unsigned long) (x3 + 32768)) >> 15;

//   b7 = ((unsigned long)(up - b3) * (50000>>p_bmp180->oversampling_setting));
//	if (b7 < 0x80000000)
//		pressure = (b7 << 1) / b4;
//	else
//		pressure = (b7 / b4) << 1;

//   x1 = pressure >> 8;
//   x1 *= x1;
//   x1 = (x1 * PARAM_MG) >> 16;
//   x2 = (pressure * PARAM_MH) >> 16;
//   pressure += (x1 + x2 + PARAM_MI) >> 4;/* pressure in Pa*/
//   return pressure;
//}
///** read out ut for temperature conversion
//   \return ut parameter that represents the uncompensated
//    temperature sensors conversion value*/

//unsigned short bmp180_get_ut()
//{
//   unsigned short ut;
//   unsigned char data[2];
//   unsigned char ctrl_reg_data;
//   int wait_time;
//   int comres;
////	if (p_bmp180->chip_id == BMP180_CHIP_ID  /* get bitslice */) {
////		ctrl_reg_data = BMP180_T_MEASURE;
////		wait_time = BMP180_TEMP_CONVERSION_TIME;
////	}
////   comres = p_bmp180->BMP180_BUS_WRITE_FUNC(p_bmp180->dev_addr,\
////		BMP180_CTRL_MEAS_REG, &ctrl_reg_data, 1);

////   p_bmp180->delay_msec(wait_time);
////   comres += p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
////		BMP180_ADC_OUT_MSB_REG, data, 2);
////   ut = (data[0] << 8) | data[1];
//	ctrl_reg_data = 0x2e;
//	p_bmp180->BMP180_BUS_WRITE_FUNC(p_bmp180->dev_addr,\
//		0xf4, &ctrl_reg_data, 1);
//	DelayMS(5);
//	p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
//		0xf6, data, 2);
//		ut = (data[0] << 8) | data[1];
//		ut &=0xffff;
//   return ut;
//}
///** read out up for pressure conversion
//  depending on the oversampling ratio setting up can be 16 to 19 bit
//   \return up parameter that represents the uncompensated pressure value*/

//unsigned long bmp180_get_up()
//{
//   int j;         /* j included for loop*/
//   unsigned long up = 0;
//   unsigned long sum = 0; /* get the calculated pressure data*/
//   unsigned char data[3];
//   unsigned char ctrl_reg_data;
//   int comres = 0;
////	if (p_bmp180->chip_id == BMP180_CHIP_ID && p_bmp180->sw_oss == 1 && \
////		p_bmp180->oversampling_setting == 3) {
////		for (j = 0 ; j < 3; j++) {
////				/* 3 times getting pressure data*/
////				ctrl_reg_data = BMP180_P_MEASURE + (p_bmp180->\
////				oversampling_setting << 6);
////				comres = p_bmp180->BMP180_BUS_WRITE_FUNC\
////				(p_bmp180->dev_addr, BMP180_CTRL_MEAS_REG, \
////				&ctrl_reg_data, 1);
////				p_bmp180->delay_msec(2 + (3 << (p_bmp180->\
////				oversampling_setting)));
////				comres += p_bmp180->BMP180_BUS_READ_FUNC\
////				(p_bmp180->dev_addr, BMP180_ADC_OUT_MSB_REG, \
////				data, 3);
////				sum = (((unsigned long) data[0] << 16) |\
////				((unsigned long) data[1] << 8) |\
////				(unsigned long) data[2]) >> \
////				(8-p_bmp180->oversampling_setting);
////				p_bmp180->number_of_samples = 1;
////				up = up + sum;  /*add up with dummy var*/
////	}
////		up = up / 3;   /* averaging*/
////	}
////  else    {
////	if (p_bmp180->chip_id == BMP180_CHIP_ID && p_bmp180->sw_oss == 0) {
////		ctrl_reg_data = BMP180_P_MEASURE + (p_bmp180->\
////		oversampling_setting << 6);
////		comres = p_bmp180->BMP180_BUS_WRITE_FUNC(p_bmp180->dev_addr,\
////		BMP180_CTRL_MEAS_REG, &ctrl_reg_data, 1);
////		p_bmp180->delay_msec(2 + (3 << (p_bmp180->\
////		oversampling_setting)));
////		comres += p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
////		BMP180_ADC_OUT_MSB_REG, data, 3);
////		up = (((unsigned long) data[0] << 16) |\
////		((unsigned long) data[1] << 8) |\
////		(unsigned long) data[2]) >> (8-p_bmp180->oversampling_setting);
////		p_bmp180->number_of_samples = 1;
////	}
////    }
//	ctrl_reg_data = 0x34;
//	p_bmp180->BMP180_BUS_WRITE_FUNC(p_bmp180->dev_addr,\
//		0xf4, &ctrl_reg_data, 1);
//	DelayMS(5);
//	p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
//		0xf6, data, 2);
//		up = (data[0] << 8) | data[1];
//		up &=0xffff;
//return up;
//}

//***************************************
// BMP180 IIC测试程序
// 使用单片机STC90C52RC
// 晶振：11.0592M
// 显示：LCD1602
// 编译环境 Keil uVision3
// 参考宏晶网站24c04通信程序
//****************************************
#include "stm32f1xx_hal.h"
#include "TaskTimeManager.h"   
#include  <math.h>    //Keil library  
#include  <stdlib.h>  //Keil library  
#include  <stdio.h>   //Keil library   
 
#define SCL_H()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
#define SCL_L()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET) 
#define SDA_H()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define SDA_L()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET) 
 
#define SDA  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)
#define delay_ms(x) DelayMS(x)
#define delay_us(x) DelayUS(x)
#define BMP180_SlaveAddr 0xee   //BMP180的器件地址
//BMP180校准系数
short AC1;
short AC2;
short AC3;
unsigned short AC4;
unsigned short AC5;
unsigned short AC6;
short B1;
short B2;
short MB;
short MC;
short MD;
 
uint8_t BMP180_ID=0;          //BMP180的ID
float True_Temp=0;       //实际温度,单位:℃
float True_Press=0;      //实际气压,单位:Pa
float True_Altitude=0;   //实际高度,单位:m
 
/*外部芯片IIC引脚初始化
 *SCL:PC1
 *SDA:PC2
*/
//void IIC_PortInit(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;  //定义一个GPIO_InitTypeDef类型的结构体
// 
//    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1|GPIO_Pin_2);    //PC1,PC2
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;           //漏极开漏
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
// 
//    GPIO_SetBits(GPIOC, GPIO_Pin_1|GPIO_Pin_2);    //拉高
// 
//}
 
void IIC_Init(void)
{
    SCL_H();  //SCL = 1;
    delay_us(5);
    SDA_H();  //SDA = 1;
    delay_us(5);    
}
 
void IIC_Start(void)
{
    SDA_H();  //SDA = 1;
    delay_us(5);
    SCL_H();  //SCL = 1;
    delay_us(5);
    SDA_L();  //SDA = 0;
    delay_us(5);    
}
 
void IIC_Stop(void)
{
    SDA_L();   //SDA = 0;
    delay_us(5);
    SCL_H();   //SCL = 1;
    delay_us(5);
    SDA_H();   //SDA = 1;
    delay_us(5);
}
 
unsigned char IIC_ReceiveACK(void)
{
    unsigned char ACK;
 
    SDA_H();     //SDA=1;//要读低电平需先拉高再读,否则读到的是错误数据,很重要！
    SCL_H();     //SCL=1;
    delay_us(5);
 
    if (SDA==1)  //SDA为高
    {
        ACK = 1;    
    }
    else ACK = 0;  //SDA为低
 
 
    SCL_L();    //SCL = 0;//SCL为低电平时SDA上的数据才允许变化,为传送下一个字节做准备 
    delay_us(5);
     
    return ACK;                 
}
 
void IIC_SendACK(unsigned char ack)
{
    if (ack == 1)SDA_H();
    else if (ack == 0)SDA_L();
    //SDA = ack;
    SCL_H();   //SCL = 1;
    delay_us(5);
    SCL_L();   //SCL = 0;
    delay_us(5);
}
 
unsigned char IIC_SendByte(unsigned char dat)
{
    unsigned char i;
    unsigned char bResult=1;
     
    SCL_L();     //SCL = 0;//拉低时钟线
    delay_us(5);        
 
    for( i=0;i<8;i++ ) //一个SCK,把dat一位一位的移送到SDA上
    {
        if( (dat<<i)&0x80 )SDA_H();   //SDA = 1;//先发高位
        else SDA_L();  //SDA = 0;
        delay_us(5);
 
        SCL_H();  //SCL = 1;
        delay_us(5);
        SCL_L();  //SCL = 0;
        delay_us(5);
    }
 
    bResult=IIC_ReceiveACK(); //发送完一个字节的数据,等待接受应答信号
 
    return bResult;  //返回应答信号
}
 
unsigned char IIC_ReadByte(void)
{
    unsigned char dat;
    unsigned char i;
     
    SCL_H();     //SCL = 1;//始终线拉高为读数据做准备
    delay_us(5);
 
    for( i=0;i<8;i++ )
    {
        dat <<= 1;
        dat = dat | (SDA);
        delay_us(5);
         
        SCL_L();   //SCL = 0;
        delay_us(5);    
        SCL_H();   //SCL = 1;
        delay_us(5);    
    }
    return dat;
}
 
/*从BMP180中读1个字节的数据*/
uint8_t BMP180_ReadOneByte(uint8_t ReadAddr)
{
    uint8_t temp = 0;
    uint8_t IIC_ComFlag = 1;   //IIC通信标志,为0标志正常,1表示通信错误
     
    IIC_Start();     //IIC start
    IIC_ComFlag = IIC_SendByte(BMP180_SlaveAddr);   //slave address+W:0
    //printf("IIC_ComFlag=%u \r\n",IIC_ComFlag);
    if (IIC_ComFlag == 0)                           //返回值为0表示通信正常,可以继续通信。否则不通信了
    {
        IIC_SendByte(ReadAddr);                      //发送命令代码
        IIC_Start();
        IIC_SendByte(BMP180_SlaveAddr|0x01);         //slave address+R:1
        temp = IIC_ReadByte();                       //读数据
        IIC_SendACK(1);                 
        IIC_Stop();
    }
    return (temp);      
}
 
/*从BMP180中读2个字节的数据*/
short BMP180_ReadTwoByte(uint8_t ReadAddr)
{
    uint8_t IIC_ComFlag = 1;   //IIC通信标志,为0标志正常,1表示通信错误
    uint8_t MSB,LSB;
    short temp;
     
    IIC_Start();
    IIC_ComFlag = IIC_SendByte(BMP180_SlaveAddr);
    if (IIC_ComFlag == 0)
    {
        IIC_SendByte(ReadAddr);
        IIC_Start();
        IIC_SendByte(BMP180_SlaveAddr|0x01);
        MSB = IIC_ReadByte();       //先读高位
        IIC_SendACK(0);         //ACK
        LSB = IIC_ReadByte();      //再读低位
        IIC_SendACK(1);        //NACK
        IIC_Stop();
    }
    temp = MSB*256+LSB;
 
    return temp;                                                    
}
 
/*向BMP180的寄存器写一个字节的数据*/
void Write_OneByteToBMP180(uint8_t RegAdd, uint8_t Data)
{
    IIC_Start();                       //IIC start
    IIC_SendByte(BMP180_SlaveAddr);   //slave address+W:0
    IIC_SendByte(RegAdd);
    IIC_SendByte(Data);
    IIC_Stop(); 
}
 
 
/*读取BMP180的校准系数*/
void Read_CalibrationData(void)
{
    AC1 = BMP180_ReadTwoByte(0xaa);
    AC2 = BMP180_ReadTwoByte(0xac);
    AC3 = BMP180_ReadTwoByte(0xae);
    AC4 = BMP180_ReadTwoByte(0xb0);
    AC5 = BMP180_ReadTwoByte(0xb2);
    AC6 = BMP180_ReadTwoByte(0xb4);
    B1 = BMP180_ReadTwoByte(0xb6);
    B2 = BMP180_ReadTwoByte(0xb8);
    MB = BMP180_ReadTwoByte(0xba);
    MC = BMP180_ReadTwoByte(0xbc);
    MD = BMP180_ReadTwoByte(0xbe);
    printf("AC1:%d \r\n",AC1);
    printf("AC2:%d \r\n",AC2);
    printf("AC3:%d \r\n",AC3);
    printf("AC4:%d \r\n",AC4);
    printf("AC5:%d \r\n",AC5);
    printf("AC6:%d \r\n",AC6);
    printf("B1:%d \r\n",B1);
    printf("B2:%d \r\n",B2);
    printf("MB:%d \r\n",MB);
    printf("MC:%d \r\n",MC);
    printf("MD:%d \r\n",MD);  
}
 
/*读BMP180没有经过补偿的温度值*/
long Get_BMP180UT(void)
{
    long UT;
 
    Write_OneByteToBMP180(0xf4,0x2e);       //write 0x2E into reg 0xf4
    delay_ms(10);                                   //wait 4.5ms
    UT = BMP180_ReadTwoByte(0xf6);          //read reg 0xF6(MSB),0xF7(LSB)
    printf("UT:%ld \r\n",UT);
 
    return UT;
}
 
/*读BMP180没有经过补偿的压力值*/
long Get_BMP180UP(void)
{
    long UP=0;
 
    Write_OneByteToBMP180(0xf4,0x34);       //write 0x34 into reg 0xf4 
    delay_ms(10);                                    //wait 4.5ms
    UP = BMP180_ReadTwoByte(0xf6); 
    UP &= 0x0000FFFF;
    printf("UP:%ld \r\n",UP);
     
    return UP;      
}
 
/*把未经过补偿的温度和压力值转换为时间的温度和压力值
 *True_Temp:实际温度值,单位:℃
 *True_Press:时间压力值,单位:Pa
 *True_Altitude:实际海拔高度,单位:m
*/
void Convert_UncompensatedToTrue(long UT,long UP)
{
    long X1,X2,X3,B3,B5,B6,B7,T,P;
    unsigned long B4;
     
    X1 = ((UT-AC6)*AC5)>>15;      //printf("X1:%ld \r\n",X1);
    X2 = ((long)MC<<11)/(X1+MD);  //printf("X2:%ld \r\n",X2);
    B5 = X1+X2;                        //printf("B5:%ld \r\n",B5);
    T = (B5+8)>>4;                      //printf("T:%ld \r\n",T);
    True_Temp = T/10.0;            printf("Temperature:%.1f \r\n",True_Temp);
 
    B6 = B5-4000;                       //printf("B6:%ld \r\n",B6);
    X1 = (B2*B6*B6)>>23;              //printf("X1:%ld \r\n",X1);
    X2 = (AC2*B6)>>11;                //printf("X2:%ld \r\n",X2);
    X3 = X1+X2;                         //printf("X3:%ld \r\n",X3);
    B3 = (((long)AC1*4+X3)+2)/4;    //printf("B3:%ld \r\n",B3);
    X1 = (AC3*B6)>>13;                //printf("X1:%ld \r\n",X1);
    X2 = (B1*(B6*B6>>12))>>16;      //printf("X2:%ld \r\n",X2);
    X3 = ((X1+X2)+2)>>2;              //printf("X3:%ld \r\n",X3);
    B4 = AC4*(unsigned long)(X3+32768)>>15;   //printf("B4:%lu \r\n",B4);
    B7 = ((unsigned long)UP-B3)*50000;        //printf("B7:%lu \r\n",B7);
    if (B7 < 0x80000000)
    {
        P = (B7*2)/B4;  
    }
    else P=(B7/B4)*2;                   //printf("P:%ld \r\n",P);         
    X1 = (P/256.0)*(P/256.0);       //printf("X1:%ld \r\n",X1);
    X1 = (X1*3038)>>16;               //printf("X1:%ld \r\n",X1);
    X2 = (-7357*P)>>16;               //printf("X2:%ld \r\n",X2);
    P = P+((X1+X2+3791)>>4);      //printf("P:%ld \r\n",P);
    True_Press = P;                 printf("Press:%.1fPa \r\n",True_Press);
 
    True_Altitude = 44330*(1-pow((P/101325.0),(1.0/5.255)));            
    printf("Altitude:%.3fm \r\n",True_Altitude);  
}

