/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    SPI_FPGA.c
Author:       ����
Date:         2019-9-21
Description:  ��fpga����spiͨ�ŵĽӿڶ������ʼ��
              �����ŷ�����ӿں�IO�ӿڵ��������Ų���
			  
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
//#define SPI_RX_DMA_CHANNEL   DMA1_Channel_2   //DMA1��ͨ��2��Ӧstm32F103��SPI1�Ľ���
//#define SPI_PORT_NAME 		 SPI1
//#define SPI_DMA_IRQ				 DMA2_Stream0_IRQn
//#define SPIRx_BUF_LENGTH 	 6
//#define SPITx_BUF_LENGTH 	 6
//#define SPI_SPI_Align			 1
//#define SPIFPGA_NAME			 SPI1
//#define SPI_DMA_Re_Flag    DMA_FLAG_TCIF3

u8 test_spi;

u8 spi_buf[4]="";
u16 DMA1_MEM_LEN;//����DMAÿ�����ݴ��͵ĳ��� 	
extern u32 g_frequency_data;

  enum  SPIReState
	{
			ALIGN=0,
			RECEIVE=1,
			SEND=2
	} SpiReState=ALIGN;

//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ����� 
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	
    DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA1_MEM_LEN=cndtr;
	
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽������ĳ��˴������ȡ���洢���ڴ�
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	  	
} 
//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_InitTypeDef DMA_InitStructure;
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA1_Channel4,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	  


/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date:    2019-08-30
* @F_name:  SPI_FPGA_Interface_Init
* @brief :  ���ڳ�ʼ��spi1�ӿڣ�������fpgaͨ��
* @param :  None
* @retval:  None
* @note  �� 1��fpga��Ϊ������������stm32���ʹ���������
            2��stm32��spi�ж��д������ݡ�
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	SPI_Cmd(SPI1, DISABLE); //����˵�����Ƚ���

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�.....��֪��Ϊɶ����֮ǰ��̻��������棬����spi��Ȼѡ���high������Ȼ���ã�������
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256,,��̻��ϵľ���2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ//////��̻��ϵ���LSB��������ĵ�
	//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ

	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	//����SPI��DMA
	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;    //����DMA�ж�
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

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽������ĳ��˴������ȡ���洢���ڴ�
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buf;
	DMA_InitStructure.DMA_PeripheralBaseAddr= ((uint32_t)SPI1 + 0x0C);//(uint32_t)(&(SPI_COM_PORT_NAME->DR));
	DMA_InitStructure.DMA_BufferSize = 4;

	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ���Ե���
	DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize =DMA_PeripheralDataSize_Byte;// DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //֮ǰ��normalģʽ�����ڸ��˿�������
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��

	DMA_Init(DMA1_Channel2, &DMA_InitStructure); 
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_RXNE);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
	//����SPI��DMA

	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE, ENABLE);//��״̬��Ȼ���жϴ�״̬����


	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
}
void DMA1_Channel2_IRQHandler(void)//ò��ֻ��stm32F4�����������ĸ��F1����ֻ��ͨ���ĸ���
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
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��

	DMA_Init(DMA1_Channel2, &DMA_InitStructure); 

	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_RXNE);
	//		/* Enable DMA Stream Transfer Complete interrupt */
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
	SPI_Cmd(SPI1, ENABLE);

	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}
//�������spi1���жϷ�����һ��Ҫ�У���Ȼ������SPI1���յ�DMA�жϷ�������������
void SPI1_IRQHandler(void)//ò��ֻ��stm32F4�����������ĸ��F1����ֻ��ͨ���ĸ���
{	
}

 
