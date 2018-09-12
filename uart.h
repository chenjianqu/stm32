#ifndef __UART_H
#define __UART_H
#include "stm32f4xx_it.h"

//��ʼ������
void initUSART(USART_TypeDef* USARTx,uint32_t USART_BaudRate);

//���ڷ�������
void USART_SendNumber(USART_TypeDef* USARTx,int num);

//���ڷ����ַ���
void USART_SendString(USART_TypeDef* USARTx,u8 buff[],u16 len);

void USART_SendChar(USART_TypeDef* USARTx,u8 c);

/*************��ȡ��ǰ������������************/
u8 getRXString(u8 *pBuff);

/******************�������״̬*********************/
void setRX_Ready(void);



void USART_NiMing_Report(USART_TypeDef* USARTx,u8 fun,u8*data,u8 len);
void MPU6050_Send_Data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);
void USART_Report_IMU(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);

#endif

