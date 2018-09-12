#include "irremote.h"

/*����ң�صı���Ŀǰ�㷺ʹ�õ��ǣ�NEC Protocol ��PWM(�����ȵ���)��Philips
RC-5 Protocol ��PPM(����λ�õ���)*/

/*
NEC˵��:
1.8λ��ַ��8λָ��ȣ�
2.��ַ������2�δ��䣨ȷ���ɿ��ԣ�
3.PWM����λ�õ��ƣ��Է�������ز���ռ�ձȴ���0���͡�1����
4.�ز�Ƶ��Ϊ38Khz��
5.λʱ��Ϊ1.125ms��2.25ms���ߵ�ƽ����ʱ�������֣���

NEC���λ���壺һ�������Ӧ560us�������ز���һ���߼�1������Ҫ2.25ms��560us����+1680us�͵�ƽ����һ���߼�0�Ĵ�����Ҫ1.125ms��560us����+560us�͵�ƽ����
��ң�ؽ���ͷ���յ������ʱ��Ϊ�͵�ƽ����û�������ʱ��Ϊ�ߵ�ƽ�������������ڽ���ͷ���յ����ź�Ϊ���߼�1Ӧ����560us��+1680us�ߣ��߼�0Ӧ����560us��+560us�ߡ�

���ݸ�ʽ��
NECң��ָ������ݸ�ʽΪ��ͬ����ͷ+��ַ��+��ַ����+������+���Ʒ��롣
ͬ������һ��9ms�ĵ͵�ƽ��һ��4.5ms�ĸߵ�ƽ��ɣ���ַ�롢��ַ���롢�����롢���Ʒ������8λ���ݸ�ʽ��
���յ�λ��ǰ����λ�ں��˳���͡����÷�����Ϊ�����Ӵ���Ŀɿ��ԣ�������У�飩��

�����룺��9ms�͵�ƽ+2.5m�ߵ�ƽ+0.56ms�͵�ƽ+97.94ms�ߵ�ƽ��ɣ������һ֡���ݷ������֮�󣬰�����Ȼû�зſ��������ظ��룬
�������룬����ͨ��ͳ��������Ĵ�������ǰ������µĳ���/������
*/

/*
����˼·��
������ʱ����Ӧͨ�����벶���ܣ�Ĭ�������ز��񡣶�ʱ���ļ���Ƶ��Ϊ1MHz,�Զ�װ��ֵΪ10000��Ҳ�������ʱ��10ms��
������ʱ�����벶������жϺͲ����жϡ������������ز��������жϣ�����ʱ��������������������жϡ�

����ߵ�ƽʱ�䣺�����������ص�ʱ�����ò�����Ϊ�½��ز���Ϊ�´β����½�����׼������Ȼ����ն�ʱ����ͬʱ���ñ���RmtSta��λ4ֵΪ1������Ѿ����������ء������½��ص�ʱ��
��ȡ��ʱ����ֵ��ֵ������Dval��Ȼ�����ò�����Ϊ�����ز���Ϊ�´β�����������׼������ͬʱ�Ա���RmtSta��λ4�����жϣ����RmtStaλ4λ1��˵��֮ǰ�Ѿ����񵽹������أ���ô�Բ���ֵ
Dval�����жϡ�

300-800֮�䣬˵�����յ���������0,
1400-1800֮�� ˵�����յ�������Ϊ1,
2200-2600��˵���������룬
4200-4700˵��Ϊͬ���롣
������������Ӧ�ı�־λ������Ƕ�ʱ����������жϣ���ô���������֮ǰ���յ���ͬ���룬�����ǵ�һ�������������һ�ΰ�����Ϣ�ɼ���
*/


#define IR_GPIOX GPIOA
#define IR_GPIO_PIN GPIO_Pin_8

/*******************************��ʼ��IRremote****************************************/
void initIRremote(void)
{
  TIM_ICInitTypeDef  TIM1_ICInitStructure;
	
  //GPIOA8  ���ù���,����
	initGPIO(IR_GPIOX,IR_GPIO_PIN,GPIO_Mode_AF,GPIO_AF_TIM1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//��ʼ��TIMBase
	initTIMBase(TIM1,167,TIM_CounterMode_Up,9999,TIM_CKD_DIV1,0);
	
	TIM_Cmd(TIM1,ENABLE );
	
 	//��ʼ��TIM1���벶�����
	TIM1_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM1_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM1, &TIM1_ICInitStructure);
	 
	initNVIC(TIM1_CC_IRQn,			1,3);
	initNVIC(TIM1_UP_TIM10_IRQn,1,2);
	
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������жϣ�������ʱ�����ʱ��������CC1�����ж�
}






u8 recflag=0;//�Ƿ���յ�ͬ����
u8 upflag=0;//�Ƿ���յ�������
u8 validflag=0;//�Ѿ���ð�����Ϣ��־

u8 overflowCnt=0;//��ʱ��������� 

u16 pulseWidth=0;//������
u32 value;//���յ���ֵ

u8  RmtCnt=0;	//�������µĴ���	 


/*******************************��ʱ��1�ж�****************************************/
void TIM1_UP_TIM10_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET) //��ʱ��1����жϣ�������δ�յ����ݣ�Ҳ�����ǽ���������֮�������
	{
		if(recflag)//�Ѿ����յ�ͬ���룬���Ѿ����������ݣ���ʱ�ӽ��յ�ͬ���� �Ѿ���ȥ��10ms ��
		{	
			upflag=0;//ȡ���������Ѿ���������
			if(overflowCnt==0)
				validflag=1;//��־��Ϣ��ȡ���
			
			if(overflowCnt<14)//��ʱ�������������14 ���Ѿ���ȥ��140ms 
				overflowCnt++;
			else
			{
				recflag=0;//���������ʶ
				overflowCnt=0;	//��ռ�����	
			}
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
} 






/*******************************��ʱ��1���벶���жϷ������	****************************************/
void TIM1_CC_IRQHandler(void)
{ 		    	 
	if(TIM_GetITStatus(TIM1,TIM_IT_CC1)==SET) //������(CC1IE)�ж�
		{
		if(GPIO_ReadInputDataBit(IR_GPIOX,IR_GPIO_PIN))//�����ز���
		{
			TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			TIM_SetCounter(TIM1,0);//��ն�ʱ��
			upflag=1;	//����������Ѿ�������	
		}
		else //�½��ز���
		{
			pulseWidth=TIM_GetCapture1(TIM1);//��ȡ��������ֵ������ȡ������
			TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Rising); //CC1P=0	����Ϊ�����ز���
			
			if(upflag)//�����һ�θߵ�ƽ���� 
			{
 				if(recflag)//���յ���ͬ����
				{
					if(pulseWidth>300&&pulseWidth<800)			//560Ϊ��׼ֵ,560us  ���յ�0	 
					{
						value<<=1;	//����һλ.
						value|=0;	  
					}else if(pulseWidth>1400&&pulseWidth<1800)	//1680Ϊ��׼ֵ,1680us ���յ�1
					{
						value<<=1;	//����һλ.
						value|=1;
					}else if(pulseWidth>2200&&pulseWidth<2600)	//���յ�������
					{
						RmtCnt++; 		//������������1��
						overflowCnt=0;	//��ռ�ʱ��		
					}
				}
				
				else if(pulseWidth>4200&&pulseWidth<4700)//4500Ϊ��׼ֵ4.5ms
				{
					recflag=1;	//��ǳɹ����յ���ͬ����
					RmtCnt=0;		//�����������������
				}
			}
			
			//��������ز����־
			upflag=0;
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC1);  //����жϱ�־λ 
}






//���ص�ַ��
/*******************************����ɨ��****************************************/

u8 scanRemote(void)
{        
	//value ��ַ��+��ַ����+������+���Ʒ���
	u8 result=0;       
  u8 t1,t2;  
	if(validflag)//���յ�������
	{ 
	    t1=value>>24;//�õ���ַ��
	    t2=(value>>16)&0xff;//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ ,ÿ��ң������id��һ��
	    { 
	        t1=value>>8;
	        t2=value; 	
	        if(t1==(u8)~t2)
						result=t1;//��������ڿ��Ʒ���
			}   
			if((result==0)||(recflag==0))//�������ݴ���/ң���Ѿ�û�а�����
			{
				validflag=0;//������յ���Ч������ʶ
				RmtCnt=0;		//�����������������
			}
	}  
  return result;
}


