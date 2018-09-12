#include "led.h"


/*左边LED是PC0 右边LED是PD3 低电平亮*/

//初始化LED
void initLED(void){
	initGPIO(GPIOC,GPIO_Pin_0,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_9,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_10,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_3,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
}

u8 led_i=0;
void changeLED(u8 n)
{
	led_i=!led_i;
	if(n==0){
		if(led_i)
			GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		else
			GPIO_SetBits(GPIOF,GPIO_Pin_9);
	}
	else if(n==1){
		if(led_i)
			GPIO_ResetBits(GPIOF,GPIO_Pin_10);
		else
			GPIO_SetBits(GPIOF,GPIO_Pin_10);
	}
	else if(n==2){
		if(led_i)
			GPIO_ResetBits(GPIOC,GPIO_Pin_0);
		else
			GPIO_SetBits(GPIOC,GPIO_Pin_0);
	}
	else if(n==3){
		if(led_i)
			GPIO_ResetBits(GPIOD,GPIO_Pin_3);
		else
			GPIO_SetBits(GPIOD,GPIO_Pin_3);
	}
	
}


void twinkleLED(u8 n)
{
	while(n>0){
		GPIO_SetBits(GPIOD,GPIO_Pin_3);
		delay_ms(50);
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);
		delay_ms(50);
		n--;
	}
}

void disableLED(u8 LED1,u8 LED2)
{
	if(LED1)
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
	else
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	
	if(LED2)
		GPIO_SetBits(GPIOD,GPIO_Pin_3);
	else
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);
}


