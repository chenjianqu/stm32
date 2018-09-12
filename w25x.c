#include "w25x.h"
#include "spi.h"
#include "led.h"
#include "sys.h"


#define	W25QXX_CS 		PBout(14)  		//W25QXX��Ƭѡ�ź�

#define NCS W25QXX_CS=1 //Ƭѡ�ź�Ϊ�ߣ���ʾ��ѡ��
#define ECS W25QXX_CS=0 //Ƭѡѡ��


u16 W25QXX_TYPE=W25Q128;	//Ĭ����W25Q128

/*����W25Q64�����ֲ�����д
��8M�ֽڡ�64M���ش��SPI flash

�����ص�flash��
miso PB14
mosi PB15
sck PB13
cs PB12
*/

SPI_TypeDef* W25x_SPIx;

/*******************************��ʼ��оƬ*********************************/

void initW25x(void)
{
	
	W25x_SPIx=SPI1;
	
	//��ʼ��CS
	initGPIO(GPIOB,GPIO_Pin_14,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_7,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	GPIO_SetBits(GPIOG,GPIO_Pin_7);//PG7���1,��ֹNRF����SPI FLASH��ͨ�� 
	
	NCS;
	
	//��ʼ��SPI����ģ��ʹ��SPI2
	initSPI(W25x_SPIx,GPIOB,GPIO_Pin_3,GPIOB,GPIO_Pin_4,GPIOB,GPIO_Pin_5);
	
	setSPISpeed(W25x_SPIx,SPI_BaudRatePrescaler_4);//����SPI�Ĳ�����Ԥ��Ƶϵ��Ϊ2
	W25QXX_TYPE=readW25xID();
}


/*��ȡW25QXX��״̬�Ĵ���
BIT7  6   5   4   3   2   1   0
SPR   RV  TB BP2 BP1 BP0 WEL BUSY
SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
TB,BP2,BP1,BP0:FLASH����д��������
WEL:дʹ������
BUSY:æ���λ(1,æ;0,����)
Ĭ��:0x00*/

/*******************************��ȡ״̬�Ĵ�����ֵ*********************************/

u8 readW25xSR(void)   
{  
	u8 byte=0;   
	ECS;  
	rwSPI(W25x_SPIx,W25X_ReadStatusReg);//���Ͷ�ȡ״̬�Ĵ�������
	byte=rwSPI(W25x_SPIx,0xff); //��ȡһ���ֽ�  
	NCS;   
	return byte;   
} 

/*******************************д״̬�Ĵ���*********************************/

//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void writeW25xSR(u8 v)   
{  
	ECS;  
	rwSPI(W25x_SPIx,W25X_WriteStatusReg);//����д״̬�Ĵ�������
	rwSPI(W25x_SPIx,v); //дһ���ֽ�  
	NCS;
}

  

/*******************************W25QXXдʹ��*********************************/

void enableW25xWrite(void)   
{
	ECS;
  rwSPI(W25x_SPIx,W25X_WriteEnable);//����дʹ�� ��WEL��λ 
	NCS;
} 


/*******************************W25QXXд��ֹ	*********************************/

void disableW25xWrite(void)   
{
	ECS;
  rwSPI(W25x_SPIx,W25X_WriteDisable);//���ͽ�ֹдʹ��  ��WEL����
	NCS;
}


/*******************************�ȴ�����********************************/

void waitW25xBusy(void)   
{   
	while((readW25xSR()&0x01)==0x01);// �ȴ�BUSYλ���
}


/*******************************�������ģʽ****************************/

void pdW25q64(void)
{ 
  ECS;
  rwSPI(W25x_SPIx,W25X_PowerDown);//���͵�������  
	NCS;
  delay_us(3);//�ȴ�TPD  
}  


/*******************************����*********************************/

void wpW25x(void)   
{  
  ECS;
  rwSPI(W25x_SPIx,W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	NCS;
  delay_us(3);//�ȴ�TRES1
}


/*����ֵ����:				   
0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
0XEF17,��ʾоƬ�ͺ�ΪW25Q128
*/


/*******************************��ȡоƬID*********************************/

u16 readW25xID(void)
{
	/*��ȡID�ȷ������� �ٷ��͵�ַ(000000h)*/
	u16 Temp = 0;	  
	ECS;
	rwSPI(W25x_SPIx,W25X_ManufactDeviceID);//���Ͷ�ȡID����	    
	
	//����24λ��ַ
	rwSPI(W25x_SPIx,0x00);
	rwSPI(W25x_SPIx,0x00);
	rwSPI(W25x_SPIx,0x00);
	
	Temp|=rwSPI(W25x_SPIx,0xFF)<<8;  //�ȶ�ȡ��8λ�ٶ�ȡ��8λ
	Temp|=rwSPI(W25x_SPIx,0xFF);	 
	NCS;
	return Temp;
}


//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)

/*******************************��ȡSPI FLASH*********************************/

void readW25x(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	ECS;   
  rwSPI(W25x_SPIx,W25X_ReadData);//���Ͷ�ȡ����   
	
	//����24bit��ַ
  rwSPI(W25x_SPIx,(u8)((ReadAddr)>>16));      
  rwSPI(W25x_SPIx,(u8)((ReadAddr)>>8));   
  rwSPI(W25x_SPIx,(u8)ReadAddr);   
	
	//ѭ������
  for(i=0;i<NumByteToRead;i++)
		pBuffer[i]=rwSPI(W25x_SPIx,0XFF);  
	
	NCS;
}


//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 

/*******************************SPI��һҳ(0~65535)��д������256���ֽڵ�����*********************************/

void writeW25xPage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 i;  
	enableW25xWrite();//ʹ��д
	ECS;
  rwSPI(W25x_SPIx,W25X_PageProgram);      //����дҳ����   
  
	rwSPI(W25x_SPIx,(u8)((WriteAddr)>>16)); //����24bit��ַ    
  rwSPI(W25x_SPIx,(u8)((WriteAddr)>>8));   
  rwSPI(W25x_SPIx,(u8)WriteAddr);  
	
  for(i=0;i<NumByteToWrite;i++)
		rwSPI(W25x_SPIx,pBuffer[i]);//ѭ��д��  
	
	NCS;//ȡ��Ƭѡ 
	USART_SendChar(USART1,'*');
	waitW25xBusy();//�ȴ�д�����
	
	
} 

 
/*����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!�����Զ���ҳ���� ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
pBuffer:���ݴ洢��
WriteAddr:��ʼд��ĵ�ַ(24bit)
NumByteToWrite:Ҫд����ֽ���(���65535)*/

/*******************************�޼���дSPI FLASH*********************************/

void writeW25x_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{
		writeW25xPage(pBuffer,WriteAddr,pageremain);
		
		if(NumByteToWrite==pageremain)
			break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 

/*ÿ��������4k 4096����ַ   ��д�κ�һ����ַ֮ǰ������õ�ַ��ֵ����0xFF�������Ȳ�����Ӧ��sector,Ȼ����д

����Ҫд����ʼ��ַ��ȷ��Ҫд����ʼ�����Sector���Լ�����ʼ sector�е�ƫ������
����Ҫд����ʼ��ַ���ֽ�����ȷ��Ҫд�������Ƿ��sector��
ȷ����Ҫ������sector�Լ�sector�ĵ�ַ��Χ��
��ÿһ��sector���ȱ���Ҫд�ĵ�ַ���򱣴�������ǲ���0xff��������ǣ��Ͳ��ò���������в���0xff�������ȶ�����������ݣ������ڻ���W25QXX_BUFFER��Ȼ�������������ݡ�
Ȼ������sectorҪ���������ݣ�д�����档���һ���԰ɻ���W25QXX_BUFFER������д�������Ӧ��sector��
*/


  
/*��ָ����ַ��ʼд��ָ�����ȵ����� �ú�������������!
pBuffer:���ݴ洢��
WriteAddr:��ʼд��ĵ�ַ(24bit)						
NumByteToWrite:Ҫд����ֽ���(���65535)   */

/*******************************дSPI FLASH*********************************/

u8 W25QXX_BUFFER[4096];		 
void writeW25x(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;
	
 	secpos=WriteAddr/4096;//������ַ�����ĸ�����
	u16 secoff=WriteAddr%4096;//�������ڵ�ƫ��
	u16 secremain=4096-secoff;//����ʣ��ռ��С   
	
 	if(NumByteToWrite<=secremain)//��secremain��Ϊд��Ľ�ֹ��ַ
		secremain=NumByteToWrite;//������4096���ֽ�
	USART_SendChar(USART1,'1');
	while(1) 
	{	
		readW25x(W25QXX_BUF,secpos*4096,4096);//��������secpos���������ݷŵ�BUF����
		USART_SendChar(USART1,'2');
		//У�����ݣ���ȡ���������Ƿ���δ������������
		for(i=0;i<secremain;i++){if(W25QXX_BUF[secoff+i]!=0XFF)break;//���ݲ�Ϊ�� ��Ҫ����  	  
			USART_SendChar(USART1,'3');
		}
		USART_SendChar(USART1,'4');
		//��ǰ����ѭ�� ��Ҫ����
		if(i<secremain)
		{
			USART_SendChar(USART1,'5');
			eraseW25xSector(secpos);//�����������
			
			//��Ҫд������ݸ�ֵ��BUF
			for(i=0;i<secremain;i++)W25QXX_BUF[i+secoff]=pBuffer[i];
			//д����������
			writeW25x_NoCheck(W25QXX_BUF,secpos*4096,4096);  
		}
		else{
			writeW25x_NoCheck(pBuffer,WriteAddr,secremain);//ֱ��д������ʣ������. 				   
			USART_SendChar(USART1,'6');
		}
		
		USART_SendChar(USART1,'7');
		
		if(NumByteToWrite==secremain)//д�������
			break;
		else//д��δ����
		{
			secpos++;//������1
			secoff=0;//ƫ��λ��Ϊ0 	 
			pBuffer+=secremain;//ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		  NumByteToWrite-=secremain;//�ֽ����ݼ�
			
			if(NumByteToWrite>4096)secremain=4096;//��һ����������д����
			else secremain=NumByteToWrite;//��һ����������д����
		}
		
	};	 
	
}


/*******************************��������оƬ*********************************/
//�ȴ�ʱ�䳬��...
void eraseW25xChip(void)   
{                                   
  enableW25xWrite();
  waitW25xBusy();
  ECS;
  rwSPI(W25x_SPIx,W25X_ChipErase);        //����Ƭ��������  
	NCS;  	      
	waitW25xBusy();//�ȴ�оƬ��������
}   



//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms

/*******************************��������*********************************/

void eraseW25xSector(u32 Dst_Addr)
{  
	//����falsh�������,������   
	u8 arr[]="Start to Erase:\n";
 	USART_SendString(USART1,arr,sizeof(arr));
	
 	Dst_Addr*=4096;
	
  enableW25xWrite();
  waitW25xBusy();//�ȴ���æ
  ECS;
  rwSPI(W25x_SPIx,W25X_SectorErase);//������������ָ�� 
	
	//����24λ������ַ
  rwSPI(W25x_SPIx,(u8)((Dst_Addr)>>16));//����16-24bit��ַ    
  rwSPI(W25x_SPIx,(u8)((Dst_Addr)>>8));//����8-16bit��ַ
  rwSPI(W25x_SPIx,(u8)Dst_Addr);//����1-8bit��ַ
	
	NCS;
  waitW25xBusy();//�ȴ��������
}


u32 getW25xSize(void)
{
	return 8*1024*1024;
}


