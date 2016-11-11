#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H 
#include "stm32f1xx_hal.h"

typedef enum {ID_MOTOR_L = 1,ID_MOTOR_R,ID_MOTOR_H,ID_MOTOR_LC,ID_MOTOR_RC}MOTOR_ID;
#define ENCODER_CON_RES 0x7fffffff

extern void MotorInt(void);
extern void ScanMotorLimit(void); 
extern uint8_t CheckPower(void);
extern uint8_t PowerFlag;
#endif  
