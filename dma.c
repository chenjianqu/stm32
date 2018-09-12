#include "stm32f4xx_it.h"
#include "dma.h"

/**DMA������DMA��������ÿ����������8��DMA����ÿ������8��ͨ��(����)**/
/*ÿ�������DMA����ͨ���ǲ�ͬ�ģ�ʹ��ʱע��ѡ��
DMA���������ڼ䣬��ռ��CPU����ʱCPU�ɼ���ִ�к���ĳ���*/

/*
����ʵ����
u8 NAME[]={"11111111111111111111111111111111111111111111111111111111111111"};
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//ʹ�ܴ���1DMA����
	initDMA(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)NAME,(u16)20);
	startDMA(DMA2_Stream7);
*/


u16 dma_NumToSend;

//��������������ͨ�����������ַ���洢������ַ�������������
void initDMA(DMA_Stream_TypeDef* DMA_Streamx,u32 DMA_Channel_x,u32 addressPeriph,u32 addressMemory,u16 dataNum)
{
	
	dma_NumToSend=dataNum;
	
	//�ж�Sreamx���ĸ�DMA�ģ�DMA2�ĵ�ַ����DMA1
	if((u32)DMA_Streamx>(u32)DMA2)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	else
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
	
	//DMA��λ
	DMA_DeInit(DMA_Streamx);	
	
	
	while(DMA_GetCmdStatus(DMA_Streamx)!=DISABLE){}//�ȴ�DMA������
		
		//DMAͨ������ 
		DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_Channel =DMA_Channel_x;//����DMA�Ĵ���ͨ����ȷ��˫���Ĵ����ַ��ͨ��Ҳ��ȷ���ˣ�����ͨ�����Բ��
		DMA_InitStructure.DMA_PeripheralBaseAddr = addressPeriph;//�����������ַ
		DMA_InitStructure.DMA_Memory0BaseAddr =addressMemory;//�����ڴ����ַ
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//�������ݴ��䷽��
		DMA_InitStructure.DMA_BufferSize = dataNum;//DMAͨ����DMA����Ĵ�С��������DMA�����������ÿ����һ��ֵ��һ
		
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ַ�Ĵ���������
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�����ڴ��ַ�Ĵ�������
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//���ô������ݿ��Ϊ�ֽڣ�8λ��,����������Ϊ���֣�16λ�����֣�32λ��
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte ;//ͬ��
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//�Ƿ���ѭ��ģʽ������ѡ��
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//DMAͨ�������ȼ�Ϊ�У��и��еͳ���
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ ��ʹ��FIFOģʽ ��FIFO���ǵ�Դ��Ŀ������ݴ����Ȳ�һ��ʱ�Դ�������ݽ��з�װ�ͽ��
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//ָ����FIFO��ֵˮƽ
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
		
		DMA_Init(DMA_Streamx, &DMA_InitStructure);//��ʼ��DMA��
		
}

//����һ��DMA����
void startDMA(DMA_Stream_TypeDef* DMA_Streamx)
{
	DMA_Cmd(DMA_Streamx,DISABLE);//�ر���
	while(DMA_GetCmdStatus(DMA_Streamx)!=DISABLE){}//���Ĵ�����ENλ��ȷ��û�б������������ʹ�� DMA���Ա�����
	DMA_SetCurrDataCounter(DMA_Streamx,dma_NumToSend);//ÿ�ε��ö��������ô��䳤��
	DMA_Cmd(DMA_Streamx,ENABLE);
}


/*
if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������

DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//���DMA2_Steam7������ɱ�־

DMA_GetCurrDataCounter(DMA2_Stream7);//�õ���ǰ��ʣ����ٸ�����

*/

