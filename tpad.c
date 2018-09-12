#include "tpad.h"


#define TPAD_ARR_MAX_VAL  0XFFFFFFFF	//����ARRֵ(TIM2��32λ��ʱ��)


vu16 TPAD_DEFAULT_VAL=0;				//���ص�ʱ��(û���ְ���),��������Ҫ��ʱ��



/******************************��ʼ��������******************************/
u8 initTPAD(void)
{
	

	initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_TIM2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);
	
	initTIMBase(TIM2,8,TIM_CounterMode_Up,0XFFFFFFFF,TIM_CKD_DIV1,0);
	
	TIM_ICInitTypeDef  TIM2_ICInitStructure;
	//��ʼ��ͨ��1
  TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TIM2��
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC2F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM2, &TIM2_ICInitStructure);//��ʼ��TIM2 IC1
	TIM_Cmd(TIM2,ENABLE ); 	//ʹ�ܶ�ʱ��2
	
	
	
	
	u16 buf[10];
	u16 temp;
	u8 j,i;
	
	for(i=0;i<10;i++)//������ȡ10��
	{				 
		buf[i]=TPAD_getChargeTime();
		delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//����
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//ȡ�м��8�����ݽ���ƽ��
	TPAD_DEFAULT_VAL=temp/8;
	if(TPAD_DEFAULT_VAL>TPAD_ARR_MAX_VAL/2)
		return 1;//��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ,������!
	return 0;		
}






//�����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
//����ֵ������ֵ/����ֵ����ʱ������·��أ�
/******************************�õ����ݳ��ʱ��******************************/

u16 TPAD_getChargeTime(void)
{				   
	TPAD_Reset();//���õ���
	while(TIM_GetFlagStatus(TIM2, TIM_IT_CC1) == RESET)//�ȴ�����������
	{
		if(TIM_GetCounter(TIM2)>TPAD_ARR_MAX_VAL-500)
			return TIM_GetCounter(TIM2);//��ʱ��,ֱ�ӷ���CNT��ֵ
	};	
	return TIM_GetCapture1(TIM2);	 
} 	




//n��������ȡ�Ĵ���
//����ֵ��n�ζ������������������ֵ
/*************************��ȡn��,ȡ���ֵ***************************/
u16 TPAD_getMaxVal(u8 n)
{
	u16 temp=0;
	u16 res=0; 
	while(n--)
	{
		temp=TPAD_getChargeTime();//�õ�һ��ֵ
		if(temp>res)res=temp;
	};
	return res;
} 




/*************************���õ��ݵ������ȵ͵�ƽ�ŵ��ٸ��ճ��***************************/

void TPAD_Reset(void)
{		
	initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);//���0,�ŵ�
	delay_ms(5);
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־
	TIM_SetCounter(TIM2,0);		//��ʱ����0
	//����ԭ��IO���գ����ݳ��
	initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_TIM2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);	
}




//mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
//����ֵ:0,û�а���;1,�а���;										  
#define TPAD_GATE_VAL 	100	//����������ֵ,Ҳ���Ǳ������tpad_default_val+TPAD_GATE_VAL,����Ϊ����Ч����.

/***************************************ɨ�败������**************************************/

u8 TPAD_Scan(u8 mode)
{
	static u8 keyen=0;	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	u8 res=0;
	u8 sample=3;		//Ĭ�ϲ�������Ϊ3��	 
	u16 rval;
	if(mode)
	{
		sample=6;	//֧��������ʱ�����ò�������Ϊ6��
		keyen=0;	//֧������	  
	}
	rval=TPAD_getMaxVal(sample); 
	if(rval>(TPAD_DEFAULT_VAL+TPAD_GATE_VAL)&&rval<(10*TPAD_DEFAULT_VAL))//����tpad_default_val+TPAD_GATE_VAL,��С��10��tpad_default_val,����Ч
	{							 
		if((keyen==0)&&(rval>(TPAD_DEFAULT_VAL+TPAD_GATE_VAL)))	//����tpad_default_val+TPAD_GATE_VAL,��Ч
		{
			res=1;
		}	   
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 

