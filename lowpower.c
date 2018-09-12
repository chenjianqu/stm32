#include "lowpower.h"
#include "delay.h"
#include "led.h"


u8 checkWakeup(u8 s)//检测是否长按
{
	u8 t=0;
	s=s*20;
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)){
			while(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)&&t<s){//如果一直按且按的时间小于5s
				delay_ms(50);
				t++;
			}
	}
	if(t<s)
		return 0;
	else
		return 1;
}


void enterStandby(void)
{
	while(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11));//等待按键松开（在有RTC中断时,必须等WK_UP松开再进入待机）
	
	RCC_AHB1PeriphResetCmd(0X04FF,ENABLE);//复位所有的IO口
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能PWR时钟
	
	PWR_BackupAccessCmd(ENABLE);//后备区域访问
	
	//关闭RTC中断
	RTC_ITConfig(RTC_IT_TS|RTC_IT_WUT|RTC_IT_ALRA|RTC_IT_ALRB,DISABLE);
	RTC_ClearITPendingBit(RTC_IT_TS|RTC_IT_WUT|RTC_IT_ALRA|RTC_IT_ALRB);//清楚RTC相关中断标志位。
	
	PWR_ClearFlag(PWR_FLAG_WU);//清除Wakeup标志
	
	PWR_WakeUpPinCmd(ENABLE);//设置WKUP用于唤醒
	
	PWR_EnterSTANDBYMode();//进入待机模式
}


	
