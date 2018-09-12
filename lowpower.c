#include "lowpower.h"
#include "delay.h"
#include "led.h"


u8 checkWakeup(u8 s)//����Ƿ񳤰�
{
	u8 t=0;
	s=s*20;
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)){
			while(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)&&t<s){//���һֱ���Ұ���ʱ��С��5s
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
	while(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11));//�ȴ������ɿ�������RTC�ж�ʱ,�����WK_UP�ɿ��ٽ��������
	
	RCC_AHB1PeriphResetCmd(0X04FF,ENABLE);//��λ���е�IO��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//ʹ��PWRʱ��
	
	PWR_BackupAccessCmd(ENABLE);//���������
	
	//�ر�RTC�ж�
	RTC_ITConfig(RTC_IT_TS|RTC_IT_WUT|RTC_IT_ALRA|RTC_IT_ALRB,DISABLE);
	RTC_ClearITPendingBit(RTC_IT_TS|RTC_IT_WUT|RTC_IT_ALRA|RTC_IT_ALRB);//���RTC����жϱ�־λ��
	
	PWR_ClearFlag(PWR_FLAG_WU);//���Wakeup��־
	
	PWR_WakeUpPinCmd(ENABLE);//����WKUP���ڻ���
	
	PWR_EnterSTANDBYMode();//�������ģʽ
}


	
