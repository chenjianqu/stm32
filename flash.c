#include "flash.h"

//获取地址所在的扇区
uint16_t getFlashSector(u32 addr);

/*
STM32编程方式：
 在线编程（ICP,In-Circuit Programming）:
      通过JTAG/SWD协议或者系统加载程序(Bootloader)下载用户应用程序到微控制器中。
② 在程序中编程(IAP,In Application Programming):
      通过任何一种通信接口(如IO端口,USB,CAN,UART,I2C,SPI等)下载程序或者应用数据到存储器中。也就是说，STM32允许用户在应用程序中重新烧写闪存存储器中的内容。
			然而，IAP需要至少有一部分程序已经使用ICP方式烧到闪存存储器中（Bootloader)。
*/

/*
STM32F40x的闪存模块由：主存储器、系统存储器、OPT区域和选项字节等4部分组成。 
主存储器:	 该部分用来存放代码和数据常数（如const类型的数据）。分为12个扇区，前4个扇区为16KB大小，然后扇区4是64KB大小，扇区5~11是128K大小，不同容量的STM32F4，
拥有的扇区数不一样，比如我们的STM32F407ZGT6，则拥有全部12个扇区。从上图可以看出主存储器的起始地址就是0X08000000， B0、B1都接GND的时候，就是从0X08000000开始运行代码的。

系统存储器: 这个主要用来存放STM32F4的bootloader代码，此代码是出厂的时候就固化在STM32F4里面了，专门来给主存储器下载代码的。当B0接V3.3，B1接GND的时候，从该存储器启动（
即进入串口下载模式）。

OTP区域:   即一次性可编程区域，共528字节，被分成两个部分，前面512字节（32字节为1块，分成16块），可以用来存储一些用户数据（一次性的，写完一次，永远不可以擦除！！），
后面16字节，用于锁定对应块。

选项字节:	 用于配置读保护、BOR级别、软件/硬件看门狗以及器件处于待机或停止模式下的复位。 
*/

/*
在执行闪存写操作时，任何对闪存的读操作都会锁住总线，在写操作完成后读操作才能正确地进行；既在进行写或擦除操作时，不能进行代码或数据的读取操作。
*/

/*STM32的读操作:
为了准确读取 Flash 数据，必须根据 CPU 时钟 (HCLK) 频率和器件电源电压在 Flash 存取控制寄存器 (FLASH_ACR) 中正确地设置等待周期数 (LATENCY)。当电源电压低于2.1V 时，
必须关闭预取缓冲器。Flash 等待周期与CPU时钟频率之间具有对应关系。（查表）
*/
/*
STM32F4复位后，FLASH编程操作是被保护的，不能写入FLASH_CR寄存器；通过写入特定的序列（0X45670123和0XCDEF89AB）到FLASH_KEYR寄存器才可解除写保护，只有在写保护被解除后，我们才能操作相关寄存器。
   FLASH_CR的解锁序列为：
      1）写0X45670123（KEY1)到FLASH_KEYR
      2）写0XCDEF89AB(KEY2)到FLASH_KEYR
通过这两个步骤，即可解锁FLASH_CR，如果写入错误，那么FLASH_CR将被锁定，直到下次复位后才可以再次解锁。

STM32F4闪存的编程位数可以通过FLASH_CR的PSIZE字段配置，PSIZE的设置必须和电源电压匹配，由于我们开发板用的电压是3.3V，所以PSIZE必须设置为10，即32位并行位数。擦除或者编程，都必须以32位为基础进行。 

STM32F4的FLASH在编程的时候，也必须要求其写入地址的FLASH是被擦除了的（也就是其值必须是0XFFFFFFFF），否则无法写入。
*/

/*stm32 flash编程步骤：
检查FLASH_SR中的BSY位，确保当前未执行任何FLASH操作。
将FLASH_CR寄存器中的PG位置1，激活FLASH编程。
针对所需存储器地址（主存储器块或OTP区域内）执行数据写入操作：
     —并行位数为x8时按字节写入（PSIZE=00）
     —并行位数为x16时按半字写入（PSIZE=01）
     —并行位数为x32时按字写入（PSIZE=02）
     —并行位数为x64时按双字写入（PSIZE=03）
④ 等待BSY位清零，完成一次编程。
以上四步操作，就可以完成一次FLASH编程。不过有几点要注意：1，编程前，要确保要写如地址的FLASH已经擦除。2，要先解锁（否则不能操作FLASH_CR）。3，编程操作对OPT区域也有效，方法一模一样。
*/
/*stm32 flash擦除：
STM32F4的闪存擦除分为两种：
扇区擦除
整片擦除。

扇区擦除步骤
检查FLASH_CR的LOCK是否解锁，如果没有则先解锁
检查FLASH_SR寄存器中的BSY 位，确保当前未执行任何FLASH操作
在FLASH_CR寄存器中，将SER位置1，并从主存储块的12个扇区中选择要擦除的扇区 (SNB)
将FLASH_CR寄存器中的STRT位置1，触发擦除操作
等待BSY位清零
经过以上五步，就可以擦除某个扇区
*/

/*flash状态：
④获取状态函数
主要是用的函数是：
       FLASH_Status FLASH_GetStatus(void)；
返回值是通过枚举类型定义的：
typedef enum
{ 
  FLASH_BUSY = 1,//操作忙
  FLASH_ERROR_RD,//读保护错误
  FLASH_ERROR_PGS,//编程顺序错误
  FLASH_ERROR_PGP,//编程并行位数错误
  FLASH_ERROR_PGA,//编程对齐错误
  FLASH_ERROR_WRP,//写保护错误
  FLASH_ERROR_PROGRAM,//编程错误
  FLASH_ERROR_OPERATION,//操作错误
  FLASH_COMPLETE//操作结束
}FLASH_Status;


*/

/***************************************获取地址所在的扇区**************************************/ 
uint16_t getFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}


/***************************************读取指定地址的字(32位数据)**************************************/ 
//STM32F4闪存的编程位数可以通过FLASH_CR的PSIZE字段配置，PSIZE的设置必须和电源电压匹配，由于我们开发板用的电压是3.3V，所以PSIZE必须设置为10，即32位并行位数。擦除或者编程，都必须以32位为基础进行。
u32 readFlashWord(u32 faddr)
{
	return *(vu32*)faddr;//返回值:对应数据.
}



/*特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里没有重要数据,
最好是整个扇区先擦除了,然后慢慢往后写.*/

/*该函数对OTP区域也有效!可以用来写OTP区!OTP区域地址范围:0X1FFF7800~0X1FFF7A0F*/
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 

/***************************************从指定地址开始写入指定长度的数据**************************************/
void writeFlash(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<ADDR_FLASH_BASE||WriteAddr%4)return;	//非法地址,起始地址必须为4的倍数
	FLASH_Unlock();//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 		
	addrx=WriteAddr;//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
	
	if(addrx<0X1FFF0000)//只有主存储区,才需要执行擦除操作!!
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(readFlashWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{
				status=FLASH_EraseSector(getFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V之间!!
				if(status!=FLASH_COMPLETE)break;//发生错误了
			}else 
				addrx+=4;
		}
	}
	
	while(WriteAddr<endaddr)//写数据
	{
		if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)break;//写入数据,异常则退出 
		
		WriteAddr+=4;
		pBuffer++;
	} 
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
} 


//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数

/***************************************从指定地址开始读出指定长度的数据*************************************/
void readFlash(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=readFlashWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}