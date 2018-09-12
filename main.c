#include "stm32f4xx_it.h"
#include "led.h"
#include "clockInit.h"
#include "dma.h"
#include "key.h"
#include "test.h"
#include "ultrasonic.h"
#include "matrixkeyboard.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "mpu6050.h"
#include "stmflash.h"
#include "camera.h"



int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	EXTI_DeInit();

	initClock();
	initLED();
	initKey();
	initDelay();
	initUSART(USART1,115200);
	LCD_Init();	  

	
	
	
	__enable_irq();


	//mallocTest();
	//sdTest();
	//fatTest(); 
	
	hzTest();
	
	while(1)
	{
  
	}  
}



