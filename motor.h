#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f4xx_it.h"

//与舵机的定时器频率不同
void initMotorTIM(TIM_TypeDef* TIMx,u16 period);

void initMotorChannel(TIM_TypeDef* TIMx,u8 ch,GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x,u16 motorInitSpeed);

void setMotorSpeed(TIM_TypeDef* TIMx,u8 ch,u16 speed);

void startMotor(TIM_TypeDef* TIMx,u8 ch);

void stopMotor(TIM_TypeDef* TIMx,u8 ch);

#endif

