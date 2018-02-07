#include "delay.h"
#include "stm32f4xx_it.h"


/*
通过SysTick来进行延时

SysTick为24位倒计数计时器

CTRL SysTick控制和状态寄存器
LOAD SysTick自动重装载除值寄存器
VAL SysTick当前值寄存器
CALIB SysTick校准值寄存器

*/


/********使用SysTick定时器来进行延时，延时完成后关闭中断***********************/

static u16 ms_fac=0;//每延时1ms需要的机器周期
static u16 us_fac=0;//每延时1us需要的机器周期


/************************************定时器初始化************************************/
void initDelay(void)
{
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//选择SysTick的时钟源频率为HCLK的八分之一，HLCK为AHB总线时钟，HCLK为168MHz,除以8为21MHz,2.1x10^7 Hz
	
	us_fac=21;//每us的机器周期数
/*系统时钟等于APB总线时钟值，除以8因为上一句中选的是八分之一的时钟源，
	除以1000000是每秒的微秒数。每秒的APB机器周期数等于SystemCoreClock。
	最后得fac_us为每微秒定时器溢出所需的的机器周期数。*/
	ms_fac=us_fac*1000;
}


/***********************************毫秒延迟**********************************/
void delay_ms(unsigned int ms)
{
	unsigned int temp;
	SysTick->LOAD=ms*ms_fac;//设置每次中断所需的机器周期，24位寄存器，最大值为16777216
	SysTick->VAL=0x00;//计数器清0,开始后计数器将从LOAD加载
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//设置CTRL最低位可使能计数器
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	/*若CTRL的第16位为1则计时时间到了,1左移16位，与temp进行与运算,若本身为0则与运算为0，当16为1且定时器已经使能，则跳出循环
	CTRL寄存器：0：使能位
	1位：为1是开启中断，为0时无中断
	2位：0=外部时钟源，1=内部时钟源
	16位：上次读取寄存器后SysTick已经数到了0则该位为1.如果读取该位 则该位自动清零
	*/
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭计数器
	SysTick->VAL=0x00;//清空计数器
}

/***********************************微秒延迟**********************************/
void delay_us(unsigned int us)
{
	unsigned int temp;
	SysTick->LOAD=us*us_fac;//设置每次中断所需的机器周期
	SysTick->VAL=0x00;//计数器清0,开始后计数器将从LOAD加载
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//通过设置CTRL最低位使能计数器
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	/*若CTRL的第十六位为1则计时时间到了,1左移16位，与temp进行与运算,若本身为0则与运算为0*/
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭计数器
	SysTick->VAL=0x00;//清空计数器
}

/***********************************秒延迟**********************************/
void delay_s(unsigned int s)
{
	s*=10;
	for(;s>0;s--){
		delay_ms(100);
	}
}


/***********************************分秒延迟**********************************/
void delay_ds(unsigned int ds)
{
	for(;ds>0;ds--){
		delay_ms(100);
	}
}




/*********************使用SysTick中断来延时,比较耗费资源***********************/
void delay_IT_Init(void)
{
	if(SysTick_Config(SystemCoreClock/1000))//SystemCoreClock为系统时钟频率的值。除以1000即是系统时钟频率值的1/1000。这句话是设置中断时间间隔为1ms
		while(1);
}
static unsigned int timingDelay;
void delay_ms1(unsigned int t)
{
	timingDelay=t;
	while(timingDelay!=0);//等待中断函数把timeDelay减完。
}
/*
void SysTick_Handler(void)//1ms调用一次中断,每次中断timingDelay--;
{
	if(timingDelay!=0x00)
	{
		timingDelay--;
	}
}
*/
