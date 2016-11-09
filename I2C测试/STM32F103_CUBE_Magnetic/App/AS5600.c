#include "as5600.h"
#include "i2c.h"
uint16_t getRawAngle(void){
  //获取高8位数据
  uint8_t Register = AS5600_RAW_ANGLE_H;
  uint16_t Raw_Angle = 0;
  uint8_t Data = 0;
  HAL_I2C_Master_Transmit_DMA(&hi2c1, AS5600_ADDR << 1, &Register, 1);
  DelayMS(20);
  HAL_I2C_Master_Receive_DMA(&hi2c1, AS5600_ADDR<<1, &Data, 1);
  Raw_Angle |= Data;
  //获取低8位数据
//  Register = AS5600_RAW_ANGLE_L;
//  HAL_I2C_Master_Transmit_DMA(&hi2c1, AS5600_ADDR << 1, &Register, 1);
//  HAL_I2C_Master_Receive_DMA(&hi2c1, AS5600_ADDR<<1, &Data, 1);
//  Raw_Angle = (Raw_Angle << 8) | Data;
  //计算角度
  Raw_Angle = (Raw_Angle * 255) >> 8;
  return Raw_Angle;
}