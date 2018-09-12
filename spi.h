#ifndef __SPI_H
#define __SPI_H
#include "stm32f4xx_it.h"

void initSPI(SPI_TypeDef* SPIx,GPIO_TypeDef* GPIOx_sck,uint16_t GPIO_Pin_sck,GPIO_TypeDef* GPIOx_miso,uint16_t GPIO_Pin_miso,GPIO_TypeDef* GPIOx_mosi,uint16_t GPIO_Pin_mosi);

u8 rwSPI(SPI_TypeDef* SPIx,u8 mbyte);

void setSPISpeed(SPI_TypeDef* SPIx,u8 SPI_BaudRatePrescaler);

#endif

