#ifndef __IWDG_H
#define __IWDG_H
#include "stm32f4xx_it.h"

void initIWDG(u8 prer,u16 rlr);//IWDG��ʼ��
void IWDG_Feed(void);  //ι������

#endif
