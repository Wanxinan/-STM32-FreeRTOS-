#ifndef __BLUE_TOOTH_H__
#define __BLUE_TOOTH_H__
#include "Config.h"  

// 基于串口3实现和蓝牙之间的通信
extern QueueHandle_t queueBlueTooth; // 字符串传输队列


void BlueTooth_Init(void); // 初始化串口
void BlueTooth_NVIC(void);// 初始化中断
void messageToApp(char *strs); // 当WIFI热点配置成功, 回传配置成功给蓝牙
void readBlueTooth(char * buf);

#endif
