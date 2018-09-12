#include "dac.h"

/*
DAC��������ת��Ϊģ���ѹ�����

DACͨ��1�����ţ�A4
DACͨ��2�����ţ�A5
*/


/*****************DACͨ�������ʼ��**************************/

void initDAC(uint32_t DAC_Channel_x)
{ 
	DAC_InitTypeDef DAC_InitType;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ��DACʱ��
	
	if(DAC_Channel_x==DAC_Channel_1)
		initGPIO(GPIOA,GPIO_Pin_4,GPIO_Mode_AN,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	else if(DAC_Channel_x==DAC_Channel_2)
		initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_AN,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//��ʹ�ô������� TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�ò��η���
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1�������ر� BOFF1=1
	DAC_Init(DAC_Channel_x,&DAC_InitType);	 //��ʼ��DACͨ��

	DAC_Cmd(DAC_Channel_x, ENABLE);  //ʹ��DACͨ��
  
  DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
}



/*****************����ͨ��1�����ѹ**************************/

//vol:0~3300,����0~3.3V
void setDAC_Vol(u32 DAC_Channel_x,u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	
	if(DAC_Channel_x==DAC_Channel_1)
		DAC_SetChannel1Data(DAC_Align_12b_R,temp);
	else if(DAC_Channel_x==DAC_Channel_2)
		DAC_SetChannel2Data(DAC_Align_12b_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

