#ifndef __USART_H
#define __USART_H
#include "stdio.h" 
#include "sys.h" 

#define DMA_Rec_Len 200      //����һ������Ϊ200���ֽڵ����ݻ������������鶨��ĳ��ȱ�����ܽ��յ������֡���ݳ��ȳ�����

void uart_init(u32 bound);
#endif

