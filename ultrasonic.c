#include "ultrasonic.h"

/*
超声波模块 TRIG触发引脚 ECHO信号脉冲引脚
*/

/*
发一个10ms的高脉冲就能触发TrigPin，该模块被触发后，超声波发射头将发出8个40kHz周期电平，同时检测回波。一旦检测到有回波信号则输出回响信号。
回响信号是一个脉冲的宽度成正比的距离对象。可通过发射信号到收到的回响信号时间间隔可以计算得到距离。公式: uS/58=厘米；或者uS/148=英寸。
*/


#define ULTRA_ECHO_GPIOX 		GPIOB
#define ULTRA_ECHO_GPIO_PIN GPIO_Pin_11
#define ULTRA_TRIG_GPIOX 		GPIOE
#define ULTRA_TRIG_GPIO_PIN GPIO_Pin_13

void initUltrasonic(void)
{
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TypeDef* TIMx=TIM2;
	
	//ECHO引脚 GPIOB4  复用功能,上拉
	initGPIO(ULTRA_ECHO_GPIOX,ULTRA_ECHO_GPIO_PIN,GPIO_Mode_AF,	GPIO_AF_TIM2,	GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	//TRIG引脚 GPIOB4
	initGPIO(ULTRA_TRIG_GPIOX,ULTRA_TRIG_GPIO_PIN,GPIO_Mode_OUT,AF_DISABLE,		GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);

	//初始化TIMBase
	initTIMBase(TIMx,167,TIM_CounterMode_Up,9999,TIM_CKD_DIV1,0);

	TIM_ICInitStructure.TIM_Channel=TIM_Channel_4;//选择输入的，IC1映射到TI1
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//映射到TI1上,即信号可以映射到别的通道上
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//配置输入分频，不分频。分频器的作用：比如2分频就是捕获到两次上升沿采集一次
	TIM_ICInitStructure.TIM_ICFilter=0x00;//配置输入滤波器，不滤波（滤波的概念：比如当定时器检测到上升沿时，会根据设置的次数连续检测几次是否为高电平，
	//若干次之后若仍然为高电平则进入中断。因此高电平时间不够长的脉冲将被忽略，进而达到滤波的作用）
	TIM_ICInit(TIMx,&TIM_ICInitStructure);

	TIM_ITConfig(TIMx,TIM_IT_Update|TIM_IT_CC4,ENABLE);
	TIM_Cmd(TIMx,ENABLE);

	initNVIC(TIM2_IRQn,2,0);
}	

u8 ultra_upflag=0;//是否接收到上升沿
u16 ultra_pulseWidth=0;//脉冲宽度
u16 ultra_temp=0;


/***************************************定时器3中断*************************************/

void TIM2_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM2,TIM_IT_CC4))
	{
		USART_SendData(USART1,'*');
		if(GPIO_ReadInputDataBit(ULTRA_ECHO_GPIOX,ULTRA_ECHO_GPIO_PIN))//上升沿捕获
		{
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Falling);//设置为下降沿捕获
			TIM_SetCounter(TIM2,0);//清空定时器
			ultra_upflag=1;	//标记上升沿已经被捕获	
		}
		else //下降沿捕获
		{
			ultra_temp=TIM_GetCapture4(TIM2);//读取计数器的值，即获取脉冲宽度
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Rising); //设置为上升沿捕获
			
			if(ultra_upflag)//完成了一次高电平捕获 
			{
				ultra_pulseWidth=ultra_temp;
				ultra_upflag=0;
			}
			
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC4);
	}
}


u16 getDistance(void)
{
	GPIO_ResetBits(ULTRA_TRIG_GPIOX,ULTRA_TRIG_GPIO_PIN);
	delay_us(2);
	GPIO_SetBits	(ULTRA_TRIG_GPIOX,ULTRA_TRIG_GPIO_PIN);
	delay_us(12);
	GPIO_ResetBits(ULTRA_TRIG_GPIOX,ULTRA_TRIG_GPIO_PIN);
	delay_ms(100);
	u16 distance=ultra_pulseWidth;
	distance=distance/58;
	return distance;
}

