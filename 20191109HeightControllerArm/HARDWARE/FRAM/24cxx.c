/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    24cxx.c
Author:       ����
Date:         2018-11-19
Description:  FRAM��ʼ������д��API����ʵ�֡�
              FRAM��������2k Byte���ͺ�MB85RC16��
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#include "sys.h"
#include "24cxx.h" 

/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date:    2018-11-19
* @F_name:  fm24cxx_init
* @brief :  FRAM��IIC�ӿڳ�ʼ��
* @param :  None
* @retval:  None
* @note  �� ���ڳ�ʼ��FRAM��ģ��IIC�ӿ�
*****************************************************************************/ 
void fm24cxx_init(void)
{
	IIC_Init();
}

/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date  :  2018-11-19
* @F_name:  fm24cxx_check
* @brief :  ���FRAM�Ƿ���������
* @param :  None
* @retval:  
            0��FRAM������д
            1��FRAM��дʧ��
* @note  �� ��������24XX�����һ����ַ(2047)���洢��־��,���������24Cϵ��,
			�����ַҪ�޸�
*****************************************************************************/
u8 fm24cxx_check(void)
{
	u8 temp;
	temp=fm24cxx_read_one_byte(2047);//����ÿ�ο�����дFRAM		   
	if(temp==0X55)return 0;		   
	else						   //�ų���һ�γ�ʼ�������
	{
		fm24cxx_write_one_byte(2047,0X55);
		temp=fm24cxx_read_one_byte(2047);	  
		if(temp==0X55)return 0;
	}
	return 1;	
}

/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date:    2018-11-19
* @F_name:  fm24cxx_read_one_byte
* @brief :  ��FRAM��ָ����ַ����һ���ֽڵ�����
* @param :  ReadAddr��0~2048
* @retval:  ��������һ���ֽ�����
* @note  �� ���ڳ�ʼ��FRAM��ģ��IIC�ӿ�
            ֮ǰ����ԭ���Դ������������⣬��256�ֽ��Ժ�����ݲ���������ȡ��
            ������˲�����
*****************************************************************************/
u8 fm24cxx_read_one_byte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   	
	IIC_Send_Byte(0XA1+((ReadAddr/256)<<1)); //��ǰ�� IIC_Send_Byte(0XA1);���²�����256�ֽڵĵ�ַû�ж�ȡ������ʵ����д���Ѿ�û�������ˡ�	
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();                    //����һ��ֹͣ����	    
	return temp;
}

/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date:    2018-11-19
* @F_name:  fm24cxx_write_one_byte
* @brief :  ��FRAM��ָ����ַд��һ���ֽڵ�����
* @param :  WriteAddr��0~2048
* @retval:  None
* @note  �� ���ڳ�ʼ��FRAM��ģ��IIC�ӿ�
            ֮ǰ����ԭ���Դ������������⣬��256�ֽ��Ժ�����ݲ���������ȡ��
            ������˲�����
*****************************************************************************/
void fm24cxx_write_one_byte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack(); 
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	 
}
/****************************************************************************
* @Author�� Zhang Zhenhua
* @Date  :  2018-11-19
* @F_name:  fm24cxx_write_len_byte
* @brief :  ��FRAM��ָ����ַд�����������ֽڵ�����
* @param :  WriteAddr��0~2047
			DataToWrite��Ҫд�����������׵�ַ
			Len��Ҫд�����ݵĳ���2,4
* @retval:  None
* @note  �� �ú�������д��16bit����32bit������.
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
* @Author�� Zhang Zhenhua
* @Date  :  2018-11-19
* @F_name:  fm24cxx_read_len_byte
* @brief :  ��FRAM��ָ����ַ�������������ֽڵ�����
* @param :  WriteAddr��0~2047
			
			Len��Ҫд�����ݵĳ���2,4
* @retval:  DataToWrite�����������ݣ�������u32���������洢
* @note  �� �ú������ڶ�����16bit����32bit������.
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
* @Author�� Zhang Zhenhua
* @Date  :  2018-11-20
* @F_name:  fm24cxx_read
* @brief :  ��FM24CXX�����ָ����ַ��ʼ����ָ������������
* @param :  
			ReadAddr :��ʼ�����ĵ�ַ �Դ˿�FRAMΪ0~2047
			pBuffer  :���������׵�ַ
			NumToRead:Ҫ�������ݵĸ���			
* @retval:  None
* @note  �� �˺��������ڰ汾�������õĺܽ���
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
* @Author�� Zhang Zhenhua
* @Date  :  2018-11-20
* @F_name:  fm24cxx_read
* @brief :  ��FM24CXX�����ָ����ַ��ʼд��ָ������������
* @param :  
			WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
			pBuffer   :���������׵�ַ
			NumToWrite:Ҫд�����ݵĸ���		
* @retval:  None
* @note  �� �˺��������ڰ汾�������õĺܽ���
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














