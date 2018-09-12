#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f4xx_it.h"

void initServoTIM(TIM_TypeDef* TIMx);

void initServoChannel(TIM_TypeDef* TIMx,u8 ch,GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x,u16 servoInitAngle);

void setServoAngle(TIM_TypeDef* TIMx,u8 ch,u16 angle,u16 delaytime);

void startServo(TIM_TypeDef* TIMx,u8 ch,u16 servoInitAngle);

void stopServo(TIM_TypeDef* TIMx,u8 ch);


#endif


