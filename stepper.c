#include "stepper.h"
#include "led.h"

//内部函数
void setStepperGPIO(u8 A,u8 B,u8 C,u8 D);

/*
使用 TIM7基本定时器 进行步进电机调速，步进电机速度越快，扭矩越小
拍数：完成一个磁场周期性变化所需脉冲数或导电状态用n表示，或指电机转过一个齿距角所需脉冲数，
以四相电机为例，有四相四拍运行方式即AB-BC-CD-DA-AB，四相八拍运行方式即 A-AB-B-BC-C-CD-D-DA-A.
*/

u8 stepper_beatMode;
u8 stepper_beat=0;

GPIO_TypeDef* stepper_GPIOx_A;
GPIO_TypeDef* stepper_GPIOx_B;
GPIO_TypeDef* stepper_GPIOx_C;
GPIO_TypeDef* stepper_GPIOx_D;
uint16_t stepper_GPIO_Pin_x_A;
uint16_t stepper_GPIO_Pin_x_B;
uint16_t stepper_GPIO_Pin_x_C;
uint16_t stepper_GPIO_Pin_x_D;

/*****初始化步进电机的GPIO和定时器*****/
void initStepper(u8 stepper_beats_x,GPIO_TypeDef* GPIOx_A,uint16_t GPIO_Pin_x_A,GPIO_TypeDef* GPIOx_B,uint16_t GPIO_Pin_x_B,GPIO_TypeDef* GPIOx_C,uint16_t GPIO_Pin_x_C,GPIO_TypeDef* GPIOx_D,uint16_t GPIO_Pin_x_D)
{
	stepper_beatMode=stepper_beats_x;
	
	stepper_GPIOx_A=GPIOx_A;
	stepper_GPIOx_B=GPIOx_B;
	stepper_GPIOx_C=GPIOx_C;
	stepper_GPIOx_D=GPIOx_D;
	stepper_GPIO_Pin_x_A=GPIO_Pin_x_A;
	stepper_GPIO_Pin_x_B=GPIO_Pin_x_B;
	stepper_GPIO_Pin_x_C=GPIO_Pin_x_C;
	stepper_GPIO_Pin_x_D=GPIO_Pin_x_D;
	
	//输出端脚推挽输出 上拉。上拉就输入模式时输入高电阻，下拉就是输入模式是输入低电阻
	initGPIO(GPIOx_A,GPIO_Pin_x_A,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_B,GPIO_Pin_x_B,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_C,GPIO_Pin_x_C,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_D,GPIO_Pin_x_D,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);

	//初始化定时器7
	//10kHz,0.1ms,  100ms
	initTIMBase(TIM7,8399,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	initNVIC(TIM7_IRQn,1,1);
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
}


/*
TIM_FLAG_Update计数器向上溢出/向下溢出，计数器初始化(通过软件或者内部/外部触发)  
TIM_FLAG_Trigger触发事件(计数器启动、停止、初始化或者由内部/外部触发计数) ,触发是防止你中途修改定时器计数值什么的
*/

/***************************************定时器7中断*************************************/
void TIM7_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM7,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'^');
		
		//步进电机以4拍的方式运行，AB-BC-CD-DA-AB
			if(stepper_beatMode==STEPPER_BEAT_4)
			{
				switch(stepper_beat)
				{
					case 0:
						setStepperGPIO(HIGH,HIGH,LOW,LOW);
						break;
					case 1:
						setStepperGPIO(LOW,HIGH,HIGH,LOW);
						break;
					case 2:
						setStepperGPIO(LOW,LOW,HIGH,HIGH);
						break;
					case 3:
						setStepperGPIO(HIGH,LOW,LOW,HIGH);
						break;
				}
				stepper_beat++;
				
				if(stepper_beat>3)
					stepper_beat=0;
			}
			
			//步进电机以8拍的方式运行，A-AB-B-BC-C-CD-D-DA-A
			else if(stepper_beatMode==STEPPER_BEAT_8)
			{
				switch(stepper_beat)
				{
					case 0:
						setStepperGPIO(HIGH,LOW,LOW,LOW);
						break;
					case 1:
						setStepperGPIO(HIGH,HIGH,LOW,LOW);
						break;
					case 2:
						setStepperGPIO(LOW,HIGH,LOW,LOW);
						break;
					case 3:
						setStepperGPIO(LOW,HIGH,HIGH,LOW);
						break;
					case 4:
						setStepperGPIO(LOW,LOW,HIGH,LOW);
						break;
					case 5:
						setStepperGPIO(LOW,LOW,HIGH,HIGH);
						break;
					case 6:
						setStepperGPIO(LOW,LOW,LOW,HIGH);
						break;
					case 7:
						setStepperGPIO(HIGH,LOW,LOW,HIGH);
						break;
				}
				stepper_beat++;
				if(stepper_beat>7)
					stepper_beat=0;
		}
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	}
}





/***************************设置步进电机每拍各个线的电平*************************/
void setStepperGPIO(u8 A,u8 B,u8 C,u8 D)
{
	if(A==HIGH)
		GPIO_SetBits(stepper_GPIOx_A,stepper_GPIO_Pin_x_A);
	else
		GPIO_ResetBits(stepper_GPIOx_A,stepper_GPIO_Pin_x_A);
	
	if(B==HIGH)
		GPIO_SetBits(stepper_GPIOx_B,stepper_GPIO_Pin_x_B);
	else
		GPIO_ResetBits(stepper_GPIOx_B,stepper_GPIO_Pin_x_B);
	
	if(C==HIGH)
		GPIO_SetBits(stepper_GPIOx_C,stepper_GPIO_Pin_x_C);
	else
		GPIO_ResetBits(stepper_GPIOx_C,stepper_GPIO_Pin_x_C);
	
	if(D==HIGH)
		GPIO_SetBits(stepper_GPIOx_D,stepper_GPIO_Pin_x_D);
	else
		GPIO_ResetBits(stepper_GPIOx_D,stepper_GPIO_Pin_x_D);
}


/***************************改变步进电机的转动速度*************************/
void setStepperSpeed(u16 period)
{
	TIM_Cmd(TIM7,DISABLE);
	//设置自动重装载值
	TIM_SetAutoreload(TIM7,period);
	
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_Cmd(TIM7,ENABLE);
}


/***************************启动步进电机*************************/
void startStepper(void)
{
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_Cmd(TIM7,ENABLE);
}

/***************************暂停步进电机*************************/
void stopStepper(void)
{
	TIM_Cmd(TIM7,DISABLE);
}

