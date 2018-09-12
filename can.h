#ifndef __CAN_H
#define __CAN_H
#include "stm32f4xx_it.h"

//是否启用CAN接收中断
#define CAN_RX0_INT_ENABLE 0

u8 initCAN(CAN_TypeDef* CANx,GPIO_TypeDef* GPIOx_H,uint16_t GPIO_Pin_H,GPIO_TypeDef* GPIOx_L,uint16_t GPIO_Pin_L,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);

u8 sendCANMessage(CAN_TypeDef* CANx,u8* msg,u8 len);

u8 recerveCANMessage(CAN_TypeDef* CANx,u8 *buf);

#endif

