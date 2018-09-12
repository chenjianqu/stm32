#include "adc.h"
#include "led.h"
#include "delay.h"
#include "gpio.h"

/*���ⲿ�����ģ���ѹ��ת��Ϊ������*/

/*
ADCͨ�������Ӧ���ţ�
ͨ����		ADC1	ADC2	ADC3
ͨ��0		PA0		PA0		PA0
ͨ��1		PA1		PA1		PA1
ͨ��2		PA2		PA2		PA2
ͨ��3		PA3		PA3		PA3
ͨ��4		PA4		PA4		PF6
ͨ��5		PA5		PA5		PF7
ͨ��6		PA6		PA6		PF8
ͨ��7		PA7		PA7		PF9
ͨ��8		PB0		PB0		PF10
ͨ��9		PB1		PB1		PF3
ͨ��10	PC0		PC0		PC0
ͨ��11	PC1		PC1		PC1
ͨ��12	PC2		PC2		PC2
ͨ��13	PC13	PC13	PC13
ͨ��14	PC4		PC4		PF4
ͨ��15	PC5		PC5		PF5
*/

/*
����ʱ����ADC��¼�����ѹģ������ʱ�䣬��ʱ����Ҫ��Ҫ�������ѹ�㹻�ȶ����������ѹ������ɼ�¼��ֵ��׼��
STM32��ADC����ʱ����㹫ʽΪ��
  T=��������+12.5������
*/

/**************************************ADC��ʼ��********************************************/

void initADC(ADC_TypeDef* ADCx)
{
	//����ADCʱ��
	if(ADCx==ADC1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	else if(ADCx==ADC2)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	else if(ADCx==ADC3)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	
	ADC_DeInit();//ADC��λ
	
	//ADC����
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonInitStruct.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;//DMAʧ��
	ADC_CommonInitStruct.ADC_Mode=ADC_Mode_Independent;//ʹ�ö���ģʽ
	ADC_CommonInitStruct.ADC_Prescaler=ADC_Prescaler_Div4;//����Ԥ��Ƶϵ��Ϊ4��APB2/4=��ʱƵ��Ϊ21MHz,���뱣֤ADC����ʱ����32M���ڡ�
	ADC_CommonInitStruct.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//�������β���ʱ��Ϊ5��ʱ������
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;//�ر�����ת��
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStruct.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
	ADC_InitStruct.ADC_NbrOfConversion=1;//1��ת���ڹ��������У�Ҳ����֮ת������
	ADC_InitStruct.ADC_Resolution=ADC_Resolution_12b;//12λģʽ
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;//��ɨ��ģʽ
	ADC_Init(ADCx,&ADC_InitStruct);
	
	/*********************ADC���Ź�����***************************/
	/*
	
	ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable); 
	ADC_AnalogWatchdogThresholdsConfig(ADC1,4000,0);  //��ֵ���á��ߣ���
	ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_1);
	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);//ADC���Ź��ж�
	
	initNVIC(ADC_IRQn,1,1);
	*/
	

	ADC_Cmd(ADCx,ENABLE);
}


/****************************************��ʼ��ADCͨ��******************************************/

void initADC_GPIO(GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x)
{
	//����GPIO,��������������
	initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_AN,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);
}


//ADC�ж�
void ADC_IRQHandler(void)
{
	//ADC���Ź��ж�
	if(ADC_GetITStatus(ADC1,ADC_IT_AWD))
	{
		
		
		USART_SendChar(USART1,'!');
		
		ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
	}
	//ת������ж�
  else if(ADC_GetITStatus(ADC1,ADC_IT_EOC))
  {
		//mydata[ma_i]=ADC1->DR;
    //ma_i++;
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
		ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 1, ADC_SampleTime_3Cycles );
		ADC_SoftwareStartConv(ADC1);
  }
}




/****************************************��ȡĳ��ͨ����ADCֵ******************************************/

u16 getADC(ADC_TypeDef* ADCx,uint8_t ADC_Channel_x,uint8_t ADC_SampleTime_xCycles)
{
	/*
	�������������˵����
	���ĸ��������ò���ʱ�����ڣ�STM32F4��ADC����ʱ����㹫ʽΪ��T=��������+12��������߲������ڿ�����߾�ȷ��
		��ǰadcʱ����21MHz,��3����������,����ʱ�Ĳ���ʱ��=��3+12��/12=1.25us
	
	������������Rank:��������ͨ���ɼ���DMAӦ��ʱ���кܴ�����ã���������Ҫ��ͨ���ɼ���������ÿ��ͨ���Ĳɼ�˳����ʵ������������������ģ�
									�����㶨��channle1  ��rank=1��channle2 ��rank=2����ô��Ӧ����DMA����ռ�ı�������AdcDMA[0] ��i��channle1��ת�������AdcDMA[1]����ͨ��2��ת�������
	*/
	ADC_RegularChannelConfig(ADCx,ADC_Channel_x, 1, ADC_SampleTime_xCycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��	
	ADC_SoftwareStartConv(ADCx);//ʹ��ָ����ADC1�����ת����������
	
	while(!ADC_GetFlagStatus(ADCx,ADC_FLAG_EOC));//�ȴ�ת������
	
	return ADC_GetConversionValue(ADCx);//�������һ�ε�ADCx�������ת�����
}


/****************************************��ȡĳ��ͨ����ƽ������ADCֵ******************************************/

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





/****************************************��ʼ���ڲ��¶ȴ�����ͨ��******************************************/
/*
STM32F40x��һ���ڲ����¶ȴ�������������������CPU����Χ���¶�(TA)��
���¶ȴ��������ڲ���ADCx_IN16��F40xx/F41xx)����ADCx_IN18(F42xx/F43xx)����ͨ�������ӣ���ͨ���Ѵ���������ĵ�ѹת��������ֵ��
�¶ȴ�����ģ�������Ƽ�����ʱ����17.1��s��
STM32���ڲ��¶ȴ�����֧�ֵ��¶ȷ�ΧΪ��-40~125�ȡ����ȱȽϲΪ��1.5�����ҡ�
*/
void initADC_InterTemperature(void)
{
	//ʹ���ڲ��¶ȴ�����������ADC_CCR��TSVREFEλ
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_480Cycles);//�ڲ��¶ȴ��������ڵ�ͨ��
}

/****************************************��ȡ�¶ȴ�������ֵ******************************************/

short getTemperate(void)
{
	u32 adcx;
	short result;
	double tempetate;
	
	//�ڲ��¶ȴ�����F40x��F41x������ͨ��16��F42x��F43x������ͨ��18.����ADC1
	adcx=getADCAverage(ADC1,ADC_Channel_16,ADC_SampleTime_480Cycles,10);//��ȡͨ��16�ڲ��¶ȴ�����ͨ����10��ȡƽ��
	
	tempetate=(float)adcx*(3.3/4096);//���õ�������ת��Ϊ��ѹֵ���õ����������Ϊ4096,������ѹΪ3.3V
	/*T���棩={��V25-Vsense��/Avg_Slope}+25   V25=Vsense��25��ʱ����ֵ������ֵΪ��0.76�� Avg_Slope=�¶���Vsense���ߵ�ƽ��б�ʣ���λΪmv/���uv/�棩������ֵΪ2.5Mv/�棩*/
	
	tempetate=(tempetate-0.76)/0.0025+25;//ת��Ϊ�¶�ֵ
	result=tempetate*=100;
	return result;
}//����������һ�ٱ����¶�ֵ



