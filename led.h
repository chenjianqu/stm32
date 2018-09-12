#ifndef __LED_H
#define __LED_H
#include "stm32f4xx_it.h"

#define onLED1 GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define offLED1 GPIO_SetBits(GPIOC,GPIO_Pin_0)

#define onLED2 GPIO_ResetBits(GPIOD,GPIO_Pin_3)
#define offLED2 GPIO_SetBits(GPIOD,GPIO_Pin_3)

void initLED(void);
void changeLED(u8 n);
void twinkleLED(u8 n);
void disableLED(u8 LED1,u8 LED2);

#endif
