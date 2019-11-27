/*******************************************************************************************************
Copyright：   @山东大学数控研究中心
File name:    io.h
Author:       张振华
Date:         2019-8-30
Description:  板子控制的通用输入输出定义；
              包含伺服电机接口和IO接口的所有引脚操作
			  
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

void uln2003_init(void);//初始化

		 				    
#endif
