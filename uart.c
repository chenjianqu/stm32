#include "uart.h"
#include "led.h"

/*
UART：universal asynchronous receiver and transmitter通用异步收发器；
USART:universal synchronous asynchronous receiver and transmitter通用同步异步收发器。
一般而言，单片机中，名称为UART的接口一般只能用于异步串行通讯，而名称为USART的接口既可以用于同步串行通讯，也能用于异步串行通讯.
USART在UART基础上增加了同步功能，即USART是UART的增强型.其实当我们使用USART在异步通信的时候，它与UART没有什么区别，但是用在同步通信的时候，
区别就很明显了：大家都知道同步通信需要时钟来触发数据传输，
也就是说USART相对UART的区别之一就是能提供主动时钟。如STM32的USART可以提供时钟支持ISO7816的智能卡接口。
*/

/** 
F407串口引脚 
1	PA10(PB7)	PA9（PB6)
2	PA3(PD6)	PA2(PD5)
3	PB11(PC11/PD9)	PB10(PC10/PD8)
4	PC11(PA1)	PC10(PA0)
5	PD2	PC12
6	PC7(PG9)	PC6(PG14)
**/

#define RX_READY 1
#define RX_NOTREADY 0
#define RX_BUFF_LEN 50


/**********************************USART初始化**************************************/

void initUSART(USART_TypeDef* USARTx,uint32_t USART_BaudRate)
{
		USART_InitTypeDef   USART_InitStructure; 
		NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能串口时钟、GPIO、中断通道
	if(USARTx==USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		initGPIO(GPIOA,GPIO_Pin_10,GPIO_Mode_AF,GPIO_AF_USART1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//初始化RXIO
		initGPIO(GPIOA,GPIO_Pin_9, GPIO_Mode_AF,GPIO_AF_USART1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//初始化TXIO
	}
	else if(USARTx==USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		initGPIO(GPIOD,GPIO_Pin_6,GPIO_Mode_AF,GPIO_AF_USART2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//初始化RXIO
		initGPIO(GPIOD,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_USART2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//初始化TXIO
		//10kHz,0.1ms,  10ms
		initTIMBase(TIM13,8399,TIM_CounterMode_Up,99,TIM_CKD_DIV1,0);
		initNVIC(TIM8_UP_TIM13_IRQn,1,1);
		TIM_ClearFlag(TIM13,TIM_FLAG_Update);
		TIM_ITConfig(TIM13,TIM_IT_Update,ENABLE);
	}
	else if(USARTx==USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		initGPIO(GPIOB,GPIO_Pin_11,GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//初始化RXIO
		initGPIO(GPIOB,GPIO_Pin_10, GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//初始化TXIO
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
		initGPIO(GPIOC,GPIO_Pin_7,GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);//初始化RXIO
		initGPIO(GPIOC,GPIO_Pin_6, GPIO_Mode_AF,GPIO_AF_USART3,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);//初始化TXIO
	}
	//F407中没有UART7/8的中断向量
	else if(USARTx==UART7){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
	}
	else if(USARTx==UART8){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
	}
		
		//串口工作模式配置
		USART_InitStructure.USART_BaudRate =USART_BaudRate;//设置波特率					
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置字长
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
		USART_InitStructure.USART_Parity = USART_Parity_No ;//设置校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置不使用硬件流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能发送或接受
		USART_Init(USARTx, &USART_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;       //低优先级别的中断
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;							//响应中断等级为0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
		
		//使能接收中断
		USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);		
		
		USART_Cmd(USARTx, ENABLE);
}






u8 rxflag=RX_READY;//一串数字是否接收完成,是否准备开始接受新的字符串
u8 rxBuff[RX_BUFF_LEN];
u8 rxBuff_i=0;

/**********************************USART1中断**************************************/
void USART1_IRQHandler(void)
{
	u8 res;//设置的字长为8位，所以用char
	if(USART_GetITStatus(USART1,USART_IT_RXNE)){	
		res=USART_ReceiveData(USART1);
		USART_SendData(USART1,res);
		GPIO_ResetBits(GPIOF,GPIO_Pin_10);
		delay_ms(10);
		GPIO_SetBits(GPIOF,GPIO_Pin_10);
	}
}

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.

/**********************************USART2中断**************************************/
void USART2_IRQHandler(void)
{
	u8 res;//设置的字长为8位，所以用char
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		res=USART_ReceiveData(USART2);
		if(rxflag==RX_READY)//当前是接收的状态
		{
			if(rxBuff_i<RX_BUFF_LEN)//缓存区未满
			{
				TIM_SetCounter(TIM13,0);//计数器清零
				if(rxBuff_i==0)TIM_Cmd(TIM13,ENABLE);//如果这是接收的第一个字符
				rxBuff[rxBuff_i]=res;
				rxBuff_i++;
			}
			else{
				rxflag=RX_NOTREADY;//若缓冲区已满，则标志接收完成，等待进入接收状态
			}
		}
	}
}



/***************************************定时器1_13中断*************************************/
void TIM8_UP_TIM13_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM8,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'*');
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	}
	/*************TIM13更新中断***************/
	else if(TIM_GetFlagStatus(TIM13,TIM_IT_Update))
	{
		rxflag=RX_NOTREADY;
		TIM_ClearITPendingBit(TIM13,TIM_IT_Update);
		TIM_Cmd(TIM13, DISABLE);//关闭TIM13
	}
}



/******************************************获取当前缓冲区的内容******************************/
//要求传入的pBuff为数组，返回数据的长度
u8 getRXString(u8 *pBuff)
{
	u8 len;
	for(len=0;len<rxBuff_i;len++)
		*(pBuff+len)=rxBuff[len];
	rxBuff_i=0;
	return len;
}



/*****************************************进入接收状态********************************/
//接收完数据然后处理之后应该执行此函数
void setRX_Ready(void)
{
	rxflag=RX_READY;
}





/*****************************************UART发送数字****************************************/
u8 numChar[10];
void USART_SendNumber(USART_TypeDef* USARTx,int num)
{
	int i=0;
	if(num<=0){//是否是负数
		if(num==0)
		{
			USART_SendData(USARTx,'0');
			delay_ms(8);
			return;
		}
		num=-num;
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USARTx,'-');
	}
	for(;num>0;i++){//将数字拆开放到数组里
		numChar[i]=num%10+'0';
		num=num/10;
	}
	for(i--;i>=0;i--){
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USARTx,numChar[i]);
	}
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USARTx,32);//发送空格
}



/***************************UART发送一串字符*********************************/
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




//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void USART_NiMing_Report(USART_TypeDef* USARTx,u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)USART_SendChar(USARTx,send_buf[i]);	//发送数据到串口1 
}



//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
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
	USART_NiMing_Report(USART1,0XA1,tbuf,12);//自定义帧,0XA1
}	



//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void USART_Report_IMU(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
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
	USART_NiMing_Report(USART1,0XAF,tbuf,28);//飞控显示帧,0XAF
} 
