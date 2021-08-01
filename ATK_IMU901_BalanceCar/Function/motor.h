#ifndef  _MOTOR_H
#define  _MOTOR_H
#include "main.h"
#include "gpio.h"






extern int PWM_MAX,PWM_MIN;






void Limit(int *motoA,int *motoB);
int GFP_abs(int p);
void Load(int moto1,int moto2);
#endif

