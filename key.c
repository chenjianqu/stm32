#include "key.h"
#include "exti.h"

/*KEY1��PA0 KEY2��PF11 ����Ϊ�ߵ�ƽ*/

void initKey(void)
{
	
	//initGPIO(GPIOA,GPIO_Pin_0,	GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	//initGPIO(GPIOF,GPIO_Pin_11,	GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	
	initGPIO(GPIOE,GPIO_Pin_2,	GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	initGPIO(GPIOE,GPIO_Pin_3,	GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	initGPIO(GPIOE,GPIO_Pin_4,	GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	//�ⲿ�ж�����
	initEXTI(GPIOA,GPIO_Pin_0,	EXTI_Mode_Interrupt,EXTI_Trigger_Rising);
	initEXTI(GPIOF,GPIO_Pin_11,	EXTI_Mode_Interrupt,EXTI_Trigger_Rising);
	
	//�ж����ȼ�����
	initNVIC(EXTI0_IRQn,		1,1);
	initNVIC(EXTI15_10_IRQn,1,1);
}

u8 scanKey(u8 n)
{
	switch(n)
	{
		//��Ӧ��ӥKEY0 1 2
		case 0:
			return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2);
			break;
		case 1:
			return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);
			break;
		case 2:
			return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
			break;
		//��ӦEU������KEY0 1
		case 3:
			return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
			break;
		case 4:
			return GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11);
			break;
	}
	
}


u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;// �ް�������
}


