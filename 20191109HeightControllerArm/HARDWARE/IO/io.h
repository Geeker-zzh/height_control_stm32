/*******************************************************************************************************
Copyright��   @ɽ����ѧ�����о�����
File name:    io.h
Author:       ����
Date:         2019-8-30
Description:  ���ӿ��Ƶ�ͨ������������壻
              �����ŷ�����ӿں�IO�ӿڵ��������Ų���
			  
Others:     
History: 
Date:         
Author:
Modification:
*******************************************************************************************************/
#ifndef __IO_H
#define __IO_H	 
#include "sys.h"
#define SERVO_CLR PDout(3)	// ULN2003 IN1
#define SERVO_ON  PDout(2)	// ULN2003 IN2	
#define SERVO_0S  PDout(1)	// ULN2003 IN3	
#define GOUT1     PDout(0)	// ULN2003 IN4	
#define GOUT2     PCout(12)	// ULN2003 IN5	
#define GOUT3     PCout(11)	// ULN2003 IN6
#define GOUT4     PCout(10)	// ULN2003 IN7	

void uln2003_init(void);//��ʼ��

		 				    
#endif
