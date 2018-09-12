#include "sram.h"	  
#include "fsmc.h" 


/*
�������ϴ��ص�FLASHоƬ��IS62WV51216��1M �ֽ�
A0~18 Ϊ��ַ�ߣ��ܹ� 19 ����ַ�ߣ��� 2^19=512K�� 1K=1024���� IO0~15 Ϊ�����ߣ�CS2 �� CS1 ����Ƭѡ�źţ����� CS2 �Ǹߵ�ƽ��Ч CS1 �ǵ͵�ƽ��Ч�� 
OE�����ʹ���źţ����źţ��� WE Ϊдʹ���źţ� UB �� LB �ֱ��Ǹ��ֽڿ��ƺ͵��ֽڿ����źţ�UB �� FSMC_NBL1
LB �� FSMC_NBL0
OE �� FSMC_OE
WE �� FSMC_WE
CS �� FSMC_NE3
*/



//ʹ��NOR/SRAM�� Bank1.sector3,��ַλHADDR[27,26]=10 
//��IS61LV25616/IS62WV25616,��ַ�߷�ΧΪA0~A17 
//��IS61LV51216/IS62WV51216,��ַ�߷�ΧΪA0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	


//��ʼ���ⲿSRAM
void FSMC_SRAM_Init(void)
{	
	initFSMC();									
}


	  														  
//��ָ����ַ(WriteAddr+Bank1_SRAM3_ADDR)��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)  
	{										    
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;	  
		WriteAddr++;
		pBuffer++;
	}   
	
}			


//��ָ����ַ((WriteAddr+Bank1_SRAM3_ADDR))��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++;
	}  
} 


//�ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)�������ڴ��С	    
void SRAM_MaxSize(u16 x,u16 y)
{  
	u32 i=0;  	  
	u8 temp=0;	   
	u8 sval=0;	//�ڵ�ַ0����������	  				   
  	LCD_ShowString(x,y,239,y+16,16,"Ex Memory Test:   0KB"); 
	//ÿ��4K�ֽ�,д��һ������,�ܹ�д��256������,�պ���1M�ֽ�
	for(i=0;i<1024*1024;i+=4096)
	{
		FSMC_SRAM_WriteBuffer(&temp,i,1);
		temp++;
	}
	//���ζ���֮ǰд�������,����У��		  
 	for(i=0;i<1024*1024;i+=4096) 
	{
  	FSMC_SRAM_ReadBuffer(&temp,i,1);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;//�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.
		
		LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*4,4,16,0);//��ʾ�ڴ�����  
		
 	}					 
}	
 

