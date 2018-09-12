#include "stepper.h"
#include "led.h"

//�ڲ�����
void setStepperGPIO(u8 A,u8 B,u8 C,u8 D);

/*
ʹ�� TIM7������ʱ�� ���в���������٣���������ٶ�Խ�죬Ť��ԽС
���������һ���ų������Ա仯�����������򵼵�״̬��n��ʾ����ָ���ת��һ���ݾ��������������
��������Ϊ�����������������з�ʽ��AB-BC-CD-DA-AB������������з�ʽ�� A-AB-B-BC-C-CD-D-DA-A.
*/

u8 stepper_beatMode;
u8 stepper_beat=0;

GPIO_TypeDef* stepper_GPIOx_A;
GPIO_TypeDef* stepper_GPIOx_B;
GPIO_TypeDef* stepper_GPIOx_C;
GPIO_TypeDef* stepper_GPIOx_D;
uint16_t stepper_GPIO_Pin_x_A;
uint16_t stepper_GPIO_Pin_x_B;
uint16_t stepper_GPIO_Pin_x_C;
uint16_t stepper_GPIO_Pin_x_D;

/*****��ʼ�����������GPIO�Ͷ�ʱ��*****/
void initStepper(u8 stepper_beats_x,GPIO_TypeDef* GPIOx_A,uint16_t GPIO_Pin_x_A,GPIO_TypeDef* GPIOx_B,uint16_t GPIO_Pin_x_B,GPIO_TypeDef* GPIOx_C,uint16_t GPIO_Pin_x_C,GPIO_TypeDef* GPIOx_D,uint16_t GPIO_Pin_x_D)
{
	stepper_beatMode=stepper_beats_x;
	
	stepper_GPIOx_A=GPIOx_A;
	stepper_GPIOx_B=GPIOx_B;
	stepper_GPIOx_C=GPIOx_C;
	stepper_GPIOx_D=GPIOx_D;
	stepper_GPIO_Pin_x_A=GPIO_Pin_x_A;
	stepper_GPIO_Pin_x_B=GPIO_Pin_x_B;
	stepper_GPIO_Pin_x_C=GPIO_Pin_x_C;
	stepper_GPIO_Pin_x_D=GPIO_Pin_x_D;
	
	//����˽�������� ����������������ģʽʱ����ߵ��裬������������ģʽ������͵���
	initGPIO(GPIOx_A,GPIO_Pin_x_A,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_B,GPIO_Pin_x_B,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_C,GPIO_Pin_x_C,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);
	initGPIO(GPIOx_D,GPIO_Pin_x_D,GPIO_Mode_OUT,AF_DISABLE,GPIO_Speed_50MHz,GPIO_OType_PP,GPIO_PuPd_UP);

	//��ʼ����ʱ��7
	//10kHz,0.1ms,  100ms
	initTIMBase(TIM7,8399,TIM_CounterMode_Up,999,TIM_CKD_DIV1,0);
	initNVIC(TIM7_IRQn,1,1);
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
}


/*
TIM_FLAG_Update�������������/�����������������ʼ��(ͨ����������ڲ�/�ⲿ����)  
TIM_FLAG_Trigger�����¼�(������������ֹͣ����ʼ���������ڲ�/�ⲿ��������) ,�����Ƿ�ֹ����;�޸Ķ�ʱ������ֵʲô��
*/

/***************************************��ʱ��7�ж�*************************************/
void TIM7_IRQHandler(void){
	if(TIM_GetFlagStatus(TIM7,TIM_IT_Update)){
		changeLED(3);
		USART_SendData(USART1,'^');
		
		//���������4�ĵķ�ʽ���У�AB-BC-CD-DA-AB
			if(stepper_beatMode==STEPPER_BEAT_4)
			{
				switch(stepper_beat)
				{
					case 0:
						setStepperGPIO(HIGH,HIGH,LOW,LOW);
						break;
					case 1:
						setStepperGPIO(LOW,HIGH,HIGH,LOW);
						break;
					case 2:
						setStepperGPIO(LOW,LOW,HIGH,HIGH);
						break;
					case 3:
						setStepperGPIO(HIGH,LOW,LOW,HIGH);
						break;
				}
				stepper_beat++;
				
				if(stepper_beat>3)
					stepper_beat=0;
			}
			
			//���������8�ĵķ�ʽ���У�A-AB-B-BC-C-CD-D-DA-A
			else if(stepper_beatMode==STEPPER_BEAT_8)
			{
				switch(stepper_beat)
				{
					case 0:
						setStepperGPIO(HIGH,LOW,LOW,LOW);
						break;
					case 1:
						setStepperGPIO(HIGH,HIGH,LOW,LOW);
						break;
					case 2:
						setStepperGPIO(LOW,HIGH,LOW,LOW);
						break;
					case 3:
						setStepperGPIO(LOW,HIGH,HIGH,LOW);
						break;
					case 4:
						setStepperGPIO(LOW,LOW,HIGH,LOW);
						break;
					case 5:
						setStepperGPIO(LOW,LOW,HIGH,HIGH);
						break;
					case 6:
						setStepperGPIO(LOW,LOW,LOW,HIGH);
						break;
					case 7:
						setStepperGPIO(HIGH,LOW,LOW,HIGH);
						break;
				}
				stepper_beat++;
				if(stepper_beat>7)
					stepper_beat=0;
		}
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	}
}





/***************************���ò������ÿ�ĸ����ߵĵ�ƽ*************************/
void setStepperGPIO(u8 A,u8 B,u8 C,u8 D)
{
	if(A==HIGH)
		GPIO_SetBits(stepper_GPIOx_A,stepper_GPIO_Pin_x_A);
	else
		GPIO_ResetBits(stepper_GPIOx_A,stepper_GPIO_Pin_x_A);
	
	if(B==HIGH)
		GPIO_SetBits(stepper_GPIOx_B,stepper_GPIO_Pin_x_B);
	else
		GPIO_ResetBits(stepper_GPIOx_B,stepper_GPIO_Pin_x_B);
	
	if(C==HIGH)
		GPIO_SetBits(stepper_GPIOx_C,stepper_GPIO_Pin_x_C);
	else
		GPIO_ResetBits(stepper_GPIOx_C,stepper_GPIO_Pin_x_C);
	
	if(D==HIGH)
		GPIO_SetBits(stepper_GPIOx_D,stepper_GPIO_Pin_x_D);
	else
		GPIO_ResetBits(stepper_GPIOx_D,stepper_GPIO_Pin_x_D);
}


/***************************�ı䲽�������ת���ٶ�*************************/
void setStepperSpeed(u16 period)
{
	TIM_Cmd(TIM7,DISABLE);
	//�����Զ���װ��ֵ
	TIM_SetAutoreload(TIM7,period);
	
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_Cmd(TIM7,ENABLE);
}


/***************************�����������*************************/
void startStepper(void)
{
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);
	TIM_Cmd(TIM7,ENABLE);
}

/***************************��ͣ�������*************************/
void stopStepper(void)
{
	TIM_Cmd(TIM7,DISABLE);
}

