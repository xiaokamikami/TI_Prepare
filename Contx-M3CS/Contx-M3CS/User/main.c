//** stm
#include "main.h"
//** sys
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "pwm.h"
#include "Motor.h"
#include "bsp_nvic.h"
#include "encoder.h"
#include "TIM.h"

#include "pid.h"


//** comm
//#include "DMA.h"
#include "usart.h"

//** USART
const u8 TEXT_TO_SEND[]={"ALIENTEK Mini STM32 DMA 串口实验"};
#define TEXT_LENTH  sizeof(TEXT_TO_SEND)-1			//TEXT_TO_SEND字符串长度(不包含结束符)
u8 SendBuff[(TEXT_LENTH+2)*100];

extern __IO int k2_x;   //坐标值存放 
extern __IO int k2_y;   //
extern __IO int k2_color;   //颜色数据 
extern __IO int k2_val ;   //图像大小拟合距离 
//** LCD
u32 TimingDelay = 0;
__IO u8 Key1_num = 0 ,Key2_num =0 ,Key3_num , Res_near = 0;
int leftWheelEncoderNow    = 0;
int rightWheelEncoderNow   = 0;

//** USER
static uint8_t Set_Find_Ball = 0;
static uint8_t *Set_Find_Ball_Char [10];
void Mian_Init(void);
void Dock_angle(uint16_t angle);	//180为最右  360最左
void Clamping(uint8_t ONOFF);	//0关1开

//** TAG
extern __IO u8 BALL_RX_FLAG ; //传输完成标志
u8 time_10ms = 0;
u8 time_20ms = 0;
u8 time_50ms = 0;
static u16 Motor_Speed_Set = 0;  //期望速度

//** Motor
int Motor_Sp1 = 300;
int Motor_Sp2 = 300;
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
			if(Key1_num >= 100){Key1_num = 0;}	
			LCD_ShowString(0*8,4*16,(u8*)"SET_SED:",WHITE);	
			sprintf((char * )LCD_Buff,"%d  ",Key1_num);
			LCD_ShowString(8*8,4*16,(u8*)LCD_Buff,RED);	
		}
		Motor_Speed_Set = Key1_num *7;
		Key2_num = 2;
		
	}
	else if(mode == 2)	//Find Ball
	{


		sprintf((char * )LCD_Buff,"SPEED_SET:%d   ",Motor_Speed_Set);
		LCD_ShowString(0*8,3*16,(u8*)LCD_Buff,WHITE);
//		sprintf((char * )LCD_Buff,"Msp1:%d,Msp2:%d ",Motor_Sp1,Motor_Sp2);	//调试用
//		LCD_ShowString(0*8,4*16,(u8*)LCD_Buff,WHITE);	
		sprintf((char * )LCD_Buff,"Angle:%d   ",Angle);
		LCD_ShowString(0*8,6*16,(u8*)LCD_Buff,WHITE);		
		sprintf((char * )LCD_Buff,"L1_PRA:%d   ",leftWheelEncoderNow);
		LCD_ShowString(0*8,7*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"R1_PRA:%d   ",rightWheelEncoderNow );
		LCD_ShowString(0*8,8*16,(u8*)LCD_Buff,RED);
		
	}
	else if(mode == 3)	//Capture Ball
	{
		sprintf((char * )LCD_Buff,"Angle:%d   ",Angle);
		LCD_ShowString(0*8,6*16,(u8*)LCD_Buff,WHITE);		
		sprintf((char * )LCD_Buff,"X:%d,Y:%d    ",k2_x,k2_y);
		LCD_ShowString(0*8,7*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"Val:%d ",k2_val);
		LCD_ShowString(0*8,8*16,(u8*)LCD_Buff,RED);
		sprintf((char * )LCD_Buff,"Res:%d ",Res_near);
		LCD_ShowString(0*8,9*16,(u8*)LCD_Buff,RED);
			
	}
}	
//**Handle
void TIM1_UP_IRQHandler(void)   //TIM1中断
{

	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM1中断发生与否:TIM1 中断源 
	{
	   TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM1 中断源 	
		
	   Get_Motor_Speed(&leftWheelEncoderNow,&rightWheelEncoderNow);  //得出编码器速度
	if(Motor_Speed_Set >0)
	{
	   Motor_Sp1 = SpeedInnerControl1(leftWheelEncoderNow,Motor_Speed_Set);//PID控制电机1
		if(Motor_Sp1 < 0 ){	Motor_Sp1 = abs(710+Motor_Sp1);}		
		TIM_SetCompare1(TIM8,Motor_Sp1);
		
	   Motor_Sp2 = SpeedInnerControl2(rightWheelEncoderNow,Motor_Speed_Set);//PID控制电机2
		if(Motor_Sp2 < 0 ){	Motor_Sp2 = abs(710+Motor_Sp2);}
		TIM_SetCompare2(TIM8,Motor_Sp2);		
	}

			   
	   /* 这里写中断 */
	   
		
	}
}

int main(void)
{
	uint32_t Pluse =0;
	uint16_t speed1 =0,speed2 = 0;
	uint16_t Angle = 270;
	uint8_t Mode = 1;
	u8 Ball_color = 0;
	u8 Ball_num = 0;
	u8 Lock_Ball_Char[10] ;
	Mian_Init();
	Timer1_Init(72,1000);  	//10ms
	
	u8 dj = 0; //舵机缓冲
	while(1)
	{	
		LCD_Clear(BLACK);
		//************设置模式
		if(Key2_num == 1)
		{
	//**关闭电机
	//		TIM8_PWM_Init(1000, 719); //1000 * 720 / 72MHz = 10MS = 1000HZ
	//		MotorX_Init(1);
	//		MotorX_Init(2);
	//		TIM_SetCompare1(TIM8,700);
	//		TIM_SetCompare2(TIM8,700);
	//		TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设  
			Key1_num =0;
	//**显示标题
			LCD_ShowString(10*8,0*16,(u8*)"STM-Arduino  ",YELLOW);
			LCD_ShowString(10*8,1*16,(u8*)"Setting            ",WHITE);
			while(Key2_num == 1)
			{
				LCD_yemian(1,Pluse,Pluse,Angle);
			}
			
		}
		//************寻找模式
		else if(Key2_num == 2)
		{
	//**打开电机
			TIM8_PWM_Init(1000, 719); //1000 * 720 / 72MHz = 10MS = 1000HZ
			MotorX_Init(1);
			MotorX_Init(2);
			if(Ball_num != 0 ){MotorX_transfer (0);}
			else{MotorX_transfer (1);}
			TIM_SetCompare1(TIM8,200);
			TIM_SetCompare2(TIM8,200);
			TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设  
	//**显示标题
			LCD_ShowString(5*8,0*16,(u8*)"STM-Arduino  ",RED);
			LCD_ShowString(5*8,1*16,(u8*)"Find Ball ..... ",WHITE);
			LCD_ShowString(10*8,2*16,(u8*)Set_Find_Ball_Char,WHITE);
			while(Key2_num == 2)
			{
				delay_ms(10);
				if(Ball_num != 0 ){MotorX_transfer (0);}
				else{MotorX_transfer (1);}
				LCD_yemian(2,Pluse,Pluse,Angle);
				if(BALL_RX_FLAG == 1)	//检测到坐标输入
				{
					if(Ball_color == k2_color)
					{
						Ball_num += 1;
					}
					else
					{
						Ball_color = k2_color;
						Ball_num = 0;
					}
					BALL_RX_FLAG = 0;
					if(Ball_num >5){Key2_num = 3;break;}			//确定目标				
				}
			}					
		}
		//************捕获模式
		else if(Key2_num == 3)			
		{

			//**显示标题
			LCD_ShowString(5*8,0*16,(u8*)"STM-Arduino  ",RED);
			LCD_ShowString(5*8,1*16,(u8*)"Lock Ball ..... ",WHITE);
			if(Ball_color == 1){sprintf((char *)Set_Find_Ball_Char,"Black");}
			LCD_ShowString(10*8,2*16,(u8*)Set_Find_Ball_Char,WHITE);
			
			Motor_Speed_Set = 64; //强制减速
			MotorX_transfer (1);   //重启正转
			while(Key2_num == 3)
			{
				if(BALL_RX_FLAG == 1)	//检测到坐标输入
				{
	//*************************************微调计算	
					Angle = abs((k2_x*-1)-180); //转向值 
					if(Angle >360){Angle = 360;}
					else if(Angle <180){Angle = 180;}

				if(dj < Angle){
					if(( Angle - dj )<5){
						dj+=1;
					}
					else
					{dj+=5;	}
				}
				else if (dj > Angle){
					if(( dj - Angle )<5){
						dj-=1;
					}
					else{dj-=5;}
				}
				Dock_angle(Angle ); //微调方向

					
					LCD_yemian(3,Motor_Sp1,Motor_Sp2,Angle);
		
				//接近小球
					if(k2_val >80)		
					{
						Motor_Speed_Set = 10;
					}
					if(Res_near == 1) //接近TAG
					{
						Motor_Speed_Set = 0;
						Clamping(0);
						delay_ms(300);
						
						if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 1)	//确定到位
						{
							//Key2_num = 4;
						}
						else						
						{
								Motor_Speed_Set = 128; //强制减速
								MotorX_transfer (2);   //翻转电机IO
								delay_ms(800);
								Motor_Speed_Set = 0; //停车 重新进入捕获模式
								MotorX_transfer (1);   //翻转电机IO	
						}
					}
					
				}
			}
			
		BALL_RX_FLAG = 0;		//运行一轮清空串口标记	
			
		}
		//************放球模式
		else if(Key2_num == 4)			
		{
			
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
	delay_ms(350);
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
	
	//DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
	LCD_Clear(BLACK);
	BACK_COLOR = BLACK;
	LCD_ShowString(6*8,0*16,(u8*)"PUSH KEY1 Star",WHITE);
	
	/* 初始化USART */
	uart_init(115200);	
	
	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 1){} //按键开始自检
		delay_ms(200);
		
	//**打开舵机
	TIM4_PWM_Init(200,7200,20);	//20MS = 50HZ  PLS 0.5MS-2.5MS		
	//**执行机械部分自检
		
		//**夹爪自检		
		delay_ms(500);
		Clamping(1); 				//LOOSE
		
		//**转向自检
		Dock_angle(180);
		Dock_angle(270);


	//***********************************************
	//**编码器初始化
	Encoder_Init_TIM3();
	Encoder_Init_TIM5();
		
	EXTI_Key_Config();
	Key2_num =1;
//	LCD_ShowString(28*8,14*16,(u8*)"1",WHITE);
}

void SysTick_Handler(void)			//IT.c中的滴答定时器
{
	TimingDelay--;
}
