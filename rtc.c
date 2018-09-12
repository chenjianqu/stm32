#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "uart.h" 
#include "stm32f4xx_pwr.h"

#define return_ok 1
#define return_fail 0

/*RTCʱ������ӵ�ʱ����������LSE��HSE,LSI�ȣ�һ��ѡ��LSE(�ⲿ����ʱ��)������7λ���첽ͨ����Ƶ��֮���ʱ�������
����Ĵ������پ���15λͬ��Ԥ��Ƶ������Ϊ��Ĵ�����ʱ����Դ��*/

NVIC_InitTypeDef   NVIC_InitStructure;

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 initRTC(void)
{
	
	u16 retry=0X1FFF; 
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������
	
	/****��һ������******/
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//�Ƿ��һ������?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ����
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0)return return_fail;		//LSE ����ʧ��. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

		RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��Ϊ127+1(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��255+1(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ��Ҳ����ѡ��12��Сʱ
    RTC_Init(&RTC_InitStructure);
 
		//���ó�ʼʱ��
		RTC_TimeTypeDef RTC_TimeTypeInitStructure;
		RTC_TimeTypeInitStructure.RTC_Hours=13;
		RTC_TimeTypeInitStructure.RTC_Minutes=13;
		RTC_TimeTypeInitStructure.RTC_Seconds=40;
		RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
		RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
			
		//��ʼ��������
		RTC_DateTypeDef RTC_DateTypeInitStructure;
		RTC_DateTypeInitStructure.RTC_Date=3;
		RTC_DateTypeInitStructure.RTC_Month=RTC_Month_January ;
		RTC_DateTypeInitStructure.RTC_WeekDay=RTC_Weekday_Wednesday  ;
		RTC_DateTypeInitStructure.RTC_Year=18;
		RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
	} 
 /****��һ������******/
	
	
	return return_ok;
}



//��������ʱ��(����������,24Сʱ��)
//week:���ڼ�(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:Сʱ,����,����

void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//�ر�����A 
	
	//����ʱ������
  RTC_TimeTypeInitStructure.RTC_Hours=hour;//Сʱ
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//����
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//��
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;//����Ϊ���ڼ��֣�Ҳ��������Ϊĳ����
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//��������
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;//��ȷƥ�����ڣ�ʱ����
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
  RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//���RTC����A�ı�־
  EXTI_ClearITPendingBit(EXTI_Line17);//���LINE17�ϵ��жϱ�־λ 
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//��������A�ж�
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//��������A 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE17
  EXTI_Init(&EXTI_InitStructure);//����

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
}


//�����Ի��Ѷ�ʱ������  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP
	
	/*����ѡ��֮ǰѡ����ⲿʱ��Դ��Ƶ�� ����2 4 8 16 ��Ƶ֮���ʱ��
	��ֱ��ѡ�񾭹��첽��ͬ����Ƶ����Ƶ֮���ʱ������
	
	*/
	RTC_WakeUpClockConfig(wksel);//����ʱ��ѡ��
	
	RTC_SetWakeUpCounter(cnt);//����WAKEUP�Զ���װ�ؼĴ���
	
	RTC_ClearITPendingBit(RTC_IT_WUT); //���RTC WAKE UP�ı�־
  EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKE UP ��ʱ���ж�
	RTC_WakeUpCmd( ENABLE);//����WAKE UP ��ʱ����
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE22
  EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
}


//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A�ж�?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//����жϱ�־
		twinkleLED(10);
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//����ж���17���жϱ�־ 											 
}


//RTC WAKE UP�����жϷ�����
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP�ж�?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//����жϱ�־
		changeLED(3);
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//����ж���22���жϱ�־ 								
}






/***********���ڲ���*************/
u32 subSecond=0;
void rtcTest(void)
{
	subSecond=RTC_GetSubSecond();//��ȡ���룬����Ĵ����������ã�ȷ��������Ԥ��Ƶϵ��֮�������Զ�����
	
	//��ȡ��ǰʱ�䣬��ֵ�����ڽṹ����
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	
	//��ȡ��ǰ����
	 RTC_DateTypeDef RTC_DateStruct;
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
	
	//��ȡ���ӣ���ֵ�����ڽṹ����
	RTC_AlarmTypeDef RTC_AlarmStruct;
	RTC_GetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmStruct);

	//��������
	USART_SendNumber(USART1,RTC_DateStruct.RTC_Year);
	USART_SendNumber(USART1,RTC_DateStruct.RTC_Month);
	USART_SendNumber(USART1,RTC_DateStruct.RTC_Date);
	USART_SendNumber(USART1,RTC_DateStruct.RTC_WeekDay);
	USART_SendData(USART1,'*');
	delay_ms(20);
	//����ʱ��
	USART_SendNumber(USART1,RTC_TimeStruct.RTC_Hours);
	USART_SendNumber(USART1,RTC_TimeStruct.RTC_Minutes);
	USART_SendNumber(USART1,RTC_TimeStruct.RTC_Seconds);
	USART_SendNumber(USART1,subSecond);
	USART_SendData(USART1,'*');
	delay_ms(20);
	//�������ӵ�ʱ��
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

