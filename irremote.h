#ifndef __IRREMOTE_H
#define __IRREMOTE_H
#include "stm32f4xx_it.h"

//����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
//����ѡ�õ�ң����ʶ����Ϊ0
#define REMOTE_ID 0      		   

void initIRremote(void);

u8 scanRemote(void);

#endif

