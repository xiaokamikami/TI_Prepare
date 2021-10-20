#include "stm32f10x.h"
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "pwm.h"
#include "Motor.h"
#include <stdio.h>
#include "main.h"
#include "bsp_nvic.h"
u32 TimingDelay = 0;
u8 Key1_num = 0 ,Key2_num =0;
void Mian_Init(void);

void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);
}

void LCD_yemian(uint8_t mode,uint16_t speed1,uint16_t speed2,uint8_t Angle)
{	
	uint8_t * LCD_Buff[30];
	if(mode == 1)
	{
		LCD_ShowString(10*8,0*16,(u8*)"Contx-MC103  ",WHITE);
		
		sprintf((char * )LCD_Buff,"L1_SPEED:%d , R1_SPEED:%d   ",speed1,speed2);
		LCD_ShowString(0*8,1*16,(u8*)LCD_Buff,WHITE);
		
		
		sprintf((char * )LCD_Buff,"Angle:%d   ",Angle);
		LCD_ShowString(0*8,2*16,(u8*)LCD_Buff,WHITE);
		
		sprintf((char * )LCD_Buff,"Key1:%d,Key2:%d ",Key1_num,Key2_num);
		LCD_ShowString(0*8,4*16,(u8*)LCD_Buff,WHITE);
		
		//LCD_ShowString(28*8,14*16,(u8*)"1",WHITE);
	}
	else if(mode == 2)
	{
		
	}
}	
int main(void)
{
	uint32_t Pluse =0;
	uint16_t speed1 =0,speed2 = 0;
	uint8_t Angle = 90;
	GPIO_InitTypeDef GPIO_InitStrue;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStrue.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStrue);
	GPIO_Init(GPIOD,&GPIO_InitStrue);
	Mian_Init();
	//KEY_Init();
	EXTI_Key_Config();

	//MotorX_Init(1);
	//MotorX_Init(2);
	while(1)
	{

		delay_ms(100);
		//Key1_num = KEY_Input();
		TIM_SetCompare1(TIM8,Pluse);
		TIM_SetCompare2(TIM8,Pluse);
		delay_ms(100);
		LCD_yemian(1,Pluse,Pluse,Angle);
		Pluse +=10;
		if(Pluse >=719){Pluse = 0;}
	}
}

void Mian_Init(void)
{
	SysTick_Config(SystemCoreClock/1000);
	delay_init();
	delay_ms(500);
	Lcd_Init();
	
	LCD_Clear(BLACK);
	BACK_COLOR = BLACK;
	//TIM8_PWM_Init(1000, 719); //1000 * 720 / 72MHz = 10MS
	LCD_ShowString(10*8,0*16,(u8*)"Contx-MC4",WHITE);
	LCD_ShowString(0*8,1*16,(u8*)"Microsoft Linux [V1.9]",WHITE);
	LCD_ShowString(0*8,2*16,(u8*)"(c)Microsoft Corporation.",WHITE);
	LCD_ShowString(0*8,4*16,(u8*)"C:/Users/Champion > ",WHITE);
	LCD_ShowString(28*8,14*16,(u8*)"1",WHITE);
}

void SysTick_Handler(void)			//IT.c中的滴答定时器
{
	TimingDelay--;
}
