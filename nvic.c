#include "nvic.h"

/*�ж���������������ļ�*/

/*NVIC:Ƕ�������жϿ�����
NVIC���ж�������Ϊ5���飺
��	AIRCR[10��8]	IP bit[7��4]�������				������
0			111							0��4									0λ��ռ���ȼ���4λ��Ӧ���ȼ�
1			110							1��3									1λ��ռ���ȼ���3λ��Ӧ���ȼ�
2			101							2��2									2λ��ռ���ȼ���2λ��Ӧ���ȼ�
3			100							3��1									3λ��ռ���ȼ���1λ��Ӧ���ȼ�
4			011							4��0									4λ��ռ���ȼ���0λ��Ӧ���ȼ�
*/

/*
�����ȼ�����ռ���ȼ��ǿ��Դ�����ڽ��еĵ���ռ���ȼ��жϵġ�
��ռ���ȼ���ͬ���жϣ�����Ӧ���ȼ������Դ�ϵ���Ӧ���ȼ����жϡ�
��ռ���ȼ���ͬ���жϣ��������ж�ͬʱ����������£��ĸ���Ӧ���ȼ��ߣ��ĸ���ִ�С�
��������жϵ���ռ���ȼ�����Ӧ���ȼ�����һ���Ļ������ĸ��ж��ȷ�������ִ�У�
*/

//�����ж�����
void initNVIC(uint8_t IRQChannel,uint8_t PreemptionPriority,uint8_t SubPriority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=PreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=SubPriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}







