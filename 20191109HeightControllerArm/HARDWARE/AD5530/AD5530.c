/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    AD5530.c
Author:       ����
Date:         2019-09-06
Description:  AD7190�ĳ�ʼ������
			  ע�⣺�ڱ���Ŀ��AD7190��ģ��spi�ӿں�SD����Ӳ��spi�ӿ�ʹ����ͬ
			  ���ţ�ע����һ������Դ�Ļ���
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/

#include "sys.h"
#include "AD5530.h"//20180925����ad������Ϊ�ɵ��ڣ�����Ϊ0����Ӧ32�����̴󣩣�����Ϊ1��Ӧ64������С����
#include "math.h"
#include "delay.h"

#define AD7190_Write 				0
#define AD7190_Read 				1
#define AD7190_CLK_SPEED            1

//�Ĵ���ѡ��
#define ComState_register  (0)<<3	//
#define Mode_register  		 (1)<<3
#define Config_register  	 (2)<<3
#define Data_register  	 	 (3)<<3
#define ID_register  	 	 	 (4)<<3
#define GPOCON_register  	 (5)<<3
#define Disorders_register (6)<<3
#define FullScale_register (7)<<3

//ģʽ�Ĵ���
#define MODE_ADC_OneByOne 	(0)<<21
#define MODE_ADC_OnlyOne 		(1)<<21
#define MODE_ADC_Free 			(2)<<21
#define MODE_ADC_SavePower 	(3)<<21
#define MODE_ADC_AdjustZero (4)<<21
#define MODE_ADC_AdjustFull (5)<<21
#define MODE_ADC_SysAdjustZero (6)<<21
#define MODE_ADC_SysAdjustFull (7)<<21
#define MODE_MCLK_OUTosc		(0)<<18
#define MODE_MCLK_OUTclo		(1)<<18
#define MODE_MCLK_IN				(2)<<18
#define MODE_MCLK_INcloOut	(3)<<18
#define MODE_SINC3					(1)<<15
#define MODE_ENPAR					(1)<<13
#define MODE_Single					(1)<<11
#define MODE_REJ60					(1)<<10
//#define MODE_Filter_Speed		0
//���üĴ���
#define Config_Chop_EN			(1)<<23
#define Config_REFSEL_IO		(1)<<20
#define Config_Burn_EN			(1)<<7
#define Config_REFDET_EN		(1)<<6
#define Config_BUF_EN				(1)<<4
#define Config_UB_EN				(1)<<3

#define Config_Ch0_A1A2			(1)<<8
#define Config_Ch1_A3A4			(1)<<9
#define Config_Ch2_temp			(1)<<10
#define Config_Ch3_A2A2			(1)<<11
#define Config_Ch4_A1AC			(1)<<12
#define Config_Ch5_A2AC			(1)<<13
#define Config_Ch6_A3AC			(1)<<14
#define Config_Ch7_A4AC			(1)<<15

#define Config_ADC_Gain_1			0
#define Config_ADC_Gain_8			3
#define Config_ADC_Gain_16		4
#define Config_ADC_Gain_32		5
#define Config_ADC_Gain_64		6
#define Config_ADC_Gain_128		7


typedef struct {
u32 ADC_Mode;
u32 Return_state;
u32 ADC_SCLK;
u32 SINC3_EN;
u32 ENPAR;
u32 Single_EN;
u32 REJ60_EN;
u32 Filter;
}AD7190_MODE_SET;
typedef struct {
u32 Config_Channel;
u32 Config_ADC_Gain;	
u32 Config_Chop;			//ն��ʹ��
u32 Config_REFSEL;
u32 Config_Burn;
u32 Config_REFDET;
u32 Config_BUF;
u32 Config_UB;
}AD7190_Config_SET;

u8 ADC_Channel,text;
u8 shuju;
u32 ADC_Gain;
u8 Use_Ch = 0;
u16 NoReadCount = 0;
u16 OnlyReadCh0Count = 0;
u16 OnlyReadCh1Count = 0;
u32 tempn;          //������ADֵ��δ�����˲�


u8 ADFilterBufFull = 0;//20180926���Ӵ������˲�������ƽ�����ֻ�����ó�29�����ٱ���Ͷ�ȡ�����д���ġ�
u8  ADFilter_i=0;//20180926���Ӵ������˲�������ƽ�����ֻ�����ó�29�����ٱ���Ͷ�ȡ�����д���ġ�
u32 ADsum = 0;//20180926���Ӵ������˲�������ƽ�����ֻ�����ó�29�����ٱ���Ͷ�ȡ�����д���ġ�

void AD7190_Config_Set(AD7190_Config_SET *Mode);//�Ժ�����ǰ��������Ϊ���������Զ���Ľṹ��
void AD7190_Mode_Set(AD7190_MODE_SET *Mode);


///****************************************************************************
//* @Author�� Zhang Zhenhua
//* @Date:    2019-09-06
//* @F_name:  ad5530_interface_init
//* @brief :  ad5530оƬ��stm32�Ľӿڳ�ʼ��
//* @param :  None
//* @retval:  None
//* @note  �� 
//*****************************************************************************/ 
void ad5530_interface_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PE�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 
	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);					
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     //�����趨������ʼ��GPIOE
	GPIO_SetBits(GPIOD,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);						          
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     
}
///****************************************************************************
//* @Author�� Zhang Zhenhua
//* @Date:    2019-09-06
//* @F_name:  ad5530_set_voltage
//* @brief :  ��DAоƬ����ɵ���ѹ��
//* @param :  voltage_data��2048��4095//3072��ʾ0V
//* @retval:  None
//* @note  �� ��֪��Ϊɶ��ֻ��ͨ��2048��4095��ʾ-10V��+10V����
              //�����ѹ�ֱ��ʴ����10mV��
//*****************************************************************************/ 
void ad5530_set_voltage(u16 voltage_data)
{
	int i;
	DA_SYNC = 0;
	voltage_data <<= 4;
	for (i=15;i>=0;i--)
	{
		DA_SPI_SCK=0;
		if(0x8000 == (voltage_data & 0x8000))
		{
			DA_SPI_MOSI = 1;	  //Send one to SDO pin
		}
		else
		{
			DA_SPI_MOSI = 0;	  //Send zero to SDO pin
		}
		delay_us(1);
		DA_SPI_SCK=1;
		delay_us(1);
		voltage_data <<= 1;
	}
	
	DA_SYNC = 1;
	DA_LDAC = 0;
	
	
}
 

