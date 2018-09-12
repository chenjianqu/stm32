#ifndef __TPAD_H
#define __TPAD_H
#include "sys.h"
#include "stm32f4xx_it.h"

	
u8 initTPAD(void);
void TPAD_Reset(void);
u16 TPAD_getChargeTime(void);
u8 TPAD_Scan(u8 mode);

#endif