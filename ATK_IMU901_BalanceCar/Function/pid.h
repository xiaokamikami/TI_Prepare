#ifndef  _PID_H
#define  _PID_H






int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int encoder_left,int encoder_right);
int Turn(int gyro_Z);

#endif

