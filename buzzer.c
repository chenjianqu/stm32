#include "buzzer.h"

#define TIMx TIM11

void initBuzzer(GPIO_TypeDef* GPIOx,u16 GPIO_Pin_x)
{
	initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initTIMBase(TIMx,167,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	initNVIC(TIM1_TRG_COM_TIM11_IRQn,2,0);
	TIM_ClearFlag(TIMx,TIM_FLAG_Update);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIMx,ENABLE);
}


/***************************************¶¨Ê±Æ÷1_11ÖÐ¶Ï*************************************/
void TIM1_TRG_COM_TIM11_IRQHandler(void){
	
	if(TIM_GetFlagStatus(TIM1,TIM_IT_Trigger)){
			changeLED(3);
			USART_SendData(USART1,'*');
		TIM_ClearITPendingBit(TIM1,TIM_IT_Trigger);
	}
	else if(TIM_GetFlagStatus(TIM1,TIM_IT_COM)){
		changeLED(3);
		USART_SendData(USART1,'*');
		TIM_ClearITPendingBit(TIM1,TIM_IT_COM);
	}
	
	else if(TIM_GetFlagStatus(TIM11,TIM_IT_Update))
	{
		changeLED(3);
		USART_SendData(USART1,'*');
		
		
		TIM_ClearITPendingBit(TIM11,TIM_IT_Update);
	}
}