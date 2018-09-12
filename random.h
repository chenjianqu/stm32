#ifndef __RANDOM_H
#define __RANDOM_H
#include "stm32f4xx_it.h"

u8 initRandom(void);
u32 getRandom(void);
int getIntervalRandom(int min,int max);

#endif


