#include "spi.h"
#include "gpio.h"
#include "led.h"

/*
SPI ��Ӣ��Serial Peripheral interface����д������˼����Ǵ�����Χ�豸�ӿڡ���Motorola��������MC68HCXXϵ�д������϶���ġ�
SPI����һ�ָ��ٵģ�ȫ˫����ͬ����ͨ�����ߣ�������оƬ�Ĺܽ���ֻռ���ĸ��ߣ���Լ��оƬ�Ĺܽţ�ͬʱΪPCB�Ĳ����Ͻ�ʡ�ռ䣬�ṩ���㣬
��ҪӦ���� EEPROM��FLASH��ʵʱʱ�ӣ�ADת���������������źŴ������������źŽ�����֮�䡣

SPI�ӿ�һ��ʹ��4����ͨ�ţ�
MISO ���豸�������룬���豸���������
MOSI ���豸������������豸�������롣
SCLKʱ���źţ������豸������
CS���豸Ƭѡ�źţ������豸���ơ�
*/
/*
SPI����ԭ��
Ӳ����Ϊ4���ߡ������ʹӻ�����һ��������λ�Ĵ���������ͨ��������SPI���мĴ���д��һ���ֽ�������һ�δ��䡣
������λ�Ĵ���ͨ��MOSI�ź��߽��ֽڴ��͸��ӻ����ӻ�Ҳ���Լ��Ĵ�����λ�Ĵ����е�����ͨ��MISO�ź��߷��ظ�������������������λ�Ĵ����е����ݾͱ�������
�����д�����Ͷ�������ͬ����ɵġ�
���ֻ����д����������ֻ����Խ��յ����ֽڣ���֮��������Ҫ��ȡ�ӻ���һ���ֽڣ��ͱ��뷢��һ�����ֽ��������ӻ��Ĵ��䡣
*/
/*
SPI���ù��̣�
��ʹ��SPIx��IO��ʱ��
    RCC_AHBxPeriphClockCmd() / RCC_APBxPeriphClockCmd();
�ڳ�ʼ��IO��Ϊ���ù���
    void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
���������Ÿ���ӳ�䣺
    GPIO_PinAFConfig();
�ڳ�ʼ��SPIx,����SPIx����ģʽ
    void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
��ʹ��SPIx
    void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
��SPI��������
    void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
    uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx) ;
�ݲ鿴SPI����״̬
   SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE);
*/

/*
MISO:���豸�������룬���豸�������
MOSI:���豸������������豸��������
SCL:ʱ���ź��ߣ������豸����
CS:Ƭѡ�źţ����豸����
*/
/*
SPI1:  SCK:PA5;						MISO:PA6/PA9/PB5;			MOSI:PA7/PA10/PB6;		NSS:PA4/PA15/PA10
SPI2:  SCK:PB10/PB13/PI1;	MISO:PC2/PB14/PI2;		MOSI:PC3/PB15/PI3;		NSS:PB12/PB9/PI0
SPI3:  SCK:PC10;					MISO:PC11/PA9/PB5;		MOSI:PC12/PA10/PB6;		NSS:PA4/PA15/PA10;
*/


/************************************��ʼ��SPI******************************************************/




void initSPI(SPI_TypeDef* SPIx,GPIO_TypeDef* GPIOx_sck,uint16_t GPIO_Pin_sck,GPIO_TypeDef* GPIOx_miso,uint16_t GPIO_Pin_miso,GPIO_TypeDef* GPIOx_mosi,uint16_t GPIO_Pin_mosi)
{
	uint8_t GPIO_SPIx;
	
	//ʹ��SPI��ʱ��	
	if(SPIx==SPI1){
		GPIO_SPIx=GPIO_AF_SPI1;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	}
	else if(SPIx==SPI2){
		GPIO_SPIx=GPIO_AF_SPI2;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	}
	else if(SPIx==SPI3){
		GPIO_SPIx=GPIO_AF_SPI3;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	}
	
	
	//��ʼ��GPIO
	initGPIO(GPIOx_sck,	GPIO_Pin_sck,	GPIO_Mode_AF,GPIO_SPIx,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_miso,GPIO_Pin_miso,GPIO_Mode_AF,GPIO_SPIx,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_mosi,GPIO_Pin_mosi,GPIO_Mode_AF,GPIO_SPIx,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//SPI��λ
	SPI_DeInit(SPIx);
	
	SPI_InitTypeDef  SPI_InitStructure;
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//����SPI�ĵ����˫�������ģʽ���ڴ�����SPIΪ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//���ù���ģʽ���������Ǵӻ�
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//����SPIһ�δ������ݵĴ�С
	//������������
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ��ɼ�
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//NSS�ź�(��Ƭѡ�ź�)��Ӳ����NSS�ܽţ��������(ʹ��SSIλ)����
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_256;//������Ԥ��Ƶ��ֵ��
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;//ָ�����ݴ����MSBλ����LSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial=7;//CRCֵ����Ķ���ʽ
	SPI_Init(SPIx,&SPI_InitStructure);
	
	SPI_Cmd(SPIx,ENABLE);
	
	rwSPI(SPIx,0xff);//�������������ֽ�����������
}


/************************************SPI��д����******************************************************/

//SPI����ʱ��Ҫд������д��ʱ��Ҫ�����������Դ�Ϊ��д����
u8 rwSPI(SPI_TypeDef* SPIx,u8 mbyte)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPIx,mbyte); //ͨ������SPIx����һ��byte����
		
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPIx); //����ͨ��SPIx������յ�����	
}



//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2ʱ��һ��Ϊ84Mhz��



/************************************SPI�ٶ����ú���******************************************************/

void setSPISpeed(SPI_TypeDef* SPIx,u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	if(SPIx==SPI1){
		SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
		SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
		SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
	}
	else if(SPIx==SPI2){
		SPI2->CR1&=0XFFC7;
		SPI2->CR1|=SPI_BaudRatePrescaler; 
		SPI_Cmd(SPI2,ENABLE);
	}
	else if(SPIx==SPI3){
		SPI3->CR1&=0XFFC7;
		SPI3->CR1|=SPI_BaudRatePrescaler;
		SPI_Cmd(SPI3,ENABLE);
	}
}

