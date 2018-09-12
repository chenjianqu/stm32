#include "tim.h"


/* 
�߼���ʱ����TIM1,TIM8�����������ƺн���ɲ����PWM�������
ͨ�ö�ʱ����TIM2-TIM5,��ʱ������PWM��������벶������Ƚ�
������ʱ����TIM6,TIM7,ͨ�ö�ʱ���򻯰棬�޲���Ƚ�ͨ������Ҫ��������DAC
ͨ�ö�ʱ����TIM9-TIM14 ͨ�ö�ʱ��
ÿ����ʱ����4������ͨ����6������������������
TIM1:����ģ��
TIM2������ģ��
TIM3���
TIM4
TIM5
TIM6
TIM7�������
TIM8
TIM9
TIM10
TIM11������
TIM12�����
TIM13��ʱ��
TIM14
*/

//��ʱ��9,10,11,1,8����APB2��
	/*ֻ��APB2��Ԥ��Ƶϵ��Ϊ1ʱ ��ʱ������ʱ��=APB2ʱ�ӣ�����ʱ������ʱ��=APB2*2
	��������APB2��Ԥ��Ƶϵ��Ϊ2����ʱ������ʱ��Ϊ168M/2*2=168M��
	APB1Ԥ��Ƶϵ��Ϊ4,����168/4*2=84MHz
	*/
	/*
	��ʱ��9-14û�е������ж��������жϺ�������Ҫ��TIM1��TIM8���ã�����
	TIM1_BRK_TIM9_IRQn  TIM1_UP_TIM10_IRQn  TIM1_TRG_COM_TIM11_IRQn 
	TIM8_BRK_TIM12_IRQn TIM8_UP_TIM13_IRQn  TIM8_TRG_COM_TIM14_IRQn 
	*/




/***************************************���ö�ʱ��*************************************/

void initTIMBase(TIM_TypeDef* TIMx,uint16_t TIM_Prescaler,uint16_t TIM_CounterMode_x,uint32_t TIM_Period,uint16_t TIM_ClockDivision,uint8_t TIM_RepetitionCounter)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//������ʱ��ʱ��
	enableTIMClock(TIMx);
	
	//���ö�ʱ��
	TIM_DeInit(TIMx); 
	
	//������������������ʱ��ʱ�����ڣ�����16λ�Ĵ�������ΧΪ1-65535��
	TIM_TimeBaseStructure.TIM_Period=TIM_Period;//���ö�ʱ������ֵ����ֵ��1
	TIM_TimeBaseStructure.TIM_Prescaler=TIM_Prescaler;//����Ԥ��Ƶϵ������ѡ���ʱ��Դ���з�Ƶ��ͨ����������ʱ��Ƶ�ʿ����ӳ���ʱʱ�䡣��ʱ������һ��ʱ������ͼ�һ����ֵ��1

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_ClockDivision;//����ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_x;//���ü���ģʽ���������»�����
	TIM_TimeBaseStructure.TIM_RepetitionCounter=TIM_RepetitionCounter;//�����ظ��������ظ�������ٴβ�����ж�
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseStructure);
}




/***************************************������ʱ��ʱ��*************************************/

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



/***************************************����ʱ����ת��Ϊ�ڼ���*************************************/
//����st���еĶ���
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






//��ʱ�����ò�������ʹ�ö�ʱ��3
void initTimer3(void){
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/*��Ҫ���� 1s �Ķ�ʱ���������Զ�װ��ֵPeriod=9999,��0��ʼ����9999����10000��
	����Ҫÿ��ʱ��Դ����Ϊ100us��������ʱ��Ϊ90MHz,���������Ԥ��Ƶϵ��Ϊ��9000-1��,��Ϊ ��Ƶֵ=Ԥ��Ƶϵ��+1
	1MHzΪ1����Hz��ÿ9000����ʱ������ֻ��1��������Ч��ÿs��9ǧ�������ʱ�����壬��ʱ��ʱ������Ϊ10000�Σ��պ�1s��
	���㹫ʽΪ�����ʱ��=����װ��ֵ*Ԥ��Ƶϵ����*����ʱ��Ƶ�� ��λs
	ֻ��APB1��Ԥ��Ƶϵ��Ϊ1ʱ ��ʱ������ʱ��=APB1ʱ�ӣ�����ʱ������ʱ��=APB1*2
	��������APB1��Ԥ��Ƶϵ��Ϊ4����ʱ������ʱ��Ϊ168M/4*2=82M��
	*/
	
	//��ʱ������,0.01M,10kHz,0.1ms,
	initTIMBase(TIM3,8199,TIM_CounterMode_Up,1000,TIM_CKD_DIV1,0);//0.01ms��
	
	//�ж�����
	initNVIC(TIM3_IRQn,2,0);
	
	//���жϱ�־������һ�����жϺ����������ж�
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	
	//������ʱ���ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//������ʱ��
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

/***************************************��ʱ��1�ж�*************************************/
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
/***************************************��ʱ��1_9�ж�*************************************/
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






/***************************************��ʱ��6�ж�*************************************/
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





/***************************************��ʱ��1_14�ж�*************************************/
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

/*********************************** PWM��� *********************************/

void initPWM(void)
{
	
		TIM_TypeDef* TIMx=TIM2;
	
	//PWM�ӿڿ��� ��ִ�г�ʼ��
		TIM_OCInitTypeDef TIM_OCInitStructure;
	
		//����TIM2ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		//����GPIO
		initGPIO(GPIOB,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_TIM2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);

		//����TIM2,1kHz
		initTIMBase(TIMx,83,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);

		//PWMģʽ���� ��ͨ������TIM2_CH2
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//ģʽPWM1�����۶�ʱ�������ϼ����������¼���������ֵ>��ЧֵʱΪ��Ч��ƽ��PWM2ģʽ�෴        
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//����Ƚ�ʹ��
		TIM_OCInitStructure.TIM_Pulse = 10;//���ñȽ�ֵ��дCCRx��������������500ʱ,��ƽ��������
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//���������Ϊ�ߣ�����ЧֵΪ�ߵ�ƽ,��������<200ʱ,PWMΪ�ߵ�ƽ
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);//ͨ��2��ʼ��
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);//ʹ������Ƚ�Ԥװ��
		TIM_ARRPreloadConfig(TIMx, ENABLE);//ʹ���Զ���װ�ؼĴ�������λ
		//����TIM2
		TIM_Cmd(TIMx, ENABLE); 
		//PWMռ�ձ�����,����ƽ���������ֵ
}






/*********************************** ���벶�� *********************************/

/*��غ�����
��ȡͨ������ֵuint32_t TIM_GetCapture1(TIM_TypeDef* TIMx);
ͨ���������ö�������void TIM_OCxPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)��
*/

TIM_ICInitTypeDef TIM5_ICInitStructure;
void initInputCapture(u32 arr,u16 psc)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	//����GPIO
	initGPIO(GPIOA,GPIO_Pin_0,GPIO_Mode_AF,GPIO_AF_TIM5,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	initTIMBase(TIM5,psc,TIM_CounterMode_Up,arr,TIM_CKD_DIV1,0);
		
	TIM5_ICInitStructure.TIM_Channel=TIM_Channel_1;//ѡ������ģ�IC1ӳ�䵽TI1
	
	TIM5_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//�����ز���
	
	TIM5_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//ӳ�䵽TI1��,���źſ���ӳ�䵽���ͨ����
	
	TIM5_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//���������Ƶ������Ƶ����Ƶ�������ã�����2��Ƶ���ǲ������������زɼ�һ��
	
	TIM5_ICInitStructure.TIM_ICFilter=0x00;//���������˲��������˲����˲��ĸ�����統��ʱ����⵽������ʱ����������õĴ���������⼸���Ƿ�Ϊ�ߵ�ƽ��
	//���ɴ�֮������ȻΪ�ߵ�ƽ������жϡ���˸ߵ�ƽʱ�䲻���������彫�����ԣ������ﵽ�˲������ã�
	TIM_ICInit(TIM5,&TIM5_ICInitStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������жϣ�������ʱ�����ʱ��������CC1�����ж�
	
	TIM_Cmd(TIM5,ENABLE);//ʹ�ܶ�ʱ��5
	
	initNVIC(TIM5_IRQn,2,0);
}


/*����״̬����������������Ǽ�¼��ʱ������Ĵ��������ǵ���Щ�����ȿ��ܹ��������¶�ʱ���������ʱ�����¼��ʱ������Ĵ������ܼ���׼ȷ��������
λ[7],����һ����ɵı�־������ʱ�����������¼�ʱ�������жϣ�
λ[6],����ߵ�ƽ�ı�־����λ6Ϊ1�Ҳ����½���ʱ ��Ϊ����˲���
[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)*/
u8 TIM5CH1_CAPTURE_STA=0;//���벶��״̬
u8 tim_capture_flag=0;

u32 TIM5CH1_CAPTURE_VAL;//���벶��ֵ
void TIM5_IRQHandler(void)
{
	if((TIM5CH1_CAPTURE_STA&0x80)==0)//��û�ɹ�����
	{
		//����ж�
		if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)//��ʱΪ����ж�
		{
			if(TIM5CH1_CAPTURE_STA&0x40)//�Ѿ����񵽸ߵ�ƽ
			{
				if((TIM5CH1_CAPTURE_STA&0x3f)==0x3f)//������0-5λȫ��Ϊ1 ����ʱ���Ѿ������111111�Σ���֪�ߵ�ƽ̫����
				{
					TIM5CH1_CAPTURE_STA|=0x80;//��־�ɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0xffffffff;//����ֵǿ����Ϊ���
				}else{
					TIM5CH1_CAPTURE_STA++;
				}
			}
		}
		//�����ж�
		if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)//����1 �����ж�
		{
			if(TIM5CH1_CAPTURE_STA&0x40)//�Ա�����λ6�ж� ���Ƿ񲶻������� ���Ѿ�Ϊ1 ��˴β��񵽵����½���
			{
				TIM5CH1_CAPTURE_STA|=0x80;//��λ7��1 ��־�ɹ�������һ�θߵ�ƽ����
				
				TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);//��ȡ��ǰ����ֵ
				
				//����ͨ���������ú���
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);//����Ϊ�����ز���,
			}else{//�����β��񵽵���������
				TIM5CH1_CAPTURE_STA=0;//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0x40;//��־������������
				TIM_SetCounter(TIM5,0);//��ռ�ʱ��
				TIM_Cmd(TIM5,DISABLE);//�رն�ʱ��5
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);//����Ϊ�½��ز���
				TIM_Cmd(TIM5,ENABLE);//ʹ�ܶ�ʱ��5
			}
		}
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_CC1|TIM_IT_Update);//����жϱ�־λ
}


