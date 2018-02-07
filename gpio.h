#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_it.h"

#define AF_DISABLE 0

void initGPIO(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x,GPIOMode_TypeDef GPIO_Mode_x,uint8_t GPIO_AF,
	GPIOSpeed_TypeDef GPIO_Speed_xMHz,GPIOOType_TypeDef GPIO_OType_x,GPIOPuPd_TypeDef GPIO_PuPd_x);

void changeGPIO_Mode(GPIO_TypeDef* GPIOx,u8 pin,GPIOMode_TypeDef GPIO_Mode_x);

u8 turnPinNum(u16 GPIO_Pin_x);

#endif

