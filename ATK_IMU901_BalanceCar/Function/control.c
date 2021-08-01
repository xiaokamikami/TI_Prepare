#include "control.h"
#include "tim.h"
#include "main.h"
#include "math.h"
#include "stdlib.h"
extern int Encoder_Left,Encoder_Right;	//编码器数据（速度）
int SpeedTarget1 =300;	  // 全局变量，LM速度目标值
int SpeedTarget2 =300;	  // 全局变量，RM速度目标值
int MotorOutput1 =300;		  // 全局变量，LM电机输出
int MotorOutput2 =300;		  // 全局变量，RM电机输出

// 1.通过TIM3读取电机脉冲并计算速度
void GetMotorPulse(void)
{
  // 计数器获得电机脉冲，该电机在10ms采样的脉冲/编码器转速则为实际转速的rpm
  Encoder_Left = -(short)(__HAL_TIM_GET_COUNTER(&htim3)); 
  Encoder_Right = (short)(__HAL_TIM_GET_COUNTER(&htim4)); 


  //MotorSpeed = (short)(__HAL_TIM_GET_COUNTER(&htim3));
  __HAL_TIM_SET_COUNTER(&htim3,0);  // 计数器清零
  

  //MotorSpeed = (short)(__HAL_TIM_GET_COUNTER(&htim3));
  __HAL_TIM_SET_COUNTER(&htim4,0);  // 计数器清零

}

