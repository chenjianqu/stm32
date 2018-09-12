#ifndef __IIC_H
#define __IIC_H

#include "stm32f4xx_it.h"

//Ӧ���ź�״̬
#define ACK_FAIL 1
#define ACK_OK 0

//����һ���ֽڽ������Ƿ���SNACK
#define SACK 1
#define SNACK 0


void initIIC(GPIO_TypeDef* GPIOx_sda,uint16_t GPIO_Pin_sda,GPIO_TypeDef* GPIOx_scl,uint16_t GPIO_Pin_scl);

void IIC_Start(void);				//����IIC��ʼ�ź�

void IIC_Stop(void);	  			//����IICֹͣ�ź�

void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�

u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�

u8 IIC_WaitAck(void); 				//IIC�ȴ�ACK�ź�

void IIC_Ack(void);					//IIC����ACK�ź�

void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);

u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif

