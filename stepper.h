#ifndef __STEPPER_H
#define __STEPPER_H

#include "stm32f4xx_it.h"

//步进电机的模式
#define STEPPER_BEAT_4 4
#define STEPPER_BEAT_8 8

#define HIGH 1
#define LOW 0


void initStepper(u8 stepper_beats_x,GPIO_TypeDef* GPIOx_A,uint16_t GPIO_Pin_x_A,GPIO_TypeDef* GPIOx_B,uint16_t GPIO_Pin_x_B,GPIO_TypeDef* GPIOx_C,uint16_t GPIO_Pin_x_C,GPIO_TypeDef* GPIOx_D,uint16_t GPIO_Pin_x_D);

void setStepperSpeed(u16 period);

void startStepper(void);

void stopStepper(void);

#endif

