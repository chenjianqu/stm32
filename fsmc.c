#include "fsmc.h"



/*
stm32f4��FSMC֧��8��16��32λ���ݿ�ȣ�ͬʱ�����ⲿ�洢������Ϊ�̶���СΪ256M�ֽڵ��ĸ��洢��(Bank)

��Bank 1Ϊ����ÿ�����Ϊ4������ÿ��������64M�ֽڿռ䣬ÿ�������ж����ļĴ����������ӵĵļĴ����������á�Bank 1��256M�ֽڿռ���28(256M=2^28)����ַ��Ѱַ��
��28����ַ�����ڲ�AHB��ַ���ߣ�HADDR����HADDR��25:0�������ⲿ�洢����ַFSMC_A[25��0]����HADDR��2:6:27����4����Ѱַ.

Ҫע����ǣ�
�� Bank1 �ӵ��� 8  λ��ȴ洢����ʱ��HADDR[25:0]->FSMC_A[25:0]��
�� Bank1 �ӵ��� 16 λ��ȴ洢����ʱ��HADDR[25:1]->FSMC_A[24:0]��
������ΪHADDR��ַ��������8���ֽ�Ϊ��λѰַ�ģ����ⲿ�Ĵ���ÿ����ַ��16λ,Ҫ�����ǵĵ�ַ��ƥ��Ļ����ͱ����HADDR��ַ���ȼ���һ�룬Ҳ��������һλ������û�з���
��Ϊ������£�������Ϊ�൱�ڳ���2�������⣬HADDR[27:26]�����ã��ǲ���Ҫ���Ǹ�Ԥ�ģ����磺����ѡ��ʹ�� Bank1 �ĵ�����������ʹ�� FSMC_NE3 �������ⲿ�豸��ʱ��
����Ӧ�� HADDR[27:26]=10������Ҫ���ľ������ö�Ӧ��3���ļĴ����飬����Ӧ�ⲿ�豸���ɣ���������λ�Ǽ�����õġ�
*/


/*
��ַ����
Bank1:6000 0000H - 6FFF FFFFH
Bank1:7000 0000H - 7FFF FFFFH
Bank1:8000 0000H - 8FFF FFFFH
Bank1:9000 0000H - 9FFF FFFFH

ÿ��Bank�Ŀ飺
��Bank1:
 ��һ��:6000 0000h--63ff ffffh (DATA����Ϊ8λ�����,�ɵ�ַ��FSMC_A[25:0]������DATA����Ϊ16λ����£��ɵ�ַ��FSMC_A[24:0]����)

 �ڶ���:6400 0000h--67ff ffffh

 ������:6800 0000h--6bff ffffh

 ���Ŀ�:6c00 0000h--6fff ffffh

ע�������HADDR����Ҫת�����ⲿ�豸���ڲ�AHB��ַ�ߣ�ÿ����ַ��Ӧһ���ֽڵ�Ԫ����ˣ����ⲿ�豸�ĵ�ַ�����8λ�ģ���HADDR[25:0]��STM32��CPU����FSMC_A[25:0]һһ��Ӧ��
�����Է���64M�ֽڵĿռ䡣���ⲿ�豸�ĵ�ַ�����16λ�ģ�����HADDR[25:1]��STM32��CPU����FSMC_A[24:0]һһ��Ӧ����Ӧ�õ�ʱ�򣬿��Խ�FSMC_A�������ӵ��洢������������ĵ�ַ����������
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
	
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//ʹ��FSMCʱ��  
	
	
	/*
	 FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	//��ʱ��
	readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*16=96ns	���൱��RD�ߵ�ƽ����ʱ��
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
  readWriteTiming.FSMC_DataSetupTime = 60;			//���ݱ���ʱ��Ϊ60��HCLK	=6*60=360ns���൱��RD�͵�ƽ����ʱ��
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
    
	//дʱ��
	writeTiming.FSMC_AddressSetupTime =9;	      //��ַ����ʱ�䣨ADDSET��Ϊ9��HCLK =54ns 
  writeTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨A		
  writeTiming.FSMC_DataSetupTime = 8;		 //���ݱ���ʱ��Ϊ6ns*9��HCLK=54ns
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
*/


	readWriteTiming.FSMC_AddressSetupTime = 0x00;	 //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
  readWriteTiming.FSMC_DataSetupTime = 0x08;		 ////���ݱ���ʱ�䣨DATAST��Ϊ9��HCLK 6*9=54ns	 	 
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00; //��Щλ���ڶ���һ�ζ�����֮���������ϵ��ӳ�(�����������߸���ģʽ��NOR�������)��һ�ζ�����֮���������Ҫ������������Ϊ�´β����ͳ���ַ��
                                        //����ӳپ���Ϊ�˷�ֹ���߳�ͻ�������չ�Ĵ洢��ϵͳ���������߸���ģʽ�Ĵ洢�����������Ĵ洢��������6��HCLKʱ�������ڽ��������߻ָ�������״̬�����������������Ϊ����Сֵ��
  readWriteTiming.FSMC_CLKDivision = 0x00;//����CLKʱ������źŵ����ڣ���HCLK��������ʾ��
  readWriteTiming.FSMC_DataLatency = 0x00;//����ͬ������ģʽ��NOR���棬��Ҫ�����ڶ�ȡ��һ������֮ǰ�ȴ��Ĵ洢��������Ŀ��
                                        //���ʱ�����������HCLK��ʾ������������ʱ��(CLK)��ʾ���ڷ����첽NOR���桢SRAM��ROMʱ����������������á�����CRAMʱ�������������Ϊ0��
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 

	
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//Bank1�ĵ����� ��������ʹ��NE3 ��Ҳ�Ͷ�ӦBTCR[4],[5]��
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //��ַ\�����Ƿ���
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;//�洢�����ͣ�FSMC_MemoryType_SRAM;  
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;//�Ƿ���г���ģʽ���� 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; //�ȴ��ź���Ч����
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   //��λ�����������Ƿ�֧�ְѷǶ����AHB��������ָ��2�����Բ�������λ���ڴ洢���ĳ���ģʽ����Ч��
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  //������洢�����ڳ��鴫��ģʽʱ��NWAIT�ź�ָʾ������洢�������������Ƿ���Ч���Ƿ���Ҫ����ȴ����ڡ�
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//��λָʾFSMC�Ƿ�����/��ֹ�Դ洢����д������//�洢��дʹ�� 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  //������洢�����ڳ��鴫��ģʽʱ����һλ����/��ֹͨ��NWAIT�źŲ���ȴ�״̬��
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; //��λ����FSMCʹ��FSMC_BWTR�Ĵ��������������дʹ�ò�ͬ��ʱ��// ��дʹ����ͬ��ʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  //���ڴ��ڳ��鴫��ģʽ������洢������һλ����/��ֹͨ��NWAIT�źŲ���ȴ�״̬����������ͬ�����鴫��Э��ʹ��λ��FSMC_BCRx�Ĵ�����BURSTENλ��
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //��дͬ��ʱ��

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // ʹ��BANK1����3			
}