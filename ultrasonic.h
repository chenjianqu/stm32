#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "stm32f4xx_it.h"

void initUltrasonic(void);
u16 getDistance(void);

#endif
