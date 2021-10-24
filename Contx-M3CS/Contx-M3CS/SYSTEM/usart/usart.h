#ifndef __USART_H
#define __USART_H
#include "stdio.h" 
#include "sys.h" 

#define DMA_Rec_Len 200      //定义一个长度为200个字节的数据缓冲区。（建议定义的长度比你可能接收到的最长单帧数据长度长！）

void uart_init(u32 bound);
#endif

