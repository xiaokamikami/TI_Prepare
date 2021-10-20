#include "uart.h"
#include "stm32f10x_rcc.h"

char Uart_array_r[24];
char Uart_array_s[24];

char array[8];

unsigned char r_num,Read_Flag;
unsigned char res;

void UART_Init (char Channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStrue;	//��������
	NVIC_InitTypeDef NVIC_InitStrue;

	if(Channel == 1)
	{
		/*	Clock	*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// ʹ�� USART1  (APB2)
		
		USART_DeInit(USART1);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//TXD
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������츴��
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//RXD
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		USART_InitStrue.USART_BaudRate = 9600;
		USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // Ӳ��������
		USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // ���� ���� ģʽ��ʹ��
		USART_InitStrue.USART_Parity=USART_Parity_No; // û����żУ��
		USART_InitStrue.USART_StopBits=USART_StopBits_1; // һλֹͣλ
		USART_InitStrue.USART_WordLength=USART_WordLength_8b; // ÿ�η������ݿ��Ϊ8λ
		USART_Init(USART1,&USART_InitStrue);
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//ʹ���ж�
		USART_Cmd(USART1,ENABLE);	//ʹ�ܴ���1
		
		NVIC_InitStrue.NVIC_IRQChannel=USART1_IRQn;
	}
	else if(Channel == 2)
	{
		/*	Clock	*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	// ʹ�� USART2  (APB1)
		
		USART_DeInit(USART2);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//TXD
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������츴��
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//RXD
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		// ��ʼ�� ����ģʽ״̬
		USART_InitStrue.USART_BaudRate= 19200; // ������
		USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // Ӳ��������
		USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // ���� ���� ģʽ��ʹ��
		USART_InitStrue.USART_Parity=USART_Parity_No; // û����żУ��
		USART_InitStrue.USART_StopBits=USART_StopBits_1; // һλֹͣλ
		USART_InitStrue.USART_WordLength=USART_WordLength_8b; // ÿ�η������ݿ��Ϊ8λ
		USART_Init(USART2,&USART_InitStrue);
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//�����ж�
		USART_Cmd(USART2,ENABLE);	//ʹ�ܴ���2
		
		NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
	}
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStrue);
}

void Clean(unsigned char Length,char *Opinter,char *dat)
{
	unsigned char n;
	for (n = 0; n < Length; n++)
	{
		*(Opinter + n) = *dat++;
	}
}

char *  Num_Handle_Byte (char num)
{
	char n;
	for(n = 0;n <= 7;n++)
	{
		if(0x80 & (num << n)) array[n] = '1';
		else array[n] = '0';
	}
	return array;
}

void USART2_IRQHandler(void) // ����2�жϷ�����			//��ʼ��һ��Ҫ�� Main ���� (Read_Flag)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) && Read_Flag == 0) // �жϱ�־
	{
		res = USART_ReceiveData(USART2);  // ����2 ����
		Uart_array_r[r_num++] = res;
		USART_SendData(USART2,res);   // ����2 ����
		if(res == '}')	{Read_Flag = 1;	r_num = 0;}
	}
}

