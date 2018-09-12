#include "can.h"
#include "gpio.h"

#define CAN_CURRENT_FIFO CAN_FilterFIFO0

/*
F407的CAN引脚：
CAN1_RX:PA11/PB8/PD0 CAN1_TX:PA12/PB9/PD1
CAN2_RX:PB12/PB5     CAN1_TX:PB13/PB6
*/

/*CAN通信的ISO11898标准，物理层特征:
CAN 控制器根据CAN_L和CAN_H上的电位差来判断总线电平。总线电平分为显性电平和隐性电平，二者必居其一。发送方通过使总线电平发生变化，将消息发送给接收方。
显性电平对应逻辑：0  CAN_H和CAN_L之差为2V左右。
隐性电平对应逻辑：1  CAN_H和CAN_L之差为0V。
显性电平具有优先权，只要有一个单元输出显性电平，总线上即为显性电平。而隐形电平则具有包容的意味，只有所有的单元都输出隐性电平，总线上才为隐性电平（显性电平比隐性电平更强）。
另外，在CAN总线的起止端都有一个120Ω的终端电阻，来做阻抗匹配，以减少回波反射。
*/

/*CAN通信是以以下5种类型的帧进行的：
帧类型	帧用途
数据帧	用于发送单元向接收单元传送数据的帧
遥控帧	用于接收单元向具有相同 ID 的发送单元请求数据的帧
错误帧	用于当检测出错误时向其它单元通知错误的帧
过载帧	用于接收单元通知其尚未做好接收准备的帧
间隔帧	用于将数据帧及遥控帧与前面的帧分离开来的帧
其中，数据帧和遥控帧有标准格式和扩展格式两种格式。标准格式有11 个位的标识符（ID），扩展格式有29 个位的ID 
*/
/*数据帧由7个段组成：
①，帧起始。表示数据帧开始的段。
②，仲裁段。表示该帧优先级的段。
③，控制段。表示数据的字节数及保留位的段。
④，数据段。数据的内容，一帧可发送0~8个字节的数据。
⑤，CRC段。检查帧的传输错误的段。
⑥，ACK段。表示确认正常接收的段。
⑦，帧结束。表示数据帧结束的段。
*/
/*
1，帧起始。标准帧和扩展帧都是由1个位的显性电平表示帧起始。
2，仲裁段。表示数据优先级的段，标准帧和扩展帧格式在本段有所区别
	标准格式的仲裁段由基本ID和RTR位组成，扩展格式仲裁段由基本ID、SRR、IDE、扩展ID、RTR位组成
	ID：高位在前，低位在后。基本ID，禁止高7位都为隐性，即不能：ID=1111111XXXX。
	RTR，远程请求位。0，数据帧；1， 远程帧；即判断是远程帧还是数据帧
	SRR，替代远程请求位。设置为1（隐性电平）；
	IDE，标识符选择位。0，标准标识符；1，扩展标识符；
3，控制段。由6个位构成，表示数据段的字节数。标准帧和扩展帧的控制段稍有不同
	标准格式的控制段由基本IDE、r0、DLC位组成，扩展格式控制段由r0、r1、DLC位组成
	r0，r1：保留位。必须以显现电平发送，但是接收可以是隐性电平。
	DLC：数据长度码。0~8，表示发送/接收的数据长度（字节）。
	IDE，标识符选择位。0，标准标识符；1，扩展标识符；
4，数据段。该段可包含0~8个字节的数据，从最高位（MSB）开始输出。标准帧和扩展帧在这个段的格式完全一样
5，CRC段。该段用于检查帧传输错误。由15个位的CRC顺序和1个位的CRC界定符（用于分隔的位）组成，标准帧和扩展帧在这个段的格式也是相同的
	CRC的值计算范围包括：帧起始、仲裁段、控制段、数据段。接收方以同样的算法计算 CRC 值并进行比较，不一致时会通报错误。 
6，ACK段。此段用来确认是否正常接收。由ACK槽(ACK Slot)和ACK界定符2个位组成。标准帧和扩展帧在这个段的格式也是相同的：
	发送单元ACK段：发送2个隐性位。
	接收单元ACK段：接收到正确消息的单元，在ACK槽发送显性位，通知发送单元，正常接收结束。称之为发送ACK/返回ACK。
	注意：发送 ACK 的是既不处于总线关闭态也不处于休眠态的所有接收单元中，接收到正常消息的单元（发送单元不发送ACK）。正常消息是指：不含填充错误、格式错误、CRC 错误的消息。
7，帧结束。由7个位的隐性位组成。标准帧和扩展帧在这个段格式完全一样。
*/

/*总线仲裁：
1，总线空闲时，最先发送的单元获得发送优先权，一但发送，其他单元无法抢占。
2，如果有多个单元同时发送，则连续输出显性电平多的单元，具有较高优先级。从ID开始比较，如果ID相同，还可能会比较RTR和SRR等位。
*/

/*位速率：
这些段又由可称为 Time Quantum（以下称为Tq）的最小时间单位构成。
1 位分为4 个段，每个段又由若干个Tq 构成，这称为位时序。
位时间=1/波特率，因此，知道位时间，我们就可以知道波特率。
1 位由多少个Tq 构成、每个段又由多少个Tq 构成等，可以任意设定位时序。通过设定位时序，多个单元可同时采样，也可任意设定采样点。
同步段（SS）
传播时间段（PTS）
相位缓冲段1（PBS1）
相位缓冲段2（PBS2）
位速率。由发送单元在非同步的情况下发送的每秒钟的位数称为位速率。一个位一般可以分为如下四段：
*/

/*STM32自带了基本扩展CAN外设，又称bxCAN，bxCAN的特点如下：
支持CAN协议2.0A和2.0B主动模式
波特率最高达1Mbps
支持时间触发通信
具有3个发送邮箱
具有3级深度的2个接收FIFO
可变的筛选器组（也称过滤器组，最多28个）*/

/*stm32f407有两个can,两个CAN分别拥有自己的发送邮箱和接收FIFO，但是他们共用28个筛选器*/
/*模式
工作模式
       ①初始化模式（INRQ=1，SLEEP=0）
       ②正常模式（INRQ=0，SLEEP=0）
       ③睡眠模式（SLEEP=1）

测试模式
       ①静默模式（ LBKM=0，SILM=1 ）
       ②环回模式（ LBKM=1，SILM=0 ）
       ③环回静默模式（LBKM=1，SILM=1）

调试模式
*/
/*stm32筛选器
CAN的标识符不表示目的地址而是表示发送优先级。接收节点根据标识符的值，来决定是否接收对应消息。
STM32 CAN控制器，提供了28个可配置的筛选器组（F1仅互联型才有28个，其他的只有14个），可降低CPU处理CAN通信的开销。
STM32 CAN控制器每个筛选器组由2个32位寄存器组成（CAN_FxR1和CAN_FxR2，x=0~27）。根据位宽不同，每个筛选器组可提供：
       ● 1个32位筛选器，包括：STDID[10:0]、EXTID[17:0]、IDE和RTR位
       ● 2个16位筛选器，包括：STDID[10:0]、IDE、RTR和EXTID[17:15]位
筛选器可配置为：屏蔽位模式和标识符列表模式。在屏蔽位模式下，标识符寄存器和屏蔽寄存器一起，指定报文标识符的任何一位，应该按照“必须匹配”或“不用关心”处理。
而在标识符列表模式下，屏蔽寄存器也被当作标识符寄存器用。因此，不是采用一个标识符加一个屏蔽位的方式，而是使用2个标识符寄存器。接收报文标识符的每一位都必须跟筛选器标识符相同。

为了过滤出一组标识符，应该设置筛选器组工作在屏蔽位模式。
为了过滤出一个标识符，应该设置过滤器组工作在标识符列表模式。
应用程序不用的筛选器组，应该保持在禁用状态(通过CAN_FA1R设置)。
筛选器组中的每个筛选器，都被编号为(即：筛选器编号)从0开始，到某个最大数值－取决于筛选器组的模式和位宽的设置。
通过CAN_FFA1R的设置，可以将筛选器组关联到FIFO0/FIFO1
        例：设置筛选器组0工作在：1个32位筛选器-标识符屏蔽模式，然后设置CAN_F0R1=0XFFFF0000，CAN_F0R2=0XFF00FF00。其中存放到CAN_F0R1的值就是期望收到的ID，即（STID+EXTID+IDE+RTR）最
				好是：0XFFFF0000。而0XFF00FF00就是设置我们需要必须关心的ID，表示收到的映像，其位[31:24]和位[15:8]这16个位的必须和CAN_F0R1中对应的位一模一样，而另外的16个位则不关心，可以一样，
				也可以不一样，都认为是正确的ID，即收到的映像必须是0XFFxx00xx，才算是正确的（x表示不关心）。

*/
/*
CAN发送流程为：
程序选择1个空置的邮箱（TME=1）
->设置标识符（ID），数据长度和发送数据
->设置CAN_TIxR的TXRQ位为1，请求发送
->邮箱挂号（等待成为最高优先级）
->预定发送（等待总线空闲）
->发送
->邮箱空置。

CAN接收流程为：
FIFO空->
收到有效报文->
挂号_1（存入FIFO的一个邮箱，这个由硬件控制，我们不需要理会）
->收到有效报文
->挂号_2
->收到有效报文
->挂号_3
->收到有效报文
->溢出。
*/
/*整体流程：
①配置相关引脚的复用功能，使能CAN时钟。
        要用CAN，先要使能CAN的时钟，CAN的时钟通过APB1ENR的第25位来设置。其次要设置CAN的相关引脚为复用输出，这里我们需要设置PA11为上拉输入（CAN_RX引脚）PA12为复用输出（CAN_TX引脚），并使能PA口的时钟
②设置CAN工作模式及波特率等。
        通过先设置CAN_MCR寄存器的INRQ位，让CAN进入初始化模式，然后设置CAN_MCR的其他相关控制位。再通过CAN_BTR设置波特率和工作模式（正常模式/环回模式）等信息。 最后设置INRQ为0，退出初始化模式。
③设置滤波器。
        本例程，我们将使用筛选器组0，并工作在32位标识符屏蔽位模式下。先设置CAN_FMR的FINIT位，进入初始化模式，然后设置筛选器组0的工作模式以及标识符ID和屏蔽位。最后激活筛选器，并退出初始化模式。

*/

/**************************************CAN初始化函数*****************************************/
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
tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
则波特率为:42M/((6+7+1)*6)=500Kbps
返回值:0,初始化OK;
其他,初始化失败; 
*/
	
	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;//非时间触发通信模式
	CAN_InitStructure.CAN_ABOM=DISABLE;//软件自动离线管理
	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒
	CAN_InitStructure.CAN_NART=ENABLE;//禁止报文自动传送
	CAN_InitStructure.CAN_RFLM=DISABLE;//报文不锁定，新的覆盖旧的
	CAN_InitStructure.CAN_TXFP=DISABLE;//优先级由报文标识符决定
	CAN_InitStructure.CAN_Mode=mode;//模式设置,可设为普通、睡眠、环回、环回睡眠模式
	//设置波特率，波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
	CAN_InitStructure.CAN_SJW=tsjw;//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位，范围为：CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1;//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;//分频系数，brp+1	
	CAN_Init(CANx,&CAN_InitStructure);
	
	//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32位
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32为MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_CURRENT_FIFO;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);
	
#if CAN_RX0_INT_ENABLE
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);
	initNVIC(CAN1_RX0_IRQn,1,0);
#endif
	return 0;
}

/**************************************中断服务函数*****************************************/
#if CAN1_RX0_INT_ENABLE	//使能RX0中断
void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
	int i=0;
  
	CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif



//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
/**************************************can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)*****************************************/
u8 sendCANMessage(CAN_TypeDef* CANx,u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
	
  CanTxMsg TxMessage;
  TxMessage.StdId=0x12;//标准标识符为0
  TxMessage.ExtId=0x12;//设置扩展标示符（29位），当下句设置为标准帧时 该句没用
  TxMessage.IDE=CAN_Id_Standard;//设置帧类型，在此设置为标准标识符
  TxMessage.RTR=CAN_RTR_Data;//设置远程请求位，在此消息类型设置为为数据帧，一帧8位
  TxMessage.DLC=len;//要发送的数据长度
  
	for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];//把参数数据赋给CanTxMsg
  
	mbox= CAN_Transmit(CANx, &TxMessage);//查询可用邮箱然后发送Message,发送完成之后返回发送的邮箱
  i=0;
	
  while((CAN_TransmitStatus(CANx,mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))//若一段时间内一直发送失败
		i++;
	
  if(i>=0XFFF)//则返回1
		return 1;
	
  return 0;		
}


//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//其他,接收的数据长度;
/**************************************can口接收数据查询*****************************************/
u8 recerveCANMessage(CAN_TypeDef* CANx,u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
	
  if( CAN_MessagePending(CANx,CAN_CURRENT_FIFO)==0)return 0;//查询FIFO0的邮箱若查询到没有接收到数据,直接退出 
	
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
	
  for(i=0;i<RxMessage.DLC;i++)//DLC为数据帧控制段的数据长度码
    buf[i]=RxMessage.Data[i];  
	
	return RxMessage.DLC;//返回接收到的数据长度
}

