#include "irremote.h"

/*红外遥控的编码目前广泛使用的是：NEC Protocol 的PWM(脉冲宽度调制)和Philips
RC-5 Protocol 的PPM(脉冲位置调制)*/

/*
NEC说明:
1.8位地址和8位指令长度；
2.地址和命令2次传输（确保可靠性）
3.PWM脉冲位置调制，以发射红外载波的占空比代表“0”和“1”；
4.载波频率为38Khz；
5.位时间为1.125ms或2.25ms（高电平持续时间来区分）；

NEC码的位定义：一个脉冲对应560us的连续载波，一个逻辑1传输需要2.25ms（560us脉冲+1680us低电平），一个逻辑0的传输需要1.125ms（560us脉冲+560us低电平）。
而遥控接收头在收到脉冲的时候为低电平，在没有脉冲的时候为高电平，这样，我们在接收头端收到的信号为：逻辑1应该是560us低+1680us高，逻辑0应该是560us低+560us高。

数据格式：
NEC遥控指令的数据格式为：同步码头+地址码+地址反码+控制码+控制反码。
同步码由一个9ms的低电平和一个4.5ms的高电平组成，地址码、地址反码、控制码、控制反码均是8位数据格式。
按照低位在前，高位在后的顺序发送。采用反码是为了增加传输的可靠性（可用于校验）。

连发码：由9ms低电平+2.5m高电平+0.56ms低电平+97.94ms高电平组成，如果在一帧数据发送完毕之后，按键仍然没有放开，则发射重复码，
即连发码，可以通过统计连发码的次数来标记按键按下的长短/次数。
*/

/*
程序思路：
开启定时器对应通道输入捕获功能，默认上升沿捕获。定时器的计数频率为1MHz,自动装载值为10000，也就是溢出时间10ms。
开启定时器输入捕获更新中断和捕获中断。当捕获到上升沿产生捕获中断，当定时器计数溢出，产生更新中断。

捕获高电平时间：当捕获到上升沿的时候，设置捕获极性为下降沿捕获（为下次捕获下降沿做准备），然后清空定时器，同时设置变量RmtSta的位4值为1，标记已经捕获到上升沿。捕获到下降沿的时候，
读取定时器的值赋值给变量Dval，然后设置捕获极性为上升沿捕获（为下次捕获上升沿做准备），同时对变量RmtSta的位4进行判断：如果RmtSta位4位1，说明之前已经捕获到过上升沿，那么对捕获值
Dval进行判断。

300-800之间，说明接收到的是数据0,
1400-1800之间 说明接收到的数据为1,
2200-2600，说明是连发码，
4200-4700说明为同步码。
分析后设置相应的标志位。如果是定时器发生溢出中断，那么分析，如果之前接收到了同步码，并且是第一次溢出，标记完成一次按键信息采集。
*/


#define IR_GPIOX GPIOA
#define IR_GPIO_PIN GPIO_Pin_8

/*******************************初始化IRremote****************************************/
void initIRremote(void)
{
  TIM_ICInitTypeDef  TIM1_ICInitStructure;
	
  //GPIOA8  复用功能,上拉
	initGPIO(IR_GPIOX,IR_GPIO_PIN,GPIO_Mode_AF,GPIO_AF_TIM1,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	
	//初始化TIMBase
	initTIMBase(TIM1,167,TIM_CounterMode_Up,9999,TIM_CKD_DIV1,0);
	
	TIM_Cmd(TIM1,ENABLE );
	
 	//初始化TIM1输入捕获参数
	TIM1_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM1_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8个定时器时钟周期滤波
  TIM_ICInit(TIM1, &TIM1_ICInitStructure);
	 
	initNVIC(TIM1_CC_IRQn,			1,3);
	initNVIC(TIM1_UP_TIM10_IRQn,1,2);
	
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断（即当定时器溢出时），允许CC1捕获中断
}






u8 recflag=0;//是否接收到同步码
u8 upflag=0;//是否接收到上升沿
u8 validflag=0;//已经获得按键信息标志

u8 overflowCnt=0;//定时器溢出次数 

u16 pulseWidth=0;//脉冲宽度
u32 value;//接收到的值

u8  RmtCnt=0;	//按键按下的次数	 


/*******************************定时器1中断****************************************/
void TIM1_UP_TIM10_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET) //定时器1溢出中断（可能是未收到数据，也可能是接收了数据之后溢出）
	{
		if(recflag)//已经接收到同步码，即已经接收了数据（此时从接收到同步码 已经过去了10ms ）
		{	
			upflag=0;//取消上升沿已经被捕获标记
			if(overflowCnt==0)
				validflag=1;//标志信息获取完成
			
			if(overflowCnt<14)//定时器溢出次数大于14 即已经过去了140ms 
				overflowCnt++;
			else
			{
				recflag=0;//清空引导标识
				overflowCnt=0;	//清空计数器	
			}
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
} 






/*******************************定时器1输入捕获中断服务程序	****************************************/
void TIM1_CC_IRQHandler(void)
{ 		    	 
	if(TIM_GetITStatus(TIM1,TIM_IT_CC1)==SET) //处理捕获(CC1IE)中断
		{
		if(GPIO_ReadInputDataBit(IR_GPIOX,IR_GPIO_PIN))//上升沿捕获
		{
			TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Falling);//设置为下降沿捕获
			TIM_SetCounter(TIM1,0);//清空定时器
			upflag=1;	//标记上升沿已经被捕获	
		}
		else //下降沿捕获
		{
			pulseWidth=TIM_GetCapture1(TIM1);//读取计数器的值，即获取脉冲宽度
			TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Rising); //CC1P=0	设置为上升沿捕获
			
			if(upflag)//完成了一次高电平捕获 
			{
 				if(recflag)//接收到了同步码
				{
					if(pulseWidth>300&&pulseWidth<800)			//560为标准值,560us  接收到0	 
					{
						value<<=1;	//左移一位.
						value|=0;	  
					}else if(pulseWidth>1400&&pulseWidth<1800)	//1680为标准值,1680us 接收到1
					{
						value<<=1;	//左移一位.
						value|=1;
					}else if(pulseWidth>2200&&pulseWidth<2600)	//接收到连发码
					{
						RmtCnt++; 		//按键次数增加1次
						overflowCnt=0;	//清空计时器		
					}
				}
				
				else if(pulseWidth>4200&&pulseWidth<4700)//4500为标准值4.5ms
				{
					recflag=1;	//标记成功接收到了同步码
					RmtCnt=0;		//清除按键次数计数器
				}
			}
			
			//清除上升沿捕获标志
			upflag=0;
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC1);  //清除中断标志位 
}






//返回地址码
/*******************************按键扫描****************************************/

u8 scanRemote(void)
{        
	//value 地址码+地址反码+控制码+控制反码
	u8 result=0;       
  u8 t1,t2;  
	if(validflag)//接收到了数据
	{ 
	    t1=value>>24;//得到地址码
	    t2=(value>>16)&0xff;//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 ,每个遥控器的id不一样
	    { 
	        t1=value>>8;
	        t2=value; 	
	        if(t1==(u8)~t2)
						result=t1;//控制码等于控制反码
			}   
			if((result==0)||(recflag==0))//按键数据错误/遥控已经没有按下了
			{
				validflag=0;//清除接收到有效按键标识
				RmtCnt=0;		//清除按键次数计数器
			}
	}  
  return result;
}


