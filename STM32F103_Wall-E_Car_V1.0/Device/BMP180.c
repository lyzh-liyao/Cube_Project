/*  $Date: 2011/09/15 15:09:57 $
 *  Revision: 1.14 */
/*
* Copyright (C) 2011 Bosch Sensortec GmbH
*
* BMP180 pressure sensor API
*
* Usage:  Application Programming Interface for BMP180 configuration
*         and data read out

* Author:    venkatakrishnan.venkataraman@in.bosch.com

* Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License and the following
  stipulations.The Apache License , Version 2.0 is applicable unless otherwise
  stated by the stipulations of the disclaimer below.

* You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

* Disclaimer

* Common:
* Bosch Sensortec products are developed for the consumer goods industry. They
* may only be used within the parameters of the respective valid product data
* sheet.Bosch Sensortec products are provided with the express understanding
* that there is no warranty of fitness for a particular purpose.They are not
* fit for use in life-sustaining, safety or security sensitive systems or any
* system or device that may lead to bodily harm or property damage if the
* system or device malfunctions. In addition,Bosch Sensortec products are not
* fit for use in products which interact with motor vehicle systems.The resale
* and/or use of products are at the purchaser's own risk and his own
* responsibility. The examination of fitness for the intended use is the sole
* responsibility of the Purchaser.
*
* The purchaser shall indemnify Bosch Sensortec from all third party claims,
* including any claims for incidental, or consequential damages, arising from
* any product use not covered by the parameters of the respective valid
* product data sheet or not approved by Bosch Sensortec and reimburse Bosch
* Sensortec for all costs in connection with such claims.
*
* The purchaser must monitor the market for the purchased products,
* particularly with regard to product safety and inform Bosch Sensortec
* without delay of all security relevant incidents.
*
* Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
* from the valid technical specifications of the product series. They are
* therefore not intended or fit for resale to third parties or for use in end
* products. Their sole purpose is internal client testing. The testing of an
* engineering sample may in no way replace the testing of a product series.
* Bosch Sensortec assumes no liability for the use of engineering samples. By
* accepting the engineering samples, the Purchaser agrees to indemnify Bosch
* Sensortec from all claims arising from the use of engineering samples.
*
* Special:
* This software module (hereinafter called "Software") and any information on
* application-sheets (hereinafter called "Information") is provided free of
* charge for the sole purpose to support your application work. The Software
* and Information is subject to the following terms and conditions:
*
* The Software is specifically designed for the exclusive use for Bosch
* Sensortec products by personnel who have special experience and training. Do
* not use this Software if you do not have the proper experience or training.
*
* This Software package is provided `` as is `` and without any expressed or
* implied warranties, including without limitation, the implied warranties of
* merchantability and fitness for a particular purpose.
*
* Bosch Sensortec and their representatives and agents deny any liability for
* the functional impairment of this Software in terms of fitness, performance
* and safety. Bosch Sensortec and their representatives and agents shall not
* be liable for any direct or indirect damages or injury, except as
* otherwise stipulated in mandatory applicable law.
*
* The Information provided is believed to be accurate and reliable. Bosch
* Sensortec assumes no responsibility for the consequences of use of such
* Information nor for any infringement of patents or other rights of third
* parties which may result from its use. No license is granted by implication
* or otherwise under any patent or patent rights of Bosch. Specifications
* mentioned in the Information are subject to change without notice.
*
* It is not allowed to deliver the source code of the Software to any third
* party without permission of Bosch Sensortec.*/

/*! \file bmp180.c
    \brief This file contains all function implementations for the BMP180 API
    Details.*/

#include "bmp180.h"
#include "stm32f1xx_hal.h"
#include "TaskTimeManager.h"
#include "i2c.h"
#include  <math.h>
 
struct bmp180_t *p_bmp180 = NULL;          /**< pointer to BMP180 device area */

BMP180_BUS_WR_RETURN_TYPE bus_write(unsigned char DevAddress, unsigned char Reg, unsigned char *pData, unsigned char Size){
	
	if(HAL_OK != HAL_I2C_Mem_Write(&hi2c1, DevAddress, Reg, I2C_MEMADD_SIZE_8BIT, pData, Size, 10)){
		printf("bus_write error\r\n");
		return 1;
	}
	return 0;
}
BMP180_BUS_RD_RETURN_TYPE bus_read(unsigned char DevAddress, unsigned char Reg, unsigned char *pData, unsigned char Size){
	if(HAL_OK != HAL_I2C_Mem_Read(&hi2c1, DevAddress, Reg, I2C_MEMADD_SIZE_8BIT, pData, Size, 10)){
		printf("bus_read error\r\n");
		return 1;
	}
	return 0;
}
BMP180_MDELAY_RETURN_TYPE delay_msec(BMP180_MDELAY_DATA_TYPE m){
	DelayMS(20);
}

/** initialize BMP180

  This function initializes the BMP180 pressure sensor.
  The function automatically detects the sensor type and stores this for all
  future communication and calculation steps
  \param *bmp180_t pointer to bmp180 device data structure
  \return result of communication routines */
void bmp180_reset(void){
	uint8_t val = 0xB6;
	bus_write(BMP180_I2C_ADDR, BMP180_SOFT_RESET_REG, &val, 1);
}
int bmp180_init(struct bmp180_t *bmp180)
{
   char comres = 0;
   unsigned char data;
 
   p_bmp180 = bmp180;   
	 p_bmp180->bus_read = bus_read;
	 p_bmp180->bus_write = bus_write;
	p_bmp180->delay_msec = delay_msec;
	/* assign BMP180 ptr */
   p_bmp180->sensortype = E_SENSOR_NOT_DETECTED;
   p_bmp180->dev_addr = BMP180_I2C_ADDR;   /* preset BMP180 I2C_addr */
   comres += p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
		BMP180_CHIP_ID__REG, &data, 1);  /* read Chip Id */

   p_bmp180->chip_id = BMP180_GET_BITSLICE(data, BMP180_CHIP_ID);
   p_bmp180->number_of_samples = 1;
   p_bmp180->oversampling_setting = 0;
   p_bmp180->sw_oss = 0;
	if (p_bmp180->chip_id == BMP180_CHIP_ID) {
//		bmp180_reset();
//		DelayMS(50);
		/* get bitslice */
		p_bmp180->sensortype = BOSCH_PRESSURE_BMP180;
		/* read Version reg */
		comres += p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
		BMP180_VERSION_REG, &data, 1);

		/* get ML Version */
		p_bmp180->ml_version = BMP180_GET_BITSLICE(data,\
		BMP180_ML_VERSION);
		/* get AL Version */
		p_bmp180->al_version = BMP180_GET_BITSLICE(data,\
		BMP180_AL_VERSION);
		bmp180_get_cal_param(); /*readout bmp180 calibparam structure*/
	}
   return comres;
}
/** read out parameters cal_param from BMP180 memory
   \return result of communication routines*/

int bmp180_get_cal_param(void)
{
   int comres;
   unsigned char data[22];
   comres = p_bmp180->BMP180_BUS_READ_FUNC(p_bmp180->dev_addr,\
	BMP180_PROM_START__ADDR, data, BMP180_PROM_DATA__LEN);

   /*parameters AC1-AC6*/
   p_bmp180->cal_param.ac1 =  (data[0] << 8) | data[1];
   p_bmp180->cal_param.ac2 =  (data[2] << 8) | data[3];
   p_bmp180->cal_param.ac3 =  (data[4] << 8) | data[5];
   p_bmp180->cal_param.ac4 =  (data[6] << 8) | data[7];
   p_bmp180->cal_param.ac5 =  (data[8] << 8) | data[9];
   p_bmp180->cal_param.ac6 = (data[10] << 8) | data[11];

   /*parameters B1,B2*/
   p_bmp180->cal_param.b1 =  (data[12] << 8) | data[13];
   p_bmp180->cal_param.b2 =  (data[14] << 8) | data[15];

   /*parameters MB,MC,MD*/
   p_bmp180->cal_param.mb =  (data[16] << 8) | data[17];
   p_bmp180->cal_param.mc =  (data[18] << 8) | data[19];
   p_bmp180->cal_param.md =  (data[20] << 8) | data[21];
   return comres;
}

 
/*把未经过补偿的温度和压力值转换为时间的温度和压力值
 *True_Temp:实际温度值,单位:℃
 *True_Press:时间压力值,单位:Pa
 *True_Altitude:实际海拔高度,单位:m
*/
void Convert_UncompensatedToTrue(long UT,long UP)
{
			////BMP180校准系数
		short AC1 = p_bmp180->cal_param.ac1;
		short AC2 = p_bmp180->cal_param.ac2;
		short AC3 = p_bmp180->cal_param.ac3;
		unsigned short AC4 = p_bmp180->cal_param.ac4;
		unsigned short AC5 = p_bmp180->cal_param.ac5;
		unsigned short AC6 = p_bmp180->cal_param.ac6;
		short B1 = p_bmp180->cal_param.b1;
		short B2 = p_bmp180->cal_param.b2;
//		short MB = p_bmp180->cal_param.mb;
		short MC = p_bmp180->cal_param.mc;
		short MD = p_bmp180->cal_param.md;
		 
		float True_Temp=0;       //实际温度,单位:℃
		float True_Press=0;      //实际气压,单位:Pa
		float True_Altitude=0;   //实际高度,单位:m
	
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
    True_Press = P;                 printf("Press:%dhPa \r\n",(int)True_Press/10);
 
    True_Altitude = 44330*(1-pow((P/101325.0),(1.0/5.255)));            
    printf("Altitude:%.3fm \r\n",True_Altitude);  
}

void Request_UT(void){
	uint8_t val = BMP180_T_MEASURE;
	bus_write(BMP180_I2C_ADDR, BMP180_CTRL_MEAS_REG, &val, 1);
}

void Response_UT(void){
	uint8_t data[2] = {0};
	bus_read(BMP180_I2C_ADDR, BMP180_ADC_OUT_MSB_REG, data, 2);
  p_bmp180->UT = (data[0] << 8) | data[1];
}

void Request_UP(void){
	uint8_t val = 0xf4;//BMP180_P_MEASURE;
	bus_write(BMP180_I2C_ADDR, BMP180_CTRL_MEAS_REG, &val, 1);
}

void Response_UP(void){
	uint8_t data[2] = {0};
	bus_read(BMP180_I2C_ADDR, BMP180_ADC_OUT_MSB_REG, data, 2);
  p_bmp180->UP = (data[0] << 8) | data[1];
}

struct bmp180_t bmp180 = {0};
void BMP180_Build(void){
	if(p_bmp180 == NULL) 
			bmp180_init(&bmp180); 
	switch(p_bmp180->state){ 
		case 0:
			Request_UT();
			p_bmp180->state++;
			break;
		case 1:
			Response_UT();
			p_bmp180->state++;
			break;
		case 2:
			Request_UP();
			p_bmp180->state++;
			break;
		case 3:
			Response_UP();
			Convert_UncompensatedToTrue(p_bmp180->UT, p_bmp180->UP);
			p_bmp180->state = 0;
			break;
	}
}

