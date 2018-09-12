#include "iic.h"

/*
I2C(IIC,Inter－Integrated Circuit),两线式串行总线,由PHILIPS公司开发用于连接微控制器及其外围设备。由数据线SDA和时钟SCL构成的串行总线，可发送和接收数据。
在CPU与被控IC之间、IC与IC之间进行双向传送，高速IIC总线一般可达400kbps以上。IIC是半双工通信方式。
*/

/*通过IO口模拟IIC，因为stm32自带的IIC不好用*/

/*IIC协议：空闲状态 开始信号 停止信号 应答信号 数据的有效性 数据传输

空闲状态：I2C总线总线的SDA和SCL两条信号线同时处于高电平时，规定为总线的空闲状态。此时各个器件的输出级场效应管均处在截止状态，即释放总线，由两条信号线各自的上拉电阻把电平拉高。
起始信号：当SCL为高期间，SDA由高到低的跳变；启动信号是一种电平跳变时序信号，而不是一个电平信号。

应答信号：发送器每发送一个字节，就在时钟脉冲9期间释放数据线，由接收器反馈一个应答信号。 应答信号为低电平时，规定为有效应答位（ACK简称应答位），表示接收器已经成功地接收了该字节；
应答信号为高电平时，规定为非应答位（NACK），一般表示接收器接收该字节没有成功。

对于反馈有效应答位ACK的要求是，接收器在第9个时钟脉冲之前的低电平期间将SDA线拉低，并且确保在该时钟的高电平期间为稳定的低电平。 如果接收器是主控器，则在它收到最后一个字节后，
发送一个NACK信号，以通知被控发送器结束数据发送，并释放SDA线，以便主控接收器发送一个停止信号P。
*/


//SDA引脚
GPIO_TypeDef* iic_GPIOx_SDA;
uint16_t iic_GPIO_Pin_SDA;
u8 iic_pinSDA;//用于切换输入输出模式
//SCL引脚
GPIO_TypeDef* iic_GPIOx_SCL;
uint16_t iic_GPIO_Pin_SCL;
u8 iic_pinSCL;


/************************************初始化IIC******************************************************/
void initIIC(GPIO_TypeDef* GPIOx_sda,uint16_t GPIO_Pin_sda,GPIO_TypeDef* GPIOx_scl,uint16_t GPIO_Pin_scl) 
{
	iic_GPIOx_SDA=GPIOx_sda;
	iic_GPIO_Pin_SDA=GPIO_Pin_sda;
	iic_pinSDA=turnPinNum(iic_GPIO_Pin_SDA);
	iic_GPIOx_SCL=GPIOx_scl;
	iic_GPIO_Pin_SCL=GPIO_Pin_scl;
	iic_pinSCL=turnPinNum(iic_GPIO_Pin_SCL);
	
	initGPIO(iic_GPIOx_SDA,iic_GPIO_Pin_SDA,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(iic_GPIOx_SCL,iic_GPIO_Pin_SCL,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//电平拉高，使IIC处于空闲状态
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
}


/**************************************产生IIC起始信号**************************************/

/*当SCL为高期间，SDA由高到低的跳变；启动信号是一种电平跳变时序信号，而不是一个电平信号。*/
void IIC_Start(void)
{
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);//sda线输出
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(4);
	GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);//启动信号
	delay_us(4);
	
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);//钳住I2C总线，准备发送或接收数据 
}	  



/***********************************产生IIC停止信号*****************************************/

/*停止信号：当SCL为高期间，SDA由低到高的跳变；停止信号也是一种电平跳变时序信号，而不是一个电平信号。*/
void IIC_Stop(void)
{
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);//sda线输出
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	delay_us(4);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	delay_us(4);
}



/*发送器每发送一个字节，就在时钟脉冲9（前面的八个时钟脉冲用于发送一个字节）期间释放数据线，由接收器反馈一个应答信号。 
应答信号为低电平时，规定为有效应答位（ACK简称应答位），表示接收器已经成功地接收了该字节；
应答信号为高电平时，规定为非应答位（NACK），一般表示接收器接收该字节没有成功。
对于反馈有效应答位ACK的要求是，接收器在第9个时钟脉冲之前的低电平期间将SDA线拉低，并且确保在该时钟的高电平期间为稳定的低电平。 
如果接收器是主控器，则在它收到最后一个字节后，发送一个NACK信号，以通知被控发送器结束数据发送，并释放SDA线，以便主控接收器发送一个停止信号P。
*/

/******************************作为发送方 等待应答信号到来********************************/
u8 IIC_WaitAck(void)
{
	u8 ucErrTime=0;
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_IN);//SDA设置为输入  

	//发送方进入空闲状态
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	delay_us(1);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(1);
	
	//若发送数据方SDA一直没收到低电平 且持续
	while(GPIO_ReadInputDataBit(iic_GPIOx_SDA,iic_GPIO_Pin_SDA))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return ACK_FAIL;
		}
	}
	//接收到了成功的应答，
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);//时钟输出0 	   
	return ACK_OK;  
} 


/********************************接收方 产生ACK应答*****************************************/
void IIC_Ack(void)
{
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);
	
	GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);//SDA持续为低电平，低电平为有效应答
	//SCL产生一个脉冲
	delay_us(2);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(2);
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
}


/*******************************接收方  不产生ACK应答**************************************/
void IIC_NAck(void)
{
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);
	//SDA持续为高电平，高电平为无效应答
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	//SCL发送一个脉冲
	delay_us(2);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(2);
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
}



/*
I2C总线进行数据传送时，时钟信号为高电平期间，数据线上的数据必须保持稳定，只有在时钟线上的信号为低电平期间，数据线上的高电平或低电平状态才允许变化。 
*/
/*********************************IIC发送一个字节********************************/
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);
	
	//先把时钟线拉低，然后设置数据线，在把时钟线拉高，持续高电平时间后拉低。即在时钟线高电平期间稳定数据线电平。
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	
	for(t=0;t<8;t++)
  {
		//取最高位 然后右移7位
		if((txd&0x80)>>7)
			GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
		else
			GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
		
    txd<<=1;//数据左移一位
		
		//上几句确定了数据线的电平，下几句设置时钟线。时钟线高电平期间数据线电平不变
		delay_us(2);
		GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
		delay_us(2); 
		GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);	
		delay_us(2);
  }	 
}


/*********************************IIC读1个字节********************************/

u8 IIC_Read_Byte(u8 ack)
{
	u8 i;
	u8 receive=0;
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_IN);//SDA设置为输入
	
	//接收数据
  for(i=0;i<8;i++ )
	{
		//先把时钟线拉低再拉高，产生一个上升沿之后再去读数据线的电平 
    GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL); 
    delay_us(2);
		GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
    receive<<=1;
    if(GPIO_ReadInputDataBit(iic_GPIOx_SDA,iic_GPIO_Pin_SDA))//接收到高电平，则二进制加1
			receive++;
		delay_us(1);
  }
	
	//是否发送NACK结束接收信号
  if (ack==SACK)
		IIC_Ack();
	else if(ack==SNACK)
    IIC_NAck();
  
  return receive;
}



