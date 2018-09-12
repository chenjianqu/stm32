#include "motor.h"

/*
ʹ�ö�ʱ����PWM���е������,���ĸ����ͨ������һ����ʱ������������鲻ͬռ�ձȵ�Ƶ����ͬ��pwm
�ı�ռ�ձ����ٶȸı�
PWM���������٣���һ���͵�ƽ������ת
PWM�Ӹ������٣���һ���ߵ�ƽ����ת
*/

//�������ƺ���ռ�ձ�Խ�� ת��Խ��



TIM_OCInitTypeDef motor_TIM_OCInitStructure;

/***************************************���ö�ʱ��*************************************/

//�ڶ�������Ϊ���ڣ�9999Ϊ10ms,999Ϊ1ms,9Ϊ10us
void initMotorTIM(TIM_TypeDef* TIMx,u16 period)
{
	//����TIMx,1MHz,���ʱ��Ϊ1ms
	initTIMBase(TIMx,81,TIM_CounterMode_Up,period,TIM_CKD_DIV1,0);
}



/***************************************�������ͨ��*************************************/

void initMotorChannel(TIM_TypeDef* TIMx,u8 ch,GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x,u16 motorInitSpeed)
{
		//����GPIO
		initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_AF,getTIM_AF(TIMx),GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);

		//PWMģʽ���� ��ͨ������TIMx_CHx,ÿ����ʱ��ֻ��һ��Ƶ�ʣ�ÿ��ͨ���������ò�ͬ��ռ�ձ�
		motor_TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//ģʽPWM1�����۶�ʱ�������ϼ����������¼���������ֵ>��ЧֵʱΪ��Ч��ƽ��PWM2ģʽ�෴        
		motor_TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//����Ƚ�ʹ��
		motor_TIM_OCInitStructure.TIM_Pulse = motorInitSpeed;//���ñȽ�ֵ��дCCRx��������������100ʱ,��ƽ��������,������CCRx��ֵ
		motor_TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//���������Ϊ�ߣ�����ЧֵΪ�ߵ�ƽ,��������<200ʱ,PWMΪ�ߵ�ƽ
		
	//ѡ��pwm���ͨ��
		switch(ch)
		{
			case 1:
				TIM_OC1Init(TIMx, &motor_TIM_OCInitStructure);//ͨ��2��ʼ��
				TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);//ʹ������Ƚ�Ԥװ��
				break;
			case 2:
				TIM_OC2Init(TIMx, &motor_TIM_OCInitStructure);
				TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
				break;
			case 3:
				TIM_OC3Init(TIMx, &motor_TIM_OCInitStructure);
				TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
				break;
			case 4:
				TIM_OC4Init(TIMx, &motor_TIM_OCInitStructure);
				TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
				break;
		}
		TIM_ARRPreloadConfig(TIMx, ENABLE);//ʹ���Զ���װ�ؼĴ�������λ
		TIM_Cmd(TIMx, ENABLE); 
}



/***************************************����PWMռ�ձ�*************************************/

void setMotorSpeed(TIM_TypeDef* TIMx,u8 ch,u16 speed)
{
	//ѡ��pwmͨ��
	switch(ch)
	{
		case 1:
			TIM_SetCompare1(TIMx,speed);
			break;
		case 2:
			TIM_SetCompare2(TIMx,speed);
			break;
		case 3:
			TIM_SetCompare3(TIMx,speed);
			break;
		case 4:
			TIM_SetCompare4(TIMx,speed);
			break;
	}
}



/***************************************�������*************************************/
void startMotor(TIM_TypeDef* TIMx,u8 ch)
{
	motor_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	switch(ch)
		{
			case 1:
				TIM_OC1Init(TIMx, &motor_TIM_OCInitStructure);
				break;
			case 2:
				TIM_OC2Init(TIMx, &motor_TIM_OCInitStructure);
				break;
			case 3:
				TIM_OC3Init(TIMx, &motor_TIM_OCInitStructure);
				break;
			case 4:
				TIM_OC4Init(TIMx, &motor_TIM_OCInitStructure);
				break;
		}
}




/***************************************�رյ��*************************************/
void stopMotor(TIM_TypeDef* TIMx,u8 ch)
{
	motor_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;
	switch(ch)
		{
			case 1:
				TIM_OC1Init(TIMx, &motor_TIM_OCInitStructure);
				break;
			case 2:
				TIM_OC2Init(TIMx, &motor_TIM_OCInitStructure);
				break;
			case 3:
				TIM_OC3Init(TIMx, &motor_TIM_OCInitStructure);
				break;
			case 4:
				TIM_OC4Init(TIMx, &motor_TIM_OCInitStructure);
				break;
		}
}

