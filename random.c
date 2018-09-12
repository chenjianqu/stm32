#include "random.h"
#include "stm32f4xx_it.h"
#include "delay.h"

#define return_ok 1
#define return_fail 0

u8 initRandom(void)
{
	u16 retry=0;
	//开启RNG时钟，来自PLL48CLK
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
	
	//使能RNG
	RNG_Cmd(ENABLE);
	
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)//等待随机数就绪
	{
		retry++;
		delay_us(100);
	}
	if(retry>=100000)return return_fail;//一秒后还未产生随机数则认为随机数发生器工作不正常
	
	return return_ok;
}

u32 getRandom(void)
{
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);//等待随机数就位
	
	return RNG_GetRandomNumber();
}

int getIntervalRandom(int min,int max)
{
	return getRandom()%(max-min+1)+min;
}


