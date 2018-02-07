#include "gpio.h"

/*
GPIO的八种工作模式：
（1） GPIO_Mode_AIN 				模拟输入

（2） GPIO_Mode_IN_FLOATING 浮空输入

（3） GPIO_Mode_IPD 				下拉输入

（4） GPIO_Mode_IPU 				上拉输入

（5） GPIO_Mode_Out_OD 			开漏输出
输出端相当于三极管的集电极，要得到高电平状态需上拉电阻，适合电流型驱动，吸收电流能力较强（20mA以内）
特点：1. 利用外部电路的驱动能力，减少 IC内部的驱动。当 IC 内部 MOSFET 导通时，驱动电流是从外部的
VCC 流经 R pull-up ， MOSFET 到 GND。 IC 内部仅需很下的栅极驱动电流。2. 一般来说，开漏是用来连接不同电平的器件，匹配电平用的，因为开漏引脚不连接外部的上拉电阻时，
只能输出低电平，如果需要同时具备输出高电平的功能，则需要接上拉电阻，很好的一个优点是通过改变
上拉电源的电压，便可以改变传输电平。比如加上上拉电阻就可以提供 TTL/CMOS 电平输出等。（上拉电阻的阻值决定了逻辑电平转换的沿的速度 。阻值越大，速度越低功耗越小， 
所以负载电阻的选择要兼顾功耗和速度。 ）
3. OPEN-DRAIN 提供了灵活的输出方式，但是也有其弱点，就是带来上升沿的延时。因为上升沿是通过外接上拉无源电阻对负载充电，所以当电阻选择小时延时就小，但功耗大；
反之延时大功耗小。所以如果对延时有要求，则建议用下降沿输出。
4. 可以将多个开漏输出的 Pin，连接到一条线上。通过一只上拉电阻，在不增加任何器件的情况下，形成“与逻辑”关系。

（6） GPIO_Mode_Out_PP 			推挽输出
推挽电路是两个参数相同的三极管或 MOSFET,以推挽方式存在于电路中,各负责正负半周的波形放大任
务,电路工作时，两只对称的功率开关管每次只有一个导通，所以导通损耗小、效率高。输出既可以向负载
灌电流，也可以从负载抽取电流。推拉式输出级既提高电路的负载能力，又提高开关速度。

（7） GPIO_Mode_AF_OD 			复用开漏输出

（8） GPIO_Mode_AF_PP 			复用推挽输出

*/
/*
在 STM32 中选用 IO 模式
（1） 浮空输入_IN_FLOATING ——浮空输入，可以做 KEY 识别， RX1
（2）带上拉输入_IPU——IO 内部上拉电阻输入
（3）带下拉输入_IPD—— IO 内部下拉电阻输入
（4） 模拟输入_AIN ——应用 ADC 模拟输入，或者低功耗下省电
（5）开漏输出_OUT_OD ——IO 输出 0 接 GND， IO 输出 1，悬空，需要外接上拉电阻，才能实现输出
高电平。当输出为 1 时， IO 口的状态由上拉电阻拉高电平，但由于是开漏输出模式，这样 IO 口也就可以
由外部电路改变为低电平或不变。可以读 IO 输入电平变化，实现 C51 的 IO 双向功能
（6）推挽输出_OUT_PP ——IO 输出 0-接 GND， IO 输出 1 -接 VCC，读输入值是未知的
（7）复用功能的推挽输出_AF_PP ——片内外设功能（I2C 的 SCL,SDA）
（8）复用功能的开漏输出_AF_OD——片内外设功能（TX1,MOSI,MISO.SCK.SS）
STM32 设置实例
*/
/*
注意 PD0,PD1 用于输出地时候仅能用于 50MHz 输出模式.
注意:PC13,PC14,PC15 只能用于 2MHz 的输出模式,,最多只能带 30pf 的负载,并且同时只能使用一个引脚
*/
/*
GPIO的速度是指I/O口驱动电路的速度，是用来选择不同的输出驱动模块，达到最佳的噪声控制和降低功耗的目的。
高频的驱动电路，噪声也高，当你不需要高的输出频率时，请选用低频驱动电路，这样非常有利于提高系统的EMI性能。
当然如果你要输出较高频率的信号，但却选用了较低频率的驱动模块，你很可能会得到失真的输出信号。
	2		MHZ
	25	MHz
	50	MHz
	100	MHz
*/
/*
	开漏输出  只可以输出强低电平，高电平得靠外部电阻拉高。输出端相当于三极管的集电极. 要得到高电平状态需要上拉电阻才行. 适合于做电流型的驱动,其吸收电流的能力相对强(一般20ma以内)
	推挽输出  可以输出强高低电平，连接数字器件
	复用输出：可以理解为GPIO口被用作第二功能时的配置情况（即并非作为通用IO口使用）。端口必须配置成复用功能输出模式(推挽或开漏)。
*/

void initGPIO(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin_x,GPIOMode_TypeDef GPIO_Mode_x,uint8_t GPIO_AF,GPIOSpeed_TypeDef GPIO_Speed_xMHz,GPIOOType_TypeDef GPIO_OType_x,GPIOPuPd_TypeDef GPIO_PuPd_x)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//使能GPIO时钟，AHB1总线上，GPIOA时钟使能地址位是为0位，GPIOB是1位。GPIOC是2位，依次类推
		if(GPIOx==GPIOA)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		else if(GPIOx==GPIOB)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		else if(GPIOx==GPIOC)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		else if(GPIOx==GPIOD)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		else if(GPIOx==GPIOE)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		else if(GPIOx==GPIOF)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		else if(GPIOx==GPIOG)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
		else if(GPIOx==GPIOH)RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
		
		//配置GPIO复用功能
		if(GPIO_Mode_x!=AF_DISABLE)
		{
			u16 temp=GPIO_Pin_x;
			u8 GPIO_PinSourcex=0;
			//GPIO_Pin和GPIO_PinSource之间具有对应关系
			while(temp){
				temp/=2;
				GPIO_PinSourcex++;
			}
			GPIO_PinSourcex--;
			GPIO_PinAFConfig(GPIOx,GPIO_PinSourcex,GPIO_AF);
		}
	
		//配置GPIO
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_x;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_x;
  		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_xMHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_x;	
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_x;	
  		GPIO_Init(GPIOx, &GPIO_InitStructure);
}

//改变GPIO的输出模式,使用本函数前请先初始化对应的GPIO,第二个参数是第几个io
void changeGPIO_Mode(GPIO_TypeDef* GPIOx,u8 pin,GPIOMode_TypeDef GPIO_Mode_x)
{
	//参照GPIO_Init函数写的
	if(GPIO_Mode_x==GPIO_Mode_OUT)
	{
		GPIOx->MODER&=~(3<<(pin*2));
		GPIOx->MODER|=1<<pin*2;
	}
	else if(GPIO_Mode_x==GPIO_Mode_IN)
	{
		GPIOx->MODER&=~(3<<(pin*2));
		GPIOx->MODER|=0<<pin*2;
	}
}


//将u16 GPIO_Pin_x转换成第几个io
u8 turnPinNum(u16 GPIO_Pin_x)
{
	u8 pin=0;
	while(GPIO_Pin_x)
	{
		GPIO_Pin_x/=2;
		pin++;
	}
	return pin;
}



