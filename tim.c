#include "tim.h"


/* 
高级定时器：TIM1,TIM8，带死区控制盒紧急刹车，PWM电机控制
通用定时器：TIM2-TIM5,定时计数，PWM输出，输入捕获，输出比较
基本定时器：TIM6,TIM7,通用定时器简化版，无捕获比较通道。主要用于驱动DAC
通用定时器：TIM9-TIM14 通用定时器
每个定时器有4个独立通道，6个独立的请求生成器
TIM1:红外模块
TIM2超声波模块
TIM3相机
TIM4
TIM5
TIM6
TIM7步进电机
TIM8
TIM9
TIM10
TIM11蜂鸣器
TIM12数码管
TIM13定时器
TIM14
*/

//定时器9,10,11,1,8挂在APB2上
	/*只有APB2的预分频系数为1时 定时器输入时钟=APB2时钟，否则定时器输入时钟=APB2*2
	本程序中APB2的预分频系数为2，定时器输入时钟为168M/2*2=168M。
	APB1预分频系数为4,所以168/4*2=84MHz
	*/
	/*
	定时器9-14没有单独的中断向量和中断函数，需要和TIM1和TIM8共用，所以
	TIM1_BRK_TIM9_IRQn  TIM1_UP_TIM10_IRQn  TIM1_TRG_COM_TIM11_IRQn 
	TIM8_BRK_TIM12_IRQn TIM8_UP_TIM13_IRQn  TIM8_TRG_COM_TIM14_IRQn 
	*/




/***************************************配置定时器*************************************/

void initTIMBase(TIM_TypeDef* TIMx,uint16_t TIM_Prescaler,uint16_t TIM_CounterMode_x,uint32_t TIM_Period,uint16_t TIM_ClockDivision,uint8_t TIM_RepetitionCounter)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//开启定时器时钟
	enableTIMClock(TIMx);
	
	//重置定时器
	TIM_DeInit(TIMx); 
	
	//以下两个参数决定定时器时间周期，都是16位寄存器。范围为1-65535。
	TIM_TimeBaseStructure.TIM_Period=TIM_Period;//设置定时器计数值，数值加1
	TIM_TimeBaseStructure.TIM_Prescaler=TIM_Prescaler;//设置预分频系数，对选择的时钟源进行分频。通过降低所需时钟频率可以延长定时时间。定时器接受一个时钟脉冲就减一。数值加1

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_ClockDivision;//设置时钟分频因子
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_x;//设置计数模式，向上向下或两边
	TIM_TimeBaseStructure.TIM_RepetitionCounter=TIM_RepetitionCounter;//设置重复计数，重复溢出多少次才溢出中断
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseStructure);
}




/***************************************开启定时器时钟*************************************/

void enableTIMClock(TIM_TypeDef* TIMx)
{
	if(TIMx==TIM1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	else if(TIMx==TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	else if(TIMx==TIM3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	else if(TIMx==TIM4)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	else if(TIMx==TIM5)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	else if(TIMx==TIM6)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	else if(TIMx==TIM7)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	else if(TIMx==TIM8)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	else if(TIMx==TIM9)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	else if(TIMx==TIM10)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);
	else if(TIMx==TIM11)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);
	else if(TIMx==TIM12)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);
	else if(TIMx==TIM13)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);
	else if(TIMx==TIM14)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);
}



/***************************************将定时器组转换为第几个*************************************/
//根据st库中的定义
u8 getTIM_AF(TIM_TypeDef* TIMx)
{
	u8 i;
	if(TIMx==TIM1||TIMx==TIM2)
		i=1;
	else if(TIMx==TIM3||TIMx==TIM4||TIMx==TIM5)
		i=2;
	else if(TIMx==TIM8||TIMx==TIM9||TIMx==TIM10||TIMx==TIM11)
		i=3;
	else if(TIMx==TIM12||TIMx==TIM13||TIMx==TIM14)
		i=9;
	return i;
}






//定时器配置并开启，使用定时器3
void initTimer3(void){
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/*若要设置 1s 的定时器，设置自动装载值Period=9999,从0开始，到9999正好10000。
	则需要每个时钟源周期为100us。若输入时钟为90MHz,则可以设置预分频系数为（9000-1）,因为 分频值=预分频系数+1
	1MHz为1百万Hz。每9000输入时钟脉冲只有1个脉冲有效。每s有9千万次输入时钟脉冲，则定时器时钟脉冲为10000次，刚好1s。
	计算公式为：溢出时间=（重装载值*预分频系数）*输入时间频率 单位s
	只有APB1的预分频系数为1时 定时器输入时钟=APB1时钟，否则定时器输入时钟=APB1*2
	本程序中APB1的预分频系数为4，定时器输入时钟为168M/4*2=82M。
	*/
	
	//定时器设置,0.01M,10kHz,0.1ms,
	initTIMBase(TIM3,8199,TIM_CounterMode_Up,1000,TIM_CKD_DIV1,0);//0.01ms，
	
	//中断配置
	initNVIC(TIM3_IRQn,2,0);
	
	//清中断标志，以免一启用中断后立即产生中断
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	
	//开启定时器中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//开启定时器
	TIM_Cmd(TIM3,ENABLE);
}



void initTimer1(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_DeInit(TIM1);
	
	//10kHz,0.1ms,  100ms
	initTIMBase(TIM1,16799,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	
	initNVIC(TIM1_UP_TIM10_IRQn,1,1);
	initNVIC(TIM1_CC_IRQn,			1,1);
	initNVIC(TIM1_TRG_COM_TIM11_IRQn,	1,1);
	initNVIC(TIM1_BRK_TIM9_IRQn,			1,1);

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_CC1|TIM_IT_COM|TIM_IT_Trigger|TIM_IT_Break,ENABLE);
	
	TIM_Cmd(TIM1,ENABLE);
}

/***************************************定时器1中断*************************************/
void XXX_TIM1_CC_IRQHandler(void){
	changeLED(3);
	USART_SendData(USART1,'*');
		
		
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC1);
}

void initTimer9(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	TIM_DeInit(TIM9);
	
	//10kHz,0.1ms,  100ms
	initTIMBase(TIM9,16799,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	initNVIC(TIM1_BRK_TIM9_IRQn,1,1);
	TIM_ClearFlag(TIM9,TIM_FLAG_Update);
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM9,ENABLE);
}
/***************************************定时器1_9中断*************************************/
void TIM1_BRK_TIM9_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM1,TIM_IT_Break)){
		changeLED(3);
		USART_SendData(USART1,'*');
		
		
		TIM_ClearITPendingBit(TIM10,TIM_IT_Break);
	}
	
	else if(TIM_GetFlagStatus(TIM9,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'#');
		
		
		TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
	}
}


void initTimer10(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);

	TIM_DeInit(TIM10);
	
	//10kHz,0.1ms,  100ms
	initTIMBase(TIM10,16799,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	
	initNVIC(TIM1_UP_TIM10_IRQn,1,1);

	TIM_ClearFlag(TIM10,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM10,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM10,ENABLE);
	
}






/***************************************定时器6中断*************************************/
void TIM6_DAC_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM6,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'*');
		
		
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	}

}



void initTimer8(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);

	TIM_DeInit(TIM8);
	
	//10kHz,0.1ms,  100ms
	initTIMBase(TIM8,16799,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	
	initNVIC(TIM8_UP_TIM13_IRQn,1,1);
	initNVIC(TIM8_BRK_TIM12_IRQn,			1,1);
	initNVIC(TIM8_TRG_COM_TIM14_IRQn,	1,1);
	initNVIC(TIM8_CC_IRQn,			1,1);

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_CC1|TIM_IT_COM|TIM_IT_Trigger|TIM_IT_Break,ENABLE);
	
	TIM_Cmd(TIM1,ENABLE);
}





/***************************************定时器1_14中断*************************************/
void TIM8_TRG_COM_TIM14_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM8,TIM_IT_Trigger)){
			changeLED(3);
			USART_SendData(USART1,'*');
		
		
		TIM_ClearITPendingBit(TIM8,TIM_IT_Trigger);
	}
	
	else if(TIM_GetFlagStatus(TIM8,TIM_IT_COM)){
		changeLED(3);
		USART_SendData(USART1,'*');
		
		
		TIM_ClearITPendingBit(TIM8,TIM_IT_COM);
	}
	
	else if(TIM_GetFlagStatus(TIM14,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'*');
		
		
		TIM_ClearITPendingBit(TIM14,TIM_IT_Update);
	}
}

/*********************************** PWM输出 *********************************/

void initPWM(void)
{
	
		TIM_TypeDef* TIMx=TIM2;
	
	//PWM接口开启 并执行初始化
		TIM_OCInitTypeDef TIM_OCInitStructure;
	
		//开启TIM2时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		//配置GPIO
		initGPIO(GPIOB,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_TIM2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);

		//配置TIM2,1kHz
		initTIMBase(TIMx,83,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);

		//PWM模式设置 与通道设置TIM2_CH2
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//模式PWM1，无论定时器是向上计数还是向下计数，计数值>有效值时为有效电平。PWM2模式相反        
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出比较使能
		TIM_OCInitStructure.TIM_Pulse = 10;//设置比较值，写CCRx，当计数器到达500时,电平发送跳变
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设输出极性为高，即有效值为高电平,当计数器<200时,PWM为高电平
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);//通道2初始化
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);//使能输出比较预装载
		TIM_ARRPreloadConfig(TIMx, ENABLE);//使能自动重装载寄存器允许位
		//开启TIM2
		TIM_Cmd(TIMx, ENABLE); 
		//PWM占空比设置,即电平发生跳变的值
}






/*********************************** 输入捕获 *********************************/

/*相关函数：
获取通道捕获值uint32_t TIM_GetCapture1(TIM_TypeDef* TIMx);
通道极性设置独立函数void TIM_OCxPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)；
*/

TIM_ICInitTypeDef TIM5_ICInitStructure;
void initInputCapture(u32 arr,u16 psc)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	//配置GPIO
	initGPIO(GPIOA,GPIO_Pin_0,GPIO_Mode_AF,GPIO_AF_TIM5,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	initTIMBase(TIM5,psc,TIM_CounterMode_Up,arr,TIM_CKD_DIV1,0);
		
	TIM5_ICInitStructure.TIM_Channel=TIM_Channel_1;//选择输入的，IC1映射到TI1
	
	TIM5_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//上升沿捕获
	
	TIM5_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//映射到TI1上,即信号可以映射到别的通道上
	
	TIM5_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//配置输入分频，不分频。分频器的作用：比如2分频就是捕获到两次上升沿采集一次
	
	TIM5_ICInitStructure.TIM_ICFilter=0x00;//配置输入滤波器，不滤波（滤波的概念：比如当定时器检测到上升沿时，会根据设置的次数连续检测几次是否为高电平，
	//若干次之后若仍然为高电平则进入中断。因此高电平时间不够长的脉冲将被忽略，进而达到滤波的作用）
	TIM_ICInit(TIM5,&TIM5_ICInitStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断（即当定时器溢出时），允许CC1捕获中断
	
	TIM_Cmd(TIM5,ENABLE);//使能定时器5
	
	initNVIC(TIM5_IRQn,2,0);
}


/*捕获状态，这个变量的作用是记录定时器溢出的次数，考虑到有些脉冲宽度可能过长，导致定时器溢出，此时必须记录定时器溢出的次数才能计算准确的脉冲宽度
位[7],捕获一次完成的标志，当定时器发生更新事件时（更新中断）
位[6],捕获高电平的标志，当位6为1且捕获到下降沿时 认为完成了捕获
[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)*/
u8 TIM5CH1_CAPTURE_STA=0;//输入捕获状态
u8 tim_capture_flag=0;

u32 TIM5CH1_CAPTURE_VAL;//输入捕获值
void TIM5_IRQHandler(void)
{
	if((TIM5CH1_CAPTURE_STA&0x80)==0)//还没成功捕获
	{
		//溢出中断
		if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)//此时为溢出中断
		{
			if(TIM5CH1_CAPTURE_STA&0x40)//已经捕获到高电平
			{
				if((TIM5CH1_CAPTURE_STA&0x3f)==0x3f)//变量的0-5位全部为1 即定时器已经溢出了111111次，可知高电平太长了
				{
					TIM5CH1_CAPTURE_STA|=0x80;//标志成功捕获到了一次
					TIM5CH1_CAPTURE_VAL=0xffffffff;//捕获值强制设为最大
				}else{
					TIM5CH1_CAPTURE_STA++;
				}
			}
		}
		//捕获中断
		if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)//捕获1 捕获中断
		{
			if(TIM5CH1_CAPTURE_STA&0x40)//对变量的位6判断 看是否捕获到上升沿 若已经为1 则此次捕获到的是下降沿
			{
				TIM5CH1_CAPTURE_STA|=0x80;//将位7置1 标志成功捕获到了一次高电平脉宽
				
				TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);//获取当前捕获值
				
				//捕获通道极性设置函数
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);//设置为上升沿捕获,
			}else{//如果这次捕获到的是上升沿
				TIM5CH1_CAPTURE_STA=0;//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0x40;//标志捕获到了上升沿
				TIM_SetCounter(TIM5,0);//清空计时器
				TIM_Cmd(TIM5,DISABLE);//关闭定时器5
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);//设置为下降沿捕获
				TIM_Cmd(TIM5,ENABLE);//使能定时器5
			}
		}
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_CC1|TIM_IT_Update);//清除中断标志位
}


