#include "motor.h"
#include "main.h"
#include "tim.h"

/*限幅函数*/
void Limit(int *motoA,int *motoB)
{
	if(*motoA>PWM_MAX)*motoA=PWM_MAX;
	if(*motoA<PWM_MIN)*motoA=PWM_MIN;
	
	if(*motoB>PWM_MAX)*motoB=PWM_MAX;
	if(*motoB<PWM_MIN)*motoB=PWM_MIN;
}

/*绝对值函数*/
int GFP_abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*赋值函数*/
/*入口参数：PID运算完成后的最终PWM值*/
void Load(int moto1,int moto2)//moto1=-200：反转200个脉冲
{
//		//1.确定正负号，对应正反转
//	if(moto1>0){HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);}//正转
//	else 	   {HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_SET);}//反转
//	//2.确定PWM值
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,GFP_abs(moto1));
//	
//	if(moto2>0)	{HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);}//正转
//	else 		{HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_SET);}//反转	
//	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,GFP_abs(moto1));
	//1.确定正负号，对应正反转
	if(moto1>0){HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_RESET);}//正转
	else 	   {HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,GPIO_PIN_SET);}//反转
	//2.确定PWM值
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,GFP_abs(moto1));
	
	if(moto2>0)	{HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_SET);HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_RESET);}//正转
	else 		{HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,GPIO_PIN_RESET);HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,GPIO_PIN_SET);}//反转	
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,GFP_abs(moto1));
	
}