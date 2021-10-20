#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "sys.h"

#define Motor1 1
#define Motor2 2
#define Motor3 3
#define Motor4 4

void MotorX_Init (char MotorX);
char *MotorX_Control(char MotorX,char Rotation,u16 Speed);

#endif
