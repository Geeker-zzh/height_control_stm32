/*******************************************************************************************************
Copyright：   @山东大学数控研究中心
File name:    SPI_FPGA.c
Author:       张振华
Date:         2019-9-21
Description:  与fpga进行spi通信的接口定义与初始化
              包含伺服电机接口和IO接口的所有引脚操作
			  
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/

#include "SPI_FPGA.h"
#include "delay.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_dma.h"
#include "io.h"
#include "lcd.h"
//#define SERVO_CLR PDout(3)	// ULN2003 IN1
//#define SERVO_ON  PDout(2)	// ULN2003 IN2	
//#define SERVO_0S  PDout(1)	// ULN2003 IN3	
//#define GOUT1     PDout(0)	// ULN2003 IN4	
//#define GOUT2     PCout(12)	// ULN2003 IN5	
//#define GOUT3     PCout(11)	// ULN2003 IN6
//#define GOUT4     PCout(10)	// ULN2003 IN7	

//#define SPI_RX_DMA_STREAM	 DMA1_Channel2    
//#define SPI_RX_DMA_CHANNEL   DMA1_Channel_2   //DMA1的通道2对应stm32F103的SPI1的接收
//#define SPI_PORT_NAME 		 SPI1
//#define SPI_DMA_IRQ				 DMA2_Stream0_IRQn
//#define SPIRx_BUF_LENGTH 	 6
//#define SPITx_BUF_LENGTH 	 6
//#define SPI_SPI_Align			 1
//#define SPIFPGA_NAME			 SPI1
//#define SPI_DMA_Re_Flag    DMA_FLAG_TCIF3

u8 test_spi;

u8 spi_buf[4]="";
u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 	
extern u32 g_frequency_data;

  enum  SPIReState
	{
			ALIGN=0,
			RECEIVE=1,
			SEND=2
	} SpiReState=ALIGN;

//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量 
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
    DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA1_MEM_LEN=cndtr;
	
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，这里改成了从外设读取，存储到内存
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	  	
} 
//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_InitTypeDef DMA_InitStructure;
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA1_Channel4,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	  


/****************************************************************************
* @Author： Zhang Zhenhua
* @Date:    2019-08-30
* @F_name:  SPI_FPGA_Interface_Init
* @brief :  用于初始化spi1接口，用于与fpga通信
* @param :  None
* @retval:  None
* @note  ： 1、fpga作为主机，主动往stm32发送传感器数据
            2、stm32在spi中断中处理数据。
			//SPI1_NSS  PA4   pin39
		    //SPI1_SCK  PA5   pin38
			//SPI1_MISO PA6   pin34
			//SPI1_MOSI PA7   pin33
*****************************************************************************/ 
void  SPI_FPGA_Interface_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;	

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1, ENABLE );
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	SPI_Cmd(SPI1, DISABLE); //网上说必须先禁用

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//设置SPI工作模式:设置为从SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//选择了串行时钟的稳态:时钟悬空高.....不知道为啥，在之前雕刻机板子上面，配置spi竟然选择的high。。竟然能用？？？？
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256,,雕刻机上的就是2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始//////雕刻机上的是LSB，这里更改掉
	//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式

	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	//配置SPI的DMA
	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;    //设置DMA中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);   

	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 	

	DMA_DeInit(DMA1_Channel2);											//SPI1 DMA  rx

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，这里改成了从外设读取，存储到内存
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buf;
	DMA_InitStructure.DMA_PeripheralBaseAddr= ((uint32_t)SPI1 + 0x0C);//(uint32_t)(&(SPI_COM_PORT_NAME->DR));
	DMA_InitStructure.DMA_BufferSize = 4;

	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址可以递增
	DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize =DMA_PeripheralDataSize_Byte;// DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //之前是normal模式，现在改了看看！！
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输

	DMA_Init(DMA1_Channel2, &DMA_InitStructure); 
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_RXNE);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
	//配置SPI的DMA

	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE, ENABLE);//新状态依然是中断打开状态！！


	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
}
void DMA1_Channel2_IRQHandler(void)//貌似只有stm32F4才有数据流的概念，F1里面只有通道的概念
{
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	DMA_InitTypeDef  DMA_InitStructure;	
	SERVO_ON=~SERVO_ON;//
	DMA_DeInit(DMA1_Channel2);											//??DMA,?????????

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buf;
	DMA_InitStructure.DMA_PeripheralBaseAddr= ((uint32_t)SPI1 + 0x0C);//(uint32_t)(&(SPI_COM_PORT_NAME->DR));
	DMA_InitStructure.DMA_BufferSize = 4;

	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize =DMA_PeripheralDataSize_Byte;// DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode =DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输

	DMA_Init(DMA1_Channel2, &DMA_InitStructure); 

	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_RXNE);
	//		/* Enable DMA Stream Transfer Complete interrupt */
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
	SPI_Cmd(SPI1, ENABLE);

	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}
//下面这个spi1的中断服务函数一定要有，不然进不了SPI1接收的DMA中断服务函数！！！！
void SPI1_IRQHandler(void)//貌似只有stm32F4才有数据流的概念，F1里面只有通道的概念
{	
}

 
