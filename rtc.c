#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "uart.h" 
#include "stm32f4xx_pwr.h"

#define return_ok 1
#define return_fail 0

/*RTC时间和闹钟的时钟脉冲来自LSE或HSE,LSI等，一般选择LSE(外部高速时钟)，经过7位的异步通道分频器之后的时钟脉冲给
亚秒寄存器，再经过15位同步预分频器后作为秒寄存器的时钟来源。*/

NVIC_InitTypeDef   NVIC_InitStructure;

//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
u8 initRTC(void)
{
	
	u16 retry=0X1FFF; 
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问
	
	/****第一次配置******/
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0)return return_fail;		//LSE 开启失败. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

		RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数为127+1(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数255+1(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式，也可以选择12个小时
    RTC_Init(&RTC_InitStructure);
 
		//设置初始时间
		RTC_TimeTypeDef RTC_TimeTypeInitStructure;
		RTC_TimeTypeInitStructure.RTC_Hours=13;
		RTC_TimeTypeInitStructure.RTC_Minutes=13;
		RTC_TimeTypeInitStructure.RTC_Seconds=40;
		RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
		RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
			
		//初始日期配置
		RTC_DateTypeDef RTC_DateTypeInitStructure;
		RTC_DateTypeInitStructure.RTC_Date=3;
		RTC_DateTypeInitStructure.RTC_Month=RTC_Month_January ;
		RTC_DateTypeInitStructure.RTC_WeekDay=RTC_Weekday_Wednesday  ;
		RTC_DateTypeInitStructure.RTC_Year=18;
		RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	} 
 /****第一次配置******/
	
	
	return return_ok;
}



//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:小时,分钟,秒钟

void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟A 
	
	//闹钟时间设置
  RTC_TimeTypeInitStructure.RTC_Hours=hour;//小时
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//分钟
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//秒
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;//设置为星期几闹，也可以设置为某日闹
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//按星期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;//精确匹配星期，时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
  RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//清除RTC闹钟A的标志
  EXTI_ClearITPendingBit(EXTI_Line17);//清除LINE17上的中断标志位 
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启闹钟A 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE17
  EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
}


//周期性唤醒定时器设置  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);//关闭WAKE UP
	
	/*可以选择之前选择的外部时钟源的频率 经过2 4 8 16 分频之后的时钟
	或直接选择经过异步和同步分频器分频之后的时钟脉冲
	
	*/
	RTC_WakeUpClockConfig(wksel);//唤醒时钟选择
	
	RTC_SetWakeUpCounter(cnt);//设置WAKEUP自动重装载寄存器
	
	RTC_ClearITPendingBit(RTC_IT_WUT); //清除RTC WAKE UP的标志
  EXTI_ClearITPendingBit(EXTI_Line22);//清除LINE22上的中断标志位 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启WAKE UP 定时器中断
	RTC_WakeUpCmd( ENABLE);//开启WAKE UP 定时器　
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE22
  EXTI_Init(&EXTI_InitStructure);//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
}


//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志
		twinkleLED(10);
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断线17的中断标志 											 
}


//RTC WAKE UP周期中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP中断?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//清除中断标志
		changeLED(3);
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22的中断标志 								
}






/***********用于测试*************/
u32 subSecond=0;
void rtcTest(void)
{
	subSecond=RTC_GetSubSecond();//获取亚秒，亚秒寄存器不用配置，确定好两个预分频系数之后亚秒自动配置
	
	//获取当前时间，把值储存在结构体里
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	
	//获取当前日期
	 RTC_DateTypeDef RTC_DateStruct;
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
	
	//获取闹钟，将值储存在结构体中
	RTC_AlarmTypeDef RTC_AlarmStruct;
	RTC_GetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmStruct);

	//发送日期
	USART_SendNumber(USART1,RTC_DateStruct.RTC_Year);
	USART_SendNumber(USART1,RTC_DateStruct.RTC_Month);
	USART_SendNumber(USART1,RTC_DateStruct.RTC_Date);
	USART_SendNumber(USART1,RTC_DateStruct.RTC_WeekDay);
	USART_SendData(USART1,'*');
	delay_ms(20);
	//发送时间
	USART_SendNumber(USART1,RTC_TimeStruct.RTC_Hours);
	USART_SendNumber(USART1,RTC_TimeStruct.RTC_Minutes);
	USART_SendNumber(USART1,RTC_TimeStruct.RTC_Seconds);
	USART_SendNumber(USART1,subSecond);
	USART_SendData(USART1,'*');
	delay_ms(20);
	//发送闹钟的时间
	USART_SendNumber(USART1,RTC_AlarmStruct.RTC_AlarmDateWeekDay);
	RTC_TimeTypeDef RTC_TimeTypeStructure=RTC_AlarmStruct.RTC_AlarmTime;
	USART_SendNumber(USART1,RTC_TimeTypeStructure.RTC_Hours);
	USART_SendData(USART1,'*');
	delay_ms(5);
	USART_SendData(USART1,32);
	delay_ms(5);
	USART_SendData(USART1,32);
	delay_ms(5);
	delay_ms(20);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	delay_ms(300);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	delay_ms(300);
	delay_ms(3000);
}

