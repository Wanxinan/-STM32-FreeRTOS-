#ifndef __MAIN_H__
#define __MAIN_H__
#include "Config.h"  

QueueHandle_t queueBlueTooth; // 字符串传输队列
QueueHandle_t queueWIFI; // 字符串传输队列
QueueHandle_t tipsQueue; // 通知程序运行状态的队列

void task1 ( void * arg );

#endif
