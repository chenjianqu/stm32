#include "test.h"
#include "sram.h"
#include "malloc.h"
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h" 
#include "fontupd.h"
#include "text.h"	


void mallocTest(void)
{        
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 paddr[18];				//���P Addr:+p��ַ��ASCIIֵ
	u8 sramx=0;					//Ĭ��Ϊ�ڲ�sram


 	FSMC_SRAM_Init();			//��ʼ���ⲿSRAM  
	
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
	
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"STM32F4");	
	LCD_ShowString(30,70,200,16,16,"MALLOC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@YTCE");
	LCD_ShowString(30,110,200,16,16,"2014/5/15");   
	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Read"); 
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(30,170,200,16,16,"SRAMIN");
	LCD_ShowString(30,190,200,16,16,"SRAMIN  USED:   %");
	LCD_ShowString(30,210,200,16,16,"SRAMEX  USED:   %");
	LCD_ShowString(30,230,200,16,16,"SRAMCCM USED:   %");
 	while(1)
	{	
		key=KEY_Scan(0);//��֧������	
		switch(key)
		{
			case 0://û�а�������	
				break;
			case KEY0_PRES:	//KEY0����
				p=mymalloc(sramx,2048);//����2K�ֽ�
				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//��pд��һЩ����
				break;
			case KEY1_PRES:	//KEY1����	   
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//������ʾ���� 	 
					LCD_ShowString(30,270,200,16,16,p);			 //��ʾP������
				}
				break;
			case KEY2_PRES:	//KEY2����	  
				myfree(sramx,p);//�ͷ��ڴ�
				p=0;			//ָ��յ�ַ
				break;
			case WKUP_PRES:	//KEY UP���� 
				sramx++; 
				if(sramx>2)sramx=0;
				if(sramx==0)LCD_ShowString(30,170,200,16,16,"SRAMIN ");
				else if(sramx==1)LCD_ShowString(30,170,200,16,16,"SRAMEX ");
				else LCD_ShowString(30,170,200,16,16,"SRAMCCM");
				break;
		}
		if(tp!=p)
		{
			tp=p;
			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
			LCD_ShowString(30,250,200,16,16,paddr);	//��ʾp�ĵ�ַ
			if(p)LCD_ShowString(30,270,200,16,16,p);//��ʾP������
		    else LCD_Fill(30,270,239,266,WHITE);	//p=0,�����ʾ
		}
		delay_ms(10);   
		i++;
		if((i%20)==0)//DS0��˸.
		{
			LCD_ShowNum(30+104,190,my_mem_perused(SRAMIN),3,16);//��ʾ�ڲ��ڴ�ʹ����
			LCD_ShowNum(30+104,210,my_mem_perused(SRAMEX),3,16);//��ʾ�ⲿ�ڴ�ʹ����
			LCD_ShowNum(30+104,230,my_mem_perused(SRAMCCM),3,16);//��ʾCCM�ڴ�ʹ����
 			changeLED(0);
 		}
	}	   
}





//ͨ�����ڴ�ӡSD�������Ϣ
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//������ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//����Ե�ַ
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//��ʾ����
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//��ʾ���С
}


void sdTest(void)
{        
	u8 key;		 
	u32 sd_size;
	u8 t=0;	
	u8 *buf;

	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
	
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"SD CARD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@YTCE");
	LCD_ShowString(30,110,200,16,16,"2014/5/15");   
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 0");	   
	
 	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		changeLED(0);//DS0��˸
	}
	
	//show_sdcard_info();	//��ӡSD�������Ϣ
 	POINT_COLOR=BLUE;	//��������Ϊ��ɫ 
	//���SD���ɹ� 											    
	LCD_ShowString(30,150,200,16,16,"SD Card OK    ");
	LCD_ShowString(30,170,200,16,16,"SD Card Size:     MB");
	LCD_ShowNum(30+13*8,170,SDCardInfo.CardCapacity>>20,5,16);//��ʾSD������
	/*
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0������
		{
			buf=mymalloc(0,512);		//�����ڴ�
			if(SD_ReadDisk(buf,0,1)==0)	//��ȡ0����������
			{	
				LCD_ShowString(30,190,200,16,16,"USART1 Sending Data...");
				//printf("SECTOR 0 DATA:\r\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//��ӡ0��������    	   
				//printf("\r\nDATA ENDED\r\n");
				LCD_ShowString(30,190,200,16,16,"USART1 Send Data Over!");
			}
			myfree(0,buf);//�ͷ��ڴ�	   
		}   
		t++;
		delay_ms(10);
		if(t==20)
		{
			changeLED(0);
			t=0;
		}
	} 
	
	*/
	

}













void fatTest(void)
{        
 	u32 total,free;
	u8 t=0;	
	u8 res=0;	


	initW25x();				//��ʼ��W25Q128
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
	
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"STM32F4");	
	LCD_ShowString(30,70,200,16,16,"FATFS TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@YTCE");
	LCD_ShowString(30,110,200,16,16,"2010/9/18");   
	LCD_ShowString(30,130,200,16,16,"Use USMART for test");	   
 	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		changeLED(0);//DS0��˸
	}
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	res=f_mount(fs[1],"1:",1); 				//����FLASH.	
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:YTCE");	//����Flash���̵�����Ϊ��YTCE
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//��ʽ�����
		}else LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}													    
	LCD_Fill(30,150,240,150+16,WHITE);		//�����ʾ			  
	while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,150,240,150+16,WHITE);	//�����ʾ			  
		delay_ms(200);
		changeLED(0);//DS0��˸
	}													  			    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
	LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(30,190,200,16,16,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(30+8*14,170,total>>10,5,16);				//��ʾSD�������� MB
 	LCD_ShowNum(30+8*14,190,free>>10,5,16);					//��ʾSD��ʣ������ MB			    
	while(1)
	{
		t++; 
		delay_ms(200);		 			   
		changeLED(0);
	} 
}






void hzTest(void)
{        
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];//gbk��
	u8 key,t;
	initW25x();				//��ʼ��W25Q128
	//usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	while(font_init()) 			//����ֿ�
	{
UPD:    
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(30,50,200,16,16,"STM32F4");
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}  
	POINT_COLOR=RED;       
	Show_Str(30,50,200,16,"YTCE STM32F407������",16,0);				    	 
	Show_Str(30,70,200,16,"GBK�ֿ���Գ���",16,0);				    	 
	Show_Str(30,90,200,16,"ԭҰ����@YTCE",16,0);				    	 
	Show_Str(30,110,200,16,"2010��8��19��",16,0);
	Show_Str(30,130,200,16,"��KEY0,�����ֿ�",16,0);
 	POINT_COLOR=BLUE;  
	Show_Str(30,150,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,170,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,190,200,16,"���ּ�����:",16,0);

	Show_Str(30,220,200,24,"��Ӧ����Ϊ:",24,0); 
	Show_Str(30,244,200,16,"��Ӧ����(16*16)Ϊ:",16,0);			 
	Show_Str(30,260,200,12,"��Ӧ����(12*12)Ϊ:",12,0);			 
	while(1)
	{
		fontcnt=0;
		for(i=0x81;i<0xff;i++)
		{		
			fontx[0]=i;
			LCD_ShowNum(118,150,i,3,16);		//��ʾ������ֽ�    
			for(j=0x40;j<0xfe;j++)
			{
				if(j==0x7f)continue;
				fontcnt++;
				LCD_ShowNum(118,170,j,3,16);	//��ʾ������ֽ�	 
				LCD_ShowNum(118,190,fontcnt,5,16);//���ּ�����ʾ	 
			 	fontx[1]=j;
				Show_Font(30+132,220,fontx,24,0);	  
				Show_Font(30+144,244,fontx,16,0);	  		 		 
				Show_Font(30+108,260,fontx,12,0);	  		 		 
				t=200;
				while(t--)//��ʱ,ͬʱɨ�谴��
				{
					delay_ms(1);
					key=KEY_Scan(0);
					if(key==KEY0_PRES)goto UPD;
				}
				changeLED(0);
			}   
		}	
	} 
}














