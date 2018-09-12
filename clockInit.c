#include "clockInit.h"
#include "stm32f4xx_it.h"

#define PLL_M      8
#define PLL_N      336	
#define PLL_P      2	
#define PLL_Q      7	

/**
HSE�����ⲿʱ��
HSI�����ڲ�ʱ��
PLL���໷����VCO xN /P /Q /R���
	HSE��HSI���������Ƶ����M(2-63)��Ƶ���ΪVCO��ʱ�����룬�����ʱ�ӱ�����1-2M֮��
	�����ʱ�Ӿ���VCO��Ƶ����N(192-432)֮���ΪVCOʱ������������ʱ�ӱ�����192-432M
	���󾭹�PLLCLK��Ƶ����P(��ȡֵ2��4��6��8)��USB OTG FS��Ƶ����Q(��ȡֵ4-15)����USB OTG FS����Ϊ48M
	������ 8M/8=1M ����VCO,1M*336=336M���VCO,PLLCKΪ168M,USBCLKΪ48M
SYSCLKϵͳʱ��
AHB����ʱ��HCLK
APB2����ʱ��PCLK2 ����ʱ������
APB1����ʱ��PCLK1 ����ʱ������
**/

void initClock(void){
	
		/*ѡ��HSEΪ��Ϊʱ����Դ*/
	
		ErrorStatus HSEStartUpStatus;
	
		RCC_DeInit();//��λRCC
	
		RCC_HSEConfig(RCC_HSE_ON);//ʹ��HSE
	
		HSEStartUpStatus = RCC_WaitForHSEStartUp();//�ȴ�HSE�����ȶ�
	
		while(HSEStartUpStatus == ERROR);//��HSE����ʧ��
	
		/*��������ʱ�ӷ�Ƶ*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1);	//(HCLK=SYSCLK)=168MHz�����ñ�ƵΪ1
		RCC_PCLK1Config(RCC_HCLK_Div4);		//(PCLK1=SYSCLK/4)=42MHz
		RCC_PCLK2Config(RCC_HCLK_Div2);		//(PCLK2=SYSCLK/2)=84MHz
	
		//����PLL
		RCC_PLLConfig(RCC_PLLSource_HSE,PLL_M ,PLL_N ,PLL_P,PLL_Q);
	
		//ʹ��PLL
		RCC_PLLCmd(ENABLE);
		
		//�ȴ�PLL�ȶ�
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		
		//ϵͳʱ��Դѡ��
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
		while(RCC_GetSYSCLKSource() != 0x08);
		
		//�ر��ڲ�ʱ��HSI
		RCC_HSICmd(DISABLE);				
		
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

