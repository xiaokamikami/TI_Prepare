#include "stm32f10x.h"
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "pwm.h"
#include "Motor.h"
#include <stdio.h>
#include "main.h"
#include "bsp_nvic.h"
#include "uart.h"
#include "encoder.h"

u32 TimingDelay = 0;
__IO u8 Key1_num = 0 ,Key2_num =0 ,Key3_num;
int leftWheelEncoderNow    = 0;
int rightWheelEncoderNow   = 0;

static uint16_t Set_L1_Speed =0;
static uint16_t Set_R1_Speed =0;
static uint8_t Set_Find_Ball = 0;
static uint8_t *Set_Find_Ball_Char [10];
void Mian_Init(void);
void Dock_angle(uint16_t angle);	//180为最右  360最左
void Clamping(uint8_t ONOFF);	//0关1开

void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);
}

//**页面切换			
void LCD_yemian(uint8_t mode,uint16_t speed1,uint16_t speed2,uint16_t Angle)
{	
	uint8_t * LCD_Buff[28];

	char * huancun;
	if(mode == 1)	//Setting
	{
		

		LCD_ShowString(0*8,3*16,(u8*)"TAG:",WHITE);
		Key1_num = 0;
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 1)		//设置目标
		{
			if(Key1_num == 0)
			{
				LCD_ShowString(5*8,3*16,(u8*)"Black ",WHITE);	
				huancun	= "Black"	;
			}
			else if(Key1_num == 1)
			{
				LCD_ShowString(5*8,3*16,(u8*)"Blue  ",BLUE);	
				huancun	= "Blue"	;
			}
			else if(Key1_num == 2)
			{
				LCD_ShowString(5*8,3*16,(u8*)"Red   ",RED);		
				huancun	= "Red"	;
			}
			else if(Key1_num == 3)
			{
				LCD_ShowString(5*8,3*16,(u8*)"White ",WHITE);	
				huancun	= "White"	;				
			}		
			else if(Key1_num == 4)
			{
				LCD_ShowString(5*8,3*16,(u8*)"Green ",GREEN);
				huancun	= "Green"	;
			}		
			if(Key1_num == 5){Key1_num = 0;}			
		}
		sprintf((char *)Set_Find_Ball_Char,huancun);
		Set_Find_Ball = Key1_num;
		delay_ms(500);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 1)		//设置速度
		{
			if(Key1_num > 100){Key1_num = 0;}	
			LCD_ShowString(0*8,4*16,(u8*)"SET_SED:",WHITE);	
			sprintf((char * )LCD_Buff,"%d",Key1_num);
			LCD_ShowString(8*8,4*16,(u8*)LCD_Buff,RED);	


		}
		Key2_num = 2;
		
	}
	else if(mode == 2)	//Find Ball
	{


		sprintf((char * )LCD_Buff,"L1_SPEED_SET:%d   ",speed1);
		LCD_ShowString(0*8,4*16,(u8*)LCD_Buff,WHITE);
		sprintf((char * )LCD_Buff,"R1_SPEED_SET:%d   ",speed2);
		LCD_ShowString(0*8,5*16,(u8*)LCD_Buff,WHITE);		
		sprintf((char * )LCD_Buff,"Angle:%d   ",Angle);
		LCD_ShowString(0*8,6*16,(u8*)LCD_Buff,WHITE);
		Get_Motor_Speed(&leftWheelEncoderNow,&rightWheelEncoderNow);
		sprintf((char * )LCD_Buff,"L1_PRA:%d   ",leftWheelEncoderNow);
		LCD_ShowString(0*8,7*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"R1_PRA:%d   ",rightWheelEncoderNow );
		LCD_ShowString(0*8,8*16,(u8*)LCD_Buff,RED);

		//sprintf((char * )LCD_Buff,"L_SPEED:%d ",leftWheelEncoderNow);
		//LCD_ShowString(0*8,5*16,(u8*)LCD_Buff,WHITE);
		
		//sprintf((char * )LCD_Buff,"Key1:%d,Key2:%d ",Key1_num,Key2_num);
		//LCD_ShowString(0*8,7*16,(u8*)LCD_Buff,WHITE);
		//LCD_ShowString(28*8,14*16,(u8*)"1",WHITE);
	}
	else if(mode == 3)	//Capture Ball
	{
		
	}
}	

int main(void)
{
	uint32_t Pluse =0;
	uint16_t speed1 =0,speed2 = 0;
	uint16_t Angle = 270;
	uint8_t Mode = 1;
	
	Mian_Init();
	
	while(1)
	{	
		LCD_Clear(BLACK);
		//************
		if(Key2_num == 1)
		{
	//**关闭电机,进入设置模式
	//		TIM8_PWM_Init(1000, 719); //1000 * 720 / 72MHz = 10MS = 1000HZ
	//		MotorX_Init(1);
	//		MotorX_Init(2);
	//		TIM_SetCompare1(TIM8,700);
	//		TIM_SetCompare2(TIM8,700);
			Key1_num =0;
	//**显示标题
			LCD_ShowString(10*8,0*16,(u8*)"Contx-MC103  ",YELLOW);
			LCD_ShowString(10*8,1*16,(u8*)"Setting            ",WHITE);
			while(Key2_num == 1)
			{
				LCD_yemian(1,Pluse,Pluse,Angle);
			}
			
		}
		//************
		else if(Key2_num == 2)
		{
	//**打开电机
			TIM8_PWM_Init(1000, 719); //1000 * 720 / 72MHz = 10MS = 1000HZ
			MotorX_Init(1);
			MotorX_Init(2);
			TIM_SetCompare1(TIM8,700);
			TIM_SetCompare2(TIM8,700);
			
	//**显示目标
			LCD_ShowString(5*8,0*16,(u8*)"Contx-MC103  ",RED);
			LCD_ShowString(5*8,1*16,(u8*)"Find Ball ..... ",WHITE);
			LCD_ShowString(10*8,2*16,(u8*)Set_Find_Ball_Char,WHITE);
			while(Key2_num == 2)
			{
				delay_ms(100);
				//TIM_SetCompare1(TIM8,Pluse);
				//TIM_SetCompare2(TIM8,Pluse);
//				delay_ms(100);
				LCD_yemian(2,Pluse,Pluse,Angle);
//				Pluse +=10;
//				if(Pluse >=719){Pluse = 0;}
			}					
		}
	}
}
void Dock_angle(uint16_t angle)	//前车架转向 180为最右  360最左
{
	uint16_t Docker;
	Docker = (int)(angle/18);
	if(Docker <10)	   {Docker = 10;}
	else if(Docker >20){Docker = 20;}
	TIM_SetCompare2(TIM4,Docker);
	delay_ms(500);
	TIM_SetCompare2(TIM4,0);
}

void Clamping(uint8_t ONOFF)		//夹爪控制
{
	if(ONOFF==0)
	{
		TIM_SetCompare1(TIM4,25);	//PUSH	
	}
	else if(ONOFF ==1)
	{
		TIM_SetCompare1(TIM4,15);	//LOOSE
	}
	delay_ms(800);
	TIM_SetCompare1(TIM4,0);
}

void Mian_Init(void)
{
	SysTick_Config(SystemCoreClock/1000);
	delay_init();
	delay_ms(500);
	
	//**外设初始化
	KEY_Init();
	Lcd_Init();
	
	//UART_Init(3);
	LCD_Clear(BLACK);
	BACK_COLOR = BLACK;
	LCD_ShowString(6*8,0*16,(u8*)"PUSH KEY1 Star",WHITE);
	
	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 1){} //按键开始自检
		delay_ms(200);
		
	//**打开舵机
	TIM4_PWM_Init(200,7200,20);	//20MS = 50HZ  PLS 0.5MS-2.5MS		
	//**执行机械部分自检
		
		//**夹爪自检		
		delay_ms(500);
		Clamping(1); 				//LOOSE
		
		//**转向自检
		Dock_angle(270);
//		TIM_SetCompare2(TIM4,10);
//		delay_ms(300);
//		TIM_SetCompare2(TIM4,20);
//		delay_ms(300);
//		TIM_SetCompare2(TIM4,15);
//		delay_ms(300);
		//TIM_SetCompare2(TIM4,0);

	//***********************************************
	//**编码器初始化
	Encoder_Init_TIM3();
	Encoder_Init_TIM5();
		
	EXTI_Key_Config();

//	LCD_ShowString(28*8,14*16,(u8*)"1",WHITE);
}

void SysTick_Handler(void)			//IT.c中的滴答定时器
{
	TimingDelay--;
}
