#ifndef __NVIC_H
#define __NVIC_H
#include "stm32f4xx_it.h"

void initNVIC(uint8_t IRQChannel,uint8_t PreemptionPriority,uint8_t SubPriority);


#endif

