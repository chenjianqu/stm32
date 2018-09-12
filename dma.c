#include "stm32f4xx_it.h"
#include "dma.h"

/**DMA有两个DMA控制器，每个控制器有8个DMA流，每个流有8个通道(请求)**/
/*每个外设的DMA流和通道是不同的，使用时注意选择
DMA传输数据期间，不占用CPU，此时CPU可继续执行后面的程序。*/

/*
调用实例：
u8 NAME[]={"11111111111111111111111111111111111111111111111111111111111111"};
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//使能串口1DMA传输
	initDMA(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)NAME,(u16)20);
	startDMA(DMA2_Stream7);
*/


u16 dma_NumToSend;

//参数：数据流，通道，外设基地址，存储器基地址，传输的数据量
void initDMA(DMA_Stream_TypeDef* DMA_Streamx,u32 DMA_Channel_x,u32 addressPeriph,u32 addressMemory,u16 dataNum)
{
	
	dma_NumToSend=dataNum;
	
	//判断Sreamx是哪个DMA的，DMA2的地址高于DMA1
	if((u32)DMA_Streamx>(u32)DMA2)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	else
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
	
	//DMA复位
	DMA_DeInit(DMA_Streamx);	
	
	
	while(DMA_GetCmdStatus(DMA_Streamx)!=DISABLE){}//等待DMA可配置
		
		//DMA通道配置 
		DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_Channel =DMA_Channel_x;//设置DMA的传输通道，确定双方的传输地址后通道也就确定了，传输通道可以查表
		DMA_InitStructure.DMA_PeripheralBaseAddr = addressPeriph;//设置外设基地址
		DMA_InitStructure.DMA_Memory0BaseAddr =addressMemory;//设置内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//设置数据传输方向
		DMA_InitStructure.DMA_BufferSize = dataNum;//DMA通道的DMA缓存的大小，即设置DMA传输的数量，每传输一个值减一
		
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置外设地址寄存器不递增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//设置内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//设置传输数据宽度为字节（8位）,还可以设置为半字（16位），字（32位）
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte ;//同上
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//是否开启循环模式，这里选否
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//DMA通道的优先级为中，有高中低超高
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//指定如果FIFO模式或直接模式将用于指定的流 不使能FIFO模式 ，FIFO就是当源和目标的数据传输宽度不一致时对传输的数据进行封装和解封
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//指定了FIFO阈值水平
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
		
		DMA_Init(DMA_Streamx, &DMA_InitStructure);//初始化DMA流
		
}

//开启一次DMA传输
void startDMA(DMA_Stream_TypeDef* DMA_Streamx)
{
	DMA_Cmd(DMA_Streamx,DISABLE);//关闭流
	while(DMA_GetCmdStatus(DMA_Streamx)!=DISABLE){}//检查寄存器的EN位，确保没有别的数据流正在使用 DMA可以被设置
	DMA_SetCurrDataCounter(DMA_Streamx,dma_NumToSend);//每次调用都重新设置传输长度
	DMA_Cmd(DMA_Streamx,ENABLE);
}


/*
if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成

DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//清除DMA2_Steam7传输完成标志

DMA_GetCurrDataCounter(DMA2_Stream7);//得到当前还剩余多少个数据

*/

