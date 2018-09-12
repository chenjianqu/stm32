#include "flash.h"

//��ȡ��ַ���ڵ�����
uint16_t getFlashSector(u32 addr);

/*
STM32��̷�ʽ��
 ���߱�̣�ICP,In-Circuit Programming��:
      ͨ��JTAG/SWDЭ�����ϵͳ���س���(Bootloader)�����û�Ӧ�ó���΢�������С�
�� �ڳ����б��(IAP,In Application Programming):
      ͨ���κ�һ��ͨ�Žӿ�(��IO�˿�,USB,CAN,UART,I2C,SPI��)���س������Ӧ�����ݵ��洢���С�Ҳ����˵��STM32�����û���Ӧ�ó�����������д����洢���е����ݡ�
			Ȼ����IAP��Ҫ������һ���ֳ����Ѿ�ʹ��ICP��ʽ�յ�����洢���У�Bootloader)��
*/

/*
STM32F40x������ģ���ɣ����洢����ϵͳ�洢����OPT�����ѡ���ֽڵ�4������ɡ� 
���洢��:	 �ò���������Ŵ�������ݳ�������const���͵����ݣ�����Ϊ12��������ǰ4������Ϊ16KB��С��Ȼ������4��64KB��С������5~11��128K��С����ͬ������STM32F4��
ӵ�е���������һ�����������ǵ�STM32F407ZGT6����ӵ��ȫ��12������������ͼ���Կ������洢������ʼ��ַ����0X08000000�� B0��B1����GND��ʱ�򣬾��Ǵ�0X08000000��ʼ���д���ġ�

ϵͳ�洢��: �����Ҫ�������STM32F4��bootloader���룬�˴����ǳ�����ʱ��͹̻���STM32F4�����ˣ�ר���������洢�����ش���ġ���B0��V3.3��B1��GND��ʱ�򣬴Ӹô洢��������
�����봮������ģʽ����

OTP����:   ��һ���Կɱ�����򣬹�528�ֽڣ����ֳ��������֣�ǰ��512�ֽڣ�32�ֽ�Ϊ1�飬�ֳ�16�飩�����������洢һЩ�û����ݣ�һ���Եģ�д��һ�Σ���Զ�����Բ�����������
����16�ֽڣ�����������Ӧ�顣

ѡ���ֽ�:	 �������ö�������BOR�������/Ӳ�����Ź��Լ��������ڴ�����ֹͣģʽ�µĸ�λ�� 
*/

/*
��ִ������д����ʱ���κζ�����Ķ�����������ס���ߣ���д������ɺ������������ȷ�ؽ��У����ڽ���д���������ʱ�����ܽ��д�������ݵĶ�ȡ������
*/

/*STM32�Ķ�����:
Ϊ��׼ȷ��ȡ Flash ���ݣ�������� CPU ʱ�� (HCLK) Ƶ�ʺ�������Դ��ѹ�� Flash ��ȡ���ƼĴ��� (FLASH_ACR) ����ȷ�����õȴ������� (LATENCY)������Դ��ѹ����2.1V ʱ��
����ر�Ԥȡ��������Flash �ȴ�������CPUʱ��Ƶ��֮����ж�Ӧ��ϵ�������
*/
/*
STM32F4��λ��FLASH��̲����Ǳ������ģ�����д��FLASH_CR�Ĵ�����ͨ��д���ض������У�0X45670123��0XCDEF89AB����FLASH_KEYR�Ĵ����ſɽ��д������ֻ����д��������������ǲ��ܲ�����ؼĴ�����
   FLASH_CR�Ľ�������Ϊ��
      1��д0X45670123��KEY1)��FLASH_KEYR
      2��д0XCDEF89AB(KEY2)��FLASH_KEYR
ͨ�����������裬���ɽ���FLASH_CR�����д�������ôFLASH_CR����������ֱ���´θ�λ��ſ����ٴν�����

STM32F4����ı��λ������ͨ��FLASH_CR��PSIZE�ֶ����ã�PSIZE�����ñ���͵�Դ��ѹƥ�䣬�������ǿ������õĵ�ѹ��3.3V������PSIZE��������Ϊ10����32λ����λ�����������߱�̣���������32λΪ�������С� 

STM32F4��FLASH�ڱ�̵�ʱ��Ҳ����Ҫ����д���ַ��FLASH�Ǳ������˵ģ�Ҳ������ֵ������0XFFFFFFFF���������޷�д�롣
*/

/*stm32 flash��̲��裺
���FLASH_SR�е�BSYλ��ȷ����ǰδִ���κ�FLASH������
��FLASH_CR�Ĵ����е�PGλ��1������FLASH��̡�
�������洢����ַ�����洢�����OTP�����ڣ�ִ������д�������
     ������λ��Ϊx8ʱ���ֽ�д�루PSIZE=00��
     ������λ��Ϊx16ʱ������д�루PSIZE=01��
     ������λ��Ϊx32ʱ����д�루PSIZE=02��
     ������λ��Ϊx64ʱ��˫��д�루PSIZE=03��
�� �ȴ�BSYλ���㣬���һ�α�̡�
�����Ĳ��������Ϳ������һ��FLASH��̡������м���Ҫע�⣺1�����ǰ��Ҫȷ��Ҫд���ַ��FLASH�Ѿ�������2��Ҫ�Ƚ����������ܲ���FLASH_CR����3����̲�����OPT����Ҳ��Ч������һģһ����
*/
/*stm32 flash������
STM32F4�����������Ϊ���֣�
��������
��Ƭ������

������������
���FLASH_CR��LOCK�Ƿ���������û�����Ƚ���
���FLASH_SR�Ĵ����е�BSY λ��ȷ����ǰδִ���κ�FLASH����
��FLASH_CR�Ĵ����У���SERλ��1���������洢���12��������ѡ��Ҫ���������� (SNB)
��FLASH_CR�Ĵ����е�STRTλ��1��������������
�ȴ�BSYλ����
���������岽���Ϳ��Բ���ĳ������
*/

/*flash״̬��
�ܻ�ȡ״̬����
��Ҫ���õĺ����ǣ�
       FLASH_Status FLASH_GetStatus(void)��
����ֵ��ͨ��ö�����Ͷ���ģ�
typedef enum
{ 
  FLASH_BUSY = 1,//����æ
  FLASH_ERROR_RD,//����������
  FLASH_ERROR_PGS,//���˳�����
  FLASH_ERROR_PGP,//��̲���λ������
  FLASH_ERROR_PGA,//��̶������
  FLASH_ERROR_WRP,//д��������
  FLASH_ERROR_PROGRAM,//��̴���
  FLASH_ERROR_OPERATION,//��������
  FLASH_COMPLETE//��������
}FLASH_Status;


*/

/***************************************��ȡ��ַ���ڵ�����**************************************/ 
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


/***************************************��ȡָ����ַ����(32λ����)**************************************/ 
//STM32F4����ı��λ������ͨ��FLASH_CR��PSIZE�ֶ����ã�PSIZE�����ñ���͵�Դ��ѹƥ�䣬�������ǿ������õĵ�ѹ��3.3V������PSIZE��������Ϊ10����32λ����λ�����������߱�̣���������32λΪ�������С�
u32 readFlashWord(u32 faddr)
{
	return *(vu32*)faddr;//����ֵ:��Ӧ����.
}



/*�ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������û����Ҫ����,
��������������Ȳ�����,Ȼ����������д.*/

/*�ú�����OTP����Ҳ��Ч!��������дOTP��!OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F*/
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 

/***************************************��ָ����ַ��ʼд��ָ�����ȵ�����**************************************/
void writeFlash(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<ADDR_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ,��ʼ��ַ����Ϊ4�ı���
	FLASH_Unlock();//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
 		
	addrx=WriteAddr;//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
	
	if(addrx<0X1FFF0000)//ֻ�����洢��,����Ҫִ�в�������!!
	{
		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(readFlashWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{
				status=FLASH_EraseSector(getFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V֮��!!
				if(status!=FLASH_COMPLETE)break;//����������
			}else 
				addrx+=4;
		}
	}
	
	while(WriteAddr<endaddr)//д����
	{
		if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)break;//д������,�쳣���˳� 
		
		WriteAddr+=4;
		pBuffer++;
	} 
  FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
} 


//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��

/***************************************��ָ����ַ��ʼ����ָ�����ȵ�����*************************************/
void readFlash(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=readFlashWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}