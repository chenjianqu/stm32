#include "matrixkeyboard.h"


//矩阵键盘引脚
#define GPIOX_ROW1 GPIOF
#define GPIO_PIN_ROW1 GPIO_Pin_1
#define GPIOX_ROW2 GPIOF
#define GPIO_PIN_ROW2 GPIO_Pin_2
#define GPIOX_ROW3 GPIOF
#define GPIO_PIN_ROW3 GPIO_Pin_3
#define GPIOX_ROW4 GPIOF
#define GPIO_PIN_ROW4 GPIO_Pin_4

#define GPIOX_COL1 GPIOF
#define GPIO_PIN_COL1 GPIO_Pin_5
#define GPIOX_COL2 GPIOF
#define GPIO_PIN_COL2 GPIO_Pin_6
#define GPIOX_COL3 GPIOF
#define GPIO_PIN_COL3 GPIO_Pin_7
#define GPIOX_COL4 GPIOF
#define GPIO_PIN_COL4 GPIO_Pin_8

u8 MKBChar;

void initMatrixKeyboard(void)
{
	initGPIO(GPIOX_ROW1,GPIO_PIN_ROW1,GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	initGPIO(GPIOX_ROW2,GPIO_PIN_ROW2,GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	initGPIO(GPIOX_ROW3,GPIO_PIN_ROW3,GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	initGPIO(GPIOX_ROW4,GPIO_PIN_ROW4,GPIO_Mode_IN,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	initGPIO(GPIOX_COL1,GPIO_PIN_COL1,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOX_COL2,GPIO_PIN_COL2,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOX_COL3,GPIO_PIN_COL3,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOX_COL4,GPIO_PIN_COL4,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_2MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//初始化引脚电平
	deInitMatrixKeyBoard();
	
	//外部中断配置
	initEXTI(GPIOX_ROW1,GPIO_PIN_ROW1,EXTI_Mode_Interrupt,EXTI_Trigger_Rising);
	initEXTI(GPIOX_ROW2,GPIO_PIN_ROW2,EXTI_Mode_Interrupt,EXTI_Trigger_Rising);
	initEXTI(GPIOX_ROW3,GPIO_PIN_ROW3,EXTI_Mode_Interrupt,EXTI_Trigger_Rising);
	initEXTI(GPIOX_ROW4,GPIO_PIN_ROW4,EXTI_Mode_Interrupt,EXTI_Trigger_Rising);
	
	//中断优先级配置
	initNVIC(EXTI1_IRQn,1,1);
	initNVIC(EXTI2_IRQn,1,1);
	initNVIC(EXTI3_IRQn,1,1);
	initNVIC(EXTI4_IRQn,1,1);
	
	USART_SendString(USART1,"MKB init OK\n",12);
}

void deInitMatrixKeyBoard(void)
{
	//初始化引脚电平
	GPIO_SetBits(GPIOX_COL1,GPIO_PIN_COL1);
	GPIO_SetBits(GPIOX_COL2,GPIO_PIN_COL2);
	GPIO_SetBits(GPIOX_COL3,GPIO_PIN_COL3);
	GPIO_SetBits(GPIOX_COL4,GPIO_PIN_COL4);
}

/*************************外部中断1处理****************************/
void EXTI1_IRQHandler(void)
{
	USART_SendChar(USART1,'&');
	
	if(EXTI_GetITStatus(EXTI_Line1))
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		//按键防抖
		delay_ms(50);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW1,GPIO_PIN_ROW1))return;
		
		u8 i=1;
		while(i--){
		
		//把每列逐个变为低电平，看按键信号是否消失
		GPIO_ResetBits(GPIOX_COL1,GPIO_PIN_COL1);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW1,GPIO_PIN_ROW1)){MKBChar='1';break;}
		GPIO_ResetBits(GPIOX_COL2,GPIO_PIN_COL2);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW1,GPIO_PIN_ROW1)){MKBChar='4';break;}
		GPIO_ResetBits(GPIOX_COL3,GPIO_PIN_COL3);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW1,GPIO_PIN_ROW1)){MKBChar='7';break;}
		GPIO_ResetBits(GPIOX_COL4,GPIO_PIN_COL4);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW1,GPIO_PIN_ROW1)){MKBChar='*';break;}
	}
		
		USART_SendChar(USART1,MKBChar);
		deInitMatrixKeyBoard();
	}
}

/*************************外部中断2处理****************************/
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2))
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
		
	//按键防抖
		delay_ms(50);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW2,GPIO_PIN_ROW2))return;
		
		u8 i=1;
		while(i--){
		
		//把每列逐个变为低电平，看按键信号是否消失
		GPIO_ResetBits(GPIOX_COL1,GPIO_PIN_COL1);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW2,GPIO_PIN_ROW2)){MKBChar='2';break;}
		GPIO_ResetBits(GPIOX_COL2,GPIO_PIN_COL2);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW2,GPIO_PIN_ROW2)){MKBChar='5';break;}
		GPIO_ResetBits(GPIOX_COL3,GPIO_PIN_COL3);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW2,GPIO_PIN_ROW2)){MKBChar='8';break;}
		GPIO_ResetBits(GPIOX_COL4,GPIO_PIN_COL4);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW2,GPIO_PIN_ROW2)){MKBChar='0';break;}
		
		}
		USART_SendChar(USART1,MKBChar);
		deInitMatrixKeyBoard();
	}
	
}

/*************************外部中断3处理****************************/
void EXTI3_IRQHandler(void)
{
	
	
	//读中断状态
	if(EXTI_GetITStatus(EXTI_Line3))
	{
		EXTI_ClearITPendingBit(EXTI_Line3);
		delay_ms(50);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW3,GPIO_PIN_ROW3))return;
		
		u8 i=1;
		while(i--){
		//把每列逐个变为低电平，看按键信号是否消失
		GPIO_ResetBits(GPIOX_COL1,GPIO_PIN_COL1);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW3,GPIO_PIN_ROW3)){MKBChar='3';break;}
		GPIO_ResetBits(GPIOX_COL2,GPIO_PIN_COL2);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW3,GPIO_PIN_ROW3)){MKBChar='6';break;}
		GPIO_ResetBits(GPIOX_COL3,GPIO_PIN_COL3);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW3,GPIO_PIN_ROW3)){MKBChar='9';break;}
		GPIO_ResetBits(GPIOX_COL4,GPIO_PIN_COL4);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW3,GPIO_PIN_ROW3)){MKBChar='#';break;}
		
		}
		USART_SendChar(USART1,MKBChar);
		deInitMatrixKeyBoard();
	}
	
}

/*************************外部中断4处理****************************/
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4))
	{
		EXTI_ClearITPendingBit(EXTI_Line4);
		delay_ms(50);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW4,GPIO_PIN_ROW4))return;
		u8 i=1;
		while(i--){
		
			//把每列逐个变为低电平，看按键信号是否消失
		GPIO_ResetBits(GPIOX_COL1,GPIO_PIN_COL1);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW4,GPIO_PIN_ROW4)){MKBChar='A';break;}
		GPIO_ResetBits(GPIOX_COL2,GPIO_PIN_COL2);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW4,GPIO_PIN_ROW4)){MKBChar='B';break;}
		GPIO_ResetBits(GPIOX_COL3,GPIO_PIN_COL3);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW4,GPIO_PIN_ROW4)){MKBChar='C';break;}
		GPIO_ResetBits(GPIOX_COL4,GPIO_PIN_COL4);
		if(!GPIO_ReadInputDataBit(GPIOX_ROW4,GPIO_PIN_ROW4)){MKBChar='D';break;}
		
		}
		USART_SendChar(USART1,MKBChar);
		deInitMatrixKeyBoard();
	}
	
}

