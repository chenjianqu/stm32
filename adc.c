#include "adc.h"
#include "led.h"
#include "delay.h"
#include "gpio.h"

/*将外部输入的模拟电压量转换为数字量*/

/*
ADC通道极其对应引脚：
通道号		ADC1	ADC2	ADC3
通道0		PA0		PA0		PA0
通道1		PA1		PA1		PA1
通道2		PA2		PA2		PA2
通道3		PA3		PA3		PA3
通道4		PA4		PA4		PF6
通道5		PA5		PA5		PF7
通道6		PA6		PA6		PF8
通道7		PA7		PA7		PF9
通道8		PB0		PB0		PF10
通道9		PB1		PB1		PF3
通道10	PC0		PC0		PC0
通道11	PC1		PC1		PC1
通道12	PC2		PC2		PC2
通道13	PC13	PC13	PC13
通道14	PC4		PC4		PF4
通道15	PC5		PC5		PF5
*/

/*
采样时间是ADC记录输入电压模拟量的时间，该时间内要求要求输入电压足够稳定，不会因电压波动造成记录数值不准。
STM32的ADC采样时间计算公式为：
  T=采样周期+12.5个周期
*/

/**************************************ADC初始化********************************************/

void initADC(ADC_TypeDef* ADCx)
{
	//开启ADC时钟
	if(ADCx==ADC1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	else if(ADCx==ADC2)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	else if(ADCx==ADC3)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	
	ADC_DeInit();//ADC复位
	
	//ADC配置
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonInitStruct.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;//DMA失能
	ADC_CommonInitStruct.ADC_Mode=ADC_Mode_Independent;//使用独立模式
	ADC_CommonInitStruct.ADC_Prescaler=ADC_Prescaler_Div4;//设置预分频系数为4，APB2/4=此时频率为21MHz,必须保证ADC外设时钟在32M以内。
	ADC_CommonInitStruct.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//设置两次采样时间为5个时钟周期
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;//关闭连续转换
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//右对齐
	ADC_InitStruct.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
	ADC_InitStruct.ADC_NbrOfConversion=1;//1个转换在规则序列中，也就是之转换规则
	ADC_InitStruct.ADC_Resolution=ADC_Resolution_12b;//12位模式
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;//非扫描模式
	ADC_Init(ADCx,&ADC_InitStruct);
	
	/*********************ADC看门狗配置***************************/
	/*
	
	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable); 
	ADC_AnalogWatchdogThresholdsConfig(ADC1,4000,0);  //阈值设置。高，低
	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);
	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);//ADC看门狗中断
	
	initNVIC(ADC_IRQn,1,1);
	*/
	

	ADC_Cmd(ADCx,ENABLE);
}


/****************************************初始化ADC通道******************************************/

void initADC_GPIO(GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x)
{
	//配置GPIO,不带上拉和下拉
	initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_AN,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);
}


//ADC中断
void ADC_IRQHandler(void)
{
	//ADC看门狗中断
	if(ADC_GetITStatus(ADC1,ADC_IT_AWD))
	{
		
		
		USART_SendChar(USART1,'!');
		
		ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
	}
	//转换完成中断
  else if(ADC_GetITStatus(ADC1,ADC_IT_EOC))
  {
		//mydata[ma_i]=ADC1->DR;
    //ma_i++;
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
		ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 1, ADC_SampleTime_3Cycles );
		ADC_SoftwareStartConv(ADC1);
  }
}




/****************************************获取某个通道的ADC值******************************************/

u16 getADC(ADC_TypeDef* ADCx,uint8_t ADC_Channel_x,uint8_t ADC_SampleTime_xCycles)
{
	/*
	规则组参数配置说明：
	第四个参数设置采样时钟周期，STM32F4的ADC采样时间计算公式为：T=采样周期+12个周期提高采样周期可以提高精确度
		当前adc时钟是21MHz,设3个采样周期,即此时的采样时间=（3+12）/12=1.25us
	
	第三个参数是Rank:这个在你多通道采集和DMA应用时会有很大的作用，比如你需要多通道采集，你设置每个通道的采集顺序其实就是用这个变量来做的，
									假设你定义channle1  的rank=1，channle2 的rank=2，那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channle1的转换结果，AdcDMA[1]就是通道2的转换结果。
	*/
	ADC_RegularChannelConfig(ADCx,ADC_Channel_x, 1, ADC_SampleTime_xCycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度	
	ADC_SoftwareStartConv(ADCx);//使能指定的ADC1的软件转换启动功能
	
	while(!ADC_GetFlagStatus(ADCx,ADC_FLAG_EOC));//等待转换结束
	
	return ADC_GetConversionValue(ADCx);//返回最近一次的ADCx规则组的转换结果
}


/****************************************获取某个通道的平均采样ADC值******************************************/

u16 getADCAverage(ADC_TypeDef* ADCx,uint8_t ADC_Channel_x,uint8_t ADC_SampleTime_xCycles,u8 times)
{
	u8 i;
	u32 sum=0;
	for(i=0;i<times;i++){
		sum+=getADC(ADCx,ADC_Channel_x,ADC_SampleTime_xCycles);
		delay_ms(5);
	}
	return sum/times;
}





/****************************************初始化内部温度传感器通道******************************************/
/*
STM32F40x有一个内部的温度传感器，可以用来测量CPU及周围的温度(TA)。
该温度传感器在内部和ADCx_IN16（F40xx/F41xx)或者ADCx_IN18(F42xx/F43xx)输入通道相连接，此通道把传感器输出的电压转换成数字值。
温度传感器模拟输入推荐采样时间是17.1μs。
STM32的内部温度传感器支持的温度范围为：-40~125度。精度比较差，为±1.5℃左右。
*/
void initADC_InterTemperature(void)
{
	//使能内部温度传感器，操作ADC_CCR的TSVREFE位
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_480Cycles);//内部温度传感器所在的通道
}

/****************************************获取温度传感器的值******************************************/

short getTemperate(void)
{
	u32 adcx;
	short result;
	double tempetate;
	
	//内部温度传感器F40x和F41x挂在在通道16，F42x和F43x挂载在通道18.均是ADC1
	adcx=getADCAverage(ADC1,ADC_Channel_16,ADC_SampleTime_480Cycles,10);//读取通道16内部温度传感器通道，10次取平均
	
	tempetate=(float)adcx*(3.3/4096);//将得到的数字转换为电压值，得到的数字最大为4096,且最大电压为3.3V
	/*T（℃）={（V25-Vsense）/Avg_Slope}+25   V25=Vsense在25度时的数值（典型值为：0.76） Avg_Slope=温度与Vsense曲线的平均斜率（单位为mv/℃或uv/℃）（典型值为2.5Mv/℃）*/
	
	tempetate=(tempetate-0.76)/0.0025+25;//转换为温度值
	result=tempetate*=100;
	return result;
}//返回扩大了一百倍的温度值



