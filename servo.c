#include "servo.h"

/*
可设置定时器输出50Hz的脉冲，每个舵机四个输出通道可控制四个舵机

TIM输出通道与引脚的关系 请查表

通过改变定时器PWM的占空比改变舵机的角度

舵机的控制需要20ms时基脉冲，该脉冲的高电平为0.5ms-2.5ms范围内的角度控制脉冲部分
180度角度伺服的控制关系是这样的：
  0.5ms--------------0度；
  1.0ms------------45度；
  1.5ms------------90度；
  2.0ms-----------135度；
  2.5ms-----------180度；
*/

/*
调用实例：
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

/*******************************************定时器初始化*******************************************/
void initServoTIM(TIM_TypeDef* TIMx)
{
		initTIMBase(TIMx,83,TIM_CounterMode_Up,19999,TIM_CKD_DIV1,0);//20ms 50Hz
}



/*******************************************舵机通道初始化*******************************************/
void initServoChannel(TIM_TypeDef* TIMx,u8 ch,GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x,u16 servoInitAngle)
{
		//配置GPIO
		initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_AF,getTIM_AF(TIMx),GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
		//PWM模式设置 与通道设置TIMx_CHx,每个定时器只有一个频率，每个通道可以设置不同的占空比
		servo_TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//模式PWM1，无论定时器是向上计数还是向下计数，计数值>有效值时为有效电平。PWM2模式相反        
		servo_TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出比较使能
		servo_TIM_OCInitStructure.TIM_Pulse = servoInitAngle*2000/180+500;//设置比较值，写CCRx，当计数器到达100时,电平发送跳变,即设置CCRx的值
		servo_TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设输出极性为高，即有效值为高电平,当计数器<200时,PWM为高电平
		
		switch(ch)//选择pwm通道
		{
			case 1:
				TIM_OC1Init(TIMx, &servo_TIM_OCInitStructure);//通道1初始化
				TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);//使能输出比较预装载
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
		TIM_ARRPreloadConfig(TIMx, ENABLE);//使能自动重装载寄存器允许位
		TIM_Cmd(TIMx, ENABLE); 
}



/*******************************************设置舵机角度*******************************************/
void setServoAngle(TIM_TypeDef* TIMx,u8 ch,u16 angle,u16 delaytime)
{
	u16 servo_temp;
	if(angle>180)
		angle=180;
	
	servo_temp=angle*2000/180+500;//计算占空比
	
	switch(ch)//选择pwm通道
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

/***************************************开启舵机*************************************/
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



/***************************************关闭舵机*************************************/
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

