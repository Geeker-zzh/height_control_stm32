/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    SPI_FPGA.h
Author:       ����
Date:         2019-9-21
Description:  ��fpga����spiͨ�ŵĽӿڶ������ʼ��
			  
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#ifndef __SPI_FPGA_H
#define __SPI_FPGA_H	 
#include "sys.h"
#include "stm32f10x_spi.h"
//#define SERVO_CLR PDout(3)	// ULN2003 IN1
//#define SERVO_ON  PDout(2)	// ULN2003 IN2	
//#define SERVO_0S  PDout(1)	// ULN2003 IN3	
//#define GOUT1     PDout(0)	// ULN2003 IN4	
//#define GOUT2     PCout(12)	// ULN2003 IN5	
//#define GOUT3     PCout(11)	// ULN2003 IN6
//#define GOUT4     PCout(10)	// ULN2003 IN7	


void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
void SPI_FPGA_Interface_Init(void);//��ʼ��
		 				    
#endif
