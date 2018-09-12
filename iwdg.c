#include "iwdg.h"
#include "stm32f4xx_it.h"
/**����������������
�������Ź���ʱ������LSI(�����ڲ�ʱ��,32kHz)����Ԥ��Ƶ��(Ԥ��Ƶ�Ĵ���IWDG_PR����)����ݼ�������
���ֵ�Ĵ�����IWDG_KR����д��0xCCCC�����ö������Ź�
��ʱ��������Ӹ�λֵ0xfff�ݼ�������0ʱ������λ�ź�
���ۺ�ʱ���ֵ�Ĵ�����д��0xAAAA(ι��)���Զ���װ�ؼĴ���(IWDG_RLR)��ֵ������¼��ص����������Ӷ����⿴�Ź���λ��
**/

//��ʼ���������Ź�
//prer:��Ƶ��:0~7(ֻ�е�3λ��Ч!)
//rlr:�Զ���װ��ֵ,0~0XFFF.
//��Ƶ����=4*2^prer.�����ֵֻ����256!
//rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).


void initIWDG(u8 prer,u16 rlr)
{
	//Ԥ��Ƶ�Ĵ�������װ�ؼĴ�������д�������ܣ�Ҫ������ȡ��д�������ܣ���IWDG_KR��д��0x55555��
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
	
	IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��

	IWDG_SetReload(rlr);   //����IWDGװ��ֵ

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //ʹ�ܿ��Ź�
}

//ι�������Ź�
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}
