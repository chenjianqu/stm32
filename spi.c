#include "spi.h"
#include "gpio.h"
#include "led.h"

/*
SPI 是英语Serial Peripheral interface的缩写，顾名思义就是串行外围设备接口。是Motorola首先在其MC68HCXX系列处理器上定义的。
SPI，是一种高速的，全双工，同步的通信总线，并且在芯片的管脚上只占用四根线，节约了芯片的管脚，同时为PCB的布局上节省空间，提供方便，
主要应用在 EEPROM，FLASH，实时时钟，AD转换器，还有数字信号处理器和数字信号解码器之间。

SPI接口一般使用4条线通信：
MISO 主设备数据输入，从设备数据输出。
MOSI 主设备数据输出，从设备数据输入。
SCLK时钟信号，由主设备产生。
CS从设备片选信号，由主设备控制。
*/
/*
SPI工作原理：
硬件上为4根线。主机和从机都有一个串行移位寄存器，主机通过向它的SPI串行寄存器写入一个字节来发起一次传输。
串行移位寄存器通过MOSI信号线将字节传送给从机，从机也将自己的串行移位寄存器中的内容通过MISO信号线返回给主机。这样，两个移位寄存器中的内容就被交换。
外设的写操作和读操作是同步完成的。
如果只进行写操作，主机只需忽略接收到的字节；反之，若主机要读取从机的一个字节，就必须发送一个空字节来引发从机的传输。
*/
/*
SPI配置过程：
①使能SPIx和IO口时钟
    RCC_AHBxPeriphClockCmd() / RCC_APBxPeriphClockCmd();
②初始化IO口为复用功能
    void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
③设置引脚复用映射：
    GPIO_PinAFConfig();
②初始化SPIx,设置SPIx工作模式
    void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
③使能SPIx
    void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
④SPI传输数据
    void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
    uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx) ;
⑤查看SPI传输状态
   SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE);
*/

/*
MISO:主设备数据输入，从设备数据输出
MOSI:主设备数据输出，从设备数据输入
SCL:时钟信号线，由主设备产生
CS:片选信号，主设备控制
*/
/*
SPI1:  SCK:PA5;						MISO:PA6/PA9/PB5;			MOSI:PA7/PA10/PB6;		NSS:PA4/PA15/PA10
SPI2:  SCK:PB10/PB13/PI1;	MISO:PC2/PB14/PI2;		MOSI:PC3/PB15/PI3;		NSS:PB12/PB9/PI0
SPI3:  SCK:PC10;					MISO:PC11/PA9/PB5;		MOSI:PC12/PA10/PB6;		NSS:PA4/PA15/PA10;
*/


/************************************初始化SPI******************************************************/




void initSPI(SPI_TypeDef* SPIx,GPIO_TypeDef* GPIOx_sck,uint16_t GPIO_Pin_sck,GPIO_TypeDef* GPIOx_miso,uint16_t GPIO_Pin_miso,GPIO_TypeDef* GPIOx_mosi,uint16_t GPIO_Pin_mosi)
{
	uint8_t GPIO_SPIx;
	
	//使能SPI的时钟	
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
	
	
	//初始化GPIO
	initGPIO(GPIOx_sck,	GPIO_Pin_sck,	GPIO_Mode_AF,GPIO_SPIx,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_miso,GPIO_Pin_miso,GPIO_Mode_AF,GPIO_SPIx,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_mosi,GPIO_Pin_mosi,GPIO_Mode_AF,GPIO_SPIx,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//SPI复位
	SPI_DeInit(SPIx);
	
	SPI_InitTypeDef  SPI_InitStructure;
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//设置SPI的单向或双向的数据模式，在此设置SPI为双向双线全双工
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//设置工作模式，主机还是从机
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//设置SPI一次传输数据的大小
	//以下两个参数
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;//串行同步时钟的第二个跳边沿（上升或下降）数据被采集
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//NSS信号(即片选信号)由硬件（NSS管脚）还是软件(使用SSI位)管理
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_256;//波特率预分频的值；
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;//指定数据传输从MSB位还是LSB位开始
	SPI_InitStructure.SPI_CRCPolynomial=7;//CRC值计算的多项式
	SPI_Init(SPIx,&SPI_InitStructure);
	
	SPI_Cmd(SPIx,ENABLE);
	
	rwSPI(SPIx,0xff);//主机发送任意字节来启动传输
}


/************************************SPI读写函数******************************************************/

//SPI读的时候要写东西，写的时候要读东西，所以此为读写函数
u8 rwSPI(SPI_TypeDef* SPIx,u8 mbyte)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPIx,mbyte); //通过外设SPIx发送一个byte数据
		
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPIx); //返回通过SPIx最近接收的数据	
}



//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：



/************************************SPI速度设置函数******************************************************/

void setSPISpeed(SPI_TypeDef* SPIx,u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	if(SPIx==SPI1){
		SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
		SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
		SPI_Cmd(SPI1,ENABLE); //使能SPI1
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

