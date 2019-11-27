/*******************************************************************************************************
Copyright：   @山东大学数控研究中心
File name:    24cxx.c
Author:       张振华
Date:         2018-11-19
Description:  FRAM初始化，读写的API函数实现。
              FRAM的容量是2k Byte（型号MB85RC16）
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#include "sys.h"
#include "24cxx.h" 

/****************************************************************************
* @Author： Zhang Zhenhua
* @Date:    2018-11-19
* @F_name:  fm24cxx_init
* @brief :  FRAM的IIC接口初始化
* @param :  None
* @retval:  None
* @note  ： 用于初始化FRAM的模拟IIC接口
*****************************************************************************/ 
void fm24cxx_init(void)
{
	IIC_Init();
}

/****************************************************************************
* @Author： Zhang Zhenhua
* @Date  :  2018-11-19
* @F_name:  fm24cxx_check
* @brief :  检测FRAM是否连接良好
* @param :  None
* @retval:  
            0：FRAM正常读写
            1：FRAM读写失败
* @note  ： 这里用了24XX的最后一个地址(2047)来存储标志字,如果用其他24C系列,
			这个地址要修改
*****************************************************************************/
u8 fm24cxx_check(void)
{
	u8 temp;
	temp=fm24cxx_read_one_byte(2047);//避免每次开机都写FRAM		   
	if(temp==0X55)return 0;		   
	else						   //排除第一次初始化的情况
	{
		fm24cxx_write_one_byte(2047,0X55);
		temp=fm24cxx_read_one_byte(2047);	  
		if(temp==0X55)return 0;
	}
	return 1;	
}

/****************************************************************************
* @Author： Zhang Zhenhua
* @Date:    2018-11-19
* @F_name:  fm24cxx_read_one_byte
* @brief :  在FRAM的指定地址读出一个字节的数据
* @param :  ReadAddr：0~2048
* @retval:  读上来的一个字节数据
* @note  ： 用于初始化FRAM的模拟IIC接口
            之前正点原子自带的驱动有问题，在256字节以后的数据不能正常读取，
            这里打了补丁。
*****************************************************************************/
u8 fm24cxx_read_one_byte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   	
	IIC_Send_Byte(0XA1+((ReadAddr/256)<<1)); //以前是 IIC_Send_Byte(0XA1);导致不超过256字节的地址没有读取到，，实际上写入已经没有问题了。	
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();                    //产生一个停止条件	    
	return temp;
}

/****************************************************************************
* @Author： Zhang Zhenhua
* @Date:    2018-11-19
* @F_name:  fm24cxx_write_one_byte
* @brief :  在FRAM的指定地址写入一个字节的数据
* @param :  WriteAddr：0~2048
* @retval:  None
* @note  ： 用于初始化FRAM的模拟IIC接口
            之前正点原子自带的驱动有问题，在256字节以后的数据不能正常读取，
            这里打了补丁。
*****************************************************************************/
void fm24cxx_write_one_byte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack(); 
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	 
}
/****************************************************************************
* @Author： Zhang Zhenhua
* @Date  :  2018-11-19
* @F_name:  fm24cxx_write_len_byte
* @brief :  在FRAM的指定地址写入连续几个字节的数据
* @param :  WriteAddr：0~2047
			DataToWrite：要写入数据数组首地址
			Len：要写入数据的长度2,4
* @retval:  None
* @note  ： 该函数用于写入16bit或者32bit的数据.
*****************************************************************************/
void fm24cxx_write_len_byte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		fm24cxx_write_one_byte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/****************************************************************************
* @Author： Zhang Zhenhua
* @Date  :  2018-11-19
* @F_name:  fm24cxx_read_len_byte
* @brief :  在FRAM的指定地址读出连续几个字节的数据
* @param :  WriteAddr：0~2047
			
			Len：要写入数据的长度2,4
* @retval:  DataToWrite：读出的数据，可以用u32的数组来存储
* @note  ： 该函数用于读出入16bit或者32bit的数据.
*****************************************************************************/
u32 fm24cxx_read_len_byte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=fm24cxx_read_one_byte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}



/****************************************************************************
* @Author： Zhang Zhenhua
* @Date  :  2018-11-20
* @F_name:  fm24cxx_read
* @brief :  在FM24CXX里面的指定地址开始读出指定个数的数据
* @param :  
			ReadAddr :开始读出的地址 对此款FRAM为0~2047
			pBuffer  :数据数组首地址
			NumToRead:要读出数据的个数			
* @retval:  None
* @note  ： 此函数在现在版本程序中用的很较少
*****************************************************************************/
void fm24cxx_read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=fm24cxx_read_one_byte(ReadAddr++);	
		NumToRead--;
	}
}  

/****************************************************************************
* @Author： Zhang Zhenhua
* @Date  :  2018-11-20
* @F_name:  fm24cxx_read
* @brief :  在FM24CXX里面的指定地址开始写入指定个数的数据
* @param :  
			WriteAddr :开始写入的地址 对24c02为0~255
			pBuffer   :数据数组首地址
			NumToWrite:要写入数据的个数		
* @retval:  None
* @note  ： 此函数在现在版本程序中用的很较少
*****************************************************************************/
void fm24cxx_write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		fm24cxx_write_one_byte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}














