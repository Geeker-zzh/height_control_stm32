/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    AD5530.h
Author:       ����
Date:         2019-09-06
Description:  AD5530�ĳ�ʼ����������
			  ע�⣺�ڱ���Ŀ��AD5530ʹ��ģ��spi
			 
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#ifndef __AD5530_H_
#define __AD5530_H_

#include "stm32f10x.h"

////0
////1			PCout(11)		SPI_SCK
////2			PCout(10)		SET_SDO			DIN
////3			PCout(9)		SPI_DOUT		DOUT
////4			PCout(12)		SET_CS

#define DA_SPI_MISO  PDin(4)  //��DA����������ǿ�©����ģ�Ӧ�ü����������裬����PCB����������ˡ�������
#define DA_SPI_MOSI  PBout(3)
#define DA_SPI_SCK   PDout(5)

#define DA_LDAC     PBout(4)  //����λ�Ĵ������ݣ�ת��ΪDAC�Ĵ������͵�ƽ ��Ч������ һֱΪ���͡�
#define DA_CLR      PBout(5)  //�������ѹ��ΪDUTGND���͵�ƽ��Ч��
#define DA_RBEN     PDout(6)  //DAC�Ĵ�������ʹ�ܣ��͵�ƽ��Ч��ʹ�ܺ�DAC�Ĵ�����MISO������
#define DA_SYNC     PDout(7)  //ʹ�ܺ�������clk�½��أ�����Ĵ档
	



/*
COMMUNICATIONS REGISTER :
CR7   	CR6   	CR5   		CR4   	CR3   	CR2   		CR1   CR0  
WEN(0)   R/W(0)   RS2(0)   RS1(0)   RS0(0)   CREAD(0)   0(0)   0(0)  
STATUS REGISTER :

SR7   		SR6   	SR5   		SR4   	SR3   	SR2   	SR1   		SR0  
RDY(1)   ERR(0)   NOREF(0)   Parity(0)   0(0)  CHD2(0)  CHD1(0)   CHD0(0)  
*/
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

//void AD7190_Mode_Init(void);
void ad5530_interface_init(void);
void ad5530_set_voltage(u16 voltage_data);
//void ad7190_init(void);
//u32 ADC_Num_Raw(void);
////u32 ADC_Num_Raw(void);
//void AveWeightDataGet(void);
//u8 WaitDataRDY(void);//���ʱ��̫������Ҫ����ʱ������,�����жϳ�ȥ
//void WriteToAD7190(unsigned char count, unsigned char *ADbuf);	
//void ReadFromAD7190(unsigned char count, unsigned char *ADbuf);
//unsigned long GET_AD7190(void);
//unsigned long GET_AD7190_1(void);
//void AD7190_Connect_Set(u8 Write_EN,u8 WR,u8 dat);

#endif


