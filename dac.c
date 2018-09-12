#include "dac.h"

/*
DAC将数字量转换为模拟电压量输出

DAC通道1的引脚：A4
DAC通道2的引脚：A5
*/


/*****************DAC通道输出初始化**************************/

void initDAC(uint32_t DAC_Channel_x)
{ 
	DAC_InitTypeDef DAC_InitType;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//使能DAC时钟
	
	if(DAC_Channel_x==DAC_Channel_1)
		initGPIO(GPIOA,GPIO_Pin_4,GPIO_Mode_AN,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	else if(DAC_Channel_x==DAC_Channel_2)
		initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_AN,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//不使用触发功能 TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1输出缓存关闭 BOFF1=1
	DAC_Init(DAC_Channel_x,&DAC_InitType);	 //初始化DAC通道

	DAC_Cmd(DAC_Channel_x, ENABLE);  //使能DAC通道
  
  DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
}



/*****************设置通道1输出电压**************************/

//vol:0~3300,代表0~3.3V
void setDAC_Vol(u32 DAC_Channel_x,u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	
	if(DAC_Channel_x==DAC_Channel_1)
		DAC_SetChannel1Data(DAC_Align_12b_R,temp);
	else if(DAC_Channel_x==DAC_Channel_2)
		DAC_SetChannel2Data(DAC_Align_12b_R,temp);//12位右对齐数据格式设置DAC值
}

