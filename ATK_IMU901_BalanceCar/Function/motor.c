#include "motor.h"
#include "main.h"
#include "tim.h"

/*�޷�����*/
void Limit(int *motoA,int *motoB)
{
	if(*motoA>PWM_MAX)*motoA=PWM_MAX;
	if(*motoA<PWM_MIN)*motoA=PWM_MIN;
	
	if(*motoB>PWM_MAX)*motoB=PWM_MAX;
	if(*motoB<PWM_MIN)*motoB=PWM_MIN;
}

/*����ֵ����*/
int GFP_abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*��ֵ����*/
/*��ڲ�����PID������ɺ������PWMֵ*/
void Load(int moto1,int moto2)//moto1=-200����ת200������
{
//		//1.ȷ�������ţ���Ӧ����ת
//	if(moto1>0){HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);}//��ת
//	else 	   {HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_SET);}//��ת
//	//2.ȷ��PWMֵ
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,GFP_abs(moto1));
//	
//	if(moto2>0)	{HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);}//��ת
//	else 		{HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_SET);}//��ת	
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,GFP_abs(moto1));
	//1.ȷ�������ţ���Ӧ����ת
	if(moto1>0){HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_RESET);}//��ת
	else 	   {HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);}//��ת
	//2.ȷ��PWMֵ
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,GFP_abs(moto1));
	
	if(moto2>0)	{HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_RESET);}//��ת
	else 		{HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);}//��ת	
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,GFP_abs(moto1));
	
}