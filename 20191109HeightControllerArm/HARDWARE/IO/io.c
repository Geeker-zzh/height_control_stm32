/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    io.c
Author:       ����
Date:         2019-8-30
Description:  ���ӿ��Ƶ�ͨ������������壻
              �����ŷ�����ӿں�IO�ӿڵ��������Ų���
			  
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/

#include "io.h"
#include "delay.h"
#define SERVO_CLR PDout(3)	// ULN2003 IN1
#define SERVO_ON  PDout(2)	// ULN2003 IN2	
#define SERVO_0S  PDout(1)	// ULN2003 IN3	
#define GOUT1     PDout(0)	// ULN2003 IN4	
#define GOUT2     PCout(12)	// ULN2003 IN5	
#define GOUT3     PCout(11)	// ULN2003 IN6
#define GOUT4     PCout(10)	// ULN2003 IN7	

/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date:    2019-08-30
* @F_name:  uln2003_init
* @brief :  ���ڳ�ʼ��uln2003��ص�gpio
* @param :  None
* @retval:  None
* @note  �� ULN2003�Ƿ�������ġ�
            ��stm32��������ߵ�ƽʱ����Ӧ�����led������uln2003����͵�ƽ��
*****************************************************************************/ 
void uln2003_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PA,PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;	  //
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz

	SERVO_CLR = 1;
	delay_ms(20);
	SERVO_ON = 1; 
	delay_ms(20);
	SERVO_0S = 1; 	
	delay_ms(20);
	GOUT1    = 1; 	
	delay_ms(20);
	GOUT2    = 1; 
	delay_ms(20);
	GOUT3    = 1; 
	delay_ms(20);
	GOUT4    = 1; 
	SERVO_CLR = 0;
	delay_ms(20);
	SERVO_ON = 0; 
	delay_ms(20);
	SERVO_0S = 0; 	
	delay_ms(20);
	GOUT1    = 0; 	
	delay_ms(20);
	GOUT2    = 0; 
	delay_ms(20);
	GOUT3    = 0; 
	delay_ms(20);
	GOUT4    = 0; 
	
}
 
