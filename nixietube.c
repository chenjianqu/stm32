#include "nixietube.h"

/*4位数码管 使用定时器12*/

/*
引脚分布，正面看，左下为1 然后逆时针到12
*/



//模块引脚11
#define NXT_GPIO_A GPIOF
#define NXT_GPIO_Pin_A GPIO_Pin_11
//7
#define NXT_GPIO_B GPIOF
#define NXT_GPIO_Pin_B GPIO_Pin_7
//4
#define NXT_GPIO_C GPIOF
#define NXT_GPIO_Pin_C GPIO_Pin_4
//2
#define NXT_GPIO_D GPIOF
#define NXT_GPIO_Pin_D GPIO_Pin_2
//1
#define NXT_GPIO_E GPIOF
#define NXT_GPIO_Pin_E GPIO_Pin_1
//10
#define NXT_GPIO_F GPIOF
#define NXT_GPIO_Pin_F GPIO_Pin_10
//5
#define NXT_GPIO_G GPIOF
#define NXT_GPIO_Pin_G GPIO_Pin_5
//3
#define NXT_GPIO_P GPIOF
#define NXT_GPIO_Pin_P GPIO_Pin_3
//12
#define NXT_GPIO_DIG1 GPIOF
#define NXT_GPIO_Pin_DIG1 GPIO_Pin_12
//9
#define NXT_GPIO_DIG2 GPIOF
#define NXT_GPIO_Pin_DIG2 GPIO_Pin_9
//8
#define NXT_GPIO_DIG3 GPIOF
#define NXT_GPIO_Pin_DIG3 GPIO_Pin_8
//6
#define NXT_GPIO_DIG4 GPIOF
#define NXT_GPIO_Pin_DIG4 GPIO_Pin_6


//内部函数声明
void showAll(void);
void offAll(void);
void showChar(u8 num,u8 setpoint);


void initNixietube(void)
{
	initGPIO(NXT_GPIO_A,		NXT_GPIO_Pin_A,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_B,		NXT_GPIO_Pin_B,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_C,		NXT_GPIO_Pin_C,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_D,		NXT_GPIO_Pin_D,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_E,		NXT_GPIO_Pin_E,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_F,		NXT_GPIO_Pin_F,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_G,		NXT_GPIO_Pin_G,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_P,		NXT_GPIO_Pin_P,		GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_DIG1,	NXT_GPIO_Pin_DIG1,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_DIG2,	NXT_GPIO_Pin_DIG2,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_DIG3,	NXT_GPIO_Pin_DIG3,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(NXT_GPIO_DIG4,	NXT_GPIO_Pin_DIG4,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//10kHz,0.1ms,30ms,定时器输入时钟为84MHZ
	initTIMBase(TIM12,8399,TIM_CounterMode_Up,19,TIM_CKD_DIV1,0);
	initNVIC(TIM8_BRK_TIM12_IRQn,1,1);
	TIM_ClearFlag(TIM12,TIM_FLAG_Update);
	TIM_ITConfig(TIM12,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM12,ENABLE);
}	

u8 nxtCnt=1;
u8 n1,n2,n3,n4;
u8 p1,p2,p3,p4;

/***************************************定时器8_12中断*************************************/
void TIM8_BRK_TIM12_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM8,TIM_IT_Break)){
		
		TIM_ClearITPendingBit(TIM8,TIM_IT_Break);
	}
	
	/*******************************************定时器12更新中断****************************************/
	else if(TIM_GetFlagStatus(TIM12,TIM_IT_Update))
	{
		switch(nxtCnt)
		{
			case 1:
				showChar(n1,p1);
				GPIO_ResetBits(NXT_GPIO_DIG1,NXT_GPIO_Pin_DIG1);	
				break;
			case 2:
				showChar(n2,p2);
				GPIO_ResetBits(NXT_GPIO_DIG2,NXT_GPIO_Pin_DIG2);	
				break;
			case 3:
				showChar(n3,p3);
				GPIO_ResetBits(NXT_GPIO_DIG3,NXT_GPIO_Pin_DIG3);	
				break;
			case 4:
				showChar(n4,p4);
				GPIO_ResetBits(NXT_GPIO_DIG4,NXT_GPIO_Pin_DIG4);
				nxtCnt=0;
				break;
		}
		nxtCnt++;
		TIM_ClearITPendingBit(TIM12,TIM_IT_Update);
	}
	
}


void showNumber(u8 a,u8 pa,u8 b,u8 pb,u8 c,u8 pc,u8 d,u8 pd)
{
	n1=a;
	p1=pa;
	n2=b;
	p2=pb;
	n3=c;
	p3=pc;
	n4=d;
	p4=pd;
}	

void showChar(u8 num,u8 setpoint)
{
	switch(num)
	{
		case 0:
			showAll();
			GPIO_ResetBits(NXT_GPIO_G,NXT_GPIO_Pin_G);
			break;
		case 1:
			offAll();
			GPIO_SetBits(NXT_GPIO_B,NXT_GPIO_Pin_B);
			GPIO_SetBits(NXT_GPIO_C,NXT_GPIO_Pin_C);
			break;
		case 2:
			showAll();
			GPIO_ResetBits(NXT_GPIO_F,NXT_GPIO_Pin_F);
			GPIO_ResetBits(NXT_GPIO_C,NXT_GPIO_Pin_C);
			break;
		case 3:
			showAll();
			GPIO_ResetBits(NXT_GPIO_F,NXT_GPIO_Pin_F);
			GPIO_ResetBits(NXT_GPIO_E,NXT_GPIO_Pin_E);
			break;
		case 4:
			showAll();
			GPIO_ResetBits(NXT_GPIO_A,NXT_GPIO_Pin_A);
			GPIO_ResetBits(NXT_GPIO_D,NXT_GPIO_Pin_D);
			GPIO_ResetBits(NXT_GPIO_E,NXT_GPIO_Pin_E);
			break;
		case 5:
			showAll();
			GPIO_ResetBits(NXT_GPIO_B,NXT_GPIO_Pin_B);
			GPIO_ResetBits(NXT_GPIO_E,NXT_GPIO_Pin_E);
			break;
		case 6:
			showAll();
			GPIO_ResetBits(NXT_GPIO_B,NXT_GPIO_Pin_B);
			break;
		case 7:
			offAll();
			GPIO_SetBits(NXT_GPIO_A,NXT_GPIO_Pin_A);
			GPIO_SetBits(NXT_GPIO_B,NXT_GPIO_Pin_B);
			GPIO_SetBits(NXT_GPIO_C,NXT_GPIO_Pin_C);
			break;
		case 8:
			showAll();
			break;
		case 9:
			showAll();
			GPIO_ResetBits(NXT_GPIO_E,NXT_GPIO_Pin_E);
			break;
		case OFF:
			offAll();
			break;
		default:
			offAll();
	}
	if(setpoint!=OFF)
		GPIO_SetBits(NXT_GPIO_P,NXT_GPIO_Pin_P);
}

//ABCDEFG引脚进入准备状态
void showAll(void)
{
	GPIO_SetBits(NXT_GPIO_A,NXT_GPIO_Pin_A);
	GPIO_SetBits(NXT_GPIO_B,NXT_GPIO_Pin_B);
	GPIO_SetBits(NXT_GPIO_C,NXT_GPIO_Pin_C);
	GPIO_SetBits(NXT_GPIO_D,NXT_GPIO_Pin_D);
	GPIO_SetBits(NXT_GPIO_E,NXT_GPIO_Pin_E);
	GPIO_SetBits(NXT_GPIO_F,NXT_GPIO_Pin_F);
	GPIO_SetBits(NXT_GPIO_G,NXT_GPIO_Pin_G);
	
	GPIO_ResetBits(NXT_GPIO_P,NXT_GPIO_Pin_P);
	
	GPIO_SetBits(NXT_GPIO_DIG1,NXT_GPIO_Pin_DIG1);
	GPIO_SetBits(NXT_GPIO_DIG2,NXT_GPIO_Pin_DIG2);
	GPIO_SetBits(NXT_GPIO_DIG4,NXT_GPIO_Pin_DIG3);
	GPIO_SetBits(NXT_GPIO_DIG3,NXT_GPIO_Pin_DIG4);
}
//所有引脚进入非准备状态
void offAll(void)
{
	GPIO_ResetBits(NXT_GPIO_A,NXT_GPIO_Pin_A);
	GPIO_ResetBits(NXT_GPIO_B,NXT_GPIO_Pin_B);
	GPIO_ResetBits(NXT_GPIO_C,NXT_GPIO_Pin_C);
	GPIO_ResetBits(NXT_GPIO_D,NXT_GPIO_Pin_D);
	GPIO_ResetBits(NXT_GPIO_E,NXT_GPIO_Pin_E);
	GPIO_ResetBits(NXT_GPIO_F,NXT_GPIO_Pin_F);
	GPIO_ResetBits(NXT_GPIO_G,NXT_GPIO_Pin_G);
	GPIO_ResetBits(NXT_GPIO_P,NXT_GPIO_Pin_P);
	
	GPIO_SetBits(NXT_GPIO_DIG1,NXT_GPIO_Pin_DIG1);
	GPIO_SetBits(NXT_GPIO_DIG2,NXT_GPIO_Pin_DIG2);
	GPIO_SetBits(NXT_GPIO_DIG4,NXT_GPIO_Pin_DIG3);
	GPIO_SetBits(NXT_GPIO_DIG3,NXT_GPIO_Pin_DIG4);
}


