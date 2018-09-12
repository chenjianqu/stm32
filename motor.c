#include "motor.h"

/*
使用定时器的PWM进行电机调速,有四个输出通道，即一个定时器可以输出四组不同占空比但频率相同的pwm
改变占空比则速度改变
PWM接正极调速，另一极低电平，则正转
PWM接负极调速，另一极高电平，则反转
*/

//经测试似乎是占空比越高 转速越慢



TIM_OCInitTypeDef motor_TIM_OCInitStructure;

/***************************************设置定时器*************************************/

//第二个参数为周期，9999为10ms,999为1ms,9为10us
void initMotorTIM(TIM_TypeDef* TIMx,u16 period)
{
	//配置TIMx,1MHz,溢出时间为1ms
	initTIMBase(TIMx,81,TIM_CounterMode_Up,period,TIM_CKD_DIV1,0);
}



/***************************************设置输出通道*************************************/

void initMotorChannel(TIM_TypeDef* TIMx,u8 ch,GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x,u16 motorInitSpeed)
{
		//配置GPIO
		initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_AF,getTIM_AF(TIMx),GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);

		//PWM模式设置 与通道设置TIMx_CHx,每个定时器只有一个频率，每个通道可以设置不同的占空比
		motor_TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//模式PWM1，无论定时器是向上计数还是向下计数，计数值>有效值时为有效电平。PWM2模式相反        
		motor_TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出比较使能
		motor_TIM_OCInitStructure.TIM_Pulse = motorInitSpeed;//设置比较值，写CCRx，当计数器到达100时,电平发送跳变,即设置CCRx的值
		motor_TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设输出极性为高，即有效值为高电平,当计数器<200时,PWM为高电平
		
	//选择pwm输出通道
		switch(ch)
		{
			case 1:
				TIM_OC1Init(TIMx, &motor_TIM_OCInitStructure);//通道2初始化
				TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);//使能输出比较预装载
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
		TIM_ARRPreloadConfig(TIMx, ENABLE);//使能自动重装载寄存器允许位
		TIM_Cmd(TIMx, ENABLE); 
}



/***************************************设置PWM占空比*************************************/

void setMotorSpeed(TIM_TypeDef* TIMx,u8 ch,u16 speed)
{
	//选择pwm通道
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



/***************************************开启电机*************************************/
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




/***************************************关闭电机*************************************/
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

