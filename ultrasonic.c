#include "ultrasonic.h"

/*
������ģ�� TRIG�������� ECHO�ź���������
*/

/*
��һ��10ms�ĸ�������ܴ���TrigPin����ģ�鱻�����󣬳���������ͷ������8��40kHz���ڵ�ƽ��ͬʱ���ز���һ����⵽�лز��ź�����������źš�
�����ź���һ������Ŀ�ȳ����ȵľ�����󡣿�ͨ�������źŵ��յ��Ļ����ź�ʱ�������Լ���õ����롣��ʽ: uS/58=���ף�����uS/148=Ӣ�硣
*/


#define ULTRA_ECHO_GPIOX 		GPIOB
#define ULTRA_ECHO_GPIO_PIN GPIO_Pin_11
#define ULTRA_TRIG_GPIOX 		GPIOE
#define ULTRA_TRIG_GPIO_PIN GPIO_Pin_13

void initUltrasonic(void)
{
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TypeDef* TIMx=TIM2;
	
	//ECHO���� GPIOB4  ���ù���,����
	initGPIO(ULTRA_ECHO_GPIOX,ULTRA_ECHO_GPIO_PIN,GPIO_Mode_AF,	GPIO_AF_TIM2,	GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	//TRIG���� GPIOB4
	initGPIO(ULTRA_TRIG_GPIOX,ULTRA_TRIG_GPIO_PIN,GPIO_Mode_OUT,AF_DISABLE,		GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);

	//��ʼ��TIMBase
	initTIMBase(TIMx,167,TIM_CounterMode_Up,9999,TIM_CKD_DIV1,0);

	TIM_ICInitStructure.TIM_Channel=TIM_Channel_4;//ѡ������ģ�IC1ӳ�䵽TI1
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//�����ز���
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//ӳ�䵽TI1��,���źſ���ӳ�䵽���ͨ����
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//���������Ƶ������Ƶ����Ƶ�������ã�����2��Ƶ���ǲ������������زɼ�һ��
	TIM_ICInitStructure.TIM_ICFilter=0x00;//���������˲��������˲����˲��ĸ�����統��ʱ����⵽������ʱ����������õĴ���������⼸���Ƿ�Ϊ�ߵ�ƽ��
	//���ɴ�֮������ȻΪ�ߵ�ƽ������жϡ���˸ߵ�ƽʱ�䲻���������彫�����ԣ������ﵽ�˲������ã�
	TIM_ICInit(TIMx,&TIM_ICInitStructure);

	TIM_ITConfig(TIMx,TIM_IT_Update|TIM_IT_CC4,ENABLE);
	TIM_Cmd(TIMx,ENABLE);

	initNVIC(TIM2_IRQn,2,0);
}	

u8 ultra_upflag=0;//�Ƿ���յ�������
u16 ultra_pulseWidth=0;//������
u16 ultra_temp=0;


/***************************************��ʱ��3�ж�*************************************/

void TIM2_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM2,TIM_IT_CC4))
	{
		USART_SendData(USART1,'*');
		if(GPIO_ReadInputDataBit(ULTRA_ECHO_GPIOX,ULTRA_ECHO_GPIO_PIN))//�����ز���
		{
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			TIM_SetCounter(TIM2,0);//��ն�ʱ��
			ultra_upflag=1;	//����������Ѿ�������	
		}
		else //�½��ز���
		{
			ultra_temp=TIM_GetCapture4(TIM2);//��ȡ��������ֵ������ȡ������
			TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Rising); //����Ϊ�����ز���
			
			if(ultra_upflag)//�����һ�θߵ�ƽ���� 
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

