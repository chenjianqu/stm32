#ifndef __SysTick_H
#define __SysTick_H

#include "stm32f4xx_it.h"
#include "led.h"
#include "delay.h"
#include "uart.h"

#define TIM_OC_1 0
#define TIM_OC_2 1
#define TIM_OC_3 2
#define TIM_OC_4 3

void initTIMBase(TIM_TypeDef* TIMx,uint16_t TIM_Prescaler,uint16_t TIM_CounterMode_x,uint32_t TIM_Period,uint16_t TIM_ClockDivision,uint8_t TIM_RepetitionCounter);

void enableTIMClock(TIM_TypeDef* TIMx);

u8 getTIM_AF(TIM_TypeDef* TIMx);

void initPWM(void);
void initInputCapture(u32 arr,u16 psc);
void initTimer1(void);
void initTimer3(void);
void initTimer6(void);
void initTimer7(void);
void initTimer8(void);
void initTimer9(void);
void initTimer10(void);

#endif

