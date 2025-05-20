#ifndef __PC_H__
#define __PC_H__
#include "Config.h"  


// 基于串口1实现: 
//		专门用来在程序执行过程中做printf
//		用以方便追踪程序运行状态

extern QueueHandle_t queue;

void PC_Init(void);
void PC_SendByte(uint8_t Byte);
void printf1(char *format, ...);


#endif
