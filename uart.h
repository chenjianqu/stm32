#ifndef __UART_H
#define __UART_H
#include "stm32f4xx_it.h"

//初始化串口
void initUSART(USART_TypeDef* USARTx,uint32_t USART_BaudRate);

//串口发送数字
void USART_SendNumber(USART_TypeDef* USARTx,int num);

//串口发送字符串
void USART_SendString(USART_TypeDef* USARTx,u8 buff[],u16 len);

void USART_SendChar(USART_TypeDef* USARTx,u8 c);

/*************获取当前缓冲区的内容************/
u8 getRXString(u8 *pBuff);

/******************进入接收状态*********************/
void setRX_Ready(void);



void USART_NiMing_Report(USART_TypeDef* USARTx,u8 fun,u8*data,u8 len);
void MPU6050_Send_Data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);
void USART_Report_IMU(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);

#endif

