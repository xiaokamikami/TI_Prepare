#include "pid.h"
float Med_Angle=0;//��е��ֵ--��ʹ��С������ƽ��ס�ĽǶȡ�
float
	Vertical_Kp=-130,	//ֱ����KP��KD
	Vertical_Kd=-3;
float
	Velocity_Kp=-0.5,	//�ٶȻ�KP��KI
	Velocity_Ki=-0.0025;
float 
	Turn_Kp=-0.8;

int Vertical_out,Velocity_out,Turn_out;//ֱ����&�ٶȻ�&ת�� ���������

int Vertical(float Med,float Angle,float gyro_Y);//��������
int Velocity(int encoder_left,int encoder_right);
int Turn(int gyro_Z);
/*********************
ֱ����PD��������Kp*Ek+Kd*Ek_D

��ڣ������Ƕȡ���ʵ�Ƕȡ���ʵ���ٶ�
���ڣ�ֱ�������
*********************/
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*Angle+Vertical_Kd*(gyro_Y-0);//��1��
	return PWM_out;
}



/*********************
�ٶȻ�PI��Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int encoder_left,int encoder_right)
{
	static int PWM_out,Encoder_Err,Encoder_S,EnC_Err_Lowout,EnC_Err_Lowout_last;//��2��
	float a=0.7;//��3��
	
	//1.�����ٶ�ƫ��
	Encoder_Err=(encoder_left+encoder_right)-0;//��ȥ���
	//2.���ٶ�ƫ����е�ͨ�˲�
	//low_out=(1-a)*Ek+a*low_out_last;
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//ʹ�ò��θ���ƽ�����˳���Ƶ���ţ���ֹ�ٶ�ͻ�䡣
	EnC_Err_Lowout_last=EnC_Err_Lowout;//��ֹ�ٶȹ����Ӱ��ֱ����������������
	//3.���ٶ�ƫ����֣����ֳ�λ��
	Encoder_S+=EnC_Err_Lowout;//��4��
	//4.�����޷�
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	//5.�ٶȻ������������
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;//��5��
	return PWM_out;
}



/*********************
ת�򻷣�ϵ��*Z����ٶ�
*********************/
int Turn(int gyro_Z)
{
	int PWM_out;
	
	PWM_out=Turn_Kp*gyro_Z;
	return PWM_out;
}
