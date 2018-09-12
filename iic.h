#ifndef __IIC_H
#define __IIC_H

#include "stm32f4xx_it.h"

//应答信号状态
#define ACK_FAIL 1
#define ACK_OK 0

//接收一个字节结束后是否发送SNACK
#define SACK 1
#define SNACK 0


void initIIC(GPIO_TypeDef* GPIOx_sda,uint16_t GPIO_Pin_sda,GPIO_TypeDef* GPIOx_scl,uint16_t GPIO_Pin_scl);

void IIC_Start(void);				//发送IIC开始信号

void IIC_Stop(void);	  			//发送IIC停止信号

void IIC_Send_Byte(u8 txd);			//IIC发送一个字节

u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节

u8 IIC_WaitAck(void); 				//IIC等待ACK信号

void IIC_Ack(void);					//IIC发送ACK信号

void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);

u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif

