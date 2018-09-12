#include "clockInit.h"
#include "stm32f4xx_it.h"

#define PLL_M      8
#define PLL_N      336	
#define PLL_P      2	
#define PLL_Q      7	

/**
HSE高速外部时钟
HSI高数内部时钟
PLL锁相环，由VCO xN /P /Q /R组成
	HSE或HSI经过输入分频因子M(2-63)分频后成为VCO的时钟输入，输入的时钟必须在1-2M之间
	输入的时钟经过VCO倍频因子N(192-432)之后成为VCO时钟输出，输出的时钟必须在192-432M
	而后经过PLLCLK分频因子P(可取值2、4、6、8)，USB OTG FS分频因子Q(可取值4-15)，但USB OTG FS必须为48M
	板子上 8M/8=1M 输入VCO,1M*336=336M输出VCO,PLLCK为168M,USBCLK为48M
SYSCLK系统时钟
AHB总线时钟HCLK
APB2总线时钟PCLK2 高速时钟总线
APB1总线时钟PCLK1 低速时钟总线
**/

void initClock(void){
	
		/*选择HSE为作为时钟来源*/
	
		ErrorStatus HSEStartUpStatus;
	
		RCC_DeInit();//复位RCC
	
		RCC_HSEConfig(RCC_HSE_ON);//使能HSE
	
		HSEStartUpStatus = RCC_WaitForHSEStartUp();//等待HSE启动稳定
	
		while(HSEStartUpStatus == ERROR);//若HSE启动失败
	
		/*设置总线时钟分频*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1);	//(HCLK=SYSCLK)=168MHz，设置倍频为1
		RCC_PCLK1Config(RCC_HCLK_Div4);		//(PCLK1=SYSCLK/4)=42MHz
		RCC_PCLK2Config(RCC_HCLK_Div2);		//(PCLK2=SYSCLK/2)=84MHz
	
		//配置PLL
		RCC_PLLConfig(RCC_PLLSource_HSE,PLL_M ,PLL_N ,PLL_P,PLL_Q);
	
		//使能PLL
		RCC_PLLCmd(ENABLE);
		
		//等待PLL稳定
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		
		//系统时钟源选择
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//读取时钟切换状态位，确保PLLCLK被选为系统时钟
		while(RCC_GetSYSCLKSource() != 0x08);
		
		//关闭内部时钟HSI
		RCC_HSICmd(DISABLE);				
		
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

