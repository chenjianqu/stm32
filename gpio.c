#include "gpio.h"

/*
GPIO�İ��ֹ���ģʽ��
��1�� GPIO_Mode_AIN 				ģ������

��2�� GPIO_Mode_IN_FLOATING ��������

��3�� GPIO_Mode_IPD 				��������

��4�� GPIO_Mode_IPU 				��������

��5�� GPIO_Mode_Out_OD 			��©���
������൱�������ܵļ��缫��Ҫ�õ��ߵ�ƽ״̬���������裬�ʺϵ��������������յ���������ǿ��20mA���ڣ�
�ص㣺1. �����ⲿ��·���������������� IC�ڲ����������� IC �ڲ� MOSFET ��ͨʱ�����������Ǵ��ⲿ��
VCC ���� R pull-up �� MOSFET �� GND�� IC �ڲ�������µ�դ������������2. һ����˵����©���������Ӳ�ͬ��ƽ��������ƥ���ƽ�õģ���Ϊ��©���Ų������ⲿ����������ʱ��
ֻ������͵�ƽ�������Ҫͬʱ�߱�����ߵ�ƽ�Ĺ��ܣ�����Ҫ���������裬�ܺõ�һ���ŵ���ͨ���ı�
������Դ�ĵ�ѹ������Ըı䴫���ƽ�����������������Ϳ����ṩ TTL/CMOS ��ƽ����ȡ��������������ֵ�������߼���ƽת�����ص��ٶ� ����ֵԽ���ٶ�Խ�͹���ԽС�� 
���Ը��ص����ѡ��Ҫ��˹��ĺ��ٶȡ� ��
3. OPEN-DRAIN �ṩ�����������ʽ������Ҳ�������㣬���Ǵ��������ص���ʱ����Ϊ��������ͨ�����������Դ����Ը��س�磬���Ե�����ѡ��Сʱ��ʱ��С�������Ĵ�
��֮��ʱ�󹦺�С�������������ʱ��Ҫ���������½��������
4. ���Խ������©����� Pin�����ӵ�һ�����ϡ�ͨ��һֻ�������裬�ڲ������κ�����������£��γɡ����߼�����ϵ��

��6�� GPIO_Mode_Out_PP 			�������
�����·������������ͬ�������ܻ� MOSFET,�����췽ʽ�����ڵ�·��,�������������ܵĲ��ηŴ���
��,��·����ʱ����ֻ�ԳƵĹ��ʿ��ع�ÿ��ֻ��һ����ͨ�����Ե�ͨ���С��Ч�ʸߡ�����ȿ�������
�������Ҳ���ԴӸ��س�ȡ����������ʽ���������ߵ�·�ĸ�������������߿����ٶȡ�

��7�� GPIO_Mode_AF_OD 			���ÿ�©���

��8�� GPIO_Mode_AF_PP 			�����������

*/
/*
�� STM32 ��ѡ�� IO ģʽ
��1�� ��������_IN_FLOATING �����������룬������ KEY ʶ�� RX1
��2������������_IPU����IO �ڲ�������������
��3������������_IPD���� IO �ڲ�������������
��4�� ģ������_AIN ����Ӧ�� ADC ģ�����룬���ߵ͹�����ʡ��
��5����©���_OUT_OD ����IO ��� 0 �� GND�� IO ��� 1�����գ���Ҫ����������裬����ʵ�����
�ߵ�ƽ�������Ϊ 1 ʱ�� IO �ڵ�״̬�������������ߵ�ƽ���������ǿ�©���ģʽ������ IO ��Ҳ�Ϳ���
���ⲿ��·�ı�Ϊ�͵�ƽ�򲻱䡣���Զ� IO �����ƽ�仯��ʵ�� C51 �� IO ˫����
��6���������_OUT_PP ����IO ��� 0-�� GND�� IO ��� 1 -�� VCC��������ֵ��δ֪��
��7�����ù��ܵ��������_AF_PP ����Ƭ�����蹦�ܣ�I2C �� SCL,SDA��
��8�����ù��ܵĿ�©���_AF_OD����Ƭ�����蹦�ܣ�TX1,MOSI,MISO.SCK.SS��
STM32 ����ʵ��
*/
/*
ע�� PD0,PD1 ���������ʱ��������� 50MHz ���ģʽ.
ע��:PC13,PC14,PC15 ֻ������ 2MHz �����ģʽ,,���ֻ�ܴ� 30pf �ĸ���,����ͬʱֻ��ʹ��һ������
*/
/*
GPIO���ٶ���ָI/O��������·���ٶȣ�������ѡ��ͬ���������ģ�飬�ﵽ��ѵ��������ƺͽ��͹��ĵ�Ŀ�ġ�
��Ƶ��������·������Ҳ�ߣ����㲻��Ҫ�ߵ����Ƶ��ʱ����ѡ�õ�Ƶ������·�������ǳ����������ϵͳ��EMI���ܡ�
��Ȼ�����Ҫ����ϸ�Ƶ�ʵ��źţ���ȴѡ���˽ϵ�Ƶ�ʵ�����ģ�飬��ܿ��ܻ�õ�ʧ�������źš�
	2		MHZ
	25	MHz
	50	MHz
	100	MHz
*/
/*
	��©���  ֻ�������ǿ�͵�ƽ���ߵ�ƽ�ÿ��ⲿ�������ߡ�������൱�������ܵļ��缫. Ҫ�õ��ߵ�ƽ״̬��Ҫ�����������. �ʺ����������͵�����,�����յ������������ǿ(һ��20ma����)
	�������  �������ǿ�ߵ͵�ƽ��������������
	����������������ΪGPIO�ڱ������ڶ�����ʱ�������������������Ϊͨ��IO��ʹ�ã����˿ڱ������óɸ��ù������ģʽ(�����©)��
*/

void initGPIO(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x,GPIOMode_TypeDef GPIO_Mode_x,uint8_t GPIO_AF,GPIOSpeed_TypeDef GPIO_Speed_xMHz,GPIOOType_TypeDef GPIO_OType_x,GPIOPuPd_TypeDef GPIO_PuPd_x)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//ʹ��GPIOʱ�ӣ�AHB1�����ϣ�GPIOAʱ��ʹ�ܵ�ַλ��Ϊ0λ��GPIOB��1λ��GPIOC��2λ����������
		if(GPIOx==GPIOA)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		else if(GPIOx==GPIOB)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		else if(GPIOx==GPIOC)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		else if(GPIOx==GPIOD)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		else if(GPIOx==GPIOE)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		else if(GPIOx==GPIOF)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		else if(GPIOx==GPIOG)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
		else if(GPIOx==GPIOH)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
		
		//����GPIO���ù���
		if(GPIO_Mode_x!=AF_DISABLE)
		{
			u16 temp=GPIO_Pin_x;
			u8 GPIO_PinSourcex=0;
			//GPIO_Pin��GPIO_PinSource֮����ж�Ӧ��ϵ
			while(temp){
				temp/=2;
				GPIO_PinSourcex++;
			}
			GPIO_PinSourcex--;
			GPIO_PinAFConfig(GPIOx,GPIO_PinSourcex,GPIO_AF);
		}
	
		//����GPIO
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_x;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_x;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_xMHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_x;	
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_x;	
  	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

//�ı�GPIO�����ģʽ,ʹ�ñ�����ǰ���ȳ�ʼ����Ӧ��GPIO,�ڶ��������ǵڼ���io
void changeGPIO_Mode(GPIO_TypeDef* GPIOx,u8 pin,GPIOMode_TypeDef GPIO_Mode_x)
{
	//����GPIO_Init����д��
	if(GPIO_Mode_x==GPIO_Mode_OUT)
	{
		GPIOx->MODER&=~(3<<(pin*2));
		GPIOx->MODER|=1<<pin*2;
	}
	else if(GPIO_Mode_x==GPIO_Mode_IN)
	{
		GPIOx->MODER&=~(3<<(pin*2));
		GPIOx->MODER|=0<<pin*2;
	}
}


//��u16 GPIO_Pin_xת���ɵڼ���io
u8 turnPinNum(u16 GPIO_Pin_x)
{
	u8 pin=0;
	while(GPIO_Pin_x)
	{
				GPIO_Pin_x/=2;
				pin++;
	}
	return pin;
}



