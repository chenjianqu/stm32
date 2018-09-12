#include "dht11.h"

/*
DHT11�Ĳ�����
������ѹ��Χ��3.3V-5.5V
�������� ��ƽ��0.5mA
����������������ź�
������Χ��ʪ��20~90��RH���¶�0~50��
���� ��ʪ�ȡ�5%���¶ȡ�2��
�ֱ��� ��ʪ��1%���¶�1�� 
*/
/*
ָ�
DHT11����ʪ�¶ȴ��������õ��������ݸ�ʽ�������������Ŷ˿�����������˫���䡣
�����ݰ���5Byte��40Bit����ɡ����ݷ�С�����ֺ��������֣�һ�����������ݴ���Ϊ40bit����λ�ȳ���
DHT11�����ݸ�ʽΪ��8bitʪ����������+8bitʪ��С������+8bit�¶���������+8bit�¶�С������+8bitУ��͡�
����У�������Ϊǰ�ĸ��ֽ���ӡ�
�����������������δ����Ķ��������ݡ�����(ʪ�ȡ��¶ȡ�������С��)֮��Ӧ�÷ֿ�����
*/
/*
���ݸ�ʽ��
�����������Ϳ�ʼ�źţ��������������ߣ�����t1������18ms��ʱ�䣬Ȼ������������t2��20~40us��ʱ�䣬
Ȼ���ȡDHT11����Ӧ�������Ļ���DHT11�����������ߣ�����t3��40~50us��ʱ�䣬��Ϊ��Ӧ�źţ�
Ȼ��DHT11���������ߣ�����t4��40~50us��ʱ��󣬿�ʼ������ݡ�
*/

GPIO_TypeDef* dht11_GPIOx;
u16 dht11_GPIO_Pin_x;
u8 dht11_pin;//�õ�GPIO_Pin_x��Ӧ������

/*******************************��ʼ��DHT11****************************************/
u8 initDHT11(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x)
{
	dht11_GPIOx=GPIOx;
	dht11_GPIO_Pin_x=GPIO_Pin_x;

	dht11_pin=turnPinNum(GPIO_Pin_x);
	
	initGPIO(GPIOx,GPIO_Pin_x,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	
	return requestDHT11();
}


/*******************************������������****************************************/
u8 requestDHT11(void)
{
	/***���������ź�***/
	
	//����Ϊ���ģʽ
	setGPIOMode(dht11_GPIOx,dht11_pin,GPIO_Mode_OUT);
	
  GPIO_ResetBits(dht11_GPIOx,dht11_GPIO_Pin_x);
  delay_ms(20);    	//��������18ms
  GPIO_SetBits(dht11_GPIOx,dht11_GPIO_Pin_x); 	//DQ=1 
	delay_us(30);     	//��������20~40us
	
	
	/***��ȡ��Ӧ***/
	u8 count=0;
	setGPIOMode(dht11_GPIOx,dht11_pin,GPIO_Mode_IN);
	
	while(GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//�ȴ��ӻ���Ӧ���͵�ƽʱ���㿪ʼ��Ӧ��
	{
		count++;
		delay_us(1);
	}
	if(count>100)//����100ms��û����͵�ƽ����δ��Ӧ�򷵻ش���
		return DHT11_REQUEST_FAIL;
	else
		count=0;
	while(!GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//��ȡ�͵�ƽ�ĳ���ʱ��
	{
		count++;
		delay_us(1);
	}
	if(count>=100)//�͵�ƽʱ�䳬��100ms�������
		return DHT11_REQUEST_FAIL;
	return DHT11_REQUEST_OK;
}


/*******************************��ȡһ������λ****************************************/
u8 readDHT11Bit(void)
{
	u8 count=0;
	//һ������λ�ĵ�һ����12-14us�ĵ͵�ƽ��Ȼ���Ǹߵ�ƽ�����ߵ�ƽ��ʱ����26-28us����Ϊ0����Ϊ116-118us��Ϊ1
	
	while(GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//�ȴ���ɵ͵�ƽ���͵�ƽ��һ������λ�Ŀ�ʼ������100us��û���
	{
		count++;
		delay_us(1);
	}
	count=0;
	while(!GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x)&&count<100)//�����͵�ƽ�ĳ���ʱ��
	{
		count++;
		delay_us(1);
	}
	delay_us(40);
	if(GPIO_ReadInputDataBit(dht11_GPIOx,dht11_GPIO_Pin_x))//��Ϊ�͵�ƽ�ٹ�40us�������Ǹߵ�ƽ �������������1
		return 1;
	else
		return 0;
}


/*******************************��ȡһ���ֽ�****************************************/
u8 readDHT11Byte(void)
{
	u8 i;
	u8 data=0;
	for(i=0;i<8;i++)
	{
		data<<=1;//����һλ
		data|=readDHT11Bit();
	}
	return data;
}


/*******************************��ȡ�¶Ⱥ�ʪ��****************************************/
u8 readDHT11Data(u8 *temp,u8 *humi)
{
	u8 buf[5];
	u8 i;
	if(requestDHT11()==DHT11_REQUEST_OK)//�������ɹ�
	{
		for(i=0;i<5;i++)
			buf[i]=readDHT11Byte();
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//�Եõ������ݽ���У��
		{
			*humi=buf[0];//ֻȡ������
			*temp=buf[2];
		}
	}
	else
		return DHT11_GETDATA_FAIL;
	
	return DHT11_GETDATA_OK;
}

