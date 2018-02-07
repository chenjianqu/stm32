#include "delay.h"
#include "stm32f4xx_it.h"


/*
ͨ��SysTick��������ʱ

SysTickΪ24λ��������ʱ��

CTRL SysTick���ƺ�״̬�Ĵ���
LOAD SysTick�Զ���װ�س�ֵ�Ĵ���
VAL SysTick��ǰֵ�Ĵ���
CALIB SysTickУ׼ֵ�Ĵ���

*/


/********ʹ��SysTick��ʱ����������ʱ����ʱ��ɺ�ر��ж�***********************/

static u16 ms_fac=0;//ÿ��ʱ1ms��Ҫ�Ļ�������
static u16 us_fac=0;//ÿ��ʱ1us��Ҫ�Ļ�������


/************************************��ʱ����ʼ��************************************/
void initDelay(void)
{
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//ѡ��SysTick��ʱ��ԴƵ��ΪHCLK�İ˷�֮һ��HLCKΪAHB����ʱ�ӣ�HCLKΪ168MHz,����8Ϊ21MHz,2.1x10^7 Hz
	
	us_fac=21;//ÿus�Ļ���������
/*ϵͳʱ�ӵ���APB����ʱ��ֵ������8��Ϊ��һ����ѡ���ǰ˷�֮һ��ʱ��Դ��
	����1000000��ÿ���΢������ÿ���APB��������������SystemCoreClock��
	����fac_usΪÿ΢�붨ʱ���������ĵĻ�����������*/
	ms_fac=us_fac*1000;
}


/***********************************�����ӳ�**********************************/
void delay_ms(unsigned int ms)
{
	unsigned int temp;
	SysTick->LOAD=ms*ms_fac;//����ÿ���ж�����Ļ������ڣ�24λ�Ĵ��������ֵΪ16777216
	SysTick->VAL=0x00;//��������0,��ʼ�����������LOAD����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//����CTRL���λ��ʹ�ܼ�����
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	/*��CTRL�ĵ�16λΪ1���ʱʱ�䵽��,1����16λ����temp����������,������Ϊ0��������Ϊ0����16Ϊ1�Ҷ�ʱ���Ѿ�ʹ�ܣ�������ѭ��
	CTRL�Ĵ�����0��ʹ��λ
	1λ��Ϊ1�ǿ����жϣ�Ϊ0ʱ���ж�
	2λ��0=�ⲿʱ��Դ��1=�ڲ�ʱ��Դ
	16λ���ϴζ�ȡ�Ĵ�����SysTick�Ѿ�������0���λΪ1.�����ȡ��λ ���λ�Զ�����
	*/
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//�رռ�����
	SysTick->VAL=0x00;//��ռ�����
}

/***********************************΢���ӳ�**********************************/
void delay_us(unsigned int us)
{
	unsigned int temp;
	SysTick->LOAD=us*us_fac;//����ÿ���ж�����Ļ�������
	SysTick->VAL=0x00;//��������0,��ʼ�����������LOAD����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//ͨ������CTRL���λʹ�ܼ�����
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	/*��CTRL�ĵ�ʮ��λΪ1���ʱʱ�䵽��,1����16λ����temp����������,������Ϊ0��������Ϊ0*/
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//�رռ�����
	SysTick->VAL=0x00;//��ռ�����
}

/***********************************���ӳ�**********************************/
void delay_s(unsigned int s)
{
	s*=10;
	for(;s>0;s--){
		delay_ms(100);
	}
}


/***********************************�����ӳ�**********************************/
void delay_ds(unsigned int ds)
{
	for(;ds>0;ds--){
		delay_ms(100);
	}
}




/*********************ʹ��SysTick�ж�����ʱ,�ȽϺķ���Դ***********************/
void delay_IT_Init(void)
{
	if(SysTick_Config(SystemCoreClock/1000))//SystemCoreClockΪϵͳʱ��Ƶ�ʵ�ֵ������1000����ϵͳʱ��Ƶ��ֵ��1/1000����仰�������ж�ʱ����Ϊ1ms
		while(1);
}
static unsigned int timingDelay;
void delay_ms1(unsigned int t)
{
	timingDelay=t;
	while(timingDelay!=0);//�ȴ��жϺ�����timeDelay���ꡣ
}
/*
void SysTick_Handler(void)//1ms����һ���ж�,ÿ���ж�timingDelay--;
{
	if(timingDelay!=0x00)
	{
		timingDelay--;
	}
}
*/
