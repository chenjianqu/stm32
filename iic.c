#include "iic.h"

/*
I2C(IIC,Inter��Integrated Circuit),����ʽ��������,��PHILIPS��˾������������΢������������Χ�豸����������SDA��ʱ��SCL���ɵĴ������ߣ��ɷ��ͺͽ������ݡ�
��CPU�뱻��IC֮�䡢IC��IC֮�����˫���ͣ�����IIC����һ��ɴ�400kbps���ϡ�IIC�ǰ�˫��ͨ�ŷ�ʽ��
*/

/*ͨ��IO��ģ��IIC����Ϊstm32�Դ���IIC������*/

/*IICЭ�飺����״̬ ��ʼ�ź� ֹͣ�ź� Ӧ���ź� ���ݵ���Ч�� ���ݴ���

����״̬��I2C�������ߵ�SDA��SCL�����ź���ͬʱ���ڸߵ�ƽʱ���涨Ϊ���ߵĿ���״̬����ʱ�����������������ЧӦ�ܾ����ڽ�ֹ״̬�����ͷ����ߣ��������ź��߸��Ե���������ѵ�ƽ���ߡ�
��ʼ�źţ���SCLΪ���ڼ䣬SDA�ɸߵ��͵����䣻�����ź���һ�ֵ�ƽ����ʱ���źţ�������һ����ƽ�źš�

Ӧ���źţ�������ÿ����һ���ֽڣ�����ʱ������9�ڼ��ͷ������ߣ��ɽ���������һ��Ӧ���źš� Ӧ���ź�Ϊ�͵�ƽʱ���涨Ϊ��ЧӦ��λ��ACK���Ӧ��λ������ʾ�������Ѿ��ɹ��ؽ����˸��ֽڣ�
Ӧ���ź�Ϊ�ߵ�ƽʱ���涨Ϊ��Ӧ��λ��NACK����һ���ʾ���������ո��ֽ�û�гɹ���

���ڷ�����ЧӦ��λACK��Ҫ���ǣ��������ڵ�9��ʱ������֮ǰ�ĵ͵�ƽ�ڼ佫SDA�����ͣ�����ȷ���ڸ�ʱ�ӵĸߵ�ƽ�ڼ�Ϊ�ȶ��ĵ͵�ƽ�� ��������������������������յ����һ���ֽں�
����һ��NACK�źţ���֪ͨ���ط������������ݷ��ͣ����ͷ�SDA�ߣ��Ա����ؽ���������һ��ֹͣ�ź�P��
*/


//SDA����
GPIO_TypeDef* iic_GPIOx_SDA;
uint16_t iic_GPIO_Pin_SDA;
u8 iic_pinSDA;//�����л��������ģʽ
//SCL����
GPIO_TypeDef* iic_GPIOx_SCL;
uint16_t iic_GPIO_Pin_SCL;
u8 iic_pinSCL;


/************************************��ʼ��IIC******************************************************/
void initIIC(GPIO_TypeDef* GPIOx_sda,uint16_t GPIO_Pin_sda,GPIO_TypeDef* GPIOx_scl,uint16_t GPIO_Pin_scl) 
{
	iic_GPIOx_SDA=GPIOx_sda;
	iic_GPIO_Pin_SDA=GPIO_Pin_sda;
	iic_pinSDA=turnPinNum(iic_GPIO_Pin_SDA);
	iic_GPIOx_SCL=GPIOx_scl;
	iic_GPIO_Pin_SCL=GPIO_Pin_scl;
	iic_pinSCL=turnPinNum(iic_GPIO_Pin_SCL);
	
	initGPIO(iic_GPIOx_SDA,iic_GPIO_Pin_SDA,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(iic_GPIOx_SCL,iic_GPIO_Pin_SCL,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//��ƽ���ߣ�ʹIIC���ڿ���״̬
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
}


/**************************************����IIC��ʼ�ź�**************************************/

/*��SCLΪ���ڼ䣬SDA�ɸߵ��͵����䣻�����ź���һ�ֵ�ƽ����ʱ���źţ�������һ����ƽ�źš�*/
void IIC_Start(void)
{
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);//sda�����
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(4);
	GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);//�����ź�
	delay_us(4);
	
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  



/***********************************����IICֹͣ�ź�*****************************************/

/*ֹͣ�źţ���SCLΪ���ڼ䣬SDA�ɵ͵��ߵ����䣻ֹͣ�ź�Ҳ��һ�ֵ�ƽ����ʱ���źţ�������һ����ƽ�źš�*/
void IIC_Stop(void)
{
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);//sda�����
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	delay_us(4);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	delay_us(4);
}



/*������ÿ����һ���ֽڣ�����ʱ������9��ǰ��İ˸�ʱ���������ڷ���һ���ֽڣ��ڼ��ͷ������ߣ��ɽ���������һ��Ӧ���źš� 
Ӧ���ź�Ϊ�͵�ƽʱ���涨Ϊ��ЧӦ��λ��ACK���Ӧ��λ������ʾ�������Ѿ��ɹ��ؽ����˸��ֽڣ�
Ӧ���ź�Ϊ�ߵ�ƽʱ���涨Ϊ��Ӧ��λ��NACK����һ���ʾ���������ո��ֽ�û�гɹ���
���ڷ�����ЧӦ��λACK��Ҫ���ǣ��������ڵ�9��ʱ������֮ǰ�ĵ͵�ƽ�ڼ佫SDA�����ͣ�����ȷ���ڸ�ʱ�ӵĸߵ�ƽ�ڼ�Ϊ�ȶ��ĵ͵�ƽ�� 
��������������������������յ����һ���ֽں󣬷���һ��NACK�źţ���֪ͨ���ط������������ݷ��ͣ����ͷ�SDA�ߣ��Ա����ؽ���������һ��ֹͣ�ź�P��
*/

/******************************��Ϊ���ͷ� �ȴ�Ӧ���źŵ���********************************/
u8 IIC_WaitAck(void)
{
	u8 ucErrTime=0;
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_IN);//SDA����Ϊ����  

	//���ͷ��������״̬
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	delay_us(1);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(1);
	
	//���������ݷ�SDAһֱû�յ��͵�ƽ �ҳ���
	while(GPIO_ReadInputDataBit(iic_GPIOx_SDA,iic_GPIO_Pin_SDA))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return ACK_FAIL;
		}
	}
	//���յ��˳ɹ���Ӧ��
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);//ʱ�����0 	   
	return ACK_OK;  
} 


/********************************���շ� ����ACKӦ��*****************************************/
void IIC_Ack(void)
{
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);
	
	GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);//SDA����Ϊ�͵�ƽ���͵�ƽΪ��ЧӦ��
	//SCL����һ������
	delay_us(2);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(2);
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
}


/*******************************���շ�  ������ACKӦ��**************************************/
void IIC_NAck(void)
{
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);
	//SDA����Ϊ�ߵ�ƽ���ߵ�ƽΪ��ЧӦ��
	GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
	//SCL����һ������
	delay_us(2);
	GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	delay_us(2);
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
}



/*
I2C���߽������ݴ���ʱ��ʱ���ź�Ϊ�ߵ�ƽ�ڼ䣬�������ϵ����ݱ��뱣���ȶ���ֻ����ʱ�����ϵ��ź�Ϊ�͵�ƽ�ڼ䣬�������ϵĸߵ�ƽ��͵�ƽ״̬������仯�� 
*/
/*********************************IIC����һ���ֽ�********************************/
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_OUT);
	
	//�Ȱ�ʱ�������ͣ�Ȼ�����������ߣ��ڰ�ʱ�������ߣ������ߵ�ƽʱ������͡�����ʱ���߸ߵ�ƽ�ڼ��ȶ������ߵ�ƽ��
	GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
	
	for(t=0;t<8;t++)
  {
		//ȡ���λ Ȼ������7λ
		if((txd&0x80)>>7)
			GPIO_SetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
		else
			GPIO_ResetBits(iic_GPIOx_SDA,iic_GPIO_Pin_SDA);
		
    txd<<=1;//��������һλ
		
		//�ϼ���ȷ���������ߵĵ�ƽ���¼�������ʱ���ߡ�ʱ���߸ߵ�ƽ�ڼ������ߵ�ƽ����
		delay_us(2);
		GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
		delay_us(2); 
		GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);	
		delay_us(2);
  }	 
}


/*********************************IIC��1���ֽ�********************************/

u8 IIC_Read_Byte(u8 ack)
{
	u8 i;
	u8 receive=0;
	setGPIOMode(iic_GPIOx_SDA,iic_pinSDA,GPIO_Mode_IN);//SDA����Ϊ����
	
	//��������
  for(i=0;i<8;i++ )
	{
		//�Ȱ�ʱ�������������ߣ�����һ��������֮����ȥ�������ߵĵ�ƽ 
    GPIO_ResetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL); 
    delay_us(2);
		GPIO_SetBits(iic_GPIOx_SCL,iic_GPIO_Pin_SCL);
    receive<<=1;
    if(GPIO_ReadInputDataBit(iic_GPIOx_SDA,iic_GPIO_Pin_SDA))//���յ��ߵ�ƽ��������Ƽ�1
			receive++;
		delay_us(1);
  }
	
	//�Ƿ���NACK���������ź�
  if (ack==SACK)
		IIC_Ack();
	else if(ack==SNACK)
    IIC_NAck();
  
  return receive;
}



