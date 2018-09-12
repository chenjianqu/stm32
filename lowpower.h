#ifndef __LOWPOWER_H
#define __LOWPOWER_H
#include "stm32f4xx_it.h"

void enterStandby(void);

u8 checkWakeup(u8 cnt);

#endif
