#include "fsmc.h"



/*
stm32f4的FSMC支持8、16、32位数据宽度，同时它将外部存储器划分为固定大小为256M字节的四个存储快(Bank)

以Bank 1为例，每个块分为4个区，每个区管理64M字节空间，每个区都有独立的寄存器对所连接的的寄存器进行配置。Bank 1的256M字节空间由28(256M=2^28)根地址线寻址。
这28根地址线是内部AHB地址总线（HADDR），HADDR【25:0】来自外部存储器地址FSMC_A[25：0]，而HADDR【2:6:27】对4个区寻址.

要注意的是，
当 Bank1 接的是 8  位宽度存储器的时候：HADDR[25:0]->FSMC_A[25:0]。
当 Bank1 接的是 16 位宽度存储器的时候：HADDR[25:1]->FSMC_A[24:0]。
这是因为HADDR地址总线是以8个字节为单位寻址的，而外部寄存器每个地址有16位,要让他们的地址相匹配的话，就必须把HADDR地址总先减少一半，也就是右移一位。（在没有发生
以为的情况下，右移以为相当于除于2），另外，HADDR[27:26]的设置，是不需要我们干预的，比如：当你选择使用 Bank1 的第三个区，即使用 FSMC_NE3 来连接外部设备的时候，
即对应了 HADDR[27:26]=10，我们要做的就是配置对应第3区的寄存器组，来适应外部设备即可，即这两个位是间接设置的。
*/


/*
地址划分
Bank1:6000 0000H - 6FFF FFFFH
Bank1:7000 0000H - 7FFF FFFFH
Bank1:8000 0000H - 8FFF FFFFH
Bank1:9000 0000H - 9FFF FFFFH

每个Bank的块：
如Bank1:
 第一块:6000 0000h--63ff ffffh (DATA长度为8位情况下,由地址线FSMC_A[25:0]决定；DATA长度为16位情况下，由地址线FSMC_A[24:0]决定)

 第二块:6400 0000h--67ff ffffh

 第三块:6800 0000h--6bff ffffh

 第四块:6c00 0000h--6fff ffffh

注：这里的HADDR是需要转换到外部设备的内部AHB地址线，每个地址对应一个字节单元。因此，若外部设备的地址宽度是8位的，则HADDR[25:0]与STM32的CPU引脚FSMC_A[25:0]一一对应，
最大可以访问64M字节的空间。若外部设备的地址宽度是16位的，则是HADDR[25:1]与STM32的CPU引脚FSMC_A[24:0]一一对应。在应用的时候，可以将FSMC_A总线连接到存储器或其他外设的地址总线引脚上
*/


void initFSMC(void)
{
	
	initGPIO(GPIOD,GPIO_Pin_0,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_1,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_4,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_8,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_9,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_11,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_12,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_13,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_14,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOD,GPIO_Pin_15,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	initGPIO(GPIOE,GPIO_Pin_0,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_1,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_7,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_8,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_9,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_11,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_12,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_13,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_14,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOE,GPIO_Pin_15,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);


	initGPIO(GPIOF,GPIO_Pin_0,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_1,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_2,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_3,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_4,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_12,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_13,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_14,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOF,GPIO_Pin_15,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	initGPIO(GPIOG,GPIO_Pin_0,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_1,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_2,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_3,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_4,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOG,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_FSMC,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
 
	
	
	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//使能FSMC时钟  
	
	
	/*
	 FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	//读时序
	readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns	，相当于RD高电平持续时间
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  readWriteTiming.FSMC_DataSetupTime = 60;			//数据保存时间为60个HCLK	=6*60=360ns，相当于RD低电平持续时间
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
	//写时序
	writeTiming.FSMC_AddressSetupTime =9;	      //地址建立时间（ADDSET）为9个HCLK =54ns 
  writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
  writeTiming.FSMC_DataSetupTime = 8;		 //数据保存时间为6ns*9个HCLK=54ns
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
*/


	readWriteTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  readWriteTiming.FSMC_DataSetupTime = 0x08;		 ////数据保持时间（DATAST）为9个HCLK 6*9=54ns	 	 
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00; //这些位用于定义一次读操作之后在总线上的延迟(仅适用于总线复用模式的NOR闪存操作)，一次读操作之后控制器需要在数据总线上为下次操作送出地址，
                                        //这个延迟就是为了防止总线冲突。如果扩展的存储器系统不包含总线复用模式的存储器，或最慢的存储器可以在6个HCLK时钟周期内将数据总线恢复到高阻状态，可以设置这个参数为其最小值。
  readWriteTiming.FSMC_CLKDivision = 0x00;//定义CLK时钟输出信号的周期，以HCLK周期数表示：
  readWriteTiming.FSMC_DataLatency = 0x00;//处于同步成组模式的NOR闪存，需要定义在读取第一个数据之前等待的存储器周期数目。
                                        //这个时间参数不是以HCLK表示，而是以闪存时钟(CLK)表示。在访问异步NOR闪存、SRAM或ROM时，这个参数不起作用。操作CRAM时，这个参数必须为0。
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

	
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//Bank1的第三区 这里我们使用NE3 ，也就对应BTCR[4],[5]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //地址\数据是否复用
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;//存储器类型，FSMC_MemoryType_SRAM;  
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;//是否进行成组模式访问 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; //等待信号有效级性
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   //该位决定控制器是否支持把非对齐的AHB成组操作分割成2次线性操作；该位仅在存储器的成组模式下有效。
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  //当闪存存储器处于成组传输模式时，NWAIT信号指示从闪存存储器出来的数据是否有效或是否需要插入等待周期。
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//该位指示FSMC是否允许/禁止对存储器的写操作。//存储器写使能 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  //当闪存存储器处于成组传输模式时，这一位允许/禁止通过NWAIT信号插入等待状态。
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; //该位允许FSMC使用FSMC_BWTR寄存器，即允许读和写使用不同的时序。// 读写使用相同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  //对于处于成组传输模式的闪存存储器，这一位允许/禁止通过NWAIT信号插入等待状态。读操作的同步成组传输协议使能位是FSMC_BCRx寄存器的BURSTEN位。
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //读写同样时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // 使能BANK1区域3			
}