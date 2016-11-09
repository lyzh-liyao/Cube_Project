#ifndef __AS5600_H__
#define __AS5600_H__
#include "stm32f1xx_hal.h" 
#define AS5600_ZMCO 0x00
#define AS5600_ZPOS_H 0x01
#define AS5600_ZPOS_L 0x02
#define AS5600_MPOS_H 0x03
#define AS5600_MPOS_L 0x04
#define AS5600_MANG_H 0x05
#define AS5600_MANG_L 0x06
#define AS5600_CONF_H 0x07
#define AS5600_CONF_L 0x08
#define AS5600_RAW_ANGLE_H 0x0c//没有处理过的角度
#define AS5600_RAW_ANGLE_L 0x0d
#define AS5600_ANGLE_H 0x0e  //处理过的角度
#define AS5600_ANGLE_L 0x0f
#define AS5600_STATUS 0x0b
#define AS5600_AGC 0x1a
#define AS5600_MAGNITUDE_H 0x1b
#define AS5600_MAGNITUDE_L 0x1c
#define AS5600_BURN 0xff

#define AS5600_ADDR 0x36

extern uint16_t getRawAngle(void);//获取未处理角度


#endif