#include "at24cx.h"
#include "myiic.h"

/*EEPROM断电可保存的存储器 AT24C02总容量是256字节 2k比特*/



/*********************************初始化IIC接口********************************/

void initAT24CXX(void)
{
	initIIC();
}



/*AT24C02读时序：SDA:起始信号+设备地址（A0）+等待应答+要读出的地址+等待应答+发送起始信号+发送设备地址（A1）+等待应答+读取数值+停止信号*/

/*************************在AT24CXX指定地址读出一个数据**************************/

//ReadAddr:开始读数的地址
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
  IIC_Start();
	
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);//发送写命令
		IIC_WaitAck();
		IIC_Send_Byte(ReadAddr>>8);//对于大容量的EEPROM，发送设备地址后发送高地址	    
	}else 
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,（ReadAddr/256）<<1的结果为0，该句的作用是兼容其它型号的芯片 
	
	IIC_WaitAck();
  IIC_Send_Byte(ReadAddr%256);//发送低地址，对于AT24C02其地址不会大于256，故取余
	IIC_WaitAck();
	
	IIC_Start();
	IIC_Send_Byte(0XA1);//进入接收模式
	IIC_WaitAck();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();   
	return temp;
}


/*AT24C02写时序：SDA:起始信号+设备地址（写的时候是A0）+等待应答+要写入的地址+等待应答+要写入地址的值+等待应答+停止信号*/

/***************************在AT24CXX指定地址写入一个数据*****************************/

//WriteAddr  :写入数据的目的地址，DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_WaitAck();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else 
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	
	IIC_WaitAck();	   
  IIC_Send_Byte(WriteAddr%256);//发送低地址
	IIC_WaitAck(); 
	
	IIC_Send_Byte(DataToWrite); //发送字节							   
	IIC_WaitAck();  		    	   
  IIC_Stop();
	delay_ms(10);	 
}



/**********************在AT24CXX里面的指定地址开始写入长度为Len的数据************************/

//该函数用于写入16bit或者32bit的数据.  WriteAddr  :开始写入的地址  DataToWrite:数据数组首地址  Len:要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
}



/***********************在AT24CXX里面的指定地址开始读出长度为Len的数据*************************/

//该函数用于读出16bit或者32bit的数据. ReadAddr:开始读出的地址 Len:要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}



/**************************************检查AT24CXX是否正常*************************************/

//这里用了24XX的最后一个地址(255)来存储标志字. 如果用其他24C系列,这个地址要修改
u8 checkAT24CXX(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX
	USART_SendNumber(USART1,(u16)temp);
	if(temp==0X55)
		return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	  temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)
			return EE_CHECK_OK;//成功
	}
	return EE_CHHCK_FAIL;	//失败										  
}



/**************************在AT24CXX里面的指定地址开始读出指定个数的数据*****************************/

//ReadAddr :开始读出的地址 对24c02为0~255  pBuffer  :数据数组首地址  NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  



/**************************在AT24CXX里面的指定地址开始写入指定个数的数据***************************/

//WriteAddr :开始写入的地址 对24c02为0~255  pBuffer   :数据数组首地址  NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}