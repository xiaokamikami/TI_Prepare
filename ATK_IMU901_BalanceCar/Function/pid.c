#include "pid.h"
#include "math.h"
float Med_Angle=6;//机械中值--能使得小车真正平衡住的角度。
float
	Vertical_Kp=290,	//直立环KP、KD 12V  调好
	//Vertical_Kp=520,	//直立环KP、KD 7.4V
	//Vertical_Kp=0,	//直立环KP、KD 0
	Vertical_Kd=5 ;		//12V			调好
	//Vertical_Kd=0 ;
	//Vertical_Kd=50;
float
	Velocity_Kp=0,	//速度环KP、KI
	Velocity_Ki=0;
float 
	Turn_Kp=0;		//转向环

int Vertical_out,Velocity_out,Turn_out;//直立环&速度环&转向环 的输出变量

int Vertical(float Med,float Angle,float gyro_Y);//函数声明
int Velocity(int encoder_left,int encoder_right);
int Turn(int gyro_Z);
/*********************
直立环PD控制器：Kp*Ek+Kd*Ek_D

入口：期望角度、真实角度、真实角速度
出口：直立环输出
*********************/
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_Y-0);//【1】
	//if(fabs(Angle)>55){return 0;}
	//else return PWM_out;
	return PWM_out;
}



/*********************
速度环PI：Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int encoder_left,int encoder_right)
{
	static int PWM_out,Encoder_Err,Encoder_S,EnC_Err_Lowout,EnC_Err_Lowout_last;//【2】
	float a=0.7;//【3】
	
	//1.计算速度偏差
	Encoder_Err=(encoder_left+encoder_right)-0;//舍去误差
	//2.对速度偏差进行低通滤波
	//low_out=(1-a)*Ek+a*low_out_last;
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//使得波形更加平滑，滤除高频干扰，防止速度突变。
	EnC_Err_Lowout_last=EnC_Err_Lowout;//防止速度过大的影响直立环的正常工作。
	//3.对速度偏差积分，积分出位移
	Encoder_S+=EnC_Err_Lowout;//【4】
	//4.积分限幅
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	//5.速度环控制输出计算
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;//【5】
	return PWM_out;
}



/*********************
转向环：系数*Z轴角速度
*********************/
int Turn(int gyro_Z)
{
	int PWM_out;
	
	PWM_out=Turn_Kp*gyro_Z;
	return PWM_out;
}
