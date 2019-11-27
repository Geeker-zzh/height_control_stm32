/*******************************************************************************************************
Copyright：   @山东大学数控研究中心
File name:    io.c
Author:       张振华
Date:         2019-8-30
Description:  板子控制的通用输入输出定义；
              包含伺服电机接口和IO接口的所有引脚操作
			  
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
* @Author： Zhang Zhenhua
* @Date:    2019-08-30
* @F_name:  uln2003_init
* @brief :  用于初始化uln2003相关的gpio
* @param :  None
* @retval:  None
* @note  ： ULN2003是反向输出的。
            当stm32引脚输出高电平时，对应输出的led点亮，uln2003输出低电平。
*****************************************************************************/ 
void uln2003_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PA,PD端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;	  //
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz

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
 
