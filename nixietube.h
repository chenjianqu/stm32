#ifndef __NIXIETUBE_H
#define __NIXIETUBE_H
#include "stm32f4xx_it.h"

//Ϩ�������
#define OFF 10
#define ON 1

void initNixietube(void);
//��������һλ���֣��Ƿ��е� ������
void showNumber(u8 a,u8 p1,u8 b,u8 p2,u8 c,u8 p3,u8 d,u8 p4);


#endif
