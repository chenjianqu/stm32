#ifndef __DAC_H
#define __DAC_H
#include "stm32f4xx_it.h"

void initDAC(uint32_t DAC_Channel_x);

void setDAC_Vol(u32 DAC_Channel_x,u16 vol);

#endif


