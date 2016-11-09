#include "as5600.h"
#include "i2c.h"

//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{ 
//  printf("I2C收到：%X\r\n", m_data); 
//}
//
//
//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
//  printf("I2C发送成功\r\n");
//  //HAL_I2C_Master_Receive_DMA(&hi2c1, 20, &rcv_sec, 1);
//}

uint8_t Newest = 0;

uint16_t getRawAngle(void){
  //获取高8位数据
  uint8_t Register = AS5600_RAW_ANGLE_H;
  uint16_t Raw_Angle = 0;
  uint8_t Data = 0;
  HAL_I2C_Master_Transmit(&hi2c1, AS5600_ADDR << 1, &Register, 1, 100);
  HAL_I2C_Master_Receive(&hi2c1, AS5600_ADDR<<1, &Data, 1, 100);
  Raw_Angle |= Data;
  //获取低8位数据
  Register = AS5600_RAW_ANGLE_L;
  HAL_I2C_Master_Transmit(&hi2c1, AS5600_ADDR << 1, &Register, 1, 100);
  HAL_I2C_Master_Receive(&hi2c1, AS5600_ADDR<<1, &Data, 1, 100);
  Raw_Angle = (Raw_Angle << 8) | Data;
  //计算角度
  //Raw_Angle = (Raw_Angle * 255) >> 8;
  
  
//  uint8_t addr = AS5600_RAW_ANGLE_H;
//  uint8_t data = 0;
//  HAL_I2C_Master_Transmit_DMA(&hi2c1, 0x36<<1, &addr, 1);  
//  HAL_I2C_Master_Receive_DMA(&hi2c1, 0x36<<1, &data, 1);
//  return data;
  return Raw_Angle;
}