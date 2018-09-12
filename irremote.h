#ifndef __IRREMOTE_H
#define __IRREMOTE_H
#include "stm32f4xx_it.h"

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0      		   

void initIRremote(void);

u8 scanRemote(void);

#endif

