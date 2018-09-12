#include "at24cx.h"
#include "myiic.h"

/*EEPROM�ϵ�ɱ���Ĵ洢�� AT24C02��������256�ֽ� 2k����*/



/*********************************��ʼ��IIC�ӿ�********************************/

void initAT24CXX(void)
{
	initIIC();
}



/*AT24C02��ʱ��SDA:��ʼ�ź�+�豸��ַ��A0��+�ȴ�Ӧ��+Ҫ�����ĵ�ַ+�ȴ�Ӧ��+������ʼ�ź�+�����豸��ַ��A1��+�ȴ�Ӧ��+��ȡ��ֵ+ֹͣ�ź�*/

/*************************��AT24CXXָ����ַ����һ������**************************/

//ReadAddr:��ʼ�����ĵ�ַ
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
  IIC_Start();
	
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);//����д����
		IIC_WaitAck();
		IIC_Send_Byte(ReadAddr>>8);//���ڴ�������EEPROM�������豸��ַ���͸ߵ�ַ	    
	}else 
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,��ReadAddr/256��<<1�Ľ��Ϊ0���þ�������Ǽ��������ͺŵ�оƬ 
	
	IIC_WaitAck();
  IIC_Send_Byte(ReadAddr%256);//���͵͵�ַ������AT24C02���ַ�������256����ȡ��
	IIC_WaitAck();
	
	IIC_Start();
	IIC_Send_Byte(0XA1);//�������ģʽ
	IIC_WaitAck();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();   
	return temp;
}


/*AT24C02дʱ��SDA:��ʼ�ź�+�豸��ַ��д��ʱ����A0��+�ȴ�Ӧ��+Ҫд��ĵ�ַ+�ȴ�Ӧ��+Ҫд���ַ��ֵ+�ȴ�Ӧ��+ֹͣ�ź�*/

/***************************��AT24CXXָ����ַд��һ������*****************************/

//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ��DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_WaitAck();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else 
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	
	IIC_WaitAck();	   
  IIC_Send_Byte(WriteAddr%256);//���͵͵�ַ
	IIC_WaitAck(); 
	
	IIC_Send_Byte(DataToWrite); //�����ֽ�							   
	IIC_WaitAck();  		    	   
  IIC_Stop();
	delay_ms(10);	 
}



/**********************��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������************************/

//�ú�������д��16bit����32bit������.  WriteAddr  :��ʼд��ĵ�ַ  DataToWrite:���������׵�ַ  Len:Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
}



/***********************��AT24CXX�����ָ����ַ��ʼ��������ΪLen������*************************/

//�ú������ڶ���16bit����32bit������. ReadAddr:��ʼ�����ĵ�ַ Len:Ҫ�������ݵĳ���2,4
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



/**************************************���AT24CXX�Ƿ�����*************************************/

//��������24XX�����һ����ַ(255)���洢��־��. ���������24Cϵ��,�����ַҪ�޸�
u8 checkAT24CXX(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX
	USART_SendNumber(USART1,(u16)temp);
	if(temp==0X55)
		return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	  temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)
			return EE_CHECK_OK;//�ɹ�
	}
	return EE_CHHCK_FAIL;	//ʧ��										  
}



/**************************��AT24CXX�����ָ����ַ��ʼ����ָ������������*****************************/

//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255  pBuffer  :���������׵�ַ  NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  



/**************************��AT24CXX�����ָ����ַ��ʼд��ָ������������***************************/

//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255  pBuffer   :���������׵�ַ  NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}