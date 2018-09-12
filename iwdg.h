#ifndef __IWDG_H
#define __IWDG_H
#include "stm32f4xx_it.h"

void initIWDG(u8 prer,u16 rlr);//IWDG³õÊ¼»¯
void IWDG_Feed(void);  //Î¹¹·º¯Êý

#endif
