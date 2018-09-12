#include "tpad.h"


#define TPAD_ARR_MAX_VAL  0XFFFFFFFF	//最大的ARR值(TIM2是32位定时器)


vu16 TPAD_DEFAULT_VAL=0;				//空载的时候(没有手按下),计数器需要的时间



/******************************初始化触摸板******************************/
u8 initTPAD(void)
{
	

	initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_TIM2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);
	
	initTIMBase(TIM2,8,TIM_CounterMode_Up,0XFFFFFFFF,TIM_CKD_DIV1,0);
	
	TIM_ICInitTypeDef  TIM2_ICInitStructure;
	//初始化通道1
  TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TIM2上
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC2F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM2, &TIM2_ICInitStructure);//初始化TIM2 IC1
	TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
	
	
	
	
	u16 buf[10];
	u16 temp;
	u8 j,i;
	
	for(i=0;i<10;i++)//连续读取10次
	{				 
		buf[i]=TPAD_getChargeTime();
		delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//排序
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//取中间的8个数据进行平均
	TPAD_DEFAULT_VAL=temp/8;
	if(TPAD_DEFAULT_VAL>TPAD_ARR_MAX_VAL/2)
		return 1;//初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常!
	return 0;		
}






//如果超时,则直接返回定时器的计数值.
//返回值：捕获值/计数值（超时的情况下返回）
/******************************得到电容充电时间******************************/

u16 TPAD_getChargeTime(void)
{				   
	TPAD_Reset();//重置电容
	while(TIM_GetFlagStatus(TIM2, TIM_IT_CC1) == RESET)//等待捕获上升沿
	{
		if(TIM_GetCounter(TIM2)>TPAD_ARR_MAX_VAL-500)
			return TIM_GetCounter(TIM2);//超时了,直接返回CNT的值
	};	
	return TIM_GetCapture1(TIM2);	 
} 	




//n：连续获取的次数
//返回值：n次读数里面读到的最大读数值
/*************************读取n次,取最大值***************************/
u16 TPAD_getMaxVal(u8 n)
{
	u16 temp=0;
	u16 res=0; 
	while(n--)
	{
		temp=TPAD_getChargeTime();//得到一次值
		if(temp>res)res=temp;
	};
	return res;
} 




/*************************重置电容电量，先低电平放电再浮空充电***************************/

void TPAD_Reset(void)
{		
	initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_DOWN);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);//输出0,放电
	delay_ms(5);
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //清除中断标志
	TIM_SetCounter(TIM2,0);		//计时器归0
	//根据原理，IO浮空，电容充电
	initGPIO(GPIOA,GPIO_Pin_5,GPIO_Mode_AF,GPIO_AF_TIM2,GPIO_Speed_100MHz,GPIO_OType_PP,GPIO_PuPd_NOPULL);	
}




//mode:0,不支持连续触发(按下一次必须松开才能按下一次);1,支持连续触发(可以一直按下)
//返回值:0,没有按下;1,有按下;										  
#define TPAD_GATE_VAL 	100	//触摸的门限值,也就是必须大于tpad_default_val+TPAD_GATE_VAL,才认为是有效触摸.

/***************************************扫描触摸按键**************************************/

u8 TPAD_Scan(u8 mode)
{
	static u8 keyen=0;	//0,可以开始检测;>0,还不能开始检测	 
	u8 res=0;
	u8 sample=3;		//默认采样次数为3次	 
	u16 rval;
	if(mode)
	{
		sample=6;	//支持连按的时候，设置采样次数为6次
		keyen=0;	//支持连按	  
	}
	rval=TPAD_getMaxVal(sample); 
	if(rval>(TPAD_DEFAULT_VAL+TPAD_GATE_VAL)&&rval<(10*TPAD_DEFAULT_VAL))//大于tpad_default_val+TPAD_GATE_VAL,且小于10倍tpad_default_val,则有效
	{							 
		if((keyen==0)&&(rval>(TPAD_DEFAULT_VAL+TPAD_GATE_VAL)))	//大于tpad_default_val+TPAD_GATE_VAL,有效
		{
			res=1;
		}	   
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//至少要再过3次之后才能按键有效   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 

