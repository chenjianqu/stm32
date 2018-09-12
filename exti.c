#include "exti.h"
#include "key.h"

/*STM32F4��ÿ��IO��������Ϊ�ⲿ�ж����롣STM32F4���жϿ�����֧��22���ⲿ�ж�/�¼�����
EXTI��0~15����Ӧ�ⲿIO�ڵ������жϡ�
EXTI��16�����ӵ�PVD�����
EXTI��17�����ӵ�RTC�����¼���
EXTI��18�����ӵ�USB OTG FS�����¼���
EXTI��19�����ӵ���̫�������¼���
EXTI��20�����ӵ�USB OTG HS(��FS������)�����¼���
EXTI��21�����ӵ�RTC���ֺ�ʱ����¼���
EXTI��22�����ӵ�RTC�����¼���
ÿ���ⲿ�ж��߿��Զ��������ô�����ʽ�������أ��½��ػ���˫���ش�����������/���Σ�ר�õ�״̬λ��
*/
/*
	�ⲿ�ж�����0-15,����GPIOx.0ӳ�䵽EXTI_Line0,GPIOx.1ӳ�䵽EXTI_Line2 ....�Դ����ƣ�xΪGPIO��ABCDEFGH.
	ĳ��ʱ��ֻ��һ��������ӳ�䵽ĳ�����ϡ�
	15���ⲿ�ж��ߣ�ֻ��7���ж���������ֻ��7���жϷ�����:0��1,2,3,4,5-9,10-15��
EXTI0_IRQHandler           
EXTI1_IRQHandler
EXTI2_IRQHandler           
EXTI3_IRQHandler           
EXTI4_IRQHandler           
EXTI9_5_IRQHandler         
EXTI15_10_IRQHandler       

*/

/*
��Щ�����Ҫ����AFIO��������ʱ�ӣ�
�ԼĴ���AFIO_MAPR��AFIO_EXTICRX��AFIO_EVCR���ж�д����ǰ��Ӧ�����ȴ�AFIOʱ�ӡ�
AFIO_MAPR�����ø��ù�����ӳ��
AFIO_EXTICRX:�����ⲿ�ж���ӳ��
AFIO_EVCR:   ����EVENTOUT�¼����
*/





//�����ⲿ�ж�
void initEXTI(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x,EXTIMode_TypeDef EXTI_Mode_x,EXTITrigger_TypeDef EXTI_Trigger_x)
{
	//����SYSCFGʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	//��GPIOx Pinxӳ�䵽�� EXTI_Linex
	SYSCFG_EXTILineConfig(turnGPIOxNum(GPIOx),turnPinNum(GPIO_Pin_x));
	
	//�ⲿ�ж�����
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=GPIO_Pin_x;//ָ��Ҫ���õ��ж���
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_x;//ģʽ���¼����ж�,�¼������ж�
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_x;//ÿ���ж��߿������ô�����ʽ�������ء��½��ء�˫�ش���.���ǵ�ѹ�����������ǵ�ѹ�½�������������
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}






u8 keyFlag=0;

/*************************�ⲿ�ж�0 ����****************************/
void EXTI0_IRQHandler(void)
{
	//��������
	delay_ms(50);
	if(!scanKey(3))
		return;
	//���ж�״̬
	if(EXTI_GetITStatus(EXTI_Line0)){
		
		EXTI_ClearITPendingBit(EXTI_Line0);
		
		onLED1;
		delay_ms(100);
		offLED1;
		delay_ms(100);
		
		u32 data[8];
		
		keyFlag=!keyFlag;
		
		if(keyFlag)
		{
			
		}
		
	}
	
}


u8 key2Flag=0;
u8 arrayWrite[]="Hello i a Chen Jianqu";
/*********************************************�ⲿ�ж�15-10**************************************************/
void EXTI15_10_IRQHandler(void)
{
	//�ж��ж����������ĸ��ж�ͨ��
	if(EXTI_GetITStatus(EXTI_Line11))
	{
		twinkleLED(5);

		key2Flag=!key2Flag;
		
		POINT_COLOR=BLUE;    
		
		if(key2Flag)
		{
			//AT24CXX_Write(0,(u8*)arrayWrite,sizeof(arrayWrite));

			u8 arr[]="WriteOK\n";
			USART_SendString(USART1,arr,8);
		}
		else
		{
			u8 arr[]="StartRead:\n";
			USART_SendString(USART1,arr,11);
			
			u8 data[30]={0};
			//AT24CXX_Read(0,data,20);
			
			USART_SendString(USART1,data,20);
		}

		EXTI_ClearITPendingBit(EXTI_Line11);
	}
		
}
