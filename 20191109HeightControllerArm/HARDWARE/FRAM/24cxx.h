/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    24cxx.h
Author:       ����
Date:         2018-11-20
Description:  FRAM��ʼ������д��API����������
              FRAM��������2k Byte���ͺ�MB85RC16��
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#ifndef __24CXX_H
#define __24CXX_H
#include "myiic.h"  
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define FM24C16		2047
#define EE_TYPE FM24C16
	

void fm24cxx_init(void); //��ʼ��IIC
u8 fm24cxx_check(void);  //�������
u8 fm24cxx_read_one_byte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void fm24cxx_write_one_byte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
void fm24cxx_write_len_byte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 fm24cxx_read_len_byte(u16 ReadAddr,u8 Len);					//ָ����ַ��ʼ��ȡָ����������
void fm24cxx_write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
void fm24cxx_read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

#endif
















