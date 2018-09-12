#include "w25x.h"
#include "spi.h"
#include "led.h"
#include "sys.h"


#define	W25QXX_CS 		PBout(14)  		//W25QXX的片选信号

#define NCS W25QXX_CS=1 //片选信号为高，表示不选中
#define ECS W25QXX_CS=0 //片选选中


u16 W25QXX_TYPE=W25Q128;	//默认是W25Q128

/*根据W25Q64数据手册来编写
是8M字节、64M比特大的SPI flash

本搭载的flash：
miso PB14
mosi PB15
sck PB13
cs PB12
*/

SPI_TypeDef* W25x_SPIx;

/*******************************初始化芯片*********************************/

void initW25x(void)
{
	
	W25x_SPIx=SPI1;
	
	//初始化CS
	initGPIO(GPIOB,GPIO_Pin_14,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_7,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	GPIO_SetBits(GPIOG,GPIO_Pin_7);//PG7输出1,防止NRF干扰SPI FLASH的通信 
	
	NCS;
	
	//初始化SPI，该模块使用SPI2
	initSPI(W25x_SPIx,GPIOB,GPIO_Pin_3,GPIOB,GPIO_Pin_4,GPIOB,GPIO_Pin_5);
	
	setSPISpeed(W25x_SPIx,SPI_BaudRatePrescaler_4);//设置SPI的波特率预分频系数为2
	W25QXX_TYPE=readW25xID();
}


/*读取W25QXX的状态寄存器
BIT7  6   5   4   3   2   1   0
SPR   RV  TB BP2 BP1 BP0 WEL BUSY
SPR:默认0,状态寄存器保护位,配合WP使用
TB,BP2,BP1,BP0:FLASH区域写保护设置
WEL:写使能锁定
BUSY:忙标记位(1,忙;0,空闲)
默认:0x00*/

/*******************************读取状态寄存器的值*********************************/

u8 readW25xSR(void)   
{  
	u8 byte=0;   
	ECS;  
	rwSPI(W25x_SPIx,W25X_ReadStatusReg);//发送读取状态寄存器命令
	byte=rwSPI(W25x_SPIx,0xff); //读取一个字节  
	NCS;   
	return byte;   
} 

/*******************************写状态寄存器*********************************/

//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void writeW25xSR(u8 v)   
{  
	ECS;  
	rwSPI(W25x_SPIx,W25X_WriteStatusReg);//发送写状态寄存器命令
	rwSPI(W25x_SPIx,v); //写一个字节  
	NCS;
}

  

/*******************************W25QXX写使能*********************************/

void enableW25xWrite(void)   
{
	ECS;
  rwSPI(W25x_SPIx,W25X_WriteEnable);//发送写使能 将WEL置位 
	NCS;
} 


/*******************************W25QXX写禁止	*********************************/

void disableW25xWrite(void)   
{
	ECS;
  rwSPI(W25x_SPIx,W25X_WriteDisable);//发送禁止写使能  将WEL清零
	NCS;
}


/*******************************等待空闲********************************/

void waitW25xBusy(void)   
{   
	while((readW25xSR()&0x01)==0x01);// 等待BUSY位清空
}


/*******************************进入掉电模式****************************/

void pdW25q64(void)
{ 
  ECS;
  rwSPI(W25x_SPIx,W25X_PowerDown);//发送掉电命令  
	NCS;
  delay_us(3);//等待TPD  
}  


/*******************************唤醒*********************************/

void wpW25x(void)   
{  
  ECS;
  rwSPI(W25x_SPIx,W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	NCS;
  delay_us(3);//等待TRES1
}


/*返回值如下:				   
0XEF13,表示芯片型号为W25Q80  
0XEF14,表示芯片型号为W25Q16    
0XEF15,表示芯片型号为W25Q32  
0XEF16,表示芯片型号为W25Q64 
0XEF17,表示芯片型号为W25Q128
*/


/*******************************读取芯片ID*********************************/

u16 readW25xID(void)
{
	/*读取ID先发送命令 再发送地址(000000h)*/
	u16 Temp = 0;	  
	ECS;
	rwSPI(W25x_SPIx,W25X_ManufactDeviceID);//发送读取ID命令	    
	
	//发送24位地址
	rwSPI(W25x_SPIx,0x00);
	rwSPI(W25x_SPIx,0x00);
	rwSPI(W25x_SPIx,0x00);
	
	Temp|=rwSPI(W25x_SPIx,0xFF)<<8;  //先读取高8位再读取低8位
	Temp|=rwSPI(W25x_SPIx,0xFF);	 
	NCS;
	return Temp;
}


//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)

/*******************************读取SPI FLASH*********************************/

void readW25x(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	ECS;   
  rwSPI(W25x_SPIx,W25X_ReadData);//发送读取命令   
	
	//发送24bit地址
  rwSPI(W25x_SPIx,(u8)((ReadAddr)>>16));      
  rwSPI(W25x_SPIx,(u8)((ReadAddr)>>8));   
  rwSPI(W25x_SPIx,(u8)ReadAddr);   
	
	//循环读数
  for(i=0;i<NumByteToRead;i++)
		pBuffer[i]=rwSPI(W25x_SPIx,0XFF);  
	
	NCS;
}


//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 

/*******************************SPI在一页(0~65535)内写入少于256个字节的数据*********************************/

void writeW25xPage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 i;  
	enableW25xWrite();//使能写
	ECS;
  rwSPI(W25x_SPIx,W25X_PageProgram);      //发送写页命令   
  
	rwSPI(W25x_SPIx,(u8)((WriteAddr)>>16)); //发送24bit地址    
  rwSPI(W25x_SPIx,(u8)((WriteAddr)>>8));   
  rwSPI(W25x_SPIx,(u8)WriteAddr);  
	
  for(i=0;i<NumByteToWrite;i++)
		rwSPI(W25x_SPIx,pBuffer[i]);//循环写数  
	
	NCS;//取消片选 
	USART_SendChar(USART1,'*');
	waitW25xBusy();//等待写入结束
	
	
} 

 
/*必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!具有自动换页功能 在指定地址开始写入指定长度的数据,但是要确保地址不越界!
pBuffer:数据存储区
WriteAddr:开始写入的地址(24bit)
NumByteToWrite:要写入的字节数(最大65535)*/

/*******************************无检验写SPI FLASH*********************************/

void writeW25x_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{
		writeW25xPage(pBuffer,WriteAddr,pageremain);
		
		if(NumByteToWrite==pageremain)
			break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

/*每个扇区有4k 4096个地址   在写任何一个地址之前，如果该地址的值不是0xFF，必须先擦除对应的sector,然后再写

根据要写的起始地址，确定要写的起始区域的Sector号以及在起始 sector中的偏移量。
根据要写的起始地址和字节数，确定要写的数据是否跨sector。
确定好要操作的sector以及sector的地址范围。
对每一个sector，先遍历要写的地址区域保存的数据是不是0xff，如果都是，就不用擦除。如果有不是0xff的区域，先读出里面的数据，保存在缓存W25QXX_BUFFER，然后擦除里面的内容。
然后把这个sector要操作的数据，写到缓存。最后一次性吧缓存W25QXX_BUFFER的数据写到这个对应的sector。
*/


  
/*在指定地址开始写入指定长度的数据 该函数带擦除操作!
pBuffer:数据存储区
WriteAddr:开始写入的地址(24bit)						
NumByteToWrite:要写入的字节数(最大65535)   */

/*******************************写SPI FLASH*********************************/

u8 W25QXX_BUFFER[4096];		 
void writeW25x(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;
	
 	secpos=WriteAddr/4096;//扇区地址，即哪个扇区
	u16 secoff=WriteAddr%4096;//在扇区内的偏移
	u16 secremain=4096-secoff;//扇区剩余空间大小   
	
 	if(NumByteToWrite<=secremain)//把secremain作为写入的截止地址
		secremain=NumByteToWrite;//不大于4096个字节
	USART_SendChar(USART1,'1');
	while(1) 
	{	
		readW25x(W25QXX_BUF,secpos*4096,4096);//读出整个secpos扇区的内容放到BUF里面
		USART_SendChar(USART1,'2');
		//校验数据，看取出的数据是否有未被擦除的数据
		for(i=0;i<secremain;i++){if(W25QXX_BUF[secoff+i]!=0XFF)break;//内容不为空 需要擦除  	  
			USART_SendChar(USART1,'3');
		}
		USART_SendChar(USART1,'4');
		//提前跳出循环 需要擦除
		if(i<secremain)
		{
			USART_SendChar(USART1,'5');
			eraseW25xSector(secpos);//擦除这个扇区
			
			//把要写入的内容赋值给BUF
			for(i=0;i<secremain;i++)W25QXX_BUF[i+secoff]=pBuffer[i];
			//写入整个扇区
			writeW25x_NoCheck(W25QXX_BUF,secpos*4096,4096);  
		}
		else{
			writeW25x_NoCheck(pBuffer,WriteAddr,secremain);//直接写入扇区剩余区间. 				   
			USART_SendChar(USART1,'6');
		}
		
		USART_SendChar(USART1,'7');
		
		if(NumByteToWrite==secremain)//写入结束了
			break;
		else//写入未结束
		{
			secpos++;//扇区增1
			secoff=0;//偏移位置为0 	 
			pBuffer+=secremain;//指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		  NumByteToWrite-=secremain;//字节数递减
			
			if(NumByteToWrite>4096)secremain=4096;//下一个扇区还是写不完
			else secremain=NumByteToWrite;//下一个扇区可以写完了
		}
		
	};	 
	
}


/*******************************擦除整个芯片*********************************/
//等待时间超长...
void eraseW25xChip(void)   
{                                   
  enableW25xWrite();
  waitW25xBusy();
  ECS;
  rwSPI(W25x_SPIx,W25X_ChipErase);        //发送片擦除命令  
	NCS;  	      
	waitW25xBusy();//等待芯片擦除结束
}   



//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms

/*******************************擦除扇区*********************************/

void eraseW25xSector(u32 Dst_Addr)
{  
	//监视falsh擦除情况,测试用   
	u8 arr[]="Start to Erase:\n";
 	USART_SendString(USART1,arr,sizeof(arr));
	
 	Dst_Addr*=4096;
	
  enableW25xWrite();
  waitW25xBusy();//等待不忙
  ECS;
  rwSPI(W25x_SPIx,W25X_SectorErase);//发送扇区擦除指令 
	
	//发送24位擦出地址
  rwSPI(W25x_SPIx,(u8)((Dst_Addr)>>16));//发送16-24bit地址    
  rwSPI(W25x_SPIx,(u8)((Dst_Addr)>>8));//发送8-16bit地址
  rwSPI(W25x_SPIx,(u8)Dst_Addr);//发送1-8bit地址
	
	NCS;
  waitW25xBusy();//等待擦除完成
}


u32 getW25xSize(void)
{
	return 8*1024*1024;
}


