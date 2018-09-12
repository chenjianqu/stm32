#include "dht11.h"

/*
DHT11的参数：
工作电压范围：3.3V-5.5V
工作电流 ：平均0.5mA
输出：单总线数字信号
测量范围：湿度20~90％RH，温度0~50℃
精度 ：湿度±5%，温度±2℃
分辨率 ：湿度1%，温度1℃ 
*/
/*
指令：
DHT11数字湿温度传感器采用单总线数据格式。单个数据引脚端口完成输入输出双向传输。
其数据包由5Byte（40Bit）组成。数据分小数部分和整数部分，一次完整的数据传输为40bit，高位先出。
DHT11的数据格式为：8bit湿度整数数据+8bit湿度小数数据+8bit温度整数数据+8bit温度小数数据+8bit校验和。
其中校验和数据为前四个字节相加。
传感器数据输出的是未编码的二进制数据。数据(湿度、温度、整数、小数)之间应该分开处理。
*/
/*
数据格式：
首先主机发送开始信号，即：拉低数据线，保持t1（至少18ms）时间，然后拉高数据线t2（20~40us）时间，
然后读取DHT11的响应，正常的话，DHT11会拉低数据线，保持t3（40~50us）时间，作为响应信号，
然后DHT11拉高数据线，保持t4（40~50us）时间后，开始输出数据。
*/

GPIO_TypeDef* dht11_GPIOx;
u16 dht11_GPIO_Pin_x;
u8 dht11_pin;//得到GPIO_Pin_x对应的数字

/*******************************初始化DHT11****************************************/
u8 initDHT11(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x)
{
	dht11_GPIOx=GPIOx;
	dht11_GPIO_Pin_x=GPIO_Pin_x;

	dht11_pin=turnPinNum(GPIO_Pin_x);
	
	initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	return requestDHT11();
}


/*******************************主机数据请求****************************************/
u8 requestDHT11(void)
{
	/***发送请求信号***/
	
	//设置为输出模式
	setGPIOMode(dht11_GPIOx,dht11_pin,GPIO_Mode_OUT);
	
  GPIO_ResetBits(dht11_GPIOx,dht11_GPIO_Pin_x);
  delay_ms(20);    	//拉低至少18ms
  GPIO_SetBits(dht11_GPIOx,dht11_GPIO_Pin_x); 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
	
	
	/***读取响应***/
	u8 count=0;
	setGPIOMode(dht11_GPIOx,dht11_pin,GPIO_Mode_IN);
	
	while(GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//等待从机响应（低电平时才算开始响应）
	{
		count++;
		delay_us(1);
	}
	if(count>100)//等了100ms都没进入低电平，即未响应则返回错误
		return DHT11_REQUEST_FAIL;
	else
		count=0;
	while(!GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//读取低电平的持续时间
	{
		count++;
		delay_us(1);
	}
	if(count>=100)//低电平时间超过100ms，则错误
		return DHT11_REQUEST_FAIL;
	return DHT11_REQUEST_OK;
}


/*******************************读取一个数据位****************************************/
u8 readDHT11Bit(void)
{
	u8 count=0;
	//一个数据位的第一段是12-14us的低电平，然后是高电平。若高电平的时间是26-28us，则为0；若为116-118us则为1
	
	while(GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//等待变成低电平，低电平是一个数据位的开始，等了100us都没变高
	{
		count++;
		delay_us(1);
	}
	count=0;
	while(!GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//计量低电平的持续时间
	{
		count++;
		delay_us(1);
	}
	delay_us(40);
	if(GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x))//变为低电平再过40us后若还是高电平 则读到的数据是1
		return 1;
	else
		return 0;
}


/*******************************读取一个字节****************************************/
u8 readDHT11Byte(void)
{
	u8 i;
	u8 data=0;
	for(i=0;i<8;i++)
	{
		data<<=1;//左移一位
		data|=readDHT11Bit();
	}
	return data;
}


/*******************************获取温度和湿度****************************************/
u8 readDHT11Data(u8 *temp,u8 *humi)
{
	u8 buf[5];
	u8 i;
	if(requestDHT11()==DHT11_REQUEST_OK)//如果请求成功
	{
		for(i=0;i<5;i++)
			buf[i]=readDHT11Byte();
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//对得到的数据进行校验
		{
			*humi=buf[0];//只取其整数
			*temp=buf[2];
		}
	}
	else
		return DHT11_GETDATA_FAIL;
	
	return DHT11_GETDATA_OK;
}

