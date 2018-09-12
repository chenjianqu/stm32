#include "servo.h"

/*
�����ö�ʱ�����50Hz�����壬ÿ������ĸ����ͨ���ɿ����ĸ����

TIM���ͨ�������ŵĹ�ϵ ����

ͨ���ı䶨ʱ��PWM��ռ�ձȸı����ĽǶ�

����Ŀ�����Ҫ20msʱ�����壬������ĸߵ�ƽΪ0.5ms-2.5ms��Χ�ڵĽǶȿ������岿��
180�ȽǶ��ŷ��Ŀ��ƹ�ϵ�������ģ�
  0.5ms--------------0�ȣ�
  1.0ms------------45�ȣ�
  1.5ms------------90�ȣ�
  2.0ms-----------135�ȣ�
  2.5ms-----------180�ȣ�
*/

/*
����ʵ����
initServoTIM(TIM2);
	initServoChannel(TIM2,1,GPIOA,GPIO_Pin_5,90);
	initServoChannel(TIM2,2,GPIOA,GPIO_Pin_1,90);
	initServoChannel(TIM2,3,GPIOA,GPIO_Pin_2,90);
	initServoChannel(TIM2,4,GPIOA,GPIO_Pin_3,90);
	
	initServoTIM(TIM3);
	initServoChannel(TIM3,1,GPIOB,GPIO_Pin_4,90);
	initServoChannel(TIM3,2,GPIOB,GPIO_Pin_5,90);
	initServoChannel(TIM3,3,GPIOB,GPIO_Pin_0,90);
	initServoChannel(TIM3,4,GPIOB,GPIO_Pin_1,90);
	
	setServoAngle(TIM2,3,10);
	
	startServo(TIM2,3,90);
	stopServo(TIM2,3);
*/



TIM_OCInitTypeDef servo_TIM_OCInitStructure;

/*******************************************��ʱ����ʼ��*******************************************/
void initServoTIM(TIM_TypeDef* TIMx)
{
		initTIMBase(TIMx,83,TIM_CounterMode_Up,19999,TIM_CKD_DIV1,0);//20ms 50Hz
}



/*******************************************���ͨ����ʼ��*******************************************/
void initServoChannel(TIM_TypeDef* TIMx,u8 ch,GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x,u16 servoInitAngle)
{
		//����GPIO
		initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_AF,getTIM_AF(TIMx),GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
		//PWMģʽ���� ��ͨ������TIMx_CHx,ÿ����ʱ��ֻ��һ��Ƶ�ʣ�ÿ��ͨ���������ò�ͬ��ռ�ձ�
		servo_TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//ģʽPWM1�����۶�ʱ�������ϼ����������¼���������ֵ>��ЧֵʱΪ��Ч��ƽ��PWM2ģʽ�෴        
		servo_TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//����Ƚ�ʹ��
		servo_TIM_OCInitStructure.TIM_Pulse = servoInitAngle*2000/180+500;//���ñȽ�ֵ��дCCRx��������������100ʱ,��ƽ��������,������CCRx��ֵ
		servo_TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//���������Ϊ�ߣ�����ЧֵΪ�ߵ�ƽ,��������<200ʱ,PWMΪ�ߵ�ƽ
		
		switch(ch)//ѡ��pwmͨ��
		{
			case 1:
				TIM_OC1Init(TIMx, &servo_TIM_OCInitStructure);//ͨ��1��ʼ��
				TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);//ʹ������Ƚ�Ԥװ��
				break;
			case 2:
				TIM_OC2Init(TIMx, &servo_TIM_OCInitStructure);
				TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
				break;
			case 3:
				TIM_OC3Init(TIMx, &servo_TIM_OCInitStructure);
				TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
				break;
			case 4:
				TIM_OC4Init(TIMx, &servo_TIM_OCInitStructure);
				TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
				break;
		}
		TIM_ARRPreloadConfig(TIMx, ENABLE);//ʹ���Զ���װ�ؼĴ�������λ
		TIM_Cmd(TIMx, ENABLE); 
}



/*******************************************���ö���Ƕ�*******************************************/
void setServoAngle(TIM_TypeDef* TIMx,u8 ch,u16 angle,u16 delaytime)
{
	u16 servo_temp;
	if(angle>180)
		angle=180;
	
	servo_temp=angle*2000/180+500;//����ռ�ձ�
	
	switch(ch)//ѡ��pwmͨ��
	{
		case 1:
			TIM_SetCompare1(TIMx,servo_temp);
			break;
		case 2:
			TIM_SetCompare2(TIMx,servo_temp);
			break;
		case 3:
			TIM_SetCompare3(TIMx,servo_temp);
			break;
		case 4:
			TIM_SetCompare4(TIMx,servo_temp);
			break;
	}
	delay_ms(delaytime);
}

/***************************************�������*************************************/
void startServo(TIM_TypeDef* TIMx,u8 ch,u16 servoInitAngle)
{
	servo_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	servo_TIM_OCInitStructure.TIM_Pulse = servoInitAngle*2000/180+500;
	switch(ch)
		{
			case 1:
				TIM_OC1Init(TIMx, &servo_TIM_OCInitStructure);
				break;
			case 2:
				TIM_OC2Init(TIMx, &servo_TIM_OCInitStructure);
				break;
			case 3:
				TIM_OC3Init(TIMx, &servo_TIM_OCInitStructure);
				break;
			case 4:
				TIM_OC4Init(TIMx, &servo_TIM_OCInitStructure);
				break;
		}
}



/***************************************�رն��*************************************/
void stopServo(TIM_TypeDef* TIMx,u8 ch)
{
	servo_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;
	switch(ch)
		{
			case 1:
				TIM_OC1Init(TIMx, &servo_TIM_OCInitStructure);
				break;
			case 2:
				TIM_OC2Init(TIMx, &servo_TIM_OCInitStructure);
				break;
			case 3:
				TIM_OC3Init(TIMx, &servo_TIM_OCInitStructure);
				break;
			case 4:
				TIM_OC4Init(TIMx, &servo_TIM_OCInitStructure);
				break;
		}
}

