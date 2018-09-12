#ifndef __ADC_H
#define __ADC_H
#include "stm32f4xx_it.h"

void initADC(ADC_TypeDef* ADCx);

void initADC_GPIO(GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x);

u16 getADC(ADC_TypeDef* ADCx,uint8_t ADC_Channel_x,uint8_t ADC_SampleTime_xCycles);

u16 getADCAverage(ADC_TypeDef* ADCx,uint8_t ADC_Channel_x,uint8_t ADC_SampleTime_xCycles,u8 times);

short getTemperate(void);

#endif

