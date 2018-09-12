#include "uart.h"
#include "led.h"

/*
UART��universal asynchronous receiver and transmitterͨ���첽�շ�����
USART:universal synchronous asynchronous receiver and transmitterͨ��ͬ���첽�շ�����
һ����ԣ���Ƭ���У�����ΪUART�Ľӿ�һ��ֻ�������첽����ͨѶ��������ΪUSART�Ľӿڼȿ�������ͬ������ͨѶ��Ҳ�������첽����ͨѶ.
USART��UART������������ͬ�����ܣ���USART��UART����ǿ��.��ʵ������ʹ��USART���첽ͨ�ŵ�ʱ������UARTû��ʲô���𣬵�������ͬ��ͨ�ŵ�ʱ��
����ͺ������ˣ���Ҷ�֪��ͬ��ͨ����Ҫʱ�����������ݴ��䣬
Ҳ����˵USART���UART������֮һ�������ṩ����ʱ�ӡ���STM32��USART�����ṩʱ��֧��ISO7816�����ܿ��ӿڡ�
*/

/** 
F407�������� 
1	PA10(PB7)	PA9��PB6)
2	PA3(PD6)	PA2(PD5)
3	PB11(PC11/PD9)	PB10(PC10/PD8)
4	PC11(PA1)	PC10(PA0)
5	PD2	PC12
6	PC7(PG9)	PC6(PG14)
**/

#define RX_READY 1
#define RX_NOTREADY 0
#define RX_BUFF_LEN 50


/**********************************USART��ʼ��**************************************/

void initUSART(USART_TypeDef* USARTx,uint32_t USART_BaudRate)
{
		USART_InitTypeDef   USART_InitStructure; 
		NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ�ܴ���ʱ�ӡ�GPIO���ж�ͨ��
	if(USARTx==USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		initGPIO(GPIOA,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_USART1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//��ʼ��RXIO
		initGPIO(GPIOA,GPIO_Pin_9, GPIO_Mode_AF,GPIO_AF_USART1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//��ʼ��TXIO
	}
	else if(USARTx==USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		initGPIO(GPIOD,GPIO_Pin_6,GPIO_Mode_AF,GPIO_AF_USART2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//��ʼ��RXIO
		initGPIO(GPIOD,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_USART2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//��ʼ��TXIO
		//10kHz,0.1ms,  10ms
		initTIMBase(TIM13,8399,TIM_CounterMode_Up,99,TIM_CKD_DIV1,0);
		initNVIC(TIM8_UP_TIM13_IRQn,1,1);
		TIM_ClearFlag(TIM13,TIM_FLAG_Update);
		TIM_ITConfig(TIM13,TIM_IT_Update,ENABLE);
	}
	else if(USARTx==USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		initGPIO(GPIOB,GPIO_Pin_11,GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//��ʼ��RXIO
		initGPIO(GPIOB,GPIO_Pin_10, GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//��ʼ��TXIO
	}
	else if(USARTx==UART4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	}
	else if(USARTx==UART5){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	}
	else if(USARTx==USART6){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
		initGPIO(GPIOC,GPIO_Pin_7,GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//��ʼ��RXIO
		initGPIO(GPIOC,GPIO_Pin_6, GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//��ʼ��TXIO
	}
	//F407��û��UART7/8���ж�����
	else if(USARTx==UART7){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
	}
	else if(USARTx==UART8){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
	}
		
		//���ڹ���ģʽ����
		USART_InitStructure.USART_BaudRate =USART_BaudRate;//���ò�����					
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�����ֳ�
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No ;//����У��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//���ò�ʹ��Ӳ��������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //ʹ�ܷ��ͻ����
		USART_Init(USARTx, &USART_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;       //�����ȼ�����ж�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;							//��Ӧ�жϵȼ�Ϊ0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
		
		//ʹ�ܽ����ж�
		USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);		
		
		USART_Cmd(USARTx, ENABLE);
}






u8 rxflag=RX_READY;//һ�������Ƿ�������,�Ƿ�׼����ʼ�����µ��ַ���
u8 rxBuff[RX_BUFF_LEN];
u8 rxBuff_i=0;

/**********************************USART1�ж�**************************************/
void USART1_IRQHandler(void)
{
	u8 res;//���õ��ֳ�Ϊ8λ��������char
	if(USART_GetITStatus(USART1,USART_IT_RXNE)){	
		res=USART_ReceiveData(USART1);
		USART_SendData(USART1,res);
		GPIO_ResetBits(GPIOF,GPIO_Pin_10);
		delay_ms(10);
		GPIO_SetBits(GPIOF,GPIO_Pin_10);
	}
}

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.

/**********************************USART2�ж�**************************************/
void USART2_IRQHandler(void)
{
	u8 res;//���õ��ֳ�Ϊ8λ��������char
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		res=USART_ReceiveData(USART2);
		if(rxflag==RX_READY)//��ǰ�ǽ��յ�״̬
		{
			if(rxBuff_i<RX_BUFF_LEN)//������δ��
			{
				TIM_SetCounter(TIM13,0);//����������
				if(rxBuff_i==0)TIM_Cmd(TIM13,ENABLE);//������ǽ��յĵ�һ���ַ�
				rxBuff[rxBuff_i]=res;
				rxBuff_i++;
			}
			else{
				rxflag=RX_NOTREADY;//�����������������־������ɣ��ȴ��������״̬
			}
		}
	}
}



/***************************************��ʱ��1_13�ж�*************************************/
void TIM8_UP_TIM13_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM8,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'*');
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	}
	/*************TIM13�����ж�***************/
	else if(TIM_GetFlagStatus(TIM13,TIM_IT_Update))
	{
		rxflag=RX_NOTREADY;
		TIM_ClearITPendingBit(TIM13,TIM_IT_Update);
		TIM_Cmd(TIM13, DISABLE);//�ر�TIM13
	}
}



/******************************************��ȡ��ǰ������������******************************/
//Ҫ�����pBuffΪ���飬�������ݵĳ���
u8 getRXString(u8 *pBuff)
{
	u8 len;
	for(len=0;len<rxBuff_i;len++)
		*(pBuff+len)=rxBuff[len];
	rxBuff_i=0;
	return len;
}



/*****************************************�������״̬********************************/
//����������Ȼ����֮��Ӧ��ִ�д˺���
void setRX_Ready(void)
{
	rxflag=RX_READY;
}





/*****************************************UART��������****************************************/
u8 numChar[10];
void USART_SendNumber(USART_TypeDef* USARTx,int num)
{
	int i=0;
	if(num<=0){//�Ƿ��Ǹ���
		if(num==0)
		{
			USART_SendData(USARTx,'0');
			delay_ms(8);
			return;
		}
		num=-num;
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USARTx,'-');
	}
	for(;num>0;i++){//�����ֲ𿪷ŵ�������
		numChar[i]=num%10+'0';
		num=num/10;
	}
	for(i--;i>=0;i--){
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USARTx,numChar[i]);
	}
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USARTx,32);//���Ϳո�
}



/***************************UART����һ���ַ�*********************************/
void USART_SendString(USART_TypeDef* USARTx,u8 *buff,u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
		USART_SendData(USARTx,*(buff+i));
	}
}


void USART_SendChar(USART_TypeDef* USARTx,u8 c)
{
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
	USART_SendData(USARTx,c);
}




//�������ݸ�����������λ�����(V2.6�汾)
//fun:������. 0XA0~0XAF
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void USART_NiMing_Report(USART_TypeDef* USARTx,u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽ����� 
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0X88;	//֡ͷ
	send_buf[1]=fun;	//������
	send_buf[2]=len;	//���ݳ���
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//��������
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//����У���	
	for(i=0;i<len+4;i++)USART_SendChar(USARTx,send_buf[i]);	//�������ݵ�����1 
}



//���ͼ��ٶȴ��������ݺ�����������
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
void MPU6050_Send_Data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	USART_NiMing_Report(USART1,0XA1,tbuf,12);//�Զ���֡,0XA1
}	



//ͨ������1�ϱ���������̬���ݸ�����
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
void USART_Report_IMU(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//��0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	USART_NiMing_Report(USART1,0XAF,tbuf,28);//�ɿ���ʾ֡,0XAF
} 
