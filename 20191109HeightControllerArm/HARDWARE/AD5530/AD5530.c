/*******************************************************************************************************
Copyright：   @山东大学数控研究中心
File name:    AD5530.c
Author:       张振华
Date:         2019-09-06
Description:  AD7190的初始化函数
			  注意：在本项目中AD7190的模拟spi接口和SD卡的硬件spi接口使用相同
			  引脚，注意这一共享资源的互斥
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/

#include "sys.h"
#include "AD5530.h"//20180925更改ad的增益为可调节，增益为0，对应32（量程大），增益为1对应64（量程小），
#include "math.h"
#include "delay.h"

#define AD7190_Write 				0
#define AD7190_Read 				1
#define AD7190_CLK_SPEED            1

//寄存器选择
#define ComState_register  (0)<<3	//
#define Mode_register  		 (1)<<3
#define Config_register  	 (2)<<3
#define Data_register  	 	 (3)<<3
#define ID_register  	 	 	 (4)<<3
#define GPOCON_register  	 (5)<<3
#define Disorders_register (6)<<3
#define FullScale_register (7)<<3

//模式寄存器
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
//配置寄存器
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
u32 Config_Chop;			//斩波使能
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
u32 tempn;          //采样的AD值，未经过滤波


u8 ADFilterBufFull = 0;//20180926增加传感器滤波。，，平均最大只能设置成29，，再保存和读取参数中处理的。
u8  ADFilter_i=0;//20180926增加传感器滤波。，，平均最大只能设置成29，，再保存和读取参数中处理的。
u32 ADsum = 0;//20180926增加传感器滤波。，，平均最大只能设置成29，，再保存和读取参数中处理的。

void AD7190_Config_Set(AD7190_Config_SET *Mode);//对函数提前声明，因为参数用了自定义的结构体
void AD7190_Mode_Set(AD7190_MODE_SET *Mode);


///****************************************************************************
//* @Author： Zhang Zhenhua
//* @Date:    2019-09-06
//* @F_name:  ad5530_interface_init
//* @brief :  ad5530芯片与stm32的接口初始化
//* @param :  None
//* @retval:  None
//* @note  ： 
//*****************************************************************************/ 
void ad5530_interface_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PE端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 
	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);					
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     //根据设定参数初始化GPIOE
	GPIO_SetBits(GPIOD,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);						          
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     
}
///****************************************************************************
//* @Author： Zhang Zhenhua
//* @Date:    2019-09-06
//* @F_name:  ad5530_set_voltage
//* @brief :  让DA芯片输出可调电压。
//* @param :  voltage_data：2048到4095//3072表示0V
//* @retval:  None
//* @note  ： 不知道为啥，只能通过2048到4095表示-10V到+10V？？
              //输出电压分辨率大概有10mV。
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
 

