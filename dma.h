#ifndef __DMA_H
#define __DMA_H
#include "stm32f4xx_it.h"

void initDMA(DMA_Stream_TypeDef* DMA_Streamx,u32 DMA_Channel_x,u32 cpar,u32 cmar,u16 cndtr);

void startDMA(DMA_Stream_TypeDef* DMA_Streamx);

#endif

