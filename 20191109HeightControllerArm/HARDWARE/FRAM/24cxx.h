/*******************************************************************************************************
Copyright：   @山东大学数控研究中心
File name:    24cxx.h
Author:       张振华
Date:         2018-11-20
Description:  FRAM初始化，读写的API函数声明。
              FRAM的容量是2k Byte（型号MB85RC16）
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
	

void fm24cxx_init(void); //初始化IIC
u8 fm24cxx_check(void);  //检查器件
u8 fm24cxx_read_one_byte(u16 ReadAddr);							//指定地址读取一个字节
void fm24cxx_write_one_byte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void fm24cxx_write_len_byte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 fm24cxx_read_len_byte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据
void fm24cxx_write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
void fm24cxx_read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据

#endif
















