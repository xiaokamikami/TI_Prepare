#include "control.h"
#include "tim.h"
#include "main.h"
#include "math.h"
#include "stdlib.h"
extern int Encoder_Left,Encoder_Right;	//���������ݣ��ٶȣ�


// 1.ͨ��TIM3��ȡ������岢�����ٶ�
void GetMotorPulse(void)
{
  // ��������õ�����壬�õ����10ms����������/������ת����Ϊʵ��ת�ٵ�rpm
  Encoder_Left = -(short)(__HAL_TIM_GET_COUNTER(&htim3)*2); 
  Encoder_Right = (short)(__HAL_TIM_GET_COUNTER(&htim4)*2); 


  //MotorSpeed = (short)(__HAL_TIM_GET_COUNTER(&htim3));
  __HAL_TIM_SET_COUNTER(&htim3,0);  // ����������
  

  //MotorSpeed = (short)(__HAL_TIM_GET_COUNTER(&htim3));
  __HAL_TIM_SET_COUNTER(&htim4,0);  // ����������

}

