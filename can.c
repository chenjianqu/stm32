#include "can.h"
#include "gpio.h"

#define CAN_CURRENT_FIFO CAN_FilterFIFO0

/*
F407��CAN���ţ�
CAN1_RX:PA11/PB8/PD0 CAN1_TX:PA12/PB9/PD1
CAN2_RX:PB12/PB5     CAN1_TX:PB13/PB6
*/

/*CANͨ�ŵ�ISO11898��׼�����������:
CAN ����������CAN_L��CAN_H�ϵĵ�λ�����ж����ߵ�ƽ�����ߵ�ƽ��Ϊ���Ե�ƽ�����Ե�ƽ�����߱ؾ���һ�����ͷ�ͨ��ʹ���ߵ�ƽ�����仯������Ϣ���͸����շ���
���Ե�ƽ��Ӧ�߼���0  CAN_H��CAN_L֮��Ϊ2V���ҡ�
���Ե�ƽ��Ӧ�߼���1  CAN_H��CAN_L֮��Ϊ0V��
���Ե�ƽ��������Ȩ��ֻҪ��һ����Ԫ������Ե�ƽ�������ϼ�Ϊ���Ե�ƽ�������ε�ƽ����а��ݵ���ζ��ֻ�����еĵ�Ԫ��������Ե�ƽ�������ϲ�Ϊ���Ե�ƽ�����Ե�ƽ�����Ե�ƽ��ǿ����
���⣬��CAN���ߵ���ֹ�˶���һ��120�����ն˵��裬�����迹ƥ�䣬�Լ��ٻز����䡣
*/

/*CANͨ����������5�����͵�֡���еģ�
֡����	֡��;
����֡	���ڷ��͵�Ԫ����յ�Ԫ�������ݵ�֡
ң��֡	���ڽ��յ�Ԫ�������ͬ ID �ķ��͵�Ԫ�������ݵ�֡
����֡	���ڵ���������ʱ��������Ԫ֪ͨ�����֡
����֡	���ڽ��յ�Ԫ֪ͨ����δ���ý���׼����֡
���֡	���ڽ�����֡��ң��֡��ǰ���֡���뿪����֡
���У�����֡��ң��֡�б�׼��ʽ����չ��ʽ���ָ�ʽ����׼��ʽ��11 ��λ�ı�ʶ����ID������չ��ʽ��29 ��λ��ID 
*/
/*����֡��7������ɣ�
�٣�֡��ʼ����ʾ����֡��ʼ�ĶΡ�
�ڣ��ٲöΡ���ʾ��֡���ȼ��ĶΡ�
�ۣ����ƶΡ���ʾ���ݵ��ֽ���������λ�ĶΡ�
�ܣ����ݶΡ����ݵ����ݣ�һ֡�ɷ���0~8���ֽڵ����ݡ�
�ݣ�CRC�Ρ����֡�Ĵ������ĶΡ�
�ޣ�ACK�Ρ���ʾȷ���������յĶΡ�
�ߣ�֡��������ʾ����֡�����ĶΡ�
*/
/*
1��֡��ʼ����׼֡����չ֡������1��λ�����Ե�ƽ��ʾ֡��ʼ��
2���ٲöΡ���ʾ�������ȼ��ĶΣ���׼֡����չ֡��ʽ�ڱ�����������
	��׼��ʽ���ٲö��ɻ���ID��RTRλ��ɣ���չ��ʽ�ٲö��ɻ���ID��SRR��IDE����չID��RTRλ���
	ID����λ��ǰ����λ�ں󡣻���ID����ֹ��7λ��Ϊ���ԣ������ܣ�ID=1111111XXXX��
	RTR��Զ������λ��0������֡��1�� Զ��֡�����ж���Զ��֡��������֡
	SRR�����Զ������λ������Ϊ1�����Ե�ƽ����
	IDE����ʶ��ѡ��λ��0����׼��ʶ����1����չ��ʶ����
3�����ƶΡ���6��λ���ɣ���ʾ���ݶε��ֽ�������׼֡����չ֡�Ŀ��ƶ����в�ͬ
	��׼��ʽ�Ŀ��ƶ��ɻ���IDE��r0��DLCλ��ɣ���չ��ʽ���ƶ���r0��r1��DLCλ���
	r0��r1������λ�����������ֵ�ƽ���ͣ����ǽ��տ��������Ե�ƽ��
	DLC�����ݳ����롣0~8����ʾ����/���յ����ݳ��ȣ��ֽڣ���
	IDE����ʶ��ѡ��λ��0����׼��ʶ����1����չ��ʶ����
4�����ݶΡ��öοɰ���0~8���ֽڵ����ݣ������λ��MSB����ʼ�������׼֡����չ֡������εĸ�ʽ��ȫһ��
5��CRC�Ρ��ö����ڼ��֡���������15��λ��CRC˳���1��λ��CRC�綨�������ڷָ���λ����ɣ���׼֡����չ֡������εĸ�ʽҲ����ͬ��
	CRC��ֵ���㷶Χ������֡��ʼ���ٲöΡ����ƶΡ����ݶΡ����շ���ͬ�����㷨���� CRC ֵ�����бȽϣ���һ��ʱ��ͨ������ 
6��ACK�Ρ��˶�����ȷ���Ƿ��������ա���ACK��(ACK Slot)��ACK�綨��2��λ��ɡ���׼֡����չ֡������εĸ�ʽҲ����ͬ�ģ�
	���͵�ԪACK�Σ�����2������λ��
	���յ�ԪACK�Σ����յ���ȷ��Ϣ�ĵ�Ԫ����ACK�۷�������λ��֪ͨ���͵�Ԫ���������ս�������֮Ϊ����ACK/����ACK��
	ע�⣺���� ACK ���ǼȲ��������߹ر�̬Ҳ����������̬�����н��յ�Ԫ�У����յ�������Ϣ�ĵ�Ԫ�����͵�Ԫ������ACK����������Ϣ��ָ�����������󡢸�ʽ����CRC �������Ϣ��
7��֡��������7��λ������λ��ɡ���׼֡����չ֡������θ�ʽ��ȫһ����
*/

/*�����ٲã�
1�����߿���ʱ�����ȷ��͵ĵ�Ԫ��÷�������Ȩ��һ�����ͣ�������Ԫ�޷���ռ��
2������ж����Ԫͬʱ���ͣ�������������Ե�ƽ��ĵ�Ԫ�����нϸ����ȼ�����ID��ʼ�Ƚϣ����ID��ͬ�������ܻ�Ƚ�RTR��SRR��λ��
*/

/*λ���ʣ�
��Щ�����ɿɳ�Ϊ Time Quantum�����³�ΪTq������Сʱ�䵥λ���ɡ�
1 λ��Ϊ4 ���Σ�ÿ�����������ɸ�Tq ���ɣ����Ϊλʱ��
λʱ��=1/�����ʣ���ˣ�֪��λʱ�䣬���ǾͿ���֪�������ʡ�
1 λ�ɶ��ٸ�Tq ���ɡ�ÿ�������ɶ��ٸ�Tq ���ɵȣ����������趨λʱ��ͨ���趨λʱ�򣬶����Ԫ��ͬʱ������Ҳ�������趨�����㡣
ͬ���Σ�SS��
����ʱ��Σ�PTS��
��λ�����1��PBS1��
��λ�����2��PBS2��
λ���ʡ��ɷ��͵�Ԫ�ڷ�ͬ��������·��͵�ÿ���ӵ�λ����Ϊλ���ʡ�һ��λһ����Է�Ϊ�����ĶΣ�
*/

/*STM32�Դ��˻�����չCAN���裬�ֳ�bxCAN��bxCAN���ص����£�
֧��CANЭ��2.0A��2.0B����ģʽ
��������ߴ�1Mbps
֧��ʱ�䴥��ͨ��
����3����������
����3����ȵ�2������FIFO
�ɱ��ɸѡ���飨Ҳ�ƹ������飬���28����*/

/*stm32f407������can,����CAN�ֱ�ӵ���Լ��ķ�������ͽ���FIFO���������ǹ���28��ɸѡ��*/
/*ģʽ
����ģʽ
       �ٳ�ʼ��ģʽ��INRQ=1��SLEEP=0��
       ������ģʽ��INRQ=0��SLEEP=0��
       ��˯��ģʽ��SLEEP=1��

����ģʽ
       �پ�Ĭģʽ�� LBKM=0��SILM=1 ��
       �ڻ���ģʽ�� LBKM=1��SILM=0 ��
       �ۻ��ؾ�Ĭģʽ��LBKM=1��SILM=1��

����ģʽ
*/
/*stm32ɸѡ��
CAN�ı�ʶ������ʾĿ�ĵ�ַ���Ǳ�ʾ�������ȼ������սڵ���ݱ�ʶ����ֵ���������Ƿ���ն�Ӧ��Ϣ��
STM32 CAN���������ṩ��28�������õ�ɸѡ���飨F1�������Ͳ���28����������ֻ��14�������ɽ���CPU����CANͨ�ŵĿ�����
STM32 CAN������ÿ��ɸѡ������2��32λ�Ĵ�����ɣ�CAN_FxR1��CAN_FxR2��x=0~27��������λ��ͬ��ÿ��ɸѡ������ṩ��
       �� 1��32λɸѡ����������STDID[10:0]��EXTID[17:0]��IDE��RTRλ
       �� 2��16λɸѡ����������STDID[10:0]��IDE��RTR��EXTID[17:15]λ
ɸѡ��������Ϊ������λģʽ�ͱ�ʶ���б�ģʽ��������λģʽ�£���ʶ���Ĵ��������μĴ���һ��ָ�����ı�ʶ�����κ�һλ��Ӧ�ð��ա�����ƥ�䡱�򡰲��ù��ġ�����
���ڱ�ʶ���б�ģʽ�£����μĴ���Ҳ��������ʶ���Ĵ����á���ˣ����ǲ���һ����ʶ����һ������λ�ķ�ʽ������ʹ��2����ʶ���Ĵ��������ձ��ı�ʶ����ÿһλ�������ɸѡ����ʶ����ͬ��

Ϊ�˹��˳�һ���ʶ����Ӧ������ɸѡ���鹤��������λģʽ��
Ϊ�˹��˳�һ����ʶ����Ӧ�����ù������鹤���ڱ�ʶ���б�ģʽ��
Ӧ�ó����õ�ɸѡ���飬Ӧ�ñ����ڽ���״̬(ͨ��CAN_FA1R����)��
ɸѡ�����е�ÿ��ɸѡ�����������Ϊ(����ɸѡ�����)��0��ʼ����ĳ�������ֵ��ȡ����ɸѡ�����ģʽ��λ������á�
ͨ��CAN_FFA1R�����ã����Խ�ɸѡ���������FIFO0/FIFO1
        ��������ɸѡ����0�����ڣ�1��32λɸѡ��-��ʶ������ģʽ��Ȼ������CAN_F0R1=0XFFFF0000��CAN_F0R2=0XFF00FF00�����д�ŵ�CAN_F0R1��ֵ���������յ���ID������STID+EXTID+IDE+RTR����
				���ǣ�0XFFFF0000����0XFF00FF00��������������Ҫ������ĵ�ID����ʾ�յ���ӳ����λ[31:24]��λ[15:8]��16��λ�ı����CAN_F0R1�ж�Ӧ��λһģһ�����������16��λ�򲻹��ģ�����һ����
				Ҳ���Բ�һ��������Ϊ����ȷ��ID�����յ���ӳ�������0XFFxx00xx����������ȷ�ģ�x��ʾ�����ģ���

*/
/*
CAN��������Ϊ��
����ѡ��1�����õ����䣨TME=1��
->���ñ�ʶ����ID�������ݳ��Ⱥͷ�������
->����CAN_TIxR��TXRQλΪ1��������
->����Һţ��ȴ���Ϊ������ȼ���
->Ԥ�����ͣ��ȴ����߿��У�
->����
->������á�

CAN��������Ϊ��
FIFO��->
�յ���Ч����->
�Һ�_1������FIFO��һ�����䣬�����Ӳ�����ƣ����ǲ���Ҫ��ᣩ
->�յ���Ч����
->�Һ�_2
->�յ���Ч����
->�Һ�_3
->�յ���Ч����
->�����
*/
/*�������̣�
������������ŵĸ��ù��ܣ�ʹ��CANʱ�ӡ�
        Ҫ��CAN����Ҫʹ��CAN��ʱ�ӣ�CAN��ʱ��ͨ��APB1ENR�ĵ�25λ�����á����Ҫ����CAN���������Ϊ�������������������Ҫ����PA11Ϊ�������루CAN_RX���ţ�PA12Ϊ���������CAN_TX���ţ�����ʹ��PA�ڵ�ʱ��
������CAN����ģʽ�������ʵȡ�
        ͨ��������CAN_MCR�Ĵ�����INRQλ����CAN�����ʼ��ģʽ��Ȼ������CAN_MCR��������ؿ���λ����ͨ��CAN_BTR���ò����ʺ͹���ģʽ������ģʽ/����ģʽ������Ϣ�� �������INRQΪ0���˳���ʼ��ģʽ��
�������˲�����
        �����̣����ǽ�ʹ��ɸѡ����0����������32λ��ʶ������λģʽ�¡�������CAN_FMR��FINITλ�������ʼ��ģʽ��Ȼ������ɸѡ����0�Ĺ���ģʽ�Լ���ʶ��ID������λ����󼤻�ɸѡ�������˳���ʼ��ģʽ��

*/

/**************************************CAN��ʼ������*****************************************/
u8 initCAN(CAN_TypeDef* CANx,GPIO_TypeDef* GPIOx_H,uint16_t GPIO_Pin_H,GPIO_TypeDef* GPIOx_L,uint16_t GPIO_Pin_L,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	if(CANx==CAN1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
		initGPIO(GPIOx_H,GPIO_Pin_H,GPIO_Mode_AF,GPIO_AF_CAN1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
		initGPIO(GPIOx_L,GPIO_Pin_L,GPIO_Mode_AF,GPIO_AF_CAN1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	}
	else if(CANx==CAN2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2,ENABLE);
		initGPIO(GPIOx_H,GPIO_Pin_H,GPIO_Mode_AF,GPIO_AF_CAN2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
		initGPIO(GPIOx_L,GPIO_Pin_L,GPIO_Mode_AF,GPIO_AF_CAN2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	}
	
/*
tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
������Ϊ:42M/((6+7+1)*6)=500Kbps
����ֵ:0,��ʼ��OK;
����,��ʼ��ʧ��; 
*/
	
	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;//��ʱ�䴥��ͨ��ģʽ
	CAN_InitStructure.CAN_ABOM=DISABLE;//����Զ����߹���
	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������
	CAN_InitStructure.CAN_NART=ENABLE;//��ֹ�����Զ�����
	CAN_InitStructure.CAN_RFLM=DISABLE;//���Ĳ��������µĸ��Ǿɵ�
	CAN_InitStructure.CAN_TXFP=DISABLE;//���ȼ��ɱ��ı�ʶ������
	CAN_InitStructure.CAN_Mode=mode;//ģʽ����,����Ϊ��ͨ��˯�ߡ����ء�����˯��ģʽ
	//���ò����ʣ�������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
	CAN_InitStructure.CAN_SJW=tsjw;//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ����ΧΪ��CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1;//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;//��Ƶϵ����brp+1	
	CAN_Init(CANx,&CAN_InitStructure);
	
	//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber=0;//������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32λ
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;//32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32ΪMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_CURRENT_FIFO;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0
	CAN_FilterInit(&CAN_FilterInitStructure);
	
#if CAN_RX0_INT_ENABLE
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);
	initNVIC(CAN1_RX0_IRQn,1,0);
#endif
	return 0;
}

/**************************************�жϷ�����*****************************************/
#if CAN1_RX0_INT_ENABLE	//ʹ��RX0�ж�
void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
	int i=0;
  
	CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif



//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
/**************************************can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)*****************************************/
u8 sendCANMessage(CAN_TypeDef* CANx,u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
	
  CanTxMsg TxMessage;
  TxMessage.StdId=0x12;//��׼��ʶ��Ϊ0
  TxMessage.ExtId=0x12;//������չ��ʾ����29λ�������¾�����Ϊ��׼֡ʱ �þ�û��
  TxMessage.IDE=CAN_Id_Standard;//����֡���ͣ��ڴ�����Ϊ��׼��ʶ��
  TxMessage.RTR=CAN_RTR_Data;//����Զ������λ���ڴ���Ϣ��������ΪΪ����֡��һ֡8λ
  TxMessage.DLC=len;//Ҫ���͵����ݳ���
  
	for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];//�Ѳ������ݸ���CanTxMsg
  
	mbox= CAN_Transmit(CANx, &TxMessage);//��ѯ��������Ȼ����Message,�������֮�󷵻ط��͵�����
  i=0;
	
  while((CAN_TransmitStatus(CANx,mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))//��һ��ʱ����һֱ����ʧ��
		i++;
	
  if(i>=0XFFF)//�򷵻�1
		return 1;
	
  return 0;		
}


//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//����,���յ����ݳ���;
/**************************************can�ڽ������ݲ�ѯ*****************************************/
u8 recerveCANMessage(CAN_TypeDef* CANx,u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
	
  if( CAN_MessagePending(CANx,CAN_CURRENT_FIFO)==0)return 0;//��ѯFIFO0����������ѯ��û�н��յ�����,ֱ���˳� 
	
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
	
  for(i=0;i<RxMessage.DLC;i++)//DLCΪ����֡���ƶε����ݳ�����
    buf[i]=RxMessage.Data[i];  
	
	return RxMessage.DLC;//���ؽ��յ������ݳ���
}

