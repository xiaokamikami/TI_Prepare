/**
  ****************************************************************************************************
  * @file    	imu901.h
  * @author		����ԭ���Ŷ�(ALIENTEK)
  * @version    V1.0
  * @date		2020-04-17
  * @brief   	imu901ģ��ָ������
  * @license   	Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
  ****************************************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����ԭ�� STM32������
  * ������Ƶ:www.yuanzige.com
  * ������̳:www.openedv.com
  * ��˾��ַ:www.alientek.com
  * �����ַ:openedv.taobao.com
  *
  * �޸�˵��
  * V1.0 2020-04-17
  * ��һ�η���
  *
  ****************************************************************************************************
  */

#ifndef __ATK_IMU901_H_
#define __ATK_IMU901_H_



#include "main.h"
#include "stm32h7xx_it.h"


#define imu901_uart_receive(data, len)	ringbuffer_out(&uart3RxFifo, data, len) /*!< ���ڻ�ȡ��������API */
#define imu901_uart_send(data, len)		HAL_UART_Transmit(&huart3, data, len, 500)	/*!< ����i��������API */

/*ģ������֡ͷ*/
#define UP_BYTE1 		0x55
#define UP_BYTE2 		0x55	/*!< ģ�������ϴ���ͷ2 */
#define UP_BYTE2_ACK 	0xAF	/*!< ģ��ָ��Ӧ���ϴ���ͷ2 */


/* ģ�������ϴ�ָ��ID */
enum upmsgid
{
    UP_ATTITUDE		= 0x01,		/*!< ��̬�� */
    UP_QUAT			= 0x02,		/*!< ��Ԫ�� */
    UP_GYROACCDATA	= 0x03,		/*!< �����Ǽ��ٶ� */
    UP_MAGDATA		= 0x04,		/*!< �ų� */
    UP_BARODATA		= 0x05,		/*!< ��ѹ�� */
    UP_D03DATA		= 0x06,		/*!< �˿�״̬ */
};

/* �Ĵ����б� */
enum regTable
{
    REG_SAVE	= 0x00,			/*!< �����������			��W�� 	*/
    REG_SENCAL	= 0x01,			/*!< У׼����			��W�� 	*/
    REG_SENSTA	= 0x02,			/*!< У׼״̬			��R�� 	*/
    REG_GYROFSR	= 0x03,			/*!< ����������			��R/W�� */
    REG_ACCFSR	= 0x04,			/*!< ���ٶ�����			��R/W�� */
    REG_GYROBW	= 0x05,			/*!< �����Ǵ���			��R/W�� */
    REG_ACCBW	= 0x06,			/*!< ���ٶȴ���			��R/W�� */
    REG_BAUD	= 0x07,			/*!< ������			��R/W�� */
    REG_UPSET	= 0x08,			/*!< �ش���������			��R/W�� */
    REG_UPSET2	= 0x09,			/*!< �ش���������2		��R/W�� */
    REG_UPRATE	= 0x0A,			/*!< �ش�����			��R/W�� */
    REG_ALG		= 0x0B,			/*!< �㷨ѡ��			��R/W�� */
    REG_ASM		= 0x0C,			/*!< ��װ��ʽ			��R/W�� */
    REG_GACOFF	= 0x0D,			/*!< ��������У׼			��R/W�� */
    REG_BACOFF	= 0x0E,			/*!< ��ѹ����У׼			��R/W�� */
    REG_LEDOFF	= 0x0F,			/*!< LED����			��R/W�� */
    REG_D0MODE	= 0x10,			/*!< �˿�D0ģʽ			��R/W�� */
    REG_D1MODE	= 0x11,			/*!< �˿�D1ģʽ			��R/W�� */
    REG_D2MODE	= 0x12,			/*!< �˿�D2ģʽ			��R/W�� */
    REG_D3MODE	= 0x13,			/*!< �˿�D3ģʽ			��R/W�� */
    REG_D0PULSE	= 0x14,			/*!< �˿�D0PWM����		��R/W�� */
    REG_D1PULSE	= 0x16,			/*!< �˿�D1PWM����		��R/W�� */
    REG_D2PULSE	= 0x18,			/*!< �˿�D2PWM����		��R/W�� */
    REG_D3PULSE	= 0x1A,			/*!< �˿�D3PWM����		��R/W�� */
    REG_D0PERIOD	= 0x1C,		/*!< �˿�D0PWM����		��R/W�� */
    REG_D1PERIOD	= 0x1F,		/*!< �˿�D1PWM����		��R/W�� */
    REG_D2PERIOD	= 0x21,		/*!< �˿�D2PWM����		��R/W�� */
    REG_D3PERIOD	= 0x23,		/*!< �˿�D2PWM����		��R/W�� */

    REG_RESET   = 0x7F,			/*!< �ָ�Ĭ������			��W�� 	*/
};


#define ATKP_MAX_DATA_SIZE 28
#define UART3_RX_BUFFER_SIZE	256



/*���λ��������ݽṹ*/
typedef struct
{
    uint8_t  *buffer;
    uint16_t size;
    uint16_t in;
    uint16_t out;
} ringbuffer_t;



/*ͨѶ���ݽṹ*/
typedef struct
{
    uint8_t startByte1;
    uint8_t startByte2;
    uint8_t msgID;
    uint8_t dataLen;
    uint8_t data[ATKP_MAX_DATA_SIZE];
    uint8_t checkSum;
} atkp_t;


/* ��̬�����ݽṹ�� */
typedef struct 
{
    float roll;
    float pitch;
    float yaw;
} attitude_t;


/* ��Ԫ�����ݽṹ�� */
typedef struct
{
    float q0, q1, q2, q3;

} quaternion_t;

/* �����Ǽ��ٶ����ݽṹ�� */
typedef struct
{
    int16_t gyro[3]; 	/*!< ������ԭʼ���� */
    int16_t acc[3];		/*!< ���ٶ�ԭʼ���� */
    float 	fgyroD[3];	/*!< ������ת�١�/S */
    float 	faccG[3];	/*!< ���ٶ����� G */
} gyroAcc_t;


/* ���������ݽṹ */
typedef struct
{
    int16_t mag[3];		/*!< �ų����� */
    float  	temp;		/*!< �������¶� */
} mag_t;

/* ��ѹ�����ݽṹ */
typedef struct
{
    int32_t pressure;	/*!< ��ѹֵPa */
    int32_t altitude;	/*!< ���θ߶�cm */
    float  	temp;		/*!< ��ѹ���¶� */
} baro_t;


/* �˿����ݽṹ */
typedef struct
{
    uint16_t d03data[4];
} ioStatus_t;



/* ���ּĴ������� */
typedef struct
{
    uint8_t gyroFsr;	/*!< REG_GYROFSR�Ĵ�����ֵ */
	uint8_t accFsr;
	uint8_t gyroBW;
	uint8_t accBW;
} regValue_t;




extern uint8_t uart3RxBuffer[];
extern ringbuffer_t uart3RxFifo;
/* ģ�������ϴ������� */
extern attitude_t		attitude;		/*!< ��̬�� */
extern quaternion_t		quaternion;
extern gyroAcc_t 		gyroAccData;
extern mag_t			magData;
extern baro_t			baroData;
extern ioStatus_t		iostatus;

/* ģ��Ĵ�������ֵ */
extern regValue_t  	imu901Param;

/* ���ڽ��ս����ɹ������ݰ� */
extern atkp_t 			rxPacket;




uint8_t usart3_sendData(uint8_t *data, uint16_t len);
uint16_t usart3_getRxData(uint8_t *buf, uint16_t len);

void usart3_init(void);


void ringbuffer_init(ringbuffer_t *fifo, uint8_t *buffer, uint16_t size);

uint16_t ringbuffer_getUsedSize(ringbuffer_t *fifo);
uint16_t ringbuffer_getRemainSize(ringbuffer_t *fifo);
uint8_t ringbuffer_isEmpty(ringbuffer_t *fifo);

void ringbuffer_in(ringbuffer_t *fifo, uint8_t *data, uint16_t len);
uint8_t ringbuffer_in_check(ringbuffer_t *fifo, uint8_t *data, uint16_t len);
uint16_t ringbuffer_out(ringbuffer_t *fifo, uint8_t *buf, uint16_t len);


void imu901_init(void);

uint8_t imu901_unpack(uint8_t ch);
void atkpParsing(atkp_t *packet);

void atkpWriteReg(enum regTable reg, uint16_t data, uint8_t datalen);
uint8_t atkpReadReg(enum regTable reg, int16_t *data);


#endif /* _IMU901_H_ */

/*******************************END OF FILE************************************/


