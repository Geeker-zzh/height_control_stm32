/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    main.c
Author:       ����
Date:         2019/08/30 16:08 
Description:  ���ݵ���ϵͳ����STM32����
			
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#include "io.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "touch.h" 
#include "24cxx.h"
#include "ad5530.h"
#include "SPI_FPGA.h"


u8 test_num;
extern u8 spi_buf[4];
u8 test_displaybuf[5]={12, 34, 56, 78, 90};
u8 updata[20]={0xa5, 0x0b, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,  0x64, 0x00, 0x5a};
u8 downdata[20] = {0xa5, 0x0b, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,  0x64, 0x00, 0x5a};
u32 g_frequency_data;
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ 
}
////////////////////////////////////////////////////////////////////////////////
//5�����ص����ɫ												 
//���败�������Ժ���
//��Ļ�ֱ���Ϊ240*320
void rtp_test(void)
{
//	u8 key;
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//���
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLACK);		//��ͼ	  			   
			}
		}else delay_ms(10);	//û�а������µ�ʱ�� 	    
//		if(key==KEY0_PRES)	//KEY0����,��ִ��У׼����
//		{
//			LCD_Clear(WHITE);//����
//		    TP_Adjust();  //��ĻУ׼ 
//			TP_Save_Adjdata();	 
//			Load_Drow_Dialog();
//		}
	}
}
void send_command(u8 *CMD)
{
    for(u8 i= 0;i<11;i++)
	{
		USART_SendData(USART1, *CMD++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}
 int main(void)
 { 
//	u8 x=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	 //���ڳ�ʼ��Ϊ9600
	uln2003_init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ�
 	LCD_Init();
	fm24cxx_init();          //��ʼ��24CXX���磬�ڴ�������ʼ��֮ǰ�����ʼ������Ϊ����洢����ĻУ׼���ݡ� 
	tp_dev.init();			 //��������ʼ��
	POINT_COLOR=RED; 
	
    LCD_Clear(WHITE);	
	ad5530_interface_init();
	SPI_FPGA_Interface_Init();
	 
	//MYDMA_Config(DMA1_Channel2,(u32)&SPI1->DR,g_frequency_data,4);//DMA1ͨ��2,����ΪSPI1,�洢��ΪSendBuff,��(TEXT_LENTH+2)*100.
	 
	//DMA_Cmd(DMA1_Channel2, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
  	while(1) 
	{		 
		
		
		g_frequency_data=(spi_buf[3]<<24)+(spi_buf[2]<<16)+(spi_buf[1]<<8)+spi_buf[0];
		 LCD_ShowNum(20,0,g_frequency_data,8,16);
	
//	  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
//		{
//			
//		}	 
//		
//		test_num=DMA_GetFlagStatus(DMA1_FLAG_GL2);
//     	//		
		SERVO_CLR = 1;
		delay_ms(500);	
		SERVO_CLR = 0;		
		delay_ms(500);	
		
		//LCD_ShowString(20,184,84,24,24,test_displaybuf);
//	     LCD_ShowNum(20,120,test_displaybuf[0],2,16);
//		 LCD_ShowNum(20,140,test_displaybuf[1],2,16);
//		 LCD_ShowNum(20,160,test_displaybuf[2],2,16);
//		 LCD_ShowNum(20,180,test_displaybuf[3],2,16);
//		 LCD_ShowNum(20,200,test_displaybuf[4],2,16);
		 
		 
//		 LCD_ShowNum(20,0,spi_buf[0],2,16);
//		 LCD_ShowNum(20,20,spi_buf[1],2,16);
//		 LCD_ShowNum(20,40,spi_buf[2],2,16);
//		 LCD_ShowNum(20,60,spi_buf[3],2,16);
//		 LCD_ShowNum(20,80,spi_buf[4],2,16);
//		 LCD_ShowNum(20,100,spi_buf[5],2,16);
//		 LCD_ShowNum(20,120,spi_buf[6],2,16);
//		 LCD_ShowNum(20,140,spi_buf[7],2,16);
//		 LCD_ShowNum(20,160,spi_buf[8],2,16);
//		 LCD_ShowNum(20,180,spi_buf[9],2,16);
//		 LCD_ShowNum(20,200,spi_buf[10],2,16);
//		 LCD_ShowNum(20,220,spi_buf[11],2,16);
//		 LCD_ShowNum(20,240,spi_buf[12],2,16);
//		 LCD_ShowNum(20,260,spi_buf[13],2,16);
//		 LCD_ShowNum(20,280,spi_buf[14],2,16);
//		 LCD_ShowNum(20,300,spi_buf[15],2,16);
//		 LCD_ShowNum(80,0,spi_buf[16],2,16);
//		 LCD_ShowNum(80,20,spi_buf[17],2,16);
//		 LCD_ShowNum(80,40,spi_buf[18],2,16);
//		 LCD_ShowNum(80,60,spi_buf[19],2,16);
//		 
		 
//		GPIO_ResetBits(GPIOA,GPIO_Pin_8); //LED0�����
//		GPIO_SetBits(GPIOD,GPIO_Pin_2);//LED1�����
//		delay_ms(300);
//		GPIO_SetBits(GPIOA,GPIO_Pin_8);//LED0�����
//		GPIO_ResetBits(GPIOD,GPIO_Pin_2);//LED1�����
//		delay_ms(300);
		
//		if(KEY0 == 0)
//		{
//			 SPI_I2S_SendData(SPI2,0xa00a);
//			GPIO_ResetBits(GPIOA,GPIO_Pin_8); //LED0�����
//		}
		
		
		
////		ad5530_set_voltage(3076);
//		
////		printf("zzh");
////		delay_ms(1000); 
//       tp_dev.scan(0); 		 
//		if(tp_dev.sta&TP_PRES_DOWN)			//������������
//		{	
//		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
//			{	
//				if(tp_dev.x[0]>20&&tp_dev.x[0]<104&&tp_dev.y[0]>160&&tp_dev.y[0]<184) 
//				{
//					SERVO_CLR=1;
//					send_command(updata);
//					delay_ms(100);
//					
//					//
//				}
//				if(tp_dev.x[0]>20&&tp_dev.x[0]<104&&tp_dev.y[0]>184&&tp_dev.y[0]<208)
//				{
//					SERVO_CLR=0;//  
//					send_command(downdata);
//					delay_ms(100);
//				}								   
//			}
//		}
//		
//	   LCD_ShowString(20,160,84,24,24,"button1");
//	   LCD_ShowString(20,184,84,24,24,"button2");
//       
	} 
}
