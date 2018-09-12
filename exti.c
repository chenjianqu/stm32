#include "exti.h"
#include "key.h"

/*STM32F4的每个IO都可以作为外部中断输入。STM32F4的中断控制器支持22个外部中断/事件请求：
EXTI线0~15：对应外部IO口的输入中断。
EXTI线16：连接到PVD输出。
EXTI线17：连接到RTC闹钟事件。
EXTI线18：连接到USB OTG FS唤醒事件。
EXTI线19：连接到以太网唤醒事件。
EXTI线20：连接到USB OTG HS(在FS中配置)唤醒事件。
EXTI线21：连接到RTC入侵和时间戳事件。
EXTI线22：连接到RTC唤醒事件。
每个外部中断线可以独立的配置触发方式（上升沿，下降沿或者双边沿触发），触发/屏蔽，专用的状态位。
*/
/*
	外部中断线有0-15,其中GPIOx.0映射到EXTI_Line0,GPIOx.1映射到EXTI_Line2 ....以此类推，x为GPIO组ABCDEFGH.
	某个时间只有一个引脚能映射到某条线上。
	15个外部中断线，只有7个中断向量，即只有7个中断服务函数:0，1,2,3,4,5-9,10-15。
EXTI0_IRQHandler           
EXTI1_IRQHandler
EXTI2_IRQHandler           
EXTI3_IRQHandler           
EXTI4_IRQHandler           
EXTI9_5_IRQHandler         
EXTI15_10_IRQHandler       

*/

/*
哪些情况需要开启AFIO辅助功能时钟？
对寄存器AFIO_MAPR，AFIO_EXTICRX和AFIO_EVCR进行读写操作前，应当首先打开AFIO时钟。
AFIO_MAPR：配置复用功能重映射
AFIO_EXTICRX:配置外部中断线映射
AFIO_EVCR:   配置EVENTOUT事件输出
*/





//配置外部中断
void initEXTI(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x,EXTIMode_TypeDef EXTI_Mode_x,EXTITrigger_TypeDef EXTI_Trigger_x)
{
	//开启SYSCFG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	//将GPIOx Pinx映射到到 EXTI_Linex
	SYSCFG_EXTILineConfig(turnGPIOxNum(GPIOx),turnPinNum(GPIO_Pin_x));
	
	//外部中断配置
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=GPIO_Pin_x;//指定要配置的中断线
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_x;//模式：事件或中断,事件包含中断
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_x;//每个中断线可以设置触发方式：上升沿、下降沿、双沿触发.即是电压上升触发还是电压下降触发，或两者
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}






u8 keyFlag=0;

/*************************外部中断0 处理****************************/
void EXTI0_IRQHandler(void)
{
	//按键防抖
	delay_ms(50);
	if(!scanKey(3))
		return;
	//读中断状态
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
/*********************************************外部中断15-10**************************************************/
void EXTI15_10_IRQHandler(void)
{
	//判断中断请求来自哪个中断通道
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
