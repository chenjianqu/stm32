#include "iwdg.h"
#include "stm32f4xx_it.h"
/**独立于主程序运行
独立看门狗的时钟来自LSI(低速内部时钟,32kHz)经过预分频器(预分频寄存器IWDG_PR设置)后给递减计数器
向键值寄存器（IWDG_KR）中写入0xCCCC，启用独立看门狗
此时计数器会从复位值0xfff递减，当到0时产生复位信号
无论何时向键值寄存器中写入0xAAAA(喂狗)，自动重装载寄存器(IWDG_RLR)的值便会重新加载到计数器，从而避免看门狗复位。
**/

//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).


void initIWDG(u8 prer,u16 rlr)
{
	//预分频寄存器和重装载寄存器具有写保护功能，要操作先取消写保护功能（向IWDG_KR中写入0x55555）
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
	
	IWDG_SetPrescaler(prer); //设置IWDG分频系数

	IWDG_SetReload(rlr);   //设置IWDG装载值

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //使能看门狗
}

//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}
