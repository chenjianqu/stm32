#include "nvic.h"

/*中断向量表放在启动文件*/

/*NVIC:嵌套向量中断控制器
NVIC把中断向量分为5个组：
组	AIRCR[10：8]	IP bit[7：4]分配情况				分配结果
0			111							0：4									0位抢占优先级，4位响应优先级
1			110							1：3									1位抢占优先级，3位响应优先级
2			101							2：2									2位抢占优先级，2位响应优先级
3			100							3：1									3位抢占优先级，1位响应优先级
4			011							4：0									4位抢占优先级，0位响应优先级
*/

/*
高优先级的抢占优先级是可以打断正在进行的低抢占优先级中断的。
抢占优先级相同的中断，高响应优先级不可以打断低响应优先级的中断。
抢占优先级相同的中断，当两个中断同时发生的情况下，哪个响应优先级高，哪个先执行。
如果两个中断的抢占优先级和响应优先级都是一样的话，则看哪个中断先发生就先执行；
*/

//配置中断向量
void initNVIC(uint8_t IRQChannel,uint8_t PreemptionPriority,uint8_t SubPriority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=PreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=SubPriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}







