#ifndef __EXTI_H
#define __EXTI_H
#include "stm32f4xx_it.h"


void initEXTI(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x,EXTIMode_TypeDef EXTI_Mode_x,EXTITrigger_TypeDef EXTI_Trigger_x);



#endif
